class CustomLongitudinalTuning:
  """Low-speed acceleration request smoothing for longitudinal control."""

  MAX_SPEED = 5.0 / 3.6
  BRAKE_JERK_V = (0.35, 0.8)
  RELEASE_JERK = 0.6

  def __init__(self, dt: float):
    self.dt = dt

  def smooth_accel(self, accel: float, last_accel: float, v_ego: float) -> float:
    if v_ego >= self.MAX_SPEED:
      return accel

    speed_ratio = min(max(v_ego, 0.0) / self.MAX_SPEED, 1.0)
    brake_jerk = self.BRAKE_JERK_V[0] + speed_ratio * (self.BRAKE_JERK_V[1] - self.BRAKE_JERK_V[0])
    lower = last_accel - brake_jerk * self.dt
    upper = last_accel + self.RELEASE_JERK * self.dt
    return min(max(accel, lower), upper)
