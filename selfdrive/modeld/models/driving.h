#ifndef MODEL_H
#define MODEL_H

// gate this here
#define TEMPORAL
#define DESIRE
#define TRAFFIC_CONVENTION


#include "common/mat.h"
#include "common/util.h"
#include "common/modeldata.h"

#include "commonmodel.h"
#include "runners/run.h"

<<<<<<< HEAD
#include <czmq.h>
#include <memory>
#include "messaging.hpp"

#define MODEL_WIDTH 512
#define MODEL_HEIGHT 256
#define MODEL_FRAME_SIZE MODEL_WIDTH * MODEL_HEIGHT * 3 / 2
#define MODEL_NAME "supercombo_dlc"

#define DESIRE_LEN 8
#define TRAFFIC_CONVENTION_LEN 2
#define LEAD_MDN_N 5 // probs for 5 groups
#define MDN_VALS 4 // output xyva for each lead group
#define SELECTION 3 //output 3 group (lead now, in 2s and 6s)
#define MDN_GROUP_SIZE 11
#define TIME_DISTANCE 100
=======
#include <memory>
#include "messaging.hpp"

#define MODEL_NAME "supercombo_dlc"

#define MODEL_WIDTH 512
#define MODEL_HEIGHT 256
#define MODEL_FRAME_SIZE MODEL_WIDTH * MODEL_HEIGHT * 3 / 2
#define DESIRE_LEN 8
#define TRAFFIC_CONVENTION_LEN 2

#define PLAN_MHP_N 5
#define PLAN_MHP_COLUMNS 30
#define PLAN_MHP_VALS 30*33
#define PLAN_MHP_SELECTION 1
#define PLAN_MHP_GROUP_SIZE (2*PLAN_MHP_VALS + PLAN_MHP_SELECTION)

#define LEAD_MHP_N 5
#define LEAD_MHP_VALS 4
#define LEAD_MHP_SELECTION 3
#define LEAD_MHP_GROUP_SIZE (2*LEAD_MHP_VALS + LEAD_MHP_SELECTION)

>>>>>>> origin/ci-clean
#define POSE_SIZE 12

#define MODEL_FREQ 20
#define MAX_FRAME_DROP 0.05

struct ModelDataRaw {
<<<<<<< HEAD
    float *path;
    float *left_lane;
    float *right_lane;
    float *lead;
    float *long_x;
    float *long_v;
    float *long_a;
    float *desire_state;
    float *meta;
=======
    float *plan;
    float *lane_lines;
    float *lane_lines_prob;
    float *road_edges;
    float *lead;
    float *lead_prob;
    float *desire_state;
    float *meta;
    float *desire_pred;
>>>>>>> origin/ci-clean
    float *pose;
  };


typedef struct ModelState {
  ModelFrame frame;
  float *output;
  float *input_frames;
  RunModel *m;
#ifdef DESIRE
  std::unique_ptr<float[]> prev_desire;
  std::unique_ptr<float[]> pulse_desire;
#endif
#ifdef TRAFFIC_CONVENTION
  std::unique_ptr<float[]> traffic_convention;
#endif
} ModelState;

void model_init(ModelState* s, cl_device_id device_id,
                cl_context context, int temporal);
ModelDataRaw model_eval_frame(ModelState* s, cl_command_queue q,
                           cl_mem yuv_cl, int width, int height,
                           mat3 transform, void* sock, float *desire_in);
void model_free(ModelState* s);
void poly_fit(float *in_pts, float *in_stds, float *out);

void model_publish(PubMaster &pm, uint32_t vipc_frame_id, uint32_t frame_id,
                   uint32_t vipc_dropped_frames, float frame_drop, const ModelDataRaw &data, uint64_t timestamp_eof);
<<<<<<< HEAD
=======
void model_publish_v2(PubMaster &pm, uint32_t vipc_frame_id, uint32_t frame_id,
                   uint32_t vipc_dropped_frames, float frame_drop, const ModelDataRaw &data, uint64_t timestamp_eof);
>>>>>>> origin/ci-clean
void posenet_publish(PubMaster &pm, uint32_t vipc_frame_id, uint32_t frame_id,
                     uint32_t vipc_dropped_frames, float frame_drop, const ModelDataRaw &data, uint64_t timestamp_eof);
#endif
