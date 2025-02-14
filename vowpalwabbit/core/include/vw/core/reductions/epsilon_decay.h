// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.
#pragma once

#include "vw/core/array_parameters.h"
#include "vw/core/distributionally_robust.h"
#include "vw/core/reductions_fwd.h"
#include "vw/core/scored_config.h"

#include <algorithm>

namespace VW
{
namespace reductions
{
VW::LEARNER::base_learner* epsilon_decay_setup(VW::setup_base_i&);

namespace epsilon_decay
{
struct epsilon_decay_score : scored_config
{
  epsilon_decay_score() = default;
  epsilon_decay_score(double alpha, double tau, uint64_t model_idx)
      : VW::scored_config(alpha, tau), _model_idx(model_idx)
  {
  }
  float decayed_epsilon(uint64_t update_count);
  float get_upper_bound() const { return this->current_ips(); }
  float get_lower_bound() const { return _lower_bound; }
  uint64_t get_model_idx() const { return _model_idx; }
  void update_bounds(float w, float r);

  float _lower_bound = 0.f;
  uint64_t _model_idx;
};

struct epsilon_decay_data
{
  epsilon_decay_data(uint64_t num_configs, uint64_t min_scope, double epsilon_decay_alpha, double epsilon_decay_tau,
      parameters& weights)
      : _min_scope(min_scope)
      , _epsilon_decay_alpha(epsilon_decay_alpha)
      , _epsilon_decay_tau(epsilon_decay_tau)
      , _weights(weights)
  {
    uint64_t model_idx = 0;
    for (uint64_t i = 0; i < num_configs; ++i)
    {
      std::vector<epsilon_decay_score> score_vec;
      for (uint64_t j = 0; j < i + 1; ++j)
      {
        epsilon_decay_score s(epsilon_decay_alpha, epsilon_decay_tau, model_idx);
        score_vec.push_back(s);
        ++model_idx;
      }
      _scored_configs.push_back(score_vec);
    }
  }
  std::vector<std::vector<epsilon_decay_score>> _scored_configs;
  uint64_t _min_scope;
  double _epsilon_decay_alpha;  // Confidence interval
  double _epsilon_decay_tau;    // Count decay time constant
  parameters& _weights;
};

}  // namespace epsilon_decay
}  // namespace reductions

namespace model_utils
{
size_t read_model_field(io_buf&, VW::reductions::epsilon_decay::epsilon_decay_score&);
size_t read_model_field(io_buf&, VW::reductions::epsilon_decay::epsilon_decay_data&);
size_t write_model_field(io_buf&, const VW::reductions::epsilon_decay::epsilon_decay_score&, const std::string&, bool);
size_t write_model_field(io_buf&, const VW::reductions::epsilon_decay::epsilon_decay_data&, const std::string&, bool);
}  // namespace model_utils
}  // namespace VW
