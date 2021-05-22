#include "settings.h"

#include <cassert>
#include <iostream>
#include <sstream>
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

JvePilotTogglesPanel::JvePilotTogglesPanel(QWidget *parent) : QWidget(parent) {
  QVBoxLayout *toggles_list = new QVBoxLayout();

  QList<AbstractControl*> toggles;
  
  // slowInCurves
  QList<struct ConfigButton> slowInCurvesConfigs = { 
    { "jvePilot.settings.slowInCurves.speedRatio", 
      0.1, 2,
      "Speed Ratio",
      "Default: 1.0, Min: 0.1, Max: 2.0\n"
        "Use this to tune the speed in curves to your liking."
        "\nFor example, 1.2 to go 20% faster and .8 to go 20% slower across all curvatures."
    }
    ,{ "jvePilot.settings.slowInCurves.speedDropOff",
      1, 3,
      "Speed Drop Off",
      "Default: 2.0\n"
        "Experimental. Lower this value to decrease how quickly speed drops as the curve increases."
        "\nTo go faster in turns at higher speeds, decrease this value.  To compensate for this change, you may need to increase the Speed Ratio."
    }
  };
  toggles.append(new ParamControl("jvePilot.settings.slowInCurves",
                                  "Slow in Curves",
                                  "jvePilot will slow in curves so that you don't have to.",
                                  "../assets/jvepilot/settings/icon_slow_in_curves.png",
                                  this,
                                  &slowInCurvesConfigs));
  // autoFollow
  QList<struct ConfigButton> autoFollowConfigs = {
    { "jvePilot.settings.autoFollow.speed1-2Bars",
      0, 300,
      "1-2 Bar Change Over",
      "Default: 15 mph, Min: 0, Max: 300\n"
        "Use this to change the speed at which Auto Follow will switch between one to two bars."
    },
    { "jvePilot.settings.autoFollow.speed2-3Bars",
      0, 300,
      "2-3 Bar Change Over",
      "Default: 30 mph, Min: 0, Max: 300\n"
        "Use this to change the speed at which Auto Follow will switch between two to three bars."
    },
    { "jvePilot.settings.autoFollow.speed3-4Bars",
      0, 300,
      "3-4 Bar Change Over",
      "Default: 65 mph, Min: 0, Max: 300\n"
        "Use this to change the speed at which Auto Follow will switch between three to four bars."
    }
  };
  toggles.append(new ParamControl("jvePilot.settings.autoFollow",
                                  "Start with Auto Follow Enabled",
                                  "When enabled, jvePilot will enable Auto Follow on the start of every drive.",
                                  "../assets/jvepilot/settings/icon_auto_follow.png",
                                  this,
                                  &autoFollowConfigs));

  // reverseAccSpeedChange
  toggles.append(new ParamControl("jvePilot.settings.reverseAccSpeedChange",
                                  "Reverse ACC +/- Speeds",
                                  "When enabled, quick pressing the ACC +/- buttons changes the speed in 5 mph increments."
                                  " Hold a little longer to change by 1 mph."
                                  " Disable to keep stock setting.",
                                  "../assets/jvepilot/settings/icon_acc_speed_change.png",
                                  this));

  // autoResume
  toggles.append(new ParamControl("jvePilot.settings.autoResume",
                                  "Auto Resume",
                                  "When enabled, jvePilot will resume after ACC comes to a stop behind another vehicle.",
                                  "../assets/jvepilot/settings/icon_auto_resume.png",
                                  this));

  // disableOnGas
  toggles.append(new ParamControl("jvePilot.settings.disableOnGas",
                                  "Disable on Gas",
                                  "When enabled, jvePilot will disengage jvePilot when the gas pedal is pressed.",
                                  "../assets/jvepilot/settings/icon_gas_pedal.png",
                                  this));

  // accEco
  QList<struct ConfigButton> ecoConfigs = {
    { "jvePilot.settings.accEco.speedAheadLevel1",
      1, 100,
      "Keep ahead at ACC Eco level 1",
      "Default: 7 mph, Min: 1, Max: 100\n"
        "The higher the number the more acceleration that occurs."
    },
    { "jvePilot.settings.accEco.speedAheadLevel2",
      1, 100,
      "Keep ahead at ACC Eco level 2",
      "Default: 5 mph, Min: 1, Max: 100\n"
        "The higher the number the more acceleration that occurs."
    }
  };
  toggles.append(new LabelControl("ACC Eco",
                                  "",
                                  "Use these settings to tune how much acceleration occurs by limiting how much ACC is set above your current speed.",
                                  this,
                                  "../assets/jvepilot/settings/icon_acc_eco.png",
                                  &ecoConfigs));

  // misc
  QList<struct ConfigButton> miscConfigs = {
    { "jvePilot.settings.deviceOffset",
      -2, 2,
      "Device Offset",
      "Default: 0.00 meters, Min: -2.00, Max: 2.00\n"
        "Compensate for mounting your device off-center in the windshield."
        "\nFor example, 0.04 if your device is 4cm left of center."
        "\nNOTE: This is not how far the CAMERA is off-center, but how far the MOUNT/DEVICE is off-center."
    },
    { "jvePilot.settings.speedAdjustRatio",
      0.9, 1.1,
      "Speed Adjust Ratio",
      "Default: 1.0, Min: 0.9, Max: 1.1\n"
        "jvePilot can report an incorrect speed compared to your vehicle or the real world."
        " Apps like Waze report you current speed using GPS which is more accurate than jvePilot or your speedometer may report."
        " Use this setting to get the speed reported by jvePilot just right."
        "\nFor example, set to 1.052 if you see 76 mph on the jvePilot display, but it should be 80 mph."
    },
    { "jvePilot.settings.accFollow1RadarRatio",
      0.5, 4,
      "Ratio at Follow Level 1",
      "Default: 2.6, Min: 0.5, Max: 4.0\n"
        "At follow level 1, apply this ratio to the radar distance."
    },
    { "jvePilot.settings.accFollow2RadarRatio",
      0.5, 4,
      "Ratio at Follow Level 2",
      "Default: 2.1, Min: 0.5, Max: 4.0\n"
        "At follow level 2, apply this ratio to the radar distance."
    },
    { "jvePilot.settings.accFollow3RadarRatio",
      0.5, 4,
      "Ratio at Follow Level 3",
      "Default: 1.5, Min: 0.5, Max: 4.0\n"
        "At follow level 3, apply this ratio to the radar distance."
    },
    { "jvePilot.settings.accFollow4RadarRatio",
      0.5, 4,
      "Ratio at Follow Level 4",
      "Default: 1.1, Min: 0.5, Max: 4.0\n"
        "At follow level 4, apply this ratio to the radar distance."
    }
  };
  toggles.append(new LabelControl("jvePilot Control Settings",
                                  "",
                                  "Use these settings tune some of jvePilot's control settings.",
                                  this,
                                  "../assets/jvepilot/settings/icon_misc.png",
                                  &miscConfigs));


  for(AbstractControl *toggle : toggles){
    if(toggles_list->count() != 0){
      toggles_list->addWidget(horizontal_line());
    }
    toggles_list->addWidget(toggle);
  }

  setLayout(toggles_list);
}

TogglesPanel::TogglesPanel(QWidget *parent) : QWidget(parent) {
  QVBoxLayout *toggles_list = new QVBoxLayout();

  QList<ParamControl*> toggles;

  toggles.append(new ParamControl("OpenpilotEnabledToggle",
                                  "Enable openpilot",
                                  "Use the openpilot system for adaptive cruise control and lane keep driver assistance. Your attention is required at all times to use this feature. Changing this setting takes effect when the car is powered off.",
                                  "../assets/offroad/icon_openpilot.png",
                                  this));
  toggles.append(new ParamControl("IsLdwEnabled",
                                  "Enable Lane Departure Warnings",
                                  "Receive alerts to steer back into the lane when your vehicle drifts over a detected lane line without a turn signal activated while driving over 31mph (50kph).",
                                  "../assets/offroad/icon_warning.png",
                                  this));
  toggles.append(new ParamControl("IsRHD",
                                  "Enable Right-Hand Drive",
                                  "Allow openpilot to obey left-hand traffic conventions and perform driver monitoring on right driver seat.",
                                  "../assets/offroad/icon_openpilot_mirrored.png",
                                  this));
  toggles.append(new ParamControl("IsMetric",
                                  "Use Metric System",
                                  "Display speed in km/h instead of mp/h.",
                                  "../assets/offroad/icon_metric.png",
                                  this));
  toggles.append(new ParamControl("CommunityFeaturesToggle",
                                  "Enable Community Features",
                                  "Use features from the open source community that are not maintained or supported by comma.ai and have not been confirmed to meet the standard safety model. These features include community supported cars and community supported hardware. Be extra cautious when using these features",
                                  "../assets/offroad/icon_shell.png",
                                  this));

  if (!Hardware::TICI()) {
    toggles.append(new ParamControl("IsUploadRawEnabled",
                                    "Upload Raw Logs",
                                    "Upload full logs and full resolution video by default while on WiFi. If not enabled, individual logs can be marked for upload at my.comma.ai/useradmin.",
                                    "../assets/offroad/icon_network.png",
                                    this));
  }

  ParamControl *record_toggle = new ParamControl("RecordFront",
                                                 "Record and Upload Driver Camera",
                                                "Upload data from the driver facing camera and help improve the driver monitoring algorithm.",
                                                "../assets/offroad/icon_monitoring.png",
                                                this);
  toggles.append(record_toggle);
  toggles.append(new ParamControl("EndToEndToggle",
                                   "\U0001f96c Disable use of lanelines (Alpha) \U0001f96c",
                                   "In this mode openpilot will ignore lanelines and just drive how it thinks a human would.",
                                   "../assets/offroad/icon_road.png",
                                   this));

  if (Hardware::TICI()) {
    toggles.append(new ParamControl("EnableWideCamera",
                                    "Enable use of Wide Angle Camera",
                                    "Use wide angle camera for driving and ui. Only takes effect after reboot.",
                                    "../assets/offroad/icon_openpilot.png",
                                    this));
    toggles.append(new ParamControl("EnableLteOnroad",
                                    "Enable LTE while onroad",
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

  offroad_btns.append(new ButtonControl("Driver Camera", "PREVIEW",
                                        "Preview the driver facing camera to help optimize device mounting position for best driver monitoring experience. (vehicle must be off)",
                                        [=]() {
                                           Params().putBool("IsDriverViewEnabled", true);
                                           QUIState::ui_state.scene.driver_view = true;
                                        }, "", this));

  QString resetCalibDesc = "openpilot requires the device to be mounted within 4° left or right and within 5° up or down. openpilot is continuously calibrating, resetting is rarely required.";
  ButtonControl *resetCalibBtn = new ButtonControl("Reset Calibration", "RESET", resetCalibDesc, [=]() {
    if (ConfirmationDialog::confirm("Are you sure you want to reset calibration?", this)) {
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
  offroad_btns.append(new ButtonControl("Uninstall " + brand, "UNINSTALL", "", [=]() {
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

DeveloperPanel::DeveloperPanel(QWidget* parent) : QFrame(parent) {
  QVBoxLayout *main_layout = new QVBoxLayout(this);
  setLayout(main_layout);
  setStyleSheet(R"(QLabel {font-size: 50px;})");
}

void DeveloperPanel::showEvent(QShowEvent *event) {
  Params params = Params();
  std::string brand = params.getBool("Passive") ? "dashcam" : "openpilot";
  QList<QPair<QString, std::string>> dev_params = {
    {"Version", brand + " v" + params.get("Version", false).substr(0, 14)},
    {"Git Branch", params.get("GitBranch", false)},
    {"Git Commit", params.get("GitCommit", false).substr(0, 10)},
    {"Panda Firmware", params.get("PandaFirmwareHex", false)},
    {"OS Version", Hardware::get_os_version()},
  };

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
  layout->addWidget(new ButtonControl("WiFi Settings", "OPEN", "",
                                      [=]() { HardwareEon::launch_wifi(); }));
  layout->addWidget(horizontal_line());

  layout->addWidget(new ButtonControl("Tethering Settings", "OPEN", "",
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
    border-radius: 85px;
    background-color: #292929;
  )");
  close_btn->setFixedSize(170, 170);
  sidebar_layout->addSpacing(45);
  sidebar_layout->addWidget(close_btn, 0, Qt::AlignCenter);
  QObject::connect(close_btn, &QPushButton::released, this, &SettingsWindow::closeSettings);

  // setup panels
  DevicePanel *device = new DevicePanel(this);
  QObject::connect(device, &DevicePanel::reviewTrainingGuide, this, &SettingsWindow::reviewTrainingGuide);

  QPair<QString, QWidget *> panels[] = {
    {"Device", device},
    {"Network", network_panel(this)},
    {"Toggles", new TogglesPanel(this)},
    {"jvePilot", new JvePilotTogglesPanel(this)},
    {"Developer", new DeveloperPanel()},
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
        font-size: 60px;
        font-weight: 500;
        padding-top: 25px;
        padding-bottom: 25px;
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