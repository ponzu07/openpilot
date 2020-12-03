<<<<<<< HEAD
from functools import total_ordering
=======
from enum import IntEnum
>>>>>>> origin/ci-clean
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
<<<<<<< HEAD
class Priority:
=======
class Priority(IntEnum):
>>>>>>> origin/ci-clean
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
<<<<<<< HEAD
=======
            alert.event_type = et
>>>>>>> origin/ci-clean
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

<<<<<<< HEAD
@total_ordering
=======
>>>>>>> origin/ci-clean
class Alert:
  def __init__(self,
               alert_text_1: str,
               alert_text_2: str,
<<<<<<< HEAD
               alert_status,
               alert_size,
               alert_priority,
               visual_alert,
               audible_alert,
=======
               alert_status: log.ControlsState.AlertStatus,
               alert_size: log.ControlsState.AlertSize,
               alert_priority: Priority,
               visual_alert: car.CarControl.HUDControl.VisualAlert,
               audible_alert: car.CarControl.HUDControl.AudibleAlert,
>>>>>>> origin/ci-clean
               duration_sound: float,
               duration_hud_alert: float,
               duration_text: float,
               alert_rate: float = 0.,
               creation_delay: float = 0.):

<<<<<<< HEAD
    self.alert_type = ""
=======
>>>>>>> origin/ci-clean
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

<<<<<<< HEAD
    self.start_time = 0.
    self.alert_rate = alert_rate
    self.creation_delay = creation_delay

    # typecheck that enums are valid on startup
    tst = car.CarControl.new_message()
    tst.hudControl.visualAlert = self.visual_alert

  def __str__(self) -> str:
    return self.alert_text_1 + "/" + self.alert_text_2 + " " + str(self.alert_priority) + "  " + str(
      self.visual_alert) + " " + str(self.audible_alert)
=======
    self.alert_rate = alert_rate
    self.creation_delay = creation_delay

    self.start_time = 0.
    self.alert_type = ""
    self.event_type = None

  def __str__(self) -> str:
    return f"{self.alert_text_1}/{self.alert_text_2} {self.alert_priority} {self.visual_alert} {self.audible_alert}"
>>>>>>> origin/ci-clean

  def __gt__(self, alert2) -> bool:
    return self.alert_priority > alert2.alert_priority

<<<<<<< HEAD
  def __eq__(self, alert2) -> bool:
    return self.alert_priority == alert2.alert_priority

class NoEntryAlert(Alert):
  def __init__(self, alert_text_2, audible_alert=AudibleAlert.chimeError,
               visual_alert=VisualAlert.none, duration_hud_alert=2.):
    super().__init__("openpilot Unavailable", alert_text_2, AlertStatus.normal,
=======
class NoEntryAlert(Alert):
  def __init__(self, alert_text_2, audible_alert=AudibleAlert.chimeError,
               visual_alert=VisualAlert.none, duration_hud_alert=2.):
    super().__init__("オープンパイロットは利用不可", alert_text_2, AlertStatus.normal,
>>>>>>> origin/ci-clean
                     AlertSize.mid, Priority.LOW, visual_alert,
                     audible_alert, .4, duration_hud_alert, 3.)


class SoftDisableAlert(Alert):
  def __init__(self, alert_text_2):
<<<<<<< HEAD
    super().__init__("TAKE CONTROL IMMEDIATELY", alert_text_2,
=======
    super().__init__("すぐに運転を交代して下さい", alert_text_2,
>>>>>>> origin/ci-clean
                     AlertStatus.critical, AlertSize.full,
                     Priority.MID, VisualAlert.steerRequired,
                     AudibleAlert.chimeWarningRepeat, .1, 2., 2.),


class ImmediateDisableAlert(Alert):
<<<<<<< HEAD
  def __init__(self, alert_text_2, alert_text_1="TAKE CONTROL IMMEDIATELY"):
=======
  def __init__(self, alert_text_2, alert_text_1="すぐに運転を交代して下さい"):
>>>>>>> origin/ci-clean
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

<<<<<<< HEAD
=======
class NormalPermanentAlert(Alert):
  def __init__(self, alert_text_1, alert_text_2):
    super().__init__(alert_text_1, alert_text_2,
                     AlertStatus.normal, AlertSize.mid,
                     Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
>>>>>>> origin/ci-clean

# ********** alert callback functions **********

def below_steer_speed_alert(CP: car.CarParams, sm: messaging.SubMaster, metric: bool) -> Alert:
  speed = int(round(CP.minSteerSpeed * (CV.MS_TO_KPH if metric else CV.MS_TO_MPH)))
  unit = "km/h" if metric else "mph"
  return Alert(
<<<<<<< HEAD
    "TAKE CONTROL",
    "Steer Unavailable Below %d %s" % (speed, unit),
=======
    "ハンドルを持って",
    "ステアリングは %d %s のため使用できません" % (speed, unit),
>>>>>>> origin/ci-clean
    AlertStatus.userPrompt, AlertSize.mid,
    Priority.MID, VisualAlert.steerRequired, AudibleAlert.none, 0., 0.4, .3)

def calibration_incomplete_alert(CP: car.CarParams, sm: messaging.SubMaster, metric: bool) -> Alert:
  speed = int(MIN_SPEED_FILTER * (CV.MS_TO_KPH if metric else CV.MS_TO_MPH))
  unit = "km/h" if metric else "mph"
  return Alert(
<<<<<<< HEAD
    "Calibration in Progress: %d%%" % sm['liveCalibration'].calPerc,
    "Drive Above %d %s" % (speed, unit),
=======
    "キャリブレーション: %d%%" % sm['liveCalibration'].calPerc,
    "%d %s で走行してください" % (speed, unit),
>>>>>>> origin/ci-clean
    AlertStatus.normal, AlertSize.mid,
    Priority.LOWEST, VisualAlert.none, AudibleAlert.none, 0., 0., .2)

def no_gps_alert(CP: car.CarParams, sm: messaging.SubMaster, metric: bool) -> Alert:
  gps_integrated = sm['health'].hwType in [log.HealthData.HwType.uno, log.HealthData.HwType.dos]
  return Alert(
<<<<<<< HEAD
    "Poor GPS reception",
    "If sky is visible, contact support" if gps_integrated else "Check GPS antenna placement",
=======
    "GPS受信不良",
    "空が見えるのにエラーが出ているならサポートに連絡" if gps_integrated else "Check GPS antenna placement",
>>>>>>> origin/ci-clean
    AlertStatus.normal, AlertSize.mid,
    Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2, creation_delay=300.)

def wrong_car_mode_alert(CP: car.CarParams, sm: messaging.SubMaster, metric: bool) -> Alert:
<<<<<<< HEAD
  text = "Cruise Mode Disabled"
  if CP.carName == "honda":
    text = "Main Switch Off"
=======
  text = "クルーズモード無効"
  if CP.carName == "honda":
    text = "メインスイッチオフ"
>>>>>>> origin/ci-clean
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
<<<<<<< HEAD
      "Be ready to take over at any time",
      "Always keep hands on wheel and eyes on road",
=======
      "常に交代できるよう準備して",
      "常にハンドルに触れ道路から目を離さない",
>>>>>>> origin/ci-clean
      AlertStatus.normal, AlertSize.mid,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., 15.),
  },

  EventName.startupMaster: {
    ET.PERMANENT: Alert(
<<<<<<< HEAD
      "WARNING: This branch is not tested",
      "Always keep hands on wheel and eyes on road",
=======
      "警告: このブランチはテストされていません",
      "常にハンドルに触れ道路から目を離さない",
>>>>>>> origin/ci-clean
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., 15.),
  },

  EventName.startupNoControl: {
    ET.PERMANENT: Alert(
      "Dashcam mode",
<<<<<<< HEAD
      "Always keep hands on wheel and eyes on road",
=======
      "常にハンドルに触れ道路から目を離さない",
>>>>>>> origin/ci-clean
      AlertStatus.normal, AlertSize.mid,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., 15.),
  },

  EventName.startupNoCar: {
    ET.PERMANENT: Alert(
<<<<<<< HEAD
      "Dashcam mode for unsupported car",
      "Always keep hands on wheel and eyes on road",
=======
      "未対応車のためダッシュカムモード",
      "常にハンドルに触れ道路から目を離さない",
>>>>>>> origin/ci-clean
      AlertStatus.normal, AlertSize.mid,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., 15.),
  },

<<<<<<< HEAD
  EventName.invalidGiraffeToyota: {
    ET.PERMANENT: Alert(
      "Unsupported Giraffe Configuration",
      "Visit comma.ai/tg",
      AlertStatus.normal, AlertSize.mid,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
  },

  EventName.whitePandaUnsupported: {
    ET.PERMANENT: Alert(
      "White Panda Is No Longer Supported",
      "Upgrade to comma two or black panda",
      AlertStatus.normal, AlertSize.mid,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
    ET.NO_ENTRY: NoEntryAlert("White panda is no longer supported"),
  },

  EventName.invalidLkasSetting: {
    ET.PERMANENT: Alert(
      "Stock LKAS is turned on",
      "Turn off stock LKAS to engage",
=======
  EventName.invalidLkasSetting: {
    ET.PERMANENT: Alert(
      "純正LKASが有効になっています",
      "純正LKASを無効にして発進",
>>>>>>> origin/ci-clean
      AlertStatus.normal, AlertSize.mid,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
  },

  EventName.communityFeatureDisallowed: {
    # LOW priority to overcome Cruise Error
    ET.PERMANENT: Alert(
<<<<<<< HEAD
      "Community Feature Detected",
      "Enable Community Features in Developer Settings",
=======
      "コミュニティ機能が検出されました",
      "開発者設定でコミュニティ機能を有効にする",
>>>>>>> origin/ci-clean
      AlertStatus.normal, AlertSize.mid,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
  },

  EventName.carUnrecognized: {
    ET.PERMANENT: Alert(
<<<<<<< HEAD
      "Dashcam Mode",
      "Car Unrecognized",
=======
      "ダッシュカムモード",
      "未対応車",
>>>>>>> origin/ci-clean
      AlertStatus.normal, AlertSize.mid,
      Priority.LOWEST, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
  },

  EventName.stockAeb: {
    ET.PERMANENT: Alert(
<<<<<<< HEAD
      "BRAKE!",
      "Stock AEB: Risk of Collision",
=======
      "ブレーキ!",
      "純正AEB: 衝突リスクあり",
>>>>>>> origin/ci-clean
      AlertStatus.critical, AlertSize.full,
      Priority.HIGHEST, VisualAlert.fcw, AudibleAlert.none, 1., 2., 2.),
  },

  EventName.stockFcw: {
    ET.PERMANENT: Alert(
<<<<<<< HEAD
      "BRAKE!",
      "Stock FCW: Risk of Collision",
=======
      "ブレーキ!",
      "純正FCW: 衝突リスクあり",
>>>>>>> origin/ci-clean
      AlertStatus.critical, AlertSize.full,
      Priority.HIGHEST, VisualAlert.fcw, AudibleAlert.none, 1., 2., 2.),
  },

  EventName.fcw: {
    ET.PERMANENT: Alert(
<<<<<<< HEAD
      "BRAKE!",
      "Risk of Collision",
=======
      "ブレーキ!",
      "衝突リスクあり",
>>>>>>> origin/ci-clean
      AlertStatus.critical, AlertSize.full,
      Priority.HIGHEST, VisualAlert.fcw, AudibleAlert.chimeWarningRepeat, 1., 2., 2.),
  },

  EventName.ldw: {
    ET.PERMANENT: Alert(
<<<<<<< HEAD
      "TAKE CONTROL",
      "Lane Departure Detected",
=======
      "ハンドルを持って",
      "車線逸脱検知",
>>>>>>> origin/ci-clean
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.chimePrompt, 1., 2., 3.),
  },

  # ********** events only containing alerts that display while engaged **********

  EventName.gasPressed: {
    ET.PRE_ENABLE: Alert(
<<<<<<< HEAD
      "openpilot will not brake while gas pressed",
=======
      "オープンパイロットはアクセル中にブレーキを掛けません",
>>>>>>> origin/ci-clean
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOWEST, VisualAlert.none, AudibleAlert.none, .0, .0, .1, creation_delay=1.),
  },

  EventName.vehicleModelInvalid: {
    ET.WARNING: Alert(
<<<<<<< HEAD
      "Vehicle Parameter Identification Failed",
=======
      "車両パラメータの識別に失敗しました",
>>>>>>> origin/ci-clean
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOWEST, VisualAlert.steerRequired, AudibleAlert.none, .0, .0, .1),
  },

  EventName.steerTempUnavailableMute: {
    ET.WARNING: Alert(
<<<<<<< HEAD
      "TAKE CONTROL",
      "Steering Temporarily Unavailable",
=======
      "ハンドルを持って",
      "ステアリングが一時的に使用できません",
>>>>>>> origin/ci-clean
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, .2, .2, .2),
  },

  EventName.preDriverDistracted: {
    ET.WARNING: Alert(
<<<<<<< HEAD
      "KEEP EYES ON ROAD: Driver Distracted",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.none, .0, .1, .1, alert_rate=0.75),
=======
      "道路を見て: 注意散漫です",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.none, .0, .1, .1),
>>>>>>> origin/ci-clean
  },

  EventName.promptDriverDistracted: {
    ET.WARNING: Alert(
<<<<<<< HEAD
      "KEEP EYES ON ROAD",
      "Driver Appears Distracted",
=======
      "道路を見て",
      "運転手は注意散漫です",
>>>>>>> origin/ci-clean
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.MID, VisualAlert.steerRequired, AudibleAlert.chimeWarning2Repeat, .1, .1, .1),
  },

  EventName.driverDistracted: {
    ET.WARNING: Alert(
<<<<<<< HEAD
      "DISENGAGE IMMEDIATELY",
      "Driver Was Distracted",
=======
      "直ちに運転を交代してください",
      "運転手は注意散漫です",
>>>>>>> origin/ci-clean
      AlertStatus.critical, AlertSize.full,
      Priority.HIGH, VisualAlert.steerRequired, AudibleAlert.chimeWarningRepeat, .1, .1, .1),
  },

  EventName.preDriverUnresponsive: {
    ET.WARNING: Alert(
<<<<<<< HEAD
      "TOUCH STEERING WHEEL: No Face Detected",
=======
      "ハンドルに触れて: 顔が見えません",
>>>>>>> origin/ci-clean
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.none, .0, .1, .1, alert_rate=0.75),
  },

  EventName.promptDriverUnresponsive: {
    ET.WARNING: Alert(
<<<<<<< HEAD
      "TOUCH STEERING WHEEL",
      "Driver Is Unresponsive",
=======
      "ハンドルに触れて",
      "運転手が無反応です",
>>>>>>> origin/ci-clean
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.MID, VisualAlert.steerRequired, AudibleAlert.chimeWarning2Repeat, .1, .1, .1),
  },

  EventName.driverUnresponsive: {
    ET.WARNING: Alert(
<<<<<<< HEAD
      "DISENGAGE IMMEDIATELY",
      "Driver Was Unresponsive",
=======
      "直ちに運転を交代してください",
      "運転手が無反応です",
>>>>>>> origin/ci-clean
      AlertStatus.critical, AlertSize.full,
      Priority.HIGH, VisualAlert.steerRequired, AudibleAlert.chimeWarningRepeat, .1, .1, .1),
  },

  EventName.driverMonitorLowAcc: {
    ET.WARNING: Alert(
<<<<<<< HEAD
      "CHECK DRIVER FACE VISIBILITY",
      "Driver Monitor Model Output Uncertain",
=======
      "運転手の顔の見え方を確認",
      "運転手監視モデルが不完全",
>>>>>>> origin/ci-clean
      AlertStatus.normal, AlertSize.mid,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.none, .4, 0., 1.5),
  },

  EventName.manualRestart: {
    ET.WARNING: Alert(
<<<<<<< HEAD
      "TAKE CONTROL",
      "Resume Driving Manually",
=======
      "ハンドルを持って",
      "手動で運転を再開",
>>>>>>> origin/ci-clean
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
  },

  EventName.resumeRequired: {
    ET.WARNING: Alert(
<<<<<<< HEAD
      "STOPPED",
      "Press Resume to Move",
=======
      "停止",
      "再開を押して移動",
>>>>>>> origin/ci-clean
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
  },

  EventName.belowSteerSpeed: {
    ET.WARNING: below_steer_speed_alert,
  },

  EventName.preLaneChangeLeft: {
    ET.WARNING: Alert(
<<<<<<< HEAD
      "Steer Left to Start Lane Change",
      "Monitor Other Vehicles",
=======
      "左ハンドルで車線変更を開始",
      "周囲の車を確認",
>>>>>>> origin/ci-clean
      AlertStatus.normal, AlertSize.mid,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.none, .0, .1, .1, alert_rate=0.75),
  },

  EventName.preLaneChangeRight: {
    ET.WARNING: Alert(
<<<<<<< HEAD
      "Steer Right to Start Lane Change",
      "Monitor Other Vehicles",
=======
      "右ハンドルで車線変更を開始",
      "周囲の車を確認",
>>>>>>> origin/ci-clean
      AlertStatus.normal, AlertSize.mid,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.none, .0, .1, .1, alert_rate=0.75),
  },

  EventName.laneChangeBlocked: {
    ET.WARNING: Alert(
<<<<<<< HEAD
      "Car Detected in Blindspot",
      "Monitor Other Vehicles",
=======
      "ブラインドスポットに反応あり",
      "周囲の車を確認",
>>>>>>> origin/ci-clean
      AlertStatus.normal, AlertSize.mid,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.none, .0, .1, .1),
  },

  EventName.laneChange: {
    ET.WARNING: Alert(
<<<<<<< HEAD
      "Changing Lane",
      "Monitor Other Vehicles",
=======
      "車線変更中",
      "周囲の車を確認",
>>>>>>> origin/ci-clean
      AlertStatus.normal, AlertSize.mid,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.none, .0, .1, .1),
  },

  EventName.steerSaturated: {
    ET.WARNING: Alert(
<<<<<<< HEAD
      "TAKE CONTROL",
      "Turn Exceeds Steering Limit",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.chimePrompt, 1., 2., 3.),
=======
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
>>>>>>> origin/ci-clean
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
<<<<<<< HEAD
    ET.NO_ENTRY: NoEntryAlert("Brake Hold Active"),
=======
    ET.NO_ENTRY: NoEntryAlert("ブレーキホールド作動"),
>>>>>>> origin/ci-clean
  },

  EventName.parkBrake: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.chimeDisengage),
<<<<<<< HEAD
    ET.NO_ENTRY: NoEntryAlert("Park Brake Engaged"),
=======
    ET.NO_ENTRY: NoEntryAlert("パーキングブレーキが作動"),
>>>>>>> origin/ci-clean
  },

  EventName.pedalPressed: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.chimeDisengage),
<<<<<<< HEAD
    ET.NO_ENTRY: NoEntryAlert("Pedal Pressed During Attempt",
=======
    ET.NO_ENTRY: NoEntryAlert("試行中にペダルを検知",
>>>>>>> origin/ci-clean
                              visual_alert=VisualAlert.brakePressed),
  },

  EventName.wrongCarMode: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.chimeDisengage),
    ET.NO_ENTRY: wrong_car_mode_alert,
  },

  EventName.wrongCruiseMode: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.chimeDisengage),
<<<<<<< HEAD
    ET.NO_ENTRY: NoEntryAlert("Enable Adaptive Cruise"),
=======
    ET.NO_ENTRY: NoEntryAlert("アダプティブクルーズを有効にする"),
>>>>>>> origin/ci-clean
  },

  EventName.steerTempUnavailable: {
    ET.WARNING: Alert(
<<<<<<< HEAD
      "TAKE CONTROL",
      "Steering Temporarily Unavailable",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.chimeWarning1, .4, 2., 3.),
    ET.NO_ENTRY: NoEntryAlert("Steering Temporarily Unavailable",
                              duration_hud_alert=0.),
  },

  EventName.focusRecoverActive: {
    ET.WARNING: Alert(
      "TAKE CONTROL",
      "Attempting Refocus: Camera Focus Invalid",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.chimeWarning1, .4, 2., 3., creation_delay=3.1),
  },

  EventName.outOfSpace: {
    ET.NO_ENTRY: NoEntryAlert("Out of Storage Space",
=======
      "ハンドルを持って",
      "ステアリングが一時的に使用できません",
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.chimeWarning1, .4, 2., 3.),
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
>>>>>>> origin/ci-clean
                              duration_hud_alert=0.),
  },

  EventName.belowEngageSpeed: {
<<<<<<< HEAD
    ET.NO_ENTRY: NoEntryAlert("Speed Too Low"),
  },

  EventName.neosUpdateRequired: {
    ET.PERMANENT: Alert(
      "NEOS Update Required",
      "Please Wait for Update",
      AlertStatus.normal, AlertSize.mid,
      Priority.HIGHEST, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
    ET.NO_ENTRY: NoEntryAlert("NEOS Update Required"),
=======
    ET.NO_ENTRY: NoEntryAlert("低速すぎる"),
>>>>>>> origin/ci-clean
  },

  EventName.sensorDataInvalid: {
    ET.PERMANENT: Alert(
<<<<<<< HEAD
      "No Data from Device Sensors",
      "Reboot your Device",
      AlertStatus.normal, AlertSize.mid,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2, creation_delay=1.),
    ET.NO_ENTRY: NoEntryAlert("No Data from Device Sensors"),
=======
      "センサーから情報が取得できない",
      "端末を再起動",
      AlertStatus.normal, AlertSize.mid,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2, creation_delay=1.),
    ET.NO_ENTRY: NoEntryAlert("センサーから情報が取得できない"),
>>>>>>> origin/ci-clean
  },

  EventName.noGps: {
    ET.PERMANENT: no_gps_alert,
  },

  EventName.soundsUnavailable: {
<<<<<<< HEAD
    ET.PERMANENT: Alert(
      "Speaker not found",
      "Reboot your Device",
      AlertStatus.normal, AlertSize.mid,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
    ET.NO_ENTRY: NoEntryAlert("Speaker not found"),
  },

  EventName.tooDistracted: {
    ET.NO_ENTRY: NoEntryAlert("Distraction Level Too High"),
  },

  EventName.overheat: {
    ET.SOFT_DISABLE: SoftDisableAlert("System Overheated"),
    ET.NO_ENTRY: NoEntryAlert("System overheated"),
  },

  EventName.wrongGear: {
    ET.SOFT_DISABLE: SoftDisableAlert("Gear not D"),
    ET.NO_ENTRY: NoEntryAlert("Gear not D"),
  },

  EventName.calibrationInvalid: {
    ET.PERMANENT: Alert(
      "Calibration Invalid",
      "Reposition Device and Recalibrate",
      AlertStatus.normal, AlertSize.mid,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
    ET.SOFT_DISABLE: SoftDisableAlert("Calibration Invalid: Reposition Device & Recalibrate"),
    ET.NO_ENTRY: NoEntryAlert("Calibration Invalid: Reposition Device & Recalibrate"),
  },

  EventName.calibrationIncomplete: {
    ET.SOFT_DISABLE: SoftDisableAlert("Calibration in Progress"),
    ET.PERMANENT: calibration_incomplete_alert,
    ET.NO_ENTRY: NoEntryAlert("Calibration in Progress"),
  },

  EventName.doorOpen: {
    ET.SOFT_DISABLE: SoftDisableAlert("Door Open"),
    ET.NO_ENTRY: NoEntryAlert("Door open"),
  },

  EventName.seatbeltNotLatched: {
    ET.SOFT_DISABLE: SoftDisableAlert("Seatbelt Unlatched"),
    ET.NO_ENTRY: NoEntryAlert("Seatbelt unlatched"),
  },

  EventName.espDisabled: {
    ET.SOFT_DISABLE: SoftDisableAlert("ESP Off"),
    ET.NO_ENTRY: NoEntryAlert("ESP Off"),
  },

  EventName.lowBattery: {
    ET.SOFT_DISABLE: SoftDisableAlert("Low Battery"),
    ET.NO_ENTRY: NoEntryAlert("Low Battery"),
  },

  EventName.commIssue: {
    ET.SOFT_DISABLE: SoftDisableAlert("Communication Issue between Processes"),
    ET.NO_ENTRY: NoEntryAlert("Communication Issue between Processes",
=======
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
    ET.SOFT_DISABLE: SoftDisableAlert("低バッテリー"),
    ET.NO_ENTRY: NoEntryAlert("低バッテリー"),
  },

  EventName.commIssue: {
    ET.SOFT_DISABLE: SoftDisableAlert("プロセス間の連携問題"),
    ET.NO_ENTRY: NoEntryAlert("プロセス間の連携問題",
>>>>>>> origin/ci-clean
                              audible_alert=AudibleAlert.chimeDisengage),
  },

  EventName.radarCommIssue: {
<<<<<<< HEAD
    ET.SOFT_DISABLE: SoftDisableAlert("Radar Communication Issue"),
    ET.NO_ENTRY: NoEntryAlert("Radar Communication Issue",
=======
    ET.SOFT_DISABLE: SoftDisableAlert("レーダーの通信問題"),
    ET.NO_ENTRY: NoEntryAlert("レーダーの通信問題",
>>>>>>> origin/ci-clean
                              audible_alert=AudibleAlert.chimeDisengage),
  },

  EventName.radarCanError: {
<<<<<<< HEAD
    ET.SOFT_DISABLE: SoftDisableAlert("Radar Error: Restart the Car"),
    ET.NO_ENTRY: NoEntryAlert("Radar Error: Restart the Car"),
  },

  EventName.radarFault: {
    ET.SOFT_DISABLE: SoftDisableAlert("Radar Error: Restart the Car"),
    ET.NO_ENTRY : NoEntryAlert("Radar Error: Restart the Car"),
  },

  EventName.modeldLagging: {
    ET.SOFT_DISABLE: SoftDisableAlert("Driving model lagging"),
    ET.NO_ENTRY : NoEntryAlert("Driving model lagging"),
  },

  EventName.posenetInvalid: {
    ET.SOFT_DISABLE: SoftDisableAlert("Vision Model Output Uncertain"),
    ET.NO_ENTRY: NoEntryAlert("Vision Model Output Uncertain"),
  },

  EventName.deviceFalling: {
    ET.SOFT_DISABLE: SoftDisableAlert("Device Fell Off Mount"),
    ET.NO_ENTRY: NoEntryAlert("Device Fell Off Mount"),
  },

  EventName.lowMemory: {
    ET.SOFT_DISABLE: SoftDisableAlert("Low Memory: Reboot Your Device"),
    ET.PERMANENT: Alert(
      "RAM Critically Low",
      "Reboot your Device",
      AlertStatus.normal, AlertSize.mid,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
    ET.NO_ENTRY : NoEntryAlert("Low Memory: Reboot Your Device",
=======
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
>>>>>>> origin/ci-clean
                               audible_alert=AudibleAlert.chimeDisengage),
  },

  EventName.controlsFailed: {
<<<<<<< HEAD
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("Controls Failed"),
    ET.NO_ENTRY: NoEntryAlert("Controls Failed"),
  },

  EventName.controlsMismatch: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("Controls Mismatch"),
  },

  EventName.canError: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("CAN Error: Check Connections"),
    ET.PERMANENT: Alert(
      "CAN Error: Check Connections",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, 0., 0., .2, creation_delay=1.),
    ET.NO_ENTRY: NoEntryAlert("CAN Error: Check Connections"),
  },

  EventName.steerUnavailable: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("LKAS Fault: Restart the Car"),
    ET.PERMANENT: Alert(
      "LKAS Fault: Restart the car to engage",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
    ET.NO_ENTRY: NoEntryAlert("LKAS Fault: Restart the Car"),
  },

  EventName.brakeUnavailable: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("Cruise Fault: Restart the Car"),
    ET.PERMANENT: Alert(
      "Cruise Fault: Restart the car to engage",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
    ET.NO_ENTRY: NoEntryAlert("Cruise Fault: Restart the Car"),
  },

  EventName.gasUnavailable: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("Gas Fault: Restart the Car"),
    ET.NO_ENTRY: NoEntryAlert("Gas Error: Restart the Car"),
=======
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
>>>>>>> origin/ci-clean
  },

  EventName.reverseGear: {
    ET.PERMANENT: Alert(
      "Reverse\nGear",
      "",
      AlertStatus.normal, AlertSize.full,
      Priority.LOWEST, VisualAlert.none, AudibleAlert.none, 0., 0., .2, creation_delay=0.5),
<<<<<<< HEAD
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("Reverse Gear"),
    ET.NO_ENTRY: NoEntryAlert("Reverse Gear"),
  },

  EventName.cruiseDisabled: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("Cruise Is Off"),
  },

  EventName.plannerError: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("Planner Solution Error"),
    ET.NO_ENTRY: NoEntryAlert("Planner Solution Error"),
  },

  EventName.relayMalfunction: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert("Harness Malfunction"),
    ET.PERMANENT: Alert(
      "Harness Malfunction",
      "Please Check Hardware",
      AlertStatus.normal, AlertSize.mid,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
    ET.NO_ENTRY: NoEntryAlert("Harness Malfunction"),
=======
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
>>>>>>> origin/ci-clean
  },

  EventName.noTarget: {
    ET.IMMEDIATE_DISABLE: Alert(
<<<<<<< HEAD
      "openpilot Canceled",
      "No close lead car",
=======
      "オープンパイロットがキャンセル",
      "前方に車はいません",
>>>>>>> origin/ci-clean
      AlertStatus.normal, AlertSize.mid,
      Priority.HIGH, VisualAlert.none, AudibleAlert.chimeDisengage, .4, 2., 3.),
    ET.NO_ENTRY : NoEntryAlert("No Close Lead Car"),
  },

  EventName.speedTooLow: {
    ET.IMMEDIATE_DISABLE: Alert(
<<<<<<< HEAD
      "openpilot Canceled",
      "Speed too low",
=======
      "オープンパイロットがキャンセル",
      "速度が低すぎます",
>>>>>>> origin/ci-clean
      AlertStatus.normal, AlertSize.mid,
      Priority.HIGH, VisualAlert.none, AudibleAlert.chimeDisengage, .4, 2., 3.),
  },

  EventName.speedTooHigh: {
    ET.WARNING: Alert(
<<<<<<< HEAD
      "Speed Too High",
      "Slow down to resume operation",
      AlertStatus.normal, AlertSize.mid,
      Priority.HIGH, VisualAlert.steerRequired, AudibleAlert.chimeWarning2Repeat, 2.2, 3., 4.),
    ET.NO_ENTRY: Alert(
      "Speed Too High",
      "Slow down to engage",
=======
      "速度が高すぎます",
      "減速して再開します",
      AlertStatus.normal, AlertSize.mid,
      Priority.HIGH, VisualAlert.steerRequired, AudibleAlert.none, 2.2, 3., 4.),
    ET.NO_ENTRY: Alert(
      "速度が高すぎます",
      "減速して発進",
>>>>>>> origin/ci-clean
      AlertStatus.normal, AlertSize.mid,
      Priority.LOW, VisualAlert.none, AudibleAlert.chimeError, .4, 2., 3.),
  },

<<<<<<< HEAD
  EventName.internetConnectivityNeeded: {
    ET.PERMANENT: Alert(
      "Please connect to Internet",
      "An Update Check Is Required to Engage",
      AlertStatus.normal, AlertSize.mid,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
    ET.NO_ENTRY: NoEntryAlert("Please Connect to Internet",
=======
  # TODO: this is unclear, update check only happens offroad
  EventName.internetConnectivityNeeded: {
    ET.PERMANENT: NormalPermanentAlert("インターネットに接続", "発進するには更新確認が必要です"),
    ET.NO_ENTRY: NoEntryAlert("インターネットに接続",
>>>>>>> origin/ci-clean
                              audible_alert=AudibleAlert.chimeDisengage),
  },

  EventName.lowSpeedLockout: {
    ET.PERMANENT: Alert(
<<<<<<< HEAD
      "Cruise Fault: Restart the car to engage",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
    ET.NO_ENTRY: NoEntryAlert("Cruise Fault: Restart the Car"),
=======
      "クルーズ異常: 車を再起動",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, 0., 0., .2),
    ET.NO_ENTRY: NoEntryAlert("クルーズ異常: 車を再起動"),
>>>>>>> origin/ci-clean
  },

}
