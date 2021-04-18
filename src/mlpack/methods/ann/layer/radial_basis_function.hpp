/**
 * @file radial_basis_function.hpp
 * @author Himanshu Pathak
 *
 * Definition of the Radial Basis Function module class.
 *
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_METHODS_ANN_LAYER_RBF_HPP
#define MLPACK_METHODS_ANN_LAYER_RBF_HPP

#include <mlpack/prereqs.hpp>
#include <mlpack/methods/ann/activation_functions/gaussian_function.hpp>

#include "layer_types.hpp"
#include "layer.hpp"

namespace mlpack {
namespace ann /** Artificial Neural Network. */ {


/**
 * Implementation of the Radial Basis Function layer. The RBF class when use with a 
 * non-linear activation function acts as a Radial Basis Function which can be used
 * with Feed-Forward neural network.
 *
 * For more information, refer to the following paper,
 *
 * @code
 * @article{Volume 51: Artificial Intelligence and Statistics,
 *   author  = {Qichao Que, Mikhail Belkin},
 *   title   = {Back to the Future: Radial Basis Function Networks Revisited},
 *   year    = {2016},
 *   url     = {http://proceedings.mlr.press/v51/que16.pdf},
 * }
 * @endcode
 *
 * @tparam InputType Type of the input data (arma::colvec, arma::mat,
 *         arma::sp_mat or arma::cube).
 * @tparam OutputType Type of the output data (arma::colvec, arma::mat,
 *         arma::sp_mat or arma::cube).
 * @tparam Activation Type of the activation function (mlpack::ann::Gaussian).
 */

template <
    typename InputType = arma::mat,
    typename OutputType = arma::mat,
    typename Activation = GaussianFunction
>
class RBFType : public Layer<InputType, OutputType>
{
 public:
  //! Create the RBF object.
  RBFType();

  /**
   * Create the Radial Basis Function layer object using the specified
   * parameters.
   *
   * @param inSize The number of input units.
   * @param outSize The number of output units.
   * @param centres The centres calculated using k-means of data.
   * @param betas The beta value to be used with centres.
   */
  RBFType(const size_t inSize,
          const size_t outSize,
          arma::mat& centres,
          double betas = 0);

  /**
   * Ordinary feed forward pass of the radial basis function.
   *
   * @param input Input data used for evaluating the specified function.
   * @param output Resulting output activation.
   */
  void Forward(const InputType& input, OutputType& output);

  /**
   * Ordinary feed backward pass of the radial basis function.
   *
   */
  void Backward(const InputType& /* input */,
                const InputType& /* gy */,
                OutputType& /* g */);

  //! Clone the RBFType object. This handles polymorphism correctly.
  RBFType* Clone() const { return new RBFType(*this); }

  //! Get the output parameter.
  OutputType const& OutputParameter() const { return outputParameter; }
  //! Modify the output parameter.
  OutputType& OutputParameter() { return outputParameter; }
  //! Get the parameters.

  //! Get the input parameter.
  InputType const& InputParameter() const { return inputParameter; }
  //! Modify the input parameter.
  InputType& InputParameter() { return inputParameter; }

  //! Get the input size.
  size_t InputSize() const { return inSize; }

  //! Get the output size.
  size_t OutputSize() const { return outSize; }

  //! Get the detla.
  OutputType const& Delta() const { return delta; }
  //! Modify the delta.
  OutputType& Delta() { return delta; }

  /**
   * Serialize the layer.
   */
  template<typename Archive>
  void serialize(Archive& ar, const uint32_t /* version */);

 private:
  //! Locally-stored number of input units.
  size_t inSize;

  //! Locally-stored number of output units.
  size_t outSize;

  //! Locally-stored delta object.
  OutputType delta;

  //! Locally-stored output parameter object.
  OutputType outputParameter;

  //! Locally-stored the sigmas values.
  double sigmas;

  //! Locally-stored the betas values.
  double betas;

  //! Locally-stored the learnable centre of the shape.
  InputType centres;

  //! Locally-stored input parameter object.
  InputType inputParameter;

  //! Locally-stored the output distances of the shape.
  OutputType distances;
}; // class RBF

// Standaed RBF layer
typedef RBFType<arma::mat, arma::mat, GaussianFunction> RBF;

} // namespace ann
} // namespace mlpack

// Include implementation.
#include "radial_basis_function_impl.hpp"

#endif
