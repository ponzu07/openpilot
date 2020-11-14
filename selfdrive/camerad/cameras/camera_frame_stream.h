<<<<<<< HEAD
#ifndef CAMERA_FRAME_STREAM_H
#define CAMERA_FRAME_STREAM_H
=======
#pragma once
>>>>>>> origin/ci-clean

#include <stdbool.h>

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

<<<<<<< HEAD
#include "common/mat.h"

#include "buffering.h"
#include "common/visionbuf.h"
=======
>>>>>>> origin/ci-clean
#include "camera_common.h"

#define FRAME_BUF_COUNT 16

<<<<<<< HEAD
#ifdef __cplusplus
extern "C" {
#endif

typedef struct CameraState {
  int camera_id;
  CameraInfo ci;
  int frame_size;

  VisionBuf *camera_bufs;
  FrameMetadata camera_bufs_metadata[FRAME_BUF_COUNT];
  TBuffer camera_tb;

  int fps;
  float digital_gain;

  float cur_gain_frac;

  mat3 transform;
} CameraState;

=======
typedef struct CameraState {
  int camera_id;
  CameraInfo ci;

  int fps;
  float digital_gain;
  float cur_gain_frac;

  mat3 transform;

  CameraBuf buf;
} CameraState;
>>>>>>> origin/ci-clean

typedef struct MultiCameraState {
  int ispif_fd;

  CameraState rear;
  CameraState front;
<<<<<<< HEAD
} MultiCameraState;

void cameras_init(MultiCameraState *s);
void cameras_open(MultiCameraState *s, VisionBuf *camera_bufs_rear, VisionBuf *camera_bufs_focus, VisionBuf *camera_bufs_stats, VisionBuf *camera_bufs_front);
void cameras_run(MultiCameraState *s);
void cameras_close(MultiCameraState *s);
void camera_autoexposure(CameraState *s, float grey_frac);
#ifdef __cplusplus
}  // extern "C"
#endif

#endif
=======

  SubMaster *sm;
  PubMaster *pm;
} MultiCameraState;

void cameras_init(MultiCameraState *s, cl_device_id device_id, cl_context ctx);
void cameras_open(MultiCameraState *s);
void cameras_run(MultiCameraState *s);
void cameras_close(MultiCameraState *s);
void camera_autoexposure(CameraState *s, float grey_frac);
>>>>>>> origin/ci-clean
