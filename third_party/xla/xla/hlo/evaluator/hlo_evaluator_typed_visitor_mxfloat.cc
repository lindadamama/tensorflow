/* Copyright 2024 The OpenXLA Authors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "xla/hlo/evaluator/hlo_evaluator.h"
#include "xla/hlo/evaluator/hlo_evaluator_typed_visitor.h"
#include "tsl/platform/ml_dtypes.h"

namespace xla {
template class HloEvaluatorTypedVisitor<tsl::float4_e2m1fn, float>;
template class HloEvaluatorTypedVisitor<tsl::float8_e8m0fnu, float>;
}  // namespace xla
