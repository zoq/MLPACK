/**
 * @file methods/ann/layer/dropout_impl.hpp
 * @author Marcus Edel
 *
 * Implementation of the Dropout class, which implements a regularizer that
 * randomly sets units to zero. Preventing units from co-adapting.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_METHODS_ANN_LAYER_DROPOUT_IMPL_HPP
#define MLPACK_METHODS_ANN_LAYER_DROPOUT_IMPL_HPP

// In case it hasn't yet been included.
#include "dropout.hpp"

namespace mlpack {
namespace ann /** Artificial Neural Network. */ {

template<typename InputType, typename OutputType>
DropoutType<InputType, OutputType>::DropoutType(
    const double ratio) :
    ratio(ratio),
    scale(1.0 / (1.0 - ratio))
{
  // Nothing to do here.
}

template<typename InputType, typename OutputType>
void DropoutType<InputType, OutputType>::Forward(
    const InputType& input,
    OutputType& output)
{
  // The dropout mask will not be multiplied in testing mode.
  if (!this->training)
  {
    output = input;
  }
  else
  {
    // Scale with input / (1 - ratio) and set values to zero with probability
    // 'ratio'.
    mask = arma::randu<OutputType>(input.n_rows, input.n_cols);
    mask.transform([&](double val) { return (val > ratio); });
    output = input % mask * scale;
  }
}

template<typename InputType, typename OutputType>
void DropoutType<InputType, OutputType>::Backward(
    const InputType& /* input */,
    const OutputType& gy,
    OutputType& g)
{
  g = gy % mask * scale;
}

template<typename InputType, typename OutputType>
template<typename Archive>
void DropoutType<InputType, OutputType>::serialize(
    Archive& ar,
    const uint32_t /* version */)
{
  ar(cereal::base_class<Layer<InputType, OutputType>>(this));

  ar(CEREAL_NVP(ratio));

  // Reset scale.
  scale = 1.0 / (1.0 - ratio);
}

} // namespace ann
} // namespace mlpack

#endif
