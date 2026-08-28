import unittest

from openpilot.selfdrive.controls.lib.custom_longitudinal_tuning import CustomLongitudinalTuning


class TestCustomLongitudinalTuning(unittest.TestCase):
  def setUp(self):
    self.tuning = CustomLongitudinalTuning(0.01)

  def test_does_not_change_accel_above_five_kph(self):
    self.assertEqual(self.tuning.smooth_accel(-2.0, -0.5, 5.1 / 3.6), -2.0)

  def test_limits_brake_application_at_standstill(self):
    self.assertAlmostEqual(self.tuning.smooth_accel(-1.0, -0.5, 0.0), -0.5035)

  def test_limits_brake_release(self):
    self.assertAlmostEqual(self.tuning.smooth_accel(0.0, -0.5, 0.0), -0.494)

  def test_brake_application_rate_increases_with_speed(self):
    stopped = self.tuning.smooth_accel(-1.0, -0.5, 0.0)
    moving = self.tuning.smooth_accel(-1.0, -0.5, 5.0 / 3.6 - 1e-3)
    self.assertLess(moving, stopped)


if __name__ == "__main__":
  unittest.main()
