/**
 * Copyright 2019 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MINDSPORE_CCSRC_PARALLEL_OPS_INFO_BATCH_PARALLEL_INFO_H_
#define MINDSPORE_CCSRC_PARALLEL_OPS_INFO_BATCH_PARALLEL_INFO_H_

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "ir/value.h"
#include "parallel/ops_info/operator_info.h"
#include "parallel/strategy.h"

namespace mindspore {
namespace parallel {
class BatchParallelInfo : public OperatorInfo {
 public:
  BatchParallelInfo(const std::string& name, const Shapes& inputs_shape, const Shapes& outputs_shape,
                    const PrimitiveAttrs& attrs)
      : OperatorInfo(name, inputs_shape, outputs_shape, attrs), dev_num_(1) {
    bp_cost_ptr_ = std::make_shared<BatchParallelCost>();
  }

  ~BatchParallelInfo() override = default;
  Status Init(const StrategyPtr& strategy) override;
  Status InitForCostModel(const StrategyPtr& strategy) override;
  Status GenerateStrategies(int32_t stage_id) override;
  Status SetCostUnderStrategy(const StrategyPtr& strategy) override;
  OperatorCostPtr GetOperatorCost() const override { return bp_cost_ptr_; }

 protected:
  Status CheckStrategy(const StrategyPtr& strategy) override;
  Status InferMirrorOps() override;
  Status InferForwardCommunication() override;
  Status InferTensorInfo() override;
  Status InferDevMatrixShape() override;
  Status InferTensorMap() override;
  Status GetAttrs() override;
  Strategys GetOutputsStrategy();
  Status InferAsLossDivisor() override;

 private:
  int32_t dev_num_;
  BatchParallelCostPtr bp_cost_ptr_;
};

class SparseSoftmaxCrossEntropyWithLogitsInfo : public BatchParallelInfo {
 public:
  SparseSoftmaxCrossEntropyWithLogitsInfo(const std::string& name, const Shapes& inputs_shape,
                                          const Shapes& outputs_shape, const PrimitiveAttrs& attrs)
      : BatchParallelInfo(name, inputs_shape, outputs_shape, attrs) {}
  ~SparseSoftmaxCrossEntropyWithLogitsInfo() override = default;
  void ReComputeBatchSplitFlagList() override;
};

class GatherV2Info : public BatchParallelInfo {
 public:
  GatherV2Info(const std::string& name, const Shapes& inputs_shape, const Shapes& outputs_shape,
               const PrimitiveAttrs& attrs)
      : BatchParallelInfo(name, inputs_shape, outputs_shape, attrs) {}
  ~GatherV2Info() override = default;
  void ReComputeBatchSplitFlagList() override;
};
}  // namespace parallel
}  // namespace mindspore

#endif  // MINDSPORE_CCSRC_PARALLEL_OPS_INFO_BATCH_PARALLEL_INFO_H_
