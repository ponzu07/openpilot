
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <eigen3/Eigen/Dense>

#include "common/timing.h"
#include "common/params.h"
#include "driving.h"

<<<<<<< HEAD
#define PATH_IDX 0
#define LL_IDX PATH_IDX + MODEL_PATH_DISTANCE*2 + 1
#define RL_IDX LL_IDX + MODEL_PATH_DISTANCE*2 + 2
#define LEAD_IDX RL_IDX + MODEL_PATH_DISTANCE*2 + 2
#define LONG_X_IDX LEAD_IDX + MDN_GROUP_SIZE*LEAD_MDN_N + SELECTION
#define LONG_V_IDX LONG_X_IDX + TIME_DISTANCE*2
#define LONG_A_IDX LONG_V_IDX + TIME_DISTANCE*2
#define DESIRE_STATE_IDX LONG_A_IDX + TIME_DISTANCE*2
=======
#define MIN_VALID_LEN 10.0
#define TRAJECTORY_SIZE 33
#define TRAJECTORY_TIME 10.0
#define TRAJECTORY_DISTANCE 192.0
#define PLAN_IDX 0
#define LL_IDX PLAN_IDX + PLAN_MHP_N*(PLAN_MHP_GROUP_SIZE)
#define LL_PROB_IDX LL_IDX + 4*2*2*33
#define RE_IDX LL_PROB_IDX + 4
#define LEAD_IDX RE_IDX + 2*2*2*33
#define LEAD_PROB_IDX LEAD_IDX + LEAD_MHP_N*(LEAD_MHP_GROUP_SIZE)
#define DESIRE_STATE_IDX LEAD_PROB_IDX + 3
>>>>>>> origin/ci-clean
#define META_IDX DESIRE_STATE_IDX + DESIRE_LEN
#define POSE_IDX META_IDX + OTHER_META_SIZE + DESIRE_PRED_SIZE
#define OUTPUT_SIZE  POSE_IDX + POSE_SIZE
#ifdef TEMPORAL
  #define TEMPORAL_SIZE 512
#else
  #define TEMPORAL_SIZE 0
#endif

// #define DUMP_YUV

Eigen::Matrix<float, MODEL_PATH_DISTANCE, POLYFIT_DEGREE - 1> vander;
<<<<<<< HEAD
=======
float X_IDXS[TRAJECTORY_SIZE];
float T_IDXS[TRAJECTORY_SIZE];
>>>>>>> origin/ci-clean

void model_init(ModelState* s, cl_device_id device_id, cl_context context, int temporal) {
  frame_init(&s->frame, MODEL_WIDTH, MODEL_HEIGHT, device_id, context);
  s->input_frames = (float*)calloc(MODEL_FRAME_SIZE * 2, sizeof(float));

  const int output_size = OUTPUT_SIZE + TEMPORAL_SIZE;
  s->output = (float*)calloc(output_size, sizeof(float));

  s->m = new DefaultRunModel("../../models/supercombo.dlc", s->output, output_size, USE_GPU_RUNTIME);

#ifdef TEMPORAL
  assert(temporal);
  s->m->addRecurrent(&s->output[OUTPUT_SIZE], TEMPORAL_SIZE);
#endif

#ifdef DESIRE
  s->prev_desire = std::make_unique<float[]>(DESIRE_LEN);
  s->pulse_desire = std::make_unique<float[]>(DESIRE_LEN);
  s->m->addDesire(s->pulse_desire.get(), DESIRE_LEN);
#endif

#ifdef TRAFFIC_CONVENTION
  s->traffic_convention = std::make_unique<float[]>(TRAFFIC_CONVENTION_LEN);
  s->m->addTrafficConvention(s->traffic_convention.get(), TRAFFIC_CONVENTION_LEN);

<<<<<<< HEAD
  bool is_rhd = read_db_bool("IsRHD");
=======
  bool is_rhd = Params().read_db_bool("IsRHD");
>>>>>>> origin/ci-clean
  if (is_rhd) {
    s->traffic_convention[1] = 1.0;
  } else {
    s->traffic_convention[0] = 1.0;
  }
#endif

  // Build Vandermonde matrix
<<<<<<< HEAD
  for(int i = 0; i < MODEL_PATH_DISTANCE; i++) {
    for(int j = 0; j < POLYFIT_DEGREE - 1; j++) {
      vander(i, j) = pow(i, POLYFIT_DEGREE-j-1);
=======
  for(int i = 0; i < TRAJECTORY_SIZE; i++) {
    for(int j = 0; j < POLYFIT_DEGREE - 1; j++) {
      X_IDXS[i] = (TRAJECTORY_DISTANCE/1024.0) * (pow(i,2));
      T_IDXS[i] = (TRAJECTORY_TIME/1024.0) * (pow(i,2));
      vander(i, j) = pow(X_IDXS[i], POLYFIT_DEGREE-j-1);
>>>>>>> origin/ci-clean
    }
  }
}

ModelDataRaw model_eval_frame(ModelState* s, cl_command_queue q,
                           cl_mem yuv_cl, int width, int height,
                           mat3 transform, void* sock,
                           float *desire_in) {
#ifdef DESIRE
  if (desire_in != NULL) {
<<<<<<< HEAD
    for (int i = 0; i < DESIRE_LEN; i++) {
=======
    for (int i = 1; i < DESIRE_LEN; i++) {
>>>>>>> origin/ci-clean
      // Model decides when action is completed
      // so desire input is just a pulse triggered on rising edge
      if (desire_in[i] - s->prev_desire[i] > .99) {
        s->pulse_desire[i] = desire_in[i];
      } else {
        s->pulse_desire[i] = 0.0;
      }
      s->prev_desire[i] = desire_in[i];
    }
  }
#endif

  //for (int i = 0; i < OUTPUT_SIZE + TEMPORAL_SIZE; i++) { printf("%f ", s->output[i]); } printf("\n");

  float *new_frame_buf = frame_prepare(&s->frame, q, yuv_cl, width, height, transform);
  memmove(&s->input_frames[0], &s->input_frames[MODEL_FRAME_SIZE], sizeof(float)*MODEL_FRAME_SIZE);
  memmove(&s->input_frames[MODEL_FRAME_SIZE], new_frame_buf, sizeof(float)*MODEL_FRAME_SIZE);
  s->m->execute(s->input_frames, MODEL_FRAME_SIZE*2);

  #ifdef DUMP_YUV
    FILE *dump_yuv_file = fopen("/sdcard/dump.yuv", "wb");
    fwrite(new_frame_buf, MODEL_HEIGHT*MODEL_WIDTH*3/2, sizeof(float), dump_yuv_file);
    fclose(dump_yuv_file);
    assert(1==2);
  #endif

  clEnqueueUnmapMemObject(q, s->frame.net_input, (void*)new_frame_buf, 0, NULL, NULL);

  // net outputs
  ModelDataRaw net_outputs;
<<<<<<< HEAD
  net_outputs.path = &s->output[PATH_IDX];
  net_outputs.left_lane = &s->output[LL_IDX];
  net_outputs.right_lane = &s->output[RL_IDX];
  net_outputs.lead = &s->output[LEAD_IDX];
  net_outputs.long_x = &s->output[LONG_X_IDX];
  net_outputs.long_v = &s->output[LONG_V_IDX];
  net_outputs.long_a = &s->output[LONG_A_IDX];
=======
  net_outputs.plan = &s->output[PLAN_IDX];
  net_outputs.lane_lines = &s->output[LL_IDX];
  net_outputs.lane_lines_prob = &s->output[LL_PROB_IDX];
  net_outputs.road_edges = &s->output[RE_IDX];
  net_outputs.lead = &s->output[LEAD_IDX];
  net_outputs.lead_prob = &s->output[LEAD_PROB_IDX];
>>>>>>> origin/ci-clean
  net_outputs.meta = &s->output[DESIRE_STATE_IDX];
  net_outputs.pose = &s->output[POSE_IDX];
  return net_outputs;
}

void model_free(ModelState* s) {
  free(s->output);
  free(s->input_frames);
  frame_free(&s->frame);
  delete s->m;
}

void poly_fit(float *in_pts, float *in_stds, float *out, int valid_len) {
  // References to inputs
  Eigen::Map<Eigen::Matrix<float, Eigen::Dynamic, 1> > pts(in_pts, valid_len);
  Eigen::Map<Eigen::Matrix<float, Eigen::Dynamic, 1> > std(in_stds, valid_len);
  Eigen::Map<Eigen::Matrix<float, POLYFIT_DEGREE - 1, 1> > p(out, POLYFIT_DEGREE - 1);

  float y0 = pts[0];
  pts = pts.array() - y0;

  // Build Least Squares equations
  Eigen::Matrix<float, Eigen::Dynamic, POLYFIT_DEGREE - 1> lhs = vander.topRows(valid_len).array().colwise() / std.array();
  Eigen::Matrix<float, Eigen::Dynamic, 1> rhs = pts.array() / std.array();

  // Improve numerical stability
  Eigen::Matrix<float, POLYFIT_DEGREE - 1, 1> scale = 1. / (lhs.array()*lhs.array()).sqrt().colwise().sum();
  lhs = lhs * scale.asDiagonal();

  // Solve inplace
  p = lhs.colPivHouseholderQr().solve(rhs);

  // Apply scale to output
  p = p.transpose() * scale.asDiagonal();
  out[3] = y0;
}

<<<<<<< HEAD
void fill_path(cereal::ModelData::PathData::Builder path, const float * data, bool has_prob, const float offset) {
  float points_arr[MODEL_PATH_DISTANCE];
  float stds_arr[MODEL_PATH_DISTANCE];
  float poly_arr[POLYFIT_DEGREE];
  float std;
  float prob;
  float valid_len;

  // clamp to 5 and MODEL_PATH_DISTANCE
  valid_len = fmin(MODEL_PATH_DISTANCE, fmax(5, data[MODEL_PATH_DISTANCE*2]));
  for (int i=0; i<MODEL_PATH_DISTANCE; i++) {
    points_arr[i] = data[i] + offset;
    stds_arr[i] = softplus(data[MODEL_PATH_DISTANCE + i]) + 1e-6;
  }
  if (has_prob) {
    prob =  sigmoid(data[MODEL_PATH_DISTANCE*2 + 1]);
  } else {
    prob = 1.0;
  }
  std = softplus(data[MODEL_PATH_DISTANCE]) + 1e-6;
  poly_fit(points_arr, stds_arr, poly_arr, valid_len);

  if (std::getenv("DEBUG")){
    kj::ArrayPtr<const float> stds(&stds_arr[0], ARRAYSIZE(stds_arr));
    path.setStds(stds);

    kj::ArrayPtr<const float> points(&points_arr[0], ARRAYSIZE(points_arr));
    path.setPoints(points);
  }
=======
void fill_path(cereal::ModelData::PathData::Builder path, const float * data, float valid_len, int valid_len_idx) {
  float points_arr[TRAJECTORY_SIZE];
  float stds_arr[TRAJECTORY_SIZE];
  float poly_arr[POLYFIT_DEGREE];
  float std;

  for (int i=0; i<TRAJECTORY_SIZE; i++) {
    // negative sign because mpc has left positive
    points_arr[i] = -data[30*i + 16];
    stds_arr[i] = exp(data[30*(33 + i) + 16]);
  }
  std = stds_arr[0];
  poly_fit(points_arr, stds_arr, poly_arr, valid_len_idx);

  path.setPoly(poly_arr);
  path.setProb(1.0);
  path.setStd(std);
  path.setValidLen(valid_len);
}

void fill_lane_line(cereal::ModelData::PathData::Builder path, const float * data, int ll_idx, float valid_len, int valid_len_idx, float prob) {
  float points_arr[TRAJECTORY_SIZE];
  float stds_arr[TRAJECTORY_SIZE];
  float poly_arr[POLYFIT_DEGREE];
  float std;

  for (int i=0; i<TRAJECTORY_SIZE; i++) {
    // negative sign because mpc has left positive
    points_arr[i] = -data[2*33*ll_idx + 2*i];
    stds_arr[i] = exp(data[2*33*(4 + ll_idx) + 2*i]);
  }
  std = stds_arr[0];
  poly_fit(points_arr, stds_arr, poly_arr, valid_len_idx);
>>>>>>> origin/ci-clean

  path.setPoly(poly_arr);
  path.setProb(prob);
  path.setStd(std);
  path.setValidLen(valid_len);
}

<<<<<<< HEAD
void fill_lead(cereal::ModelData::LeadData::Builder lead, const float * data, int mdn_max_idx, int t_offset) {
  const double x_scale = 10.0;
  const double y_scale = 10.0;

  lead.setProb(sigmoid(data[LEAD_MDN_N*MDN_GROUP_SIZE + t_offset]));
  lead.setDist(x_scale * data[mdn_max_idx*MDN_GROUP_SIZE]);
  lead.setStd(x_scale * softplus(data[mdn_max_idx*MDN_GROUP_SIZE + MDN_VALS]));
  lead.setRelY(y_scale * data[mdn_max_idx*MDN_GROUP_SIZE + 1]);
  lead.setRelYStd(y_scale * softplus(data[mdn_max_idx*MDN_GROUP_SIZE + MDN_VALS + 1]));
  lead.setRelVel(data[mdn_max_idx*MDN_GROUP_SIZE + 2]);
  lead.setRelVelStd(softplus(data[mdn_max_idx*MDN_GROUP_SIZE + MDN_VALS + 2]));
  lead.setRelA(data[mdn_max_idx*MDN_GROUP_SIZE + 3]);
  lead.setRelAStd(softplus(data[mdn_max_idx*MDN_GROUP_SIZE + MDN_VALS + 3]));
}

void fill_meta(cereal::ModelData::MetaData::Builder meta, const float * meta_data) {
  kj::ArrayPtr<const float> desire_state(&meta_data[0], DESIRE_LEN);
  meta.setDesireState(desire_state);
  meta.setEngagedProb(meta_data[DESIRE_LEN]);
  meta.setGasDisengageProb(meta_data[DESIRE_LEN + 1]);
  meta.setBrakeDisengageProb(meta_data[DESIRE_LEN + 2]);
  meta.setSteerOverrideProb(meta_data[DESIRE_LEN + 3]);
  kj::ArrayPtr<const float> desire_pred(&meta_data[DESIRE_LEN + OTHER_META_SIZE], DESIRE_PRED_SIZE);
  meta.setDesirePrediction(desire_pred);
}

void fill_longi(cereal::ModelData::LongitudinalData::Builder longi, const float * long_x_data, const float * long_v_data, const float * long_a_data) {
  // just doing 10 vals, 1 every sec for now
  float dist_arr[TIME_DISTANCE/10];
  float speed_arr[TIME_DISTANCE/10];
  float accel_arr[TIME_DISTANCE/10];
  for (int i=0; i<TIME_DISTANCE/10; i++) {
    dist_arr[i] = long_x_data[i*10];
    speed_arr[i] = long_v_data[i*10];
    accel_arr[i] = long_a_data[i*10];
  }
  kj::ArrayPtr<const float> dist(&dist_arr[0], ARRAYSIZE(dist_arr));
  longi.setDistances(dist);
  kj::ArrayPtr<const float> speed(&speed_arr[0], ARRAYSIZE(speed_arr));
  longi.setSpeeds(speed);
  kj::ArrayPtr<const float> accel(&accel_arr[0], ARRAYSIZE(accel_arr));
  longi.setAccelerations(accel);
=======
void fill_lead_v2(cereal::ModelDataV2::LeadDataV2::Builder lead, const float * data, float prob, float t) {
  lead.setProb(prob);
  lead.setT(t);
  float xyva_arr[LEAD_MHP_VALS];
  float xyva_stds_arr[LEAD_MHP_VALS];
  for (int i=0; i<LEAD_MHP_VALS; i++) {
    xyva_arr[i] = data[LEAD_MHP_VALS + i];
    xyva_stds_arr[i] = exp(data[LEAD_MHP_VALS + i]);
  }
  lead.setXyva(xyva_arr);
  lead.setXyvaStd(xyva_stds_arr);
}

void fill_lead(cereal::ModelData::LeadData::Builder lead, const float * data, float prob) {
  lead.setProb(prob);
  lead.setDist(data[0]);
  lead.setStd(exp(data[LEAD_MHP_VALS]));
  // TODO make all msgs same format
  lead.setRelY(-data[1]);
  lead.setRelYStd(exp(data[LEAD_MHP_VALS + 1]));
  lead.setRelVel(data[2]);
  lead.setRelVelStd(exp(data[LEAD_MHP_VALS + 2]));
  lead.setRelA(data[3]);
  lead.setRelAStd(exp(data[LEAD_MHP_VALS + 3]));
}

void fill_meta(cereal::ModelData::MetaData::Builder meta, const float * meta_data) {
  float desire_state_softmax[DESIRE_LEN];
  float desire_pred_softmax[4*DESIRE_LEN];
  softmax(&meta_data[0], desire_state_softmax, DESIRE_LEN);
  for (int i=0; i<4; i++) {
    softmax(&meta_data[DESIRE_LEN + OTHER_META_SIZE + i*DESIRE_LEN],
            &desire_pred_softmax[i*DESIRE_LEN], DESIRE_LEN);
  }
  meta.setDesireState(desire_state_softmax);
  meta.setEngagedProb(sigmoid(meta_data[DESIRE_LEN]));
  meta.setGasDisengageProb(sigmoid(meta_data[DESIRE_LEN + 1]));
  meta.setBrakeDisengageProb(sigmoid(meta_data[DESIRE_LEN + 2]));
  meta.setSteerOverrideProb(sigmoid(meta_data[DESIRE_LEN + 3]));
  meta.setDesirePrediction(desire_pred_softmax);
}

void fill_meta_v2(cereal::ModelDataV2::MetaData::Builder meta, const float * meta_data) {
  float desire_state_softmax[DESIRE_LEN] = {};
  float desire_pred_softmax[4*DESIRE_LEN] = {};
  softmax(&meta_data[0], desire_state_softmax, DESIRE_LEN);
  for (int i=0; i<4; i++) {
    softmax(&meta_data[DESIRE_LEN + OTHER_META_SIZE + i*DESIRE_LEN],
            &desire_pred_softmax[i*DESIRE_LEN], DESIRE_LEN);
  }
  meta.setDesireState(desire_state_softmax);
  meta.setEngagedProb(sigmoid(meta_data[DESIRE_LEN]));
  meta.setGasDisengageProb(sigmoid(meta_data[DESIRE_LEN + 1]));
  meta.setBrakeDisengageProb(sigmoid(meta_data[DESIRE_LEN + 2]));
  meta.setSteerOverrideProb(sigmoid(meta_data[DESIRE_LEN + 3]));
  meta.setDesirePrediction(desire_pred_softmax);
}

void fill_xyzt(cereal::ModelDataV2::XYZTData::Builder xyzt, const float * data,
               int columns, int column_offset, float * plan_t_arr) {
  float x_arr[TRAJECTORY_SIZE] = {};
  float y_arr[TRAJECTORY_SIZE] = {};
  float z_arr[TRAJECTORY_SIZE] = {};
  //float x_std_arr[TRAJECTORY_SIZE];
  //float y_std_arr[TRAJECTORY_SIZE];
  //float z_std_arr[TRAJECTORY_SIZE];
  float t_arr[TRAJECTORY_SIZE];
  for (int i=0; i<TRAJECTORY_SIZE; i++) {
    // column_offset == -1 means this data is X indexed not T indexed
    if (column_offset >= 0) {
      t_arr[i] = T_IDXS[i];
      x_arr[i] = data[i*columns + 0 + column_offset];
      //x_std_arr[i] = data[columns*(TRAJECTORY_SIZE + i) + 0 + column_offset];
    } else {
      t_arr[i] = plan_t_arr[i];
      x_arr[i] = X_IDXS[i];
      //x_std_arr[i] = NAN;
    }
    y_arr[i] = data[i*columns + 1 + column_offset];
    //y_std_arr[i] = data[columns*(TRAJECTORY_SIZE + i) + 1 + column_offset];
    z_arr[i] = data[i*columns + 2 + column_offset];
    //z_std_arr[i] = data[columns*(TRAJECTORY_SIZE + i) + 2 + column_offset];
  }
  //kj::ArrayPtr<const float> x_std(x_std_arr, TRAJECTORY_SIZE);
  //kj::ArrayPtr<const float> y_std(y_std_arr, TRAJECTORY_SIZE);
  //kj::ArrayPtr<const float> z_std(z_std_arr, TRAJECTORY_SIZE);
  xyzt.setX(x_arr);
  xyzt.setY(y_arr);
  xyzt.setZ(z_arr);
  //xyzt.setXStd(x_std);
  //xyzt.setYStd(y_std);
  //xyzt.setZStd(z_std);
  xyzt.setT(t_arr);
}


void model_publish_v2(PubMaster &pm, uint32_t vipc_frame_id, uint32_t frame_id,
                     uint32_t vipc_dropped_frames, float frame_drop,
                     const ModelDataRaw &net_outputs, uint64_t timestamp_eof,
                     float model_execution_time) {
  // make msg
  MessageBuilder msg;
  auto framed = msg.initEvent().initModelV2();
  uint32_t frame_age = (frame_id > vipc_frame_id) ? (frame_id - vipc_frame_id) : 0;
  framed.setFrameId(vipc_frame_id);
  framed.setFrameAge(frame_age);
  framed.setFrameDropPerc(frame_drop * 100);
  framed.setTimestampEof(timestamp_eof);
  framed.setModelExecutionTime(model_execution_time);

  // plan
  int plan_mhp_max_idx = 0;
  for (int i=1; i<PLAN_MHP_N; i++) {
    if (net_outputs.plan[(i + 1)*(PLAN_MHP_GROUP_SIZE) - 1] >
        net_outputs.plan[(plan_mhp_max_idx + 1)*(PLAN_MHP_GROUP_SIZE) - 1]) {
      plan_mhp_max_idx = i;
    }
  }

  float * best_plan = &net_outputs.plan[plan_mhp_max_idx*(PLAN_MHP_GROUP_SIZE)];
  float plan_t_arr[TRAJECTORY_SIZE];
  for (int i=0; i<TRAJECTORY_SIZE; i++) {
    plan_t_arr[i] = best_plan[i*PLAN_MHP_COLUMNS + 15];
  }

  fill_xyzt(framed.initPosition(), best_plan, PLAN_MHP_COLUMNS, 0, plan_t_arr);
  fill_xyzt(framed.initVelocity(), best_plan, PLAN_MHP_COLUMNS, 3, plan_t_arr);
  fill_xyzt(framed.initOrientation(), best_plan, PLAN_MHP_COLUMNS, 9, plan_t_arr);
  fill_xyzt(framed.initOrientationRate(), best_plan, PLAN_MHP_COLUMNS, 12, plan_t_arr);

  // lane lines
  auto lane_lines = framed.initLaneLines(4);
  float lane_line_probs_arr[4];
  float lane_line_stds_arr[4];
  for (int i = 0; i < 4; i++) {
    fill_xyzt(lane_lines[i], &net_outputs.lane_lines[i*TRAJECTORY_SIZE*2], 2, -1, plan_t_arr);
    lane_line_probs_arr[i] = sigmoid(net_outputs.lane_lines_prob[i]);
    lane_line_stds_arr[i] = exp(net_outputs.lane_lines[2*TRAJECTORY_SIZE*(4 + i)]);
  }
  framed.setLaneLineProbs(lane_line_probs_arr);
  framed.setLaneLineStds(lane_line_stds_arr);

  // road edges
  auto road_edges = framed.initRoadEdges(2);
  float road_edge_stds_arr[2];
  for (int i = 0; i < 2; i++) {
    fill_xyzt(road_edges[i], &net_outputs.road_edges[i*TRAJECTORY_SIZE*2], 2, -1, plan_t_arr);
    road_edge_stds_arr[i] = exp(net_outputs.road_edges[2*TRAJECTORY_SIZE*(2 + i)]);
  }
  framed.setRoadEdgeStds(road_edge_stds_arr);

  // meta
  auto meta = framed.initMeta();
  fill_meta_v2(meta, net_outputs.meta);

  // leads
  auto leads = framed.initLeads(LEAD_MHP_SELECTION);
  int mdn_max_idx = 0;
  float t_offsets[LEAD_MHP_SELECTION] = {0.0, 2.0, 4.0};
  for (int t_offset=0; t_offset<LEAD_MHP_SELECTION; t_offset++) {
    for (int i=1; i<LEAD_MHP_N; i++) {
      if (net_outputs.lead[(i+1)*(LEAD_MHP_GROUP_SIZE) + t_offset - LEAD_MHP_SELECTION] >
          net_outputs.lead[(mdn_max_idx + 1)*(LEAD_MHP_GROUP_SIZE) + t_offset - LEAD_MHP_SELECTION]) {
        mdn_max_idx = i;
        fill_lead_v2(leads[t_offset], &net_outputs.lead[mdn_max_idx*(LEAD_MHP_GROUP_SIZE)],
                     sigmoid(net_outputs.lead_prob[t_offset]), t_offsets[t_offset]);
      }
    }
  }
  pm.send("modelV2", msg);
>>>>>>> origin/ci-clean
}

void model_publish(PubMaster &pm, uint32_t vipc_frame_id, uint32_t frame_id,
                   uint32_t vipc_dropped_frames, float frame_drop,
                   const ModelDataRaw &net_outputs, uint64_t timestamp_eof,
                   float model_execution_time) {

  uint32_t frame_age = (frame_id > vipc_frame_id) ? (frame_id - vipc_frame_id) : 0;
  MessageBuilder msg;
  auto framed = msg.initEvent().initModel();
  framed.setFrameId(vipc_frame_id);
  framed.setFrameAge(frame_age);
  framed.setFrameDropPerc(frame_drop * 100);
  framed.setTimestampEof(timestamp_eof);
  framed.setModelExecutionTime(model_execution_time);

<<<<<<< HEAD
  fill_path(framed.initPath(), net_outputs.path, false, 0);
  fill_path(framed.initLeftLane(), net_outputs.left_lane, true, 1.8);
  fill_path(framed.initRightLane(), net_outputs.right_lane, true, -1.8);
  fill_longi(framed.initLongitudinal(), net_outputs.long_x, net_outputs.long_v, net_outputs.long_a);
=======
  // Find the distribution that corresponds to the most probable plan
  int plan_mhp_max_idx = 0;
  for (int i=1; i<PLAN_MHP_N; i++) {
    if (net_outputs.plan[(i + 1)*(PLAN_MHP_GROUP_SIZE) - 1] >
        net_outputs.plan[(plan_mhp_max_idx + 1)*(PLAN_MHP_GROUP_SIZE) - 1]) {
      plan_mhp_max_idx = i;
    }
  }

  // x pos at 10s is a good valid_len
  float valid_len = 0;
  float valid_len_candidate;
  for (int i=1; i<TRAJECTORY_SIZE; i++) {
    valid_len_candidate = net_outputs.plan[plan_mhp_max_idx*(PLAN_MHP_GROUP_SIZE) + 30*i];
    if (valid_len_candidate >= valid_len){
      valid_len = valid_len_candidate;
    }
  }
  // clamp to 10 and MODEL_PATH_DISTANCE
  valid_len = fmin(MODEL_PATH_DISTANCE, fmax(MIN_VALID_LEN, valid_len));
  int valid_len_idx = 0;
  for (int i=1; i<TRAJECTORY_SIZE; i++) {
    if (valid_len >= X_IDXS[valid_len_idx]){
      valid_len_idx = i;
    }
  }

  auto lpath = framed.initPath();
  fill_path(lpath, &net_outputs.plan[plan_mhp_max_idx*(PLAN_MHP_GROUP_SIZE)], valid_len, valid_len_idx);

  auto left_lane = framed.initLeftLane();
  int ll_idx = 1;
  fill_lane_line(left_lane, net_outputs.lane_lines, ll_idx, valid_len, valid_len_idx,
            sigmoid(net_outputs.lane_lines_prob[ll_idx]));
  auto right_lane = framed.initRightLane();
  ll_idx = 2;
  fill_lane_line(right_lane, net_outputs.lane_lines, ll_idx, valid_len, valid_len_idx,
            sigmoid(net_outputs.lane_lines_prob[ll_idx]));
>>>>>>> origin/ci-clean

  // Find the distribution that corresponds to the current lead
  int mdn_max_idx = 0;
  int t_offset = 0;
<<<<<<< HEAD
  for (int i=1; i<LEAD_MDN_N; i++) {
    if (net_outputs.lead[i*MDN_GROUP_SIZE + 8 + t_offset] > net_outputs.lead[mdn_max_idx*MDN_GROUP_SIZE + 8 + t_offset]) {
      mdn_max_idx = i;
    }
  }
  fill_lead(framed.initLead(), net_outputs.lead, mdn_max_idx, t_offset);
  // Find the distribution that corresponds to the lead in 2s
  mdn_max_idx = 0;
  t_offset = 1;
  for (int i=1; i<LEAD_MDN_N; i++) {
    if (net_outputs.lead[i*MDN_GROUP_SIZE + 8 + t_offset] > net_outputs.lead[mdn_max_idx*MDN_GROUP_SIZE + 8 + t_offset]) {
      mdn_max_idx = i;
    }
  }
  fill_lead(framed.initLeadFuture(), net_outputs.lead, mdn_max_idx, t_offset);
=======
  for (int i=1; i<LEAD_MHP_N; i++) {
    if (net_outputs.lead[(i+1)*(LEAD_MHP_GROUP_SIZE) + t_offset - 3] >
        net_outputs.lead[(mdn_max_idx + 1)*(LEAD_MHP_GROUP_SIZE) + t_offset - 3]) {
      mdn_max_idx = i;
    }
  }
  fill_lead(framed.initLead(), &net_outputs.lead[mdn_max_idx*(LEAD_MHP_GROUP_SIZE)], sigmoid(net_outputs.lead_prob[t_offset]));
  // Find the distribution that corresponds to the lead in 2s
  mdn_max_idx = 0;
  t_offset = 1;
  for (int i=1; i<LEAD_MHP_N; i++) {
    if (net_outputs.lead[(i+1)*(LEAD_MHP_GROUP_SIZE) + t_offset - 3] >
        net_outputs.lead[(mdn_max_idx + 1)*(LEAD_MHP_GROUP_SIZE) + t_offset - 3]) {
      mdn_max_idx = i;
    }
  }
  fill_lead(framed.initLeadFuture(), &net_outputs.lead[mdn_max_idx*(LEAD_MHP_GROUP_SIZE)], sigmoid(net_outputs.lead_prob[t_offset]));

>>>>>>> origin/ci-clean
  fill_meta(framed.initMeta(), net_outputs.meta);

  pm.send("model", msg);
}

void posenet_publish(PubMaster &pm, uint32_t vipc_frame_id, uint32_t frame_id,
                     uint32_t vipc_dropped_frames, float frame_drop,
                     const ModelDataRaw &net_outputs, uint64_t timestamp_eof) {
  float trans_arr[3];
  float trans_std_arr[3];
  float rot_arr[3];
  float rot_std_arr[3];

  for (int i =0; i < 3; i++) {
    trans_arr[i] = net_outputs.pose[i];
<<<<<<< HEAD
    trans_std_arr[i] = softplus(net_outputs.pose[6 + i]) + 1e-6;

    rot_arr[i] = M_PI * net_outputs.pose[3 + i] / 180.0;
    rot_std_arr[i] = M_PI * (softplus(net_outputs.pose[9 + i]) + 1e-6) / 180.0;
=======
    trans_std_arr[i] = exp(net_outputs.pose[6 + i]);

    rot_arr[i] = net_outputs.pose[3 + i];
    rot_std_arr[i] = exp(net_outputs.pose[9 + i]);
>>>>>>> origin/ci-clean
  }

  MessageBuilder msg;
  auto posenetd = msg.initEvent(vipc_dropped_frames < 1).initCameraOdometry();
  posenetd.setTrans(trans_arr);
  posenetd.setRot(rot_arr);
  posenetd.setTransStd(trans_std_arr);
  posenetd.setRotStd(rot_std_arr);

  posenetd.setTimestampEof(timestamp_eof);
  posenetd.setFrameId(vipc_frame_id);

  pm.send("cameraOdometry", msg);
}
