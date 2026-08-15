from collections.abc import Callable

from openpilot.cereal import log

from openpilot.system.ui.widgets.scroller import NavScroller
from openpilot.selfdrive.ui.mici.widgets.button import BigParamControl, BigMultiParamToggle, BigToggle, GreyBigButton, BigButton
from openpilot.selfdrive.ui.mici.widgets.dialog import BigConfirmationCircleButton
from openpilot.selfdrive.ui.mici.layouts.settings.language import LanguageLayoutMici
from openpilot.system.ui.lib.application import gui_app
from openpilot.selfdrive.ui.layouts.settings.common import restart_needed_callback
from openpilot.selfdrive.ui.ui_state import ui_state
from openpilot.system.ui.lib.multilang import tr, tr_noop

PERSONALITY_TO_INT = log.LongitudinalPersonality.schema.enumerants


class ExperimentalModeConfirmPage(NavScroller):
  def __init__(self, on_confirm: Callable[[], None]):
    super().__init__()

    accept = BigConfirmationCircleButton(lambda: tr("enable\nexperimental mode"),
                                         gui_app.texture("icons_mici/setup/driver_monitoring/dm_check.png", 64, 64),
                                         lambda: self.dismiss(on_confirm))

    self._scroller.add_widgets([
      GreyBigButton(lambda: tr("enabling\nexperimental mode"), tr_noop("scroll to continue"),
                    gui_app.texture("icons_mici/setup/warning.png", 64, 64)),
      GreyBigButton("", tr_noop("openpilot defaults to driving in chill mode.")),
      GreyBigButton("", tr_noop("Experimental mode enables alpha-level features that aren't ready for chill mode.")),
      GreyBigButton(lambda: tr("End-to-End Longitudinal Control")),
      GreyBigButton("", tr_noop("Let the driving model control the gas and brakes.")),
      GreyBigButton("", tr_noop("openpilot will drive as it thinks a human would, including stopping for red lights and stop signs.")),
      GreyBigButton("", tr_noop("The set speed will only act as an upper bound.")),
      GreyBigButton("", tr_noop("This is an alpha quality feature; mistakes should be expected.")),
      GreyBigButton(lambda: tr("New Driving Visualization")),
      GreyBigButton("", tr_noop("The path will change colors to communicate acceleration intent.")),
      GreyBigButton("", tr_noop("Red for braking, green for acceleration, and gray for coasting.")),
      accept,
    ])


class TogglesLayoutMici(NavScroller):
  def __init__(self):
    super().__init__()

    self._personality_toggle = BigMultiParamToggle(lambda: tr("driving personality"), "LongitudinalPersonality", [tr_noop("aggressive"), tr_noop("standard"), tr_noop("relaxed")])
    self._experimental_btn = BigToggle(lambda: tr("experimental mode"), initial_state=ui_state.params.get_bool("ExperimentalMode"),
                                       toggle_callback=self._on_experimental_mode)
    is_metric_toggle = BigParamControl(lambda: tr("use metric units"), "IsMetric")
    ldw_toggle = BigParamControl(lambda: tr("lane departure warnings"), "IsLdwEnabled")
    always_on_dm_toggle = BigParamControl(lambda: tr("always-on driver monitor"), "AlwaysOnDM")
    record_front = BigParamControl(lambda: tr("record & upload cabin camera"), "RecordFront", toggle_callback=restart_needed_callback)
    record_mic = BigParamControl(lambda: tr("record & upload mic audio"), "RecordAudio", toggle_callback=restart_needed_callback)
    enable_openpilot = BigParamControl(lambda: tr("enable openpilot"), "OpenpilotEnabledToggle", toggle_callback=restart_needed_callback)

    language_btn = BigButton(lambda: tr("language"), "", gui_app.texture("icons_mici/settings/device/info.png", 64, 64))
    language_btn.set_click_callback(lambda: gui_app.push_widget(LanguageLayoutMici()))

    self._scroller.add_widgets([
      self._personality_toggle,
      self._experimental_btn,
      is_metric_toggle,
      ldw_toggle,
      always_on_dm_toggle,
      record_front,
      record_mic,
      enable_openpilot,
      language_btn,
    ])

    # Toggle lists
    self._refresh_toggles = (
      ("ExperimentalMode", self._experimental_btn),
      ("IsMetric", is_metric_toggle),
      ("IsLdwEnabled", ldw_toggle),
      ("AlwaysOnDM", always_on_dm_toggle),
      ("RecordFront", record_front),
      ("RecordAudio", record_mic),
      ("OpenpilotEnabledToggle", enable_openpilot),
    )

    enable_openpilot.set_enabled(lambda: not ui_state.engaged)
    record_front.set_enabled(False if ui_state.params.get_bool("RecordFrontLock") else (lambda: not ui_state.engaged))
    record_mic.set_enabled(lambda: not ui_state.engaged)

    if ui_state.params.get_bool("ShowDebugInfo"):
      gui_app.set_show_touches(True)
      gui_app.set_show_fps(True)

    ui_state.add_engaged_transition_callback(self._update_toggles)

  def _update_state(self):
    super()._update_state()

    if ui_state.sm.updated["selfdriveState"]:
      personality = PERSONALITY_TO_INT[ui_state.sm["selfdriveState"].personality]
      if personality != ui_state.personality and ui_state.started:
        self._personality_toggle.set_value(self._personality_toggle._options[personality])
      ui_state.personality = personality

  def show_event(self):
    super().show_event()
    self._update_toggles()

  def _update_toggles(self):
    ui_state.update_params()

    # CP gating for experimental mode
    if ui_state.CP is not None:
      if ui_state.has_longitudinal_control:
        self._experimental_btn.set_visible(True)
        self._personality_toggle.set_visible(True)
      else:
        # no long for now
        self._experimental_btn.set_visible(False)
        self._experimental_btn.set_checked(False)
        self._personality_toggle.set_visible(False)
        ui_state.params.remove("ExperimentalMode")

    # Refresh toggles from params to mirror external changes
    for key, item in self._refresh_toggles:
      item.set_checked(ui_state.params.get_bool(key))

  def _on_experimental_mode(self, state: bool):
    if state and not ui_state.params.get_bool("ExperimentalModeConfirmed"):
      # Don't show enabled state until confirm
      self._experimental_btn.set_checked(False)

      def on_confirm():
        ui_state.params.put_bool("ExperimentalModeConfirmed", True)
        ui_state.params.put_bool("ExperimentalMode", True)
        self._experimental_btn.set_checked(True)

      gui_app.push_widget(ExperimentalModeConfirmPage(on_confirm))
    else:
      ui_state.params.put_bool("ExperimentalMode", state)
