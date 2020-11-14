#include "camera_frame_stream.h"

#include <unistd.h>
#include <cassert>
#include <string.h>
#include <signal.h>

#include <libyuv.h>
#include "messaging.hpp"

#include "common/util.h"
#include "common/timing.h"
#include "common/swaglog.h"
#include "buffering.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

extern volatile sig_atomic_t do_exit;

#define FRAME_WIDTH 1164
#define FRAME_HEIGHT 874

namespace {
<<<<<<< HEAD
void camera_open(CameraState *s, VisionBuf *camera_bufs, bool rear) {
  assert(camera_bufs);
  s->camera_bufs = camera_bufs;
}

void camera_close(CameraState *s) {
  tbuffer_stop(&s->camera_tb);
}

void camera_release_buffer(void *cookie, int buf_idx) {}

void camera_init(CameraState *s, int camera_id, unsigned int fps) {
=======
void camera_open(CameraState *s, bool rear) {
}

void camera_close(CameraState *s) {
  s->buf.stop();
}

void camera_init(CameraState *s, int camera_id, unsigned int fps, cl_device_id device_id, cl_context ctx) {
>>>>>>> origin/ci-clean
  assert(camera_id < ARRAYSIZE(cameras_supported));
  s->ci = cameras_supported[camera_id];
  assert(s->ci.frame_width != 0);

<<<<<<< HEAD
  s->frame_size = s->ci.frame_height * s->ci.frame_stride;
  s->fps = fps;

  tbuffer_init2(&s->camera_tb, FRAME_BUF_COUNT, "frame", camera_release_buffer, s);
}

void run_frame_stream(MultiCameraState *s) {
  SubMaster sm({"frame"});

  CameraState *const rear_camera = &s->rear;
  auto *tb = &rear_camera->camera_tb;

  while (!do_exit) {
    if (sm.update(1000) == 0) continue;

    auto frame = sm["frame"].getFrame();

    const int buf_idx = tbuffer_select(tb);
    rear_camera->camera_bufs_metadata[buf_idx] = {
=======
  s->fps = fps;
  s->buf.init(device_id, ctx, s, FRAME_BUF_COUNT, "camera");
}

void run_frame_stream(MultiCameraState *s) {
  s->sm = new SubMaster({"frame"});

  CameraState *const rear_camera = &s->rear;
  auto *tb = &rear_camera->buf.camera_tb;

  while (!do_exit) {
    if (s->sm->update(1000) == 0) continue;

    auto frame = (*(s->sm))["frame"].getFrame();

    const int buf_idx = tbuffer_select(tb);
    rear_camera->buf.camera_bufs_metadata[buf_idx] = {
>>>>>>> origin/ci-clean
      .frame_id = frame.getFrameId(),
      .timestamp_eof = frame.getTimestampEof(),
      .frame_length = static_cast<unsigned>(frame.getFrameLength()),
      .integ_lines = static_cast<unsigned>(frame.getIntegLines()),
      .global_gain = static_cast<unsigned>(frame.getGlobalGain()),
    };

<<<<<<< HEAD
    cl_command_queue q = rear_camera->camera_bufs[buf_idx].copy_q;
    cl_mem yuv_cl = rear_camera->camera_bufs[buf_idx].buf_cl;
=======
    cl_command_queue q = rear_camera->buf.camera_bufs[buf_idx].copy_q;
    cl_mem yuv_cl = rear_camera->buf.camera_bufs[buf_idx].buf_cl;
>>>>>>> origin/ci-clean

    clEnqueueWriteBuffer(q, yuv_cl, CL_TRUE, 0, frame.getImage().size(), frame.getImage().begin(), 0, NULL, NULL);
    tbuffer_dispatch(tb, buf_idx);
  }
}

}  // namespace

CameraInfo cameras_supported[CAMERA_ID_MAX] = {
  [CAMERA_ID_IMX298] = {
    .frame_width = FRAME_WIDTH,
    .frame_height = FRAME_HEIGHT,
    .frame_stride = FRAME_WIDTH*3,
    .bayer = false,
    .bayer_flip = false,
  },
  [CAMERA_ID_OV8865] = {
    .frame_width = 1632,
    .frame_height = 1224,
    .frame_stride = 2040, // seems right
    .bayer = true,
    .bayer_flip = 3,
    .hdr = false
  },
};

<<<<<<< HEAD
void cameras_init(MultiCameraState *s) {
  camera_init(&s->rear, CAMERA_ID_IMX298, 20);
=======
void cameras_init(MultiCameraState *s, cl_device_id device_id, cl_context ctx) {
  camera_init(&s->rear, CAMERA_ID_IMX298, 20, device_id, ctx);
>>>>>>> origin/ci-clean
  s->rear.transform = (mat3){{
    1.0,  0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0,  0.0, 1.0,
  }};

<<<<<<< HEAD
  camera_init(&s->front, CAMERA_ID_OV8865, 10);
=======
  camera_init(&s->front, CAMERA_ID_OV8865, 10, device_id, ctx);
>>>>>>> origin/ci-clean
  s->front.transform = (mat3){{
    1.0,  0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0,  0.0, 1.0,
  }};
}

void camera_autoexposure(CameraState *s, float grey_frac) {}

<<<<<<< HEAD
void cameras_open(MultiCameraState *s, VisionBuf *camera_bufs_rear,
                  VisionBuf *camera_bufs_focus, VisionBuf *camera_bufs_stats,
                  VisionBuf *camera_bufs_front) {
  assert(camera_bufs_rear);
  assert(camera_bufs_front);

  // LOG("*** open front ***");
  camera_open(&s->front, camera_bufs_front, false);

  // LOG("*** open rear ***");
  camera_open(&s->rear, camera_bufs_rear, true);
=======
void cameras_open(MultiCameraState *s) {
  // LOG("*** open front ***");
  camera_open(&s->front, false);

  // LOG("*** open rear ***");
  camera_open(&s->rear, true);
>>>>>>> origin/ci-clean
}

void cameras_close(MultiCameraState *s) {
  camera_close(&s->rear);
}

<<<<<<< HEAD
void cameras_run(MultiCameraState *s) {
  set_thread_name("frame_streaming");
  run_frame_stream(s);
  cameras_close(s);
=======
// called by processing_thread
void camera_process_rear(MultiCameraState *s, CameraState *c, int cnt) {
  // empty
}

void cameras_run(MultiCameraState *s) {
  std::thread t = start_process_thread(s, "processing", &s->rear, 51, camera_process_rear);
  set_thread_name("frame_streaming");
  run_frame_stream(s);
  cameras_close(s);
  t.join();
>>>>>>> origin/ci-clean
}
