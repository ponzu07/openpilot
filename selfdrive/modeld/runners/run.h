#ifndef RUN_H
#define RUN_H

#include "runmodel.h"
#include "snpemodel.h"

#ifdef QCOM
  #define DefaultRunModel SNPEModel
#else
<<<<<<< HEAD
  #ifdef USE_TF_MODEL
    #include "tfmodel.h"
    #define DefaultRunModel TFModel
=======
  #ifdef USE_ONNX_MODEL
    #include "onnxmodel.h"
    #define DefaultRunModel ONNXModel
>>>>>>> origin/ci-clean
  #else
    #define DefaultRunModel SNPEModel
  #endif
#endif

#endif
