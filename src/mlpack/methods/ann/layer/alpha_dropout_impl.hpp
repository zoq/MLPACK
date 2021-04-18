/**
 * @file methods/ann/layer/alpha_dropout_impl.hpp
 * @author Dakshit Agrawal
 *
 * Definition of the Alpha-Dropout class, which implements a regularizer that
 * randomly sets units to alpha-dash to prevent them from co-adapting and
 * makes an affine transformation so as to keep the mean and variance of
 * outputs at their original values.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */

#ifndef MLPACK_METHODS_ANN_LAYER_ALPHA_DROPOUT_IMPL_HPP
#define MLPACK_METHODS_ANN_LAYER_ALPHA_DROPOUT_IMPL_HPP

// In case it hasn't yet been included.
#include "alpha_dropout.hpp"

namespace mlpack {
namespace ann /** Artificial Neural Network. */ {

template<typename InputType, typename OutputType>
AlphaDropoutType<InputType, OutputType>::AlphaDropoutType(
    const double ratio,
    const double alphaDash) :
    ratio(ratio),
    alphaDash(alphaDash),
    deterministic(false)
{
  Ratio(ratio);
}

template<typename InputType, typename OutputType>
void AlphaDropoutType<InputType, OutputType>::Forward(
    const InputType& input, OutputType& output)
{
  // The dropout mask will not be multiplied in the deterministic mode
  // (during testing).
  if (deterministic)
  {
    output = input;
  }
  else
  {
    // Set values to alphaDash with probability ratio.  Then apply affine
    // transformation so as to keep mean and variance of outputs to their
    // original values.
    mask = arma::randu<InputType>(input.n_rows, input.n_cols);
    mask.transform( [&](double val) { return (val > ratio); } );
    output = (input % mask + alphaDash * (1 - mask)) * a + b;
  }
}

template<typename InputType, typename OutputType>
void AlphaDropoutType<InputType, OutputType>::Backward(
    const InputType& /* input */,
    const InputType& gy,
    OutputType& g)
{
  g = gy % mask * a;
}

template<typename InputType, typename OutputType>
template<typename Archive>
void AlphaDropoutType<InputType, OutputType>::serialize(
    Archive& ar, const uint32_t /* version */)
{
  ar(CEREAL_NVP(ratio));
  ar(CEREAL_NVP(alphaDash));
  ar(CEREAL_NVP(a));
  ar(CEREAL_NVP(b));
}

} // namespace ann
} // namespace mlpack

#endif
