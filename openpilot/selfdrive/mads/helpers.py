"""
Copyright (c) 2021-, Haibin Wen, sunnypilot, and a number of other contributors.

This file is part of sunnypilot and is licensed under the MIT License.
See the LICENSE.md file in the root directory for more details.
"""

from opendbc.car import structs
from opendbc.safety import ALTERNATIVE_EXPERIENCE
from openpilot.common.params import Params


class MadsSteeringModeOnBrake:
  REMAIN_ACTIVE = 0
  PAUSE = 1
  DISENGAGE = 2


def set_alternative_experience(CP: structs.CarParams, params: Params) -> None:
  if not params.get_bool("Mads"):
    return

  CP.alternativeExperience |= ALTERNATIVE_EXPERIENCE.ENABLE_MADS

  steering_mode = params.get("MadsSteeringMode", return_default=True)
  if steering_mode == MadsSteeringModeOnBrake.DISENGAGE:
    CP.alternativeExperience |= ALTERNATIVE_EXPERIENCE.MADS_DISENGAGE_LATERAL_ON_BRAKE
  elif steering_mode == MadsSteeringModeOnBrake.PAUSE:
    CP.alternativeExperience |= ALTERNATIVE_EXPERIENCE.MADS_PAUSE_LATERAL_ON_BRAKE
