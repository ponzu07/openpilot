import math


class CurveSpeedController:
  """Compute a comfortable acceleration ceiling for model-predicted curves."""

  MAX_LATERAL_ACCEL = 1.8
  MIN_CURVATURE = 1e-4
  MIN_DISTANCE = 5.0
  MAX_DISTANCE = 100.0
  MIN_ACCEL = -1.2
  JERK_LIMIT = 0.6

  def __init__(self, dt: float):
    self.dt = dt
    self.accel = 0.0

  def reset(self) -> None:
    self.accel = 0.0

  def update(self, enabled: bool, v_ego: float, distances: list[float], yaws: list[float]) -> float:
    if not enabled:
      self.reset()
      return math.inf

    target_accel = 0.0
    for distance, yaw in zip(distances, yaws, strict=False):
      if not self.MIN_DISTANCE <= distance <= self.MAX_DISTANCE:
        continue

      # For a constant-radius arc, heading change is curvature * distance.
      curvature = abs(yaw) / distance
      if curvature < self.MIN_CURVATURE:
        continue

      target_speed = math.sqrt(self.MAX_LATERAL_ACCEL / curvature)
      if target_speed < v_ego:
        accel = (target_speed ** 2 - v_ego ** 2) / (2.0 * distance)
        target_accel = min(target_accel, max(accel, self.MIN_ACCEL))

    max_delta = self.JERK_LIMIT * self.dt
    self.accel = min(max(target_accel, self.accel - max_delta), self.accel + max_delta)
    return self.accel if self.accel < 0.0 else math.inf
