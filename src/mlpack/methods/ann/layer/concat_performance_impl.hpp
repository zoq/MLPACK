/**
 * @file methods/ann/layer/concat_performance_impl.hpp
 * @author Marcus Edel
 *
 * Implementation of the ConcatPerformance class.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_METHODS_ANN_LAYER_CONCAT_PERFORMANCE_IMPL_HPP
#define MLPACK_METHODS_ANN_LAYER_CONCAT_PERFORMANCE_IMPL_HPP

// In case it hasn't yet been included.
#include "concat_performance.hpp"

namespace mlpack {
namespace ann /** Artificial Neural Network. */ {

template<
    typename OutputLayerType,
    typename InputType,
    typename OutputType
>
ConcatPerformanceType<
    OutputLayerType,
    InputType,
    OutputType
>::ConcatPerformanceType(const size_t inSize, OutputLayerType&& outputLayer) :
    inSize(inSize),
    outputLayer(std::move(outputLayer))
{
  // Nothing to do here.
}

template<
    typename OutputLayerType,
    typename InputType,
    typename OutputType
>
double ConcatPerformanceType<
    OutputLayerType,
    InputType,
    OutputType
>::Forward(const InputType& input, OutputType& target)
{
  const size_t elements = input.n_elem / inSize;

  double output = 0;
  for (size_t i = 0; i < input.n_elem; i+= elements)
  {
    arma::mat subInput = input.submat(i, 0, i + elements - 1, 0);
    output += outputLayer.Forward(subInput, target);
  }

  return output;
}

template<
    typename OutputLayerType,
    typename InputType,
    typename OutputType
>
void ConcatPerformanceType<
    OutputLayerType,
    InputType,
    OutputType
>::Backward(
    const InputType& input,
    const InputType& target,
    OutputType& output)
{
  const size_t elements = input.n_elem / inSize;

  arma::mat subInput = input.submat(0, 0, elements - 1, 0);
  arma::mat subOutput;

  outputLayer.Backward(subInput, target, subOutput);

  output = arma::zeros(subOutput.n_elem, inSize);
  output.col(0) = subOutput;

  for (size_t i = elements, j = 0; i < input.n_elem; i+= elements, ++j)
  {
    subInput = input.submat(i, 0, i + elements - 1, 0);
    outputLayer.Backward(subInput, target, subOutput);

    output.col(j) = subOutput;
  }
}

template<
    typename OutputLayerType,
    typename InputType,
    typename OutputType
>
template<typename Archive>
void ConcatPerformanceType<
    OutputLayerType,
    InputType,
    OutputType
>::serialize(Archive& ar, const uint32_t /* version */)
{
  ar(CEREAL_NVP(inSize));
}

} // namespace ann
} // namespace mlpack

// Include implementation.
#include "concat_performance_impl.hpp"

#endif
