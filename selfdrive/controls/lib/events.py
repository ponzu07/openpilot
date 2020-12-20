from enum import IntEnum
from typing import Dict, Union, Callable, Any

from cereal import log, car
import cereal.messaging as messaging
from common.realtime import DT_CTRL
from selfdrive.config import Conversions as CV
from selfdrive.locationd.calibrationd import MIN_SPEED_FILTER

AlertSize = log.ControlsState.AlertSize
AlertStatus = log.ControlsState.AlertStatus
VisualAlert = car.CarControl.HUDControl.VisualAlert
AudibleAlert = car.CarControl.HUDControl.AudibleAlert
EventName = car.CarEvent.EventName

# Alert priorities
class Priority(IntEnum):
  LOWEST = 0
  LOWER = 1
  LOW = 2
  MID = 3
  HIGH = 4
  HIGHEST = 5

# Event types
class ET:
  ENABLE = 'enable'
  PRE_ENABLE = 'preEnable'
  NO_ENTRY = 'noEntry'
  WARNING = 'warning'
  USER_DISABLE = 'userDisable'
  SOFT_DISABLE = 'softDisable'
  IMMEDIATE_DISABLE = 'immediateDisable'
  PERMANENT = 'permanent'

# get event name from enum
EVENT_NAME = {v: k for k, v in EventName.schema.enumerants.items()}


class Events:
  def __init__(self):
    self.events = []
    self.static_events = []
    self.events_prev = dict.fromkeys(EVENTS.keys(), 0)

  @property
  def names(self):
    return self.events

  def __len__(self):
    return len(self.events)

  def add(self, event_name, static=False):
    if static:
      self.static_events.append(event_name)
    self.events.append(event_name)

  def clear(self):
    self.events_prev = {k: (v+1 if k in self.events else 0) for k, v in self.events_prev.items()}
    self.events = self.static_events.copy()

  def any(self, event_type):
    for e in self.events:
      if event_type in EVENTS.get(e, {}).keys():
        return True
    return False

  def create_alerts(self, event_types, callback_args=None):
    if callback_args is None:
      callback_args = []

    ret = []
    for e in self.events:
      types = EVENTS[e].keys()
      for et in event_types:
        if et in types:
          alert = EVENTS[e][et]
          if not isinstance(alert, Alert):
            alert = alert(*callback_args)

          if DT_CTRL * (self.events_prev[e] + 1) >= alert.creation_delay:
            alert.alert_type = f"{EVENT_NAME[e]}/{et}"
            alert.event_type = et
            ret.append(alert)
    return ret

  def add_from_msg(self, events):
    for e in events:
      self.events.append(e.name.raw)

  def to_msg(self):
    ret = []
    for event_name in self.events:
      event = car.CarEvent.new_message()
      event.name = event_name
      for event_type in EVENTS.get(event_name, {}).keys():
        setattr(event, event_type , True)
      ret.append(event)
    return ret

class Alert:
  def __init__(self,
               alert_text_1: str,
               alert_text_2: str,
               alert_status: log.ControlsState.AlertStatus,
               alert_size: log.ControlsState.AlertSize,
               alert_priority: Priority,
               visual_alert: car.CarControl.HUDControl.VisualAlert,
               audible_alert: car.CarControl.HUDControl.AudibleAlert,
               duration_sound: float,
               duration_hud_alert: float,
               duration_text: float,
               alert_rate: float = 0.,
               creation_delay: float = 0.):

    self.alert_text_1 = alert_text_1
    self.alert_text_2 = alert_text_2
    self.alert_status = alert_status
    self.alert_size = alert_size
    self.alert_priority = alert_priority
    self.visual_alert = visual_alert
    self.audible_alert = audible_alert

    self.duration_sound = duration_sound
    self.duration_hud_alert = duration_hud_alert
    self.duration_text = duration_text

    self.alert_rate = alert_rate
    self.creation_delay = creation_delay

    self.start_time = 0.
    self.alert_type = ""
    self.event_type = None

  def __str__(self) -> str:
    return f"{self.alert_text_1}/{self.alert_text_2} {self.alert_priority} {self.visual_alert} {self.audible_alert}"

  def __gt__(self, alert2) -> bool:
    return self.alert_priority > alert2.alert_priority

class NoEntryAlert(Alert):
  def __init__(self, alert_text_2, audible_alert=AudibleAlert.chimeError,
               visual_alert=VisualAlert.none, duration_hud_alert=2.):
    super().__init__("オープンパイロットは利用不可", alert_text_2, AlertStatus.normal,
                     AlertSize.mid, Priority.LOW, visual_alert,
                     audible_alert, .4, duration_hud_alert, 3.)


class SoftDisableAlert(Alert):
  def __init__(self, alert_text_2):
    super().__init__("すぐに運転を交代して下さい", alert_text_2,
                     AlertStatus.critical, AlertSize.full,
                     Priority.MID, VisualAlert.steerRequired,
                     AudibleAlert.chimeWarningRepeat, .1, 2., 2.),


class ImmediateDisableAlert(Alert):
  def __init__(self, alert_text_2, alert_text_1="すぐに運転を交代して下さい"):
    super().__init__(alert_text_1, alert_text_2,
                     AlertStatus.critical, AlertSize.full,
                     Priority.HIGHEST, VisualAlert.steerRequired,
                     AudibleAlert.chimeWarningRepeat, 2.2, 3., 4.),

class EngagementAlert(Alert):
  def __init__(self, audible_alert=True):
    super().__init__("", "",
                     AlertStatus.normal, AlertSize.none,
                     Priority.MID, VisualAlert.none,
                     audible_alert, .2, 0., 0.),

class NormalPermanentAlert(Alert):
  def __init__(self, alert_text_1, alert_text_2):
    super().__init__(alert_text_1, alert_text_2,
                     AlertStatus.normal, AlertSize.mid,
                     Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2),

# ********** alert callback functions **********

def below_steer_speed_alert(CP: car.CarParams, sm: messaging.SubMaster, metric: bool) -> Alert:
  speed = int(round(CP.minSteerSpeed * (CV.MS_TO_KPH if metric else CV.MS_TO_MPH)))
  unit = "km/h" if metric else "mph"
  return Alert(
    "ハンドルを持って",
    "ステアリングは %d %s のため使用できません" % (speed, unit),
    AlertStatus.userPrompt, AlertSize.mid,
    Priority.MID, VisualAlert.steerRequired, AudibleAlert.none, 0., 0.4, .3)

def calibration_incomplete_alert(CP: car.CarParams, sm: messaging.SubMaster, metric: bool) -> Alert:
  speed = int(MIN_SPEED_FILTER * (CV.MS_TO_KPH if metric else CV.MS_TO_MPH))
  unit = "km/h" if metric else "mph"
  return Alert(
    "キャリブレーション: %d%%" % sm['liveCalibration'].calPerc,
    "%d %s で走行してください" % (speed, unit),
    AlertStatus.normal, AlertSize.mid,
    Priority.LOWEST, VisualAlert.none, AudibleAlert.none, 0., 0., .2)

def no_gps_alert(CP: car.CarParams, sm: messaging.SubMaster, metric: bool) -> Alert:
  gps_integrated = sm['health'].hwType in [log.HealthData.HwType.uno, log.HealthData.HwType.dos]
  return Alert(
    "GPS受信不良",
    "空が見えるのにエラーが出ているならサポートに連絡" if gps_integrated else "Check GPS antenna placement",
    AlertStatus.normal, AlertSize.mid,
    Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2, creation_delay=300.)

def wrong_car_mode_alert(CP: car.CarParams, sm: messaging.SubMaster, metric: bool) -> Alert:
  text = "クルーズモード無効"
  if CP.carName == "honda":
    text = "メインスイッチオフ"
  return NoEntryAlert(text, duration_hud_alert=0.)

EVENTS: Dict[int, Dict[str, Union[Alert, Callable[[Any, messaging.SubMaster, bool], Alert]]]] = {
  # ********** events with no alerts **********

  # ********** events only containing alerts displayed in all states **********

  EventName.debugAlert: {
    ET.PERMANENT: Alert(
      "DEBUG ALERT",
      "",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, .1, .1, .1),
  },

  EventName.startup: {
    ET.PERMANENT: Alert(
      "常に交代できるよう準備して",
      "常にハンドルに触れ道路から目を離さない",
      AlertStatus.normal, AlertSize.mid,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., 15.),
  },

  EventName.startupMaster: {
    ET.PERMANENT: Alert(
      "警告: このブランチはテストされていません",
      "常にハンドルに触れ道路から目を離さない",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., 15.),
  },

  EventName.startupNoControl: {
    ET.PERMANENT: Alert(
      "Dashcam mode",
      "常にハンドルに触れ道路から目を離さない",
      AlertStatus.normal, AlertSize.mid,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., 15.),
  },

  EventName.startupNoCar: {
    ET.PERMANENT: Alert(
      "未対応車のためダッシュカムモード",
      "常にハンドルに触れ道路から目を離さない",
      AlertStatus.normal, AlertSize.mid,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., 15.),
  },

  EventName.startupOneplus: {
    ET.PERMANENT: Alert(
      "WARNING: Original EON deprecated",
      "Device will no longer update",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., 15.),
  },

  EventName.invalidLkasSetting: {
    ET.PERMANENT: Alert(
      "純正LKASが有効になっています",
      "純正LKASを無効にして発進",
      AlertStatus.normal, AlertSize.mid,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
  },

  EventName.communityFeatureDisallowed: {
    # LOW priority to overcome Cruise Error
    ET.PERMANENT: Alert(
      "コミュニティ機能が検出されました",
      "開発者設定でコミュニティ機能を有効にする",
      AlertStatus.normal, AlertSize.mid,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
  },

  EventName.carUnrecognized: {
    ET.PERMANENT: Alert(
      "ダッシュカムモード",
      "未対応車",
      AlertStatus.normal, AlertSize.mid,
      Priority.LOWEST, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
  },

  EventName.stockAeb: {
    ET.PERMANENT: Alert(
      "ブレーキ!",
      "純正AEB: 衝突リスクあり",
      AlertStatus.critical, AlertSize.full,
      Priority.HIGHEST, VisualAlert.fcw, AudibleAlert.none, 1., 2., 2.),
  },

  EventName.stockFcw: {
    ET.PERMANENT: Alert(
      "ブレーキ!",
      "純正FCW: 衝突リスクあり",
      AlertStatus.critical, AlertSize.full,
      Priority.HIGHEST, VisualAlert.fcw, AudibleAlert.none, 1., 2., 2.),
  },

  EventName.fcw: {
    ET.PERMANENT: Alert(
      "ブレーキ!",
      "衝突リスクあり",
      AlertStatus.critical, AlertSize.full,
      Priority.HIGHEST, VisualAlert.fcw, AudibleAlert.chimeWarningRepeat, 1., 2., 2.),
  },

  EventName.ldw: {
    ET.PERMANENT: Alert(
      "ハンドルを持って",
      "車線逸脱検知",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.chimePrompt, 1., 2., 3.),
  },

  # ********** events only containing alerts that display while engaged **********

  EventName.gasPressed: {
    ET.PRE_ENABLE: Alert(
      "オープンパイロットはアクセル中にブレーキを掛けません",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOWEST, VisualAlert.none, AudibleAlert.none, .0, .0, .1, creation_delay=1.),
  },

  EventName.vehicleModelInvalid: {
    ET.WARNING: Alert(
      "車両パラメータの識別に失敗しました",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOWEST, VisualAlert.steerRequired, AudibleAlert.none, .0, .0, .1),
  },

  EventName.steerTempUnavailableMute: {
    ET.WARNING: Alert(
      "ハンドルを持って",
      "ステアリングが一時的に使用できません",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, .2, .2, .2),
  },

  EventName.preDriverDistracted: {
    ET.WARNING: Alert(
      "道路を見て: 注意散漫です",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.none, .0, .1, .1),
  },

  EventName.promptDriverDistracted: {
    ET.WARNING: Alert(
      "道路を見て",
      "運転手は注意散漫です",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.MID, VisualAlert.steerRequired, AudibleAlert.chimeWarning2Repeat, .1, .1, .1),
  },

  EventName.driverDistracted: {
    ET.WARNING: Alert(
      "直ちに運転を交代してください",
      "運転手は注意散漫です",
      AlertStatus.critical, AlertSize.full,
      Priority.HIGH, VisualAlert.steerRequired, AudibleAlert.chimeWarningRepeat, .1, .1, .1),
  },

  EventName.preDriverUnresponsive: {
    ET.WARNING: Alert(
      "ハンドルに触れて: 顔が見えません",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.none, .0, .1, .1, alert_rate=0.75),
  },

  EventName.promptDriverUnresponsive: {
    ET.WARNING: Alert(
      "ハンドルに触れて",
      "運転手が無反応です",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.MID, VisualAlert.steerRequired, AudibleAlert.chimeWarning2Repeat, .1, .1, .1),
  },

  EventName.driverUnresponsive: {
    ET.WARNING: Alert(
      "直ちに運転を交代してください",
      "運転手が無反応です",
      AlertStatus.critical, AlertSize.full,
      Priority.HIGH, VisualAlert.steerRequired, AudibleAlert.chimeWarningRepeat, .1, .1, .1),
  },

  EventName.driverMonitorLowAcc: {
    ET.WARNING: Alert(
      "運転手の顔の見え方を確認",
      "運転手監視モデルが不完全",
      AlertStatus.normal, AlertSize.mid,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.none, .4, 0., 1.5),
  },

  EventName.manualRestart: {
    ET.WARNING: Alert(
      "ハンドルを持って",
      "手動で運転を再開",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
  },

  EventName.resumeRequired: {
    ET.WARNING: Alert(
      "停止",
      "再開を押して移動",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
  },

  EventName.belowSteerSpeed: {
    ET.WARNING: below_steer_speed_alert,
  },

  EventName.preLaneChangeLeft: {
    ET.WARNING: Alert(
      "左ハンドルで車線変更を開始",
      "周囲の車を確認",
      AlertStatus.normal, AlertSize.mid,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.none, .0, .1, .1, alert_rate=0.75),
  },

  EventName.preLaneChangeRight: {
    ET.WARNING: Alert(
      "右ハンドルで車線変更を開始",
      "周囲の車を確認",
      AlertStatus.normal, AlertSize.mid,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.none, .0, .1, .1, alert_rate=0.75),
  },

  EventName.laneChangeBlocked: {
    ET.WARNING: Alert(
      "ブラインドスポットに反応あり",
      "周囲の車を確認",
      AlertStatus.normal, AlertSize.mid,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.none, .0, .1, .1),
  },

  EventName.laneChange: {
    ET.WARNING: Alert(
      "車線変更中",
      "周囲の車を確認",
      AlertStatus.normal, AlertSize.mid,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.none, .0, .1, .1),
  },

  EventName.steerSaturated: {
    ET.WARNING: Alert(
      "ハンドルを持って",
      "操舵限界を超えて旋回中",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.chimePrompt, 1., 1., 1.),
  },

  EventName.fanMalfunction: {
    ET.PERMANENT: NormalPermanentAlert("ファン異常", "サポートに連絡"),
  },

  EventName.cameraMalfunction: {
    ET.PERMANENT: NormalPermanentAlert("カメラ異常", "サポートに連絡"),
  },

  # ********** events that affect controls state transitions **********

  EventName.pcmEnable: {
    ET.ENABLE: EngagementAlert(AudibleAlert.chimeEngage),
  },

  EventName.buttonEnable: {
    ET.ENABLE: EngagementAlert(AudibleAlert.chimeEngage),
  },

  EventName.pcmDisable: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.chimeDisengage),
  },

  EventName.buttonCancel: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.chimeDisengage),
  },

  EventName.brakeHold: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.chimeDisengage),
    ET.NO_ENTRY: NoEntryAlert("ブレーキホールド作動"),
  },

  EventName.parkBrake: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.chimeDisengage),
    ET.NO_ENTRY: NoEntryAlert("パーキングブレーキが作動"),
  },

  EventName.pedalPressed: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.chimeDisengage),
    ET.NO_ENTRY: NoEntryAlert("試行中にペダルを検知",
                              visual_alert=VisualAlert.brakePressed),
  },

  EventName.wrongCarMode: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.chimeDisengage),
    ET.NO_ENTRY: wrong_car_mode_alert,
  },

  EventName.wrongCruiseMode: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.chimeDisengage),
    ET.NO_ENTRY: NoEntryAlert("アダプティブクルーズを有効にする"),
  },

  EventName.steerTempUnavailable: {
    ET.WARNING: Alert(
      "ハンドルを持って",
      "ステアリングが一時的に使用できません",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.chimeWarning1, .4, 2., 1.),
    ET.NO_ENTRY: NoEntryAlert("ステアリングが一時的に使用できません",
                              duration_hud_alert=0.),
  },

  EventName.outOfSpace: {
    ET.PERMANENT: Alert(
      "ストレージ外へ",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
    ET.NO_ENTRY: NoEntryAlert("空き容量不足",
                              duration_hud_alert=0.),
  },

  EventName.belowEngageSpeed: {
    ET.NO_ENTRY: NoEntryAlert("低速すぎる"),
  },

  EventName.sensorDataInvalid: {
    ET.PERMANENT: Alert(
      "センサーから情報が取得できない",
      "端末を再起動",
      AlertStatus.normal, AlertSize.mid,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2, creation_delay=1.),
    ET.NO_ENTRY: NoEntryAlert("センサーから情報が取得できない"),
  },

  EventName.noGps: {
    ET.PERMANENT: no_gps_alert,
  },

  EventName.soundsUnavailable: {
    ET.PERMANENT: NormalPermanentAlert("スピーカーが見つかりません", "端末を再起動"),
    ET.NO_ENTRY: NoEntryAlert("スピーカーが見つかりません"),
  },

  EventName.tooDistracted: {
    ET.NO_ENTRY: NoEntryAlert("注意力散漫レベルが高すぎます"),
  },

  EventName.overheat: {
    ET.PERMANENT: Alert(
      "オーバーヒート",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
    ET.SOFT_DISABLE: SoftDisableAlert("オーバーヒート"),
    ET.NO_ENTRY: NoEntryAlert("オーバーヒート"),
  },

  EventName.wrongGear: {
    ET.SOFT_DISABLE: SoftDisableAlert("ギアがDではありません"),
    ET.NO_ENTRY: NoEntryAlert("ギアがDではありません"),
  },

  EventName.calibrationInvalid: {
    ET.PERMANENT: NormalPermanentAlert("キャリブレーション失敗", "位置を調整後に再実行してください"),
    ET.SOFT_DISABLE: SoftDisableAlert("キャリブレーション失敗: 位置を調整して再実行"),
    ET.NO_ENTRY: NoEntryAlert("キャリブレーション失敗: 位置を調整して再実行"),
  },

  EventName.calibrationIncomplete: {
    ET.PERMANENT: calibration_incomplete_alert,
    ET.SOFT_DISABLE: SoftDisableAlert("キャリブレーション中"),
    ET.NO_ENTRY: NoEntryAlert("キャリブレーション中"),
  },

  EventName.doorOpen: {
    ET.SOFT_DISABLE: SoftDisableAlert("ドアが空いてます"),
    ET.NO_ENTRY: NoEntryAlert("ドアが空いてます"),
  },

  EventName.seatbeltNotLatched: {
    ET.SOFT_DISABLE: SoftDisableAlert("シートベルトがありません"),
    ET.NO_ENTRY: NoEntryAlert("シートベルトがありません"),
  },

  EventName.espDisabled: {
    ET.SOFT_DISABLE: SoftDisableAlert("ESP オフ"),
    ET.NO_ENTRY: NoEntryAlert("ESP オフ"),
  },

  EventName.lowBattery: {
    ET.SOFT_DISABLE: SoftDisableAlert("Low Battery"),
    ET.NO_ENTRY: NoEntryAlert("Low Battery"),
  },

  EventName.commIssue: {
    ET.SOFT_DISABLE: SoftDisableAlert("プロセス間の連携問題"),
    ET.NO_ENTRY: NoEntryAlert("プロセス間の連携問題",
                              audible_alert=AudibleAlert.chimeDisengage),
  },

  EventName.radarCommIssue: {
    ET.SOFT_DISABLE: SoftDisableAlert("レーダーの通信問題"),
    ET.NO_ENTRY: NoEntryAlert("レーダーの通信問題",
                              audible_alert=AudibleAlert.chimeDisengage),
  },

  EventName.radarCanError: {
    ET.SOFT_DISABLE: SoftDisableAlert("レーダー異常: 車を再起動"),
    ET.NO_ENTRY: NoEntryAlert("レーダー異常: 車を再起動"),
  },

  EventName.radarFault: {
    ET.SOFT_DISABLE: SoftDisableAlert("レーダー異常: 車を再起動"),
    ET.NO_ENTRY : NoEntryAlert("レーダー異常: 車を再起動"),
  },

  EventName.modeldLagging: {
    ET.SOFT_DISABLE: SoftDisableAlert("ドライビングモデルの遅延"),
    ET.NO_ENTRY : NoEntryAlert("ドライビングモデルの遅延"),
  },

  EventName.posenetInvalid: {
    ET.SOFT_DISABLE: SoftDisableAlert("モデル出力が不完全"),
    ET.NO_ENTRY: NoEntryAlert("モデル出力が不完全"),
  },

  EventName.deviceFalling: {
    ET.SOFT_DISABLE: SoftDisableAlert("端末が落下しました"),
    ET.NO_ENTRY: NoEntryAlert("端末が落下しました"),
  },

  EventName.lowMemory: {
    ET.SOFT_DISABLE: SoftDisableAlert("低メモリ: 端末を再起動"),
    ET.PERMANENT: NormalPermanentAlert("低メモリ", "端末を再起動"),
    ET.NO_ENTRY : NoEntryAlert("低メモリ: 端末を再起動",
                               audible_alert=AudibleAlert.chimeDisengage),
  },

  EventName.controlsFailed: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("コントロール失敗"),
    ET.NO_ENTRY: NoEntryAlert("コントロール失敗"),
  },

  EventName.controlsMismatch: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("コントロール不一致"),
  },

  EventName.canError: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("CAN異常: コネクタ接続を確認"),
    ET.PERMANENT: Alert(
      "CAN異常: コネクタ接続を確認",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, 0., 0., .2, creation_delay=1.),
    ET.NO_ENTRY: NoEntryAlert("CAN異常: コネクタ接続を確認"),
  },

  EventName.steerUnavailable: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("LKAS異常: 車を再起動"),
    ET.PERMANENT: Alert(
      "LKAS異常: 車を再起動",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
    ET.NO_ENTRY: NoEntryAlert("LKAS異常: 車を再起動"),
  },

  EventName.brakeUnavailable: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("クルーズ異常: 車を再起動"),
    ET.PERMANENT: Alert(
      "クルーズ異常: 車を再起動",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
    ET.NO_ENTRY: NoEntryAlert("クルーズ異常: 車を再起動"),
  },

  EventName.reverseGear: {
    ET.PERMANENT: Alert(
      "Reverse\nGear",
      "",
      AlertStatus.normal, AlertSize.full,
      Priority.LOWEST, VisualAlert.none, AudibleAlert.none, 0., 0., .2, creation_delay=0.5),
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("バックギア"),
    ET.NO_ENTRY: NoEntryAlert("バックギア"),
  },

  EventName.cruiseDisabled: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("クルーズオフ"),
  },

  EventName.plannerError: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("プランニングシステム異常"),
    ET.NO_ENTRY: NoEntryAlert("プランニングシステム異常"),
  },

  EventName.relayMalfunction: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("ハーネス異常"),
    ET.PERMANENT: NormalPermanentAlert("ハーネス異常", "ハードウェアを確認"),
    ET.NO_ENTRY: NoEntryAlert("ハーネス異常"),
  },

  EventName.noTarget: {
    ET.IMMEDIATE_DISABLE: Alert(
      "オープンパイロットがキャンセル",
      "前方に車はいません",
      AlertStatus.normal, AlertSize.mid,
      Priority.HIGH, VisualAlert.none, AudibleAlert.chimeDisengage, .4, 2., 3.),
    ET.NO_ENTRY : NoEntryAlert("No Close Lead Car"),
  },

  EventName.speedTooLow: {
    ET.IMMEDIATE_DISABLE: Alert(
      "オープンパイロットがキャンセル",
      "速度が低すぎます",
      AlertStatus.normal, AlertSize.mid,
      Priority.HIGH, VisualAlert.none, AudibleAlert.chimeDisengage, .4, 2., 3.),
  },

  EventName.speedTooHigh: {
    ET.WARNING: Alert(
      "速度が高すぎます",
      "減速して再開します",
      AlertStatus.normal, AlertSize.mid,
      Priority.HIGH, VisualAlert.steerRequired, AudibleAlert.none, 2.2, 3., 4.),
    ET.NO_ENTRY: Alert(
      "速度が高すぎます",
      "減速して発進",
      AlertStatus.normal, AlertSize.mid,
      Priority.LOW, VisualAlert.none, AudibleAlert.chimeError, .4, 2., 3.),
  },

  # TODO: this is unclear, update check only happens offroad
  EventName.internetConnectivityNeeded: {
    ET.PERMANENT: NormalPermanentAlert("インターネットに接続", "発進するには更新確認が必要です"),
    ET.NO_ENTRY: NoEntryAlert("インターネットに接続",
                              audible_alert=AudibleAlert.chimeDisengage),
  },

  EventName.lowSpeedLockout: {
    ET.PERMANENT: Alert(
      "クルーズ異常: 車を再起動",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
    ET.NO_ENTRY: NoEntryAlert("クルーズ異常: 車を再起動"),
  },

}
