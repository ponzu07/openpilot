from openpilot.system.ui.widgets.scroller import NavScroller
from openpilot.system.ui.lib.application import gui_app, FontWeight
from openpilot.system.ui.lib.multilang import multilang, tr_noop
from openpilot.selfdrive.ui.mici.widgets.button import BigButton


class LanguageLayoutMici(NavScroller):
  def __init__(self):
    super().__init__()

    buttons = []
    for name, code in multilang.languages.items():
      btn = BigButton(name, tr_noop("selected") if code == multilang.language else "")
      btn._label.set_font_weight(FontWeight.UNIFONT)
      btn.set_click_callback(lambda c=code: self._select(c))
      buttons.append(btn)

    self._scroller.add_widgets(buttons)

  def _select(self, code: str):
    multilang.change_language(code)
    self.dismiss()
