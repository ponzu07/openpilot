import os
BASEDIR = os.path.abspath(os.path.join(os.path.dirname(os.path.realpath(__file__)), "../"))

from common.hardware import PC
if PC:
  PERSIST = os.path.join(BASEDIR, "persist")
<<<<<<< HEAD
  PARAMS = os.path.join(BASEDIR, "persist", "params")
else:
  PERSIST = "/persist"
  PARAMS = "/data/params"
=======
else:
  PERSIST = "/persist"
>>>>>>> origin/ci-clean
