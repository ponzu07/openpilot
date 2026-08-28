import unittest

from openpilot.selfdrive.controls.lib.curve_speed_controller import CurveSpeedController


class TestCurveSpeedController(unittest.TestCase):
  def setUp(self):
    self.controller = CurveSpeedController(0.05)

  def test_disabled_is_inactive_and_resets(self):
    self.controller.update(True, 25.0, [50.0], [0.5])
    self.assertEqual(self.controller.update(False, 25.0, [50.0], [0.5]), float("inf"))
    self.assertEqual(self.controller.accel, 0.0)

  def test_straight_road_does_not_decelerate(self):
    self.assertEqual(self.controller.update(True, 25.0, [10.0, 50.0], [0.0, 0.0]), float("inf"))

  def test_curve_requests_deceleration(self):
    self.assertLess(self.controller.update(True, 25.0, [50.0], [0.5]), 0.0)

  def test_deceleration_is_jerk_limited(self):
    self.assertAlmostEqual(self.controller.update(True, 25.0, [50.0], [0.5]), -0.03)
    self.assertAlmostEqual(self.controller.update(True, 25.0, [50.0], [0.5]), -0.06)

  def test_ignores_points_outside_lookahead(self):
    self.assertEqual(self.controller.update(True, 25.0, [2.0, 120.0], [1.0, 1.0]), float("inf"))


if __name__ == "__main__":
  unittest.main()
