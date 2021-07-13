/**
 * @file methods/ann/layer/linear.hpp
 * @author Marcus Edel
 *
 * Definition of the Linear layer class also known as fully-connected layer or
 * affine transformation.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_METHODS_ANN_LAYER_LINEAR_HPP
#define MLPACK_METHODS_ANN_LAYER_LINEAR_HPP

#include <mlpack/prereqs.hpp>
#include <mlpack/methods/ann/regularizer/no_regularizer.hpp>

#include "layer.hpp"

namespace mlpack {
namespace ann /** Artificial Neural Network. */ {

/**
 * Implementation of the Linear layer class. The Linear class represents a
 * single layer of a neural network.
 *
 * The linear layer applies a linear transformation to the incoming data
 * (input), i.e. y = Ax + b. The input matrix given in Forward(input, output)
 * must be either a vector or matrix. If the input is a matrix, then each column
 * is assumed to be an input sample of given batch.
 *
 * @tparam InputType The type of the layer's inputs. The layer automatically
 *    cast inputs to this type (Default: arma::mat).
 * @tparam OutputType The type of the computation which also causes the output
 *    to also be in this type. The type also allows the computation and weight
 *    type to differ from the input type (Default: arma::mat).
 * @tparam RegularizerType Type of the regularizer to be used (Default no
 *    regularizer).
 */
template<
    typename InputType = arma::mat,
    typename OutputType = arma::mat,
    typename RegularizerType = NoRegularizer
>
class LinearType: public Layer<InputType, OutputType>
{
 public:
  //! Create the Linear object.
  LinearType();

  /**
   * Create the Linear layer object with the specified number of output
   * dimensions.
   *
   * @param outSize The output dimension.
   * @param regularizer The regularizer to use, optional (default: no
   *     regularizer).
   */
  LinearType(const size_t outSize,
             RegularizerType regularizer = RegularizerType());

  //! Clone the LinearType object. This handles polymorphism correctly.
  LinearType* Clone() const { return new LinearType(*this); }

  /**
   * Reset the layer parameter (weights and bias). The method is called to
   * assign the allocated memory to the internal learnable parameters.
   */
  void SetWeights(typename OutputType::elem_type* weightsPtr);

  /**
   * Ordinary feed forward pass of a neural network, evaluating the function
   * f(x) by propagating the activity forward through f.
   *
   * f(x) is a linear transformation: Ax + b, where x is the given input, x are
   * the layer weights and b is the layer bias.
   *
   * @param input Input data used for evaluating the specified function.
   * @param output Resulting output activation.
   */
  void Forward(const InputType& input, OutputType& output);

  /**
   * Ordinary feed backward pass of a neural network, calculating the function
   * f(x) by propagating x backwards trough f. Using the results from the feed
   * forward pass.
   *
   * To compute the downstream gradient (g) the chain rule is used.
   *
   * @param * (input) The propagated input activation.
   * @param gy The backpropagated error.
   * @param g The calculated gradient.
   */
  void Backward(const InputType& /* input */,
                const OutputType& gy,
                OutputType& g);

  /**
   * Calculate the gradient using the output delta and the input activation.
   *
   * @param input The input parameter used for calculating the gradient.
   * @param error The calculated error.
   * @param gradient The calculated gradient.
   */
  void Gradient(const InputType& input,
                const OutputType& error,
                OutputType& gradient);

  //! Get the parameters.
  const OutputType& Parameters() const { return weights; }
  //! Modify the parameters.
  OutputType& Parameters() { return weights; }

  //! Get the weight of the layer.
  OutputType const& Weight() const { return weight; }
  //! Modify the weight of the layer.
  OutputType& Weight() { return weight; }

  //! Get the bias of the layer.
  OutputType const& Bias() const { return bias; }
  //! Modify the bias weights of the layer.
  OutputType& Bias() { return bias; }

  //! Get the size of the weights.
  size_t WeightSize() const
  {
    return (inSize * outSize) + outSize;
  }

  void ComputeOutputDimensions()
  {
    inSize = std::accumulate(this->inputDimensions.begin(),
        this->inputDimensions.end(), 0);
    this->outputDimensions = std::vector<size_t>(this->inputDimensions.size(),
        1);

    // The Linear layer flattens its input.
    this->outputDimensions[0] = outSize;
  }

  //! Serialize the layer.
  template<typename Archive>
  void serialize(Archive& ar, const uint32_t /* version */);

 private:
  //! Locally-stored number of input units.
  size_t inSize;

  //! Locally-stored number of output units.
  size_t outSize;

  //! Locally-stored weight object.
  OutputType weights;

  //! Locally-stored weight parameters.
  OutputType weight;

  //! Locally-stored bias term parameters.
  OutputType bias;

  //! Locally-stored regularizer object.
  RegularizerType regularizer;
}; // class LinearType

// Convenience typedefs.

// Standard Linear layer using no regularization.
typedef LinearType<arma::mat, arma::mat, NoRegularizer> Linear;

} // namespace ann
} // namespace mlpack

// Include implementation.
#include "linear_impl.hpp"

#endif
