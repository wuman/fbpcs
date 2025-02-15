/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <stdexcept>
#include <string>

#include <emp-sh2pc/emp-sh2pc.h>
#include "folly/logging/xlog.h"

#include "fbpcs/emp_games/attribution/decoupled_aggregation/Constants.h"

namespace aggregation::private_aggregation {

struct TouchpointMetadata {
  int64_t adId;
  int64_t ts;
  bool isClick;
  int64_t campaignMetadata;

  /**
   * If both are clicks, or both are views, the earliest one comes first.
   * If one is a click but the other is a view, the view comes first.
   */
  bool operator<(const TouchpointMetadata& tpm) const {
    return (isClick == tpm.isClick) ? (ts < tpm.ts) : !isClick;
  }
};

struct MeasurementTouchpointMedata {
  const int64_t adId;

  // privatelyShareArrayFrom support
  friend bool operator==(
      const MeasurementTouchpointMedata& a,
      const MeasurementTouchpointMedata& b) {
    return a.adId == b.adId;
  }
  friend std::ostream& operator<<(
      std::ostream& os,
      const MeasurementTouchpointMedata& tp) {
    return os << "Measurement Touchpoint Metadata {"
              << " adId=" << tp.adId << "}";
  }
};

struct PrivateMeasurementTouchpointMetadata {
  emp::Integer adId;

  explicit PrivateMeasurementTouchpointMetadata()
      : adId{INT_SIZE, -1, emp::PUBLIC} {}

  explicit PrivateMeasurementTouchpointMetadata(const emp::Integer& _adId)
      : adId{_adId} {}

  // emp::batcher based construction support
  explicit PrivateMeasurementTouchpointMetadata(int len, const emp::block* b)
      : adId{INT_SIZE, b} {}

  PrivateMeasurementTouchpointMetadata select(
      const emp::Bit& useRhs,
      const PrivateMeasurementTouchpointMetadata& rhs) const {
    return PrivateMeasurementTouchpointMetadata{
        /* adId */ adId.select(useRhs, rhs.adId)};
  }

  // emp::batcher serialization support
  template <typename... Args>
  static size_t bool_size(Args...) {
    return emp::Integer::bool_size(INT_SIZE, 0 /* dummy value */);
  }

  // emp::batcher serialization support
  static void bool_data(bool* data, const MeasurementTouchpointMedata& tpm) {
    emp::Integer::bool_data(data, INT_SIZE, tpm.adId);
  }

  // string conversion support
  template <typename T = std::string>
  T reveal(int party) const {
    std::stringstream out;

    out << "Measurement Touchpoint Metadata { adId=";
    out << adId.reveal<std::string>(party);
    out << "}";

    return out.str();
  }
};

} // namespace aggregation::private_aggregation
