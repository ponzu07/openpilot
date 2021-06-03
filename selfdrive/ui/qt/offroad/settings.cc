#include "settings.h"

#include <cassert>
#include <string>

#ifndef QCOM
#include "selfdrive/ui/qt/offroad/networking.h"
#endif
#include "selfdrive/common/params.h"
#include "selfdrive/common/util.h"
#include "selfdrive/hardware/hw.h"
#include "selfdrive/ui/qt/widgets/controls.h"
#include "selfdrive/ui/qt/widgets/input.h"
#include "selfdrive/ui/qt/widgets/offroad_alerts.h"
#include "selfdrive/ui/qt/widgets/scrollview.h"
#include "selfdrive/ui/qt/widgets/ssh_keys.h"
#include "selfdrive/ui/qt/widgets/toggle.h"
#include "selfdrive/ui/ui.h"
#include "selfdrive/ui/qt/util.h"

TogglesPanel::TogglesPanel(QWidget *parent) : QWidget(parent) {
  QVBoxLayout *toggles_list = new QVBoxLayout();

  QList<ParamControl*> toggles;

  toggles.append(new ParamControl("OpenpilotEnabledToggle",
                                  "オープンパイロットを有効化",
                                  "ACCとLKDSにオープンパイロットを使用します。この機能を使用するには、常にドライバーの注意が必要です。この設定を変更すると車のエンジンを切った際に有効になります。",
                                  "../assets/offroad/icon_openpilot.png",
                                  this));
  toggles.append(new ParamControl("IsLdwEnabled",
                                  "車線逸脱警報の有効化",
                                  "時速50km以上で走行中に、方向指示器が作動していない状態で検知された車線を超えて走行した場合、車線内に戻るように警告を出すことができます。",
                                  "../assets/offroad/icon_warning.png",
                                  this));
  toggles.append(new ParamControl("IsRHD",
                                  "右ハンドルを有効化",
                                  "左車線の交通規則が適用され、右側の運転席でドライバー監視を行うようにします。",
                                  "../assets/offroad/icon_openpilot_mirrored.png",
                                  this));
  toggles.append(new ParamControl("IsMetric",
                                  "メートル法を使用",
                                  "mp/hではなくkm/hで表示",
                                  "../assets/offroad/icon_metric.png",
                                  this));
  toggles.append(new ParamControl("CommunityFeaturesToggle",
                                  "コミュニティ機能の有効化",
                                  "comma.aiがメンテナンスやサポートを行っておらず、標準的な安全モデルを満たすことが確認されていないオープンソースコミュニティの機能を使用します。これらの機能には、コミュニティがサポートする自動車やコミュニティがサポートするハードウェアが含まれます。これらの機能を使用する際には、特に注意してください。",
                                  "../assets/offroad/icon_shell.png",
                                  this));

  toggles.append(new ParamControl("IsUploadRawEnabled",
                                  "生ログをアップロード",
                                  "WiFi使用時に、フルログとフル解像度のビデオをデフォルトでアップロードします。有効になっていない場合は、my.comma.ai/useradminで個々のログをアップロード用にマークすることができます。",
                                  "../assets/offroad/icon_network.png",
                                  this));

  ParamControl *record_toggle = new ParamControl("RecordFront",
                                                 "ドライバーカメラの録画とアップロード",
                                                "ドライバーの顔データをアップロードしてドライバー監視アルゴリズムの改善に貢献します。",
                                                "../assets/offroad/icon_monitoring.png",
                                                this);
  toggles.append(record_toggle);
  toggles.append(new ParamControl("EndToEndToggle",
                                   "\U0001f96c 車線を無視 (アルファ) \U0001f96c",
                                   "車線を無視し人間が運転するように運転します。",
                                   "../assets/offroad/icon_road.png",
                                   this));

  if (Hardware::TICI()) {
    toggles.append(new ParamControl("EnableWideCamera",
                                    "広角カメラの使用を可能にする",
                                    "Use wide angle camera for driving and ui.",
                                    "../assets/offroad/icon_openpilot.png",
                                    this));
    QObject::connect(toggles.back(), &ToggleControl::toggleFlipped, [=](bool state) {
      Params().remove("CalibrationParams");
    });

    toggles.append(new ParamControl("EnableLteOnroad",
                                    "オンロードでLTEを有効にする",
                                    "",
                                    "../assets/offroad/icon_network.png",
                                    this));
  }

  bool record_lock = Params().getBool("RecordFrontLock");
  record_toggle->setEnabled(!record_lock);

  for(ParamControl *toggle : toggles){
    if(toggles_list->count() != 0){
      toggles_list->addWidget(horizontal_line());
    }
    toggles_list->addWidget(toggle);
  }

  setLayout(toggles_list);
}

DevicePanel::DevicePanel(QWidget* parent) : QWidget(parent) {
  QVBoxLayout *device_layout = new QVBoxLayout;
  Params params = Params();

  QString dongle = QString::fromStdString(params.get("DongleId", false));
  device_layout->addWidget(new LabelControl("Dongle ID", dongle));
  device_layout->addWidget(horizontal_line());

  QString serial = QString::fromStdString(params.get("HardwareSerial", false));
  device_layout->addWidget(new LabelControl("Serial", serial));

  // offroad-only buttons
  QList<ButtonControl*> offroad_btns;

  offroad_btns.append(new ButtonControl("ドライバーカメラ", "プレビュー",
                                        "ドライバ側のカメラをプレビューすることで取り付け位置を最適化し最高のドライバー監視を提供します。(車のエンジンはオフに)",
                                        [=]() { emit showDriverView(); }, "", this));

  QString resetCalibDesc = "openpilot requires the device to be mounted within 4° left or right and within 5° up or down. openpilot is continuously calibrating, resetting is rarely required.";
  ButtonControl *resetCalibBtn = new ButtonControl("キャリブレーション初期化", "リセット", resetCalibDesc, [=]() {
    if (ConfirmationDialog::confirm("本当にキャリブレーションをリセットしますか？", this)) {
      Params().remove("CalibrationParams");
    }
  }, "", this);
  connect(resetCalibBtn, &ButtonControl::showDescription, [=]() {
    QString desc = resetCalibDesc;
    std::string calib_bytes = Params().get("CalibrationParams");
    if (!calib_bytes.empty()) {
      try {
        AlignedBuffer aligned_buf;
        capnp::FlatArrayMessageReader cmsg(aligned_buf.align(calib_bytes.data(), calib_bytes.size()));
        auto calib = cmsg.getRoot<cereal::Event>().getLiveCalibration();
        if (calib.getCalStatus() != 0) {
          double pitch = calib.getRpyCalib()[1] * (180 / M_PI);
          double yaw = calib.getRpyCalib()[2] * (180 / M_PI);
          desc += QString(" Your device is pointed %1° %2 and %3° %4.")
                                .arg(QString::number(std::abs(pitch), 'g', 1), pitch > 0 ? "up" : "down",
                                     QString::number(std::abs(yaw), 'g', 1), yaw > 0 ? "right" : "left");
        }
      } catch (kj::Exception) {
        qInfo() << "invalid CalibrationParams";
      }
    }
    resetCalibBtn->setDescription(desc);
  });
  offroad_btns.append(resetCalibBtn);

  offroad_btns.append(new ButtonControl("Review Training Guide", "REVIEW",
                                        "Review the rules, features, and limitations of openpilot", [=]() {
    if (ConfirmationDialog::confirm("Are you sure you want to review the training guide?", this)) {
      Params().remove("CompletedTrainingVersion");
      emit reviewTrainingGuide();
    }
  }, "", this));

  QString brand = params.getBool("Passive") ? "dashcam" : "openpilot";
  offroad_btns.append(new ButtonControl("アンインストール " + brand, "アンインストール", "", [=]() {
    if (ConfirmationDialog::confirm("Are you sure you want to uninstall?", this)) {
      Params().putBool("DoUninstall", true);
    }
  }, "", this));

  for(auto &btn : offroad_btns){
    device_layout->addWidget(horizontal_line());
    QObject::connect(parent, SIGNAL(offroadTransition(bool)), btn, SLOT(setEnabled(bool)));
    device_layout->addWidget(btn);
  }

  // power buttons
  QHBoxLayout *power_layout = new QHBoxLayout();
  power_layout->setSpacing(30);

  QPushButton *reboot_btn = new QPushButton("Reboot");
  power_layout->addWidget(reboot_btn);
  QObject::connect(reboot_btn, &QPushButton::released, [=]() {
    if (ConfirmationDialog::confirm("Are you sure you want to reboot?", this)) {
      Hardware::reboot();
    }
  });

  QPushButton *poweroff_btn = new QPushButton("Power Off");
  poweroff_btn->setStyleSheet("background-color: #E22C2C;");
  power_layout->addWidget(poweroff_btn);
  QObject::connect(poweroff_btn, &QPushButton::released, [=]() {
    if (ConfirmationDialog::confirm("Are you sure you want to power off?", this)) {
      Hardware::poweroff();
    }
  });

  device_layout->addLayout(power_layout);

  setLayout(device_layout);
  setStyleSheet(R"(
    QPushButton {
      padding: 0;
      height: 120px;
      border-radius: 15px;
      background-color: #393939;
    }
  )");
}

SoftwarePanel::SoftwarePanel(QWidget* parent) : QFrame(parent) {
  QVBoxLayout *main_layout = new QVBoxLayout(this);
  setLayout(main_layout);
  setStyleSheet(R"(QLabel {font-size: 50px;})");

  fs_watch = new QFileSystemWatcher(this);
  QObject::connect(fs_watch, &QFileSystemWatcher::fileChanged, [=](const QString path) {
    int update_failed_count = Params().get<int>("UpdateFailedCount").value_or(0);
    if (path.contains("UpdateFailedCount") && update_failed_count > 0) {
      lastUpdateTimeLbl->setText("failed to fetch update");
      updateButton->setText("CHECK");
      updateButton->setEnabled(true);
    } else if (path.contains("LastUpdateTime")) {
      updateLabels();
    }
  });
}

void SoftwarePanel::showEvent(QShowEvent *event) {
  updateLabels();
}

void SoftwarePanel::updateLabels() {
  Params params = Params();
  std::string brand = params.getBool("Passive") ? "dashcam" : "openpilot";
  QList<QPair<QString, std::string>> dev_params = {
    {"Gitブランチ", params.get("GitBranch")},
    {"Gitコミット", params.get("GitCommit").substr(0, 10)},
    {"パンダファームウェア", params.get("PandaFirmwareHex")},
    {"OSバージョン", Hardware::get_os_version()},
  };

  QString version = QString::fromStdString(brand + " v" + params.get("Version").substr(0, 14)).trimmed();
  QString lastUpdateTime = "";

  std::string last_update_param = params.get("LastUpdateTime");
  if (!last_update_param.empty()){
    QDateTime lastUpdateDate = QDateTime::fromString(QString::fromStdString(last_update_param + "Z"), Qt::ISODate);
    lastUpdateTime = timeAgo(lastUpdateDate);
  }

  if (labels.size() < dev_params.size()) {
    versionLbl = new LabelControl("Version", version, QString::fromStdString(params.get("ReleaseNotes")).trimmed());
    layout()->addWidget(versionLbl);
    layout()->addWidget(horizontal_line());

    lastUpdateTimeLbl = new LabelControl("Last Update Check", lastUpdateTime, "The last time openpilot successfully checked for an update. The updater only runs while the car is off.");
    layout()->addWidget(lastUpdateTimeLbl);
    layout()->addWidget(horizontal_line());

    updateButton = new ButtonControl("Check for Update", "CHECK", "", [=]() {
      Params params = Params();
      if (params.getBool("IsOffroad")) {
        fs_watch->addPath(QString::fromStdString(params.getParamsPath()) + "/d/LastUpdateTime");
        fs_watch->addPath(QString::fromStdString(params.getParamsPath()) + "/d/UpdateFailedCount");
        updateButton->setText("CHECKING");
        updateButton->setEnabled(false);
      }
      std::system("pkill -1 -f selfdrive.updated");
    }, "", this);
    layout()->addWidget(updateButton);
    layout()->addWidget(horizontal_line());
  } else {
    versionLbl->setText(version);
    lastUpdateTimeLbl->setText(lastUpdateTime);
    updateButton->setText("CHECK");
    updateButton->setEnabled(true);
  }

  for (int i = 0; i < dev_params.size(); i++) {
    const auto &[name, value] = dev_params[i];
    QString val = QString::fromStdString(value).trimmed();
    if (labels.size() > i) {
      labels[i]->setText(val);
    } else {
      labels.push_back(new LabelControl(name, val));
      layout()->addWidget(labels[i]);
      if (i < (dev_params.size() - 1)) {
        layout()->addWidget(horizontal_line());
      }
    }
  }
}

QWidget * network_panel(QWidget * parent) {
#ifdef QCOM
  QVBoxLayout *layout = new QVBoxLayout;
  layout->setSpacing(30);

  // wifi + tethering buttons
  layout->addWidget(new ButtonControl("WiFi設定", "開く", "",
                                      [=]() { HardwareEon::launch_wifi(); }));
  layout->addWidget(horizontal_line());

  layout->addWidget(new ButtonControl("テザリング設定", "開く", "",
                                      [=]() { HardwareEon::launch_tethering(); }));
  layout->addWidget(horizontal_line());

  // SSH key management
  layout->addWidget(new SshToggle());
  layout->addWidget(horizontal_line());
  layout->addWidget(new SshControl());

  layout->addStretch(1);

  QWidget *w = new QWidget;
  w->setLayout(layout);
#else
  Networking *w = new Networking(parent);
#endif
  return w;
}

void SettingsWindow::showEvent(QShowEvent *event) {
  if (layout()) {
    panel_widget->setCurrentIndex(0);
    nav_btns->buttons()[0]->setChecked(true);
    return;
  }

  // setup two main layouts
  QVBoxLayout *sidebar_layout = new QVBoxLayout();
  sidebar_layout->setMargin(0);
  panel_widget = new QStackedWidget();
  panel_widget->setStyleSheet(R"(
    border-radius: 30px;
    background-color: #292929;
  )");

  // close button
  QPushButton *close_btn = new QPushButton("X");
  close_btn->setStyleSheet(R"(
    font-size: 90px;
    font-weight: bold;
    border 1px grey solid;
    border-radius: 100px;
    background-color: #292929;
  )");
  close_btn->setFixedSize(200, 200);
  sidebar_layout->addSpacing(45);
  sidebar_layout->addWidget(close_btn, 0, Qt::AlignCenter);
  QObject::connect(close_btn, &QPushButton::released, this, &SettingsWindow::closeSettings);

  // setup panels
  DevicePanel *device = new DevicePanel(this);
  QObject::connect(device, &DevicePanel::reviewTrainingGuide, this, &SettingsWindow::reviewTrainingGuide);
  QObject::connect(device, &DevicePanel::showDriverView, this, &SettingsWindow::showDriverView);

  QPair<QString, QWidget *> panels[] = {
    {"Device", device},
    {"ネット", network_panel(this)},
    {"Toggles", new TogglesPanel(this)},
    {"Software", new SoftwarePanel()},
  };

  sidebar_layout->addSpacing(45);
  nav_btns = new QButtonGroup();
  for (auto &[name, panel] : panels) {
    QPushButton *btn = new QPushButton(name);
    btn->setCheckable(true);
    btn->setChecked(nav_btns->buttons().size() == 0);
    btn->setStyleSheet(R"(
      QPushButton {
        color: grey;
        border: none;
        background: none;
        font-size: 65px;
        font-weight: 500;
        padding-top: 35px;
        padding-bottom: 35px;
      }
      QPushButton:checked {
        color: white;
      }
    )");

    nav_btns->addButton(btn);
    sidebar_layout->addWidget(btn, 0, Qt::AlignRight);

    panel->setContentsMargins(50, 25, 50, 25);

    ScrollView *panel_frame = new ScrollView(panel, this);
    panel_widget->addWidget(panel_frame);

    QObject::connect(btn, &QPushButton::released, [=, w = panel_frame]() {
      panel_widget->setCurrentWidget(w);
    });
  }
  sidebar_layout->setContentsMargins(50, 50, 100, 50);

  // main settings layout, sidebar + main panel
  QHBoxLayout *settings_layout = new QHBoxLayout();

  sidebar_widget = new QWidget;
  sidebar_widget->setLayout(sidebar_layout);
  sidebar_widget->setFixedWidth(500);
  settings_layout->addWidget(sidebar_widget);
  settings_layout->addWidget(panel_widget);

  setLayout(settings_layout);
  setStyleSheet(R"(
    * {
      color: white;
      font-size: 50px;
    }
    SettingsWindow {
      background-color: black;
    }
  )");
}

void SettingsWindow::hideEvent(QHideEvent *event){
#ifdef QCOM
  HardwareEon::close_activities();
#endif

  // TODO: this should be handled by the Dialog classes
  QList<QWidget*> children = findChildren<QWidget *>();
  for(auto &w : children){
    if(w->metaObject()->superClass()->className() == QString("QDialog")){
      w->close();
    }
  }
}
