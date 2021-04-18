/**
 * @file methods/ann/layer/batch_norm.hpp
 * @author Praveen Ch
 * @author Manthan-R-Sheth
 *
 * Definition of the Batch Normalization layer class.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_METHODS_ANN_LAYER_BATCHNORM_HPP
#define MLPACK_METHODS_ANN_LAYER_BATCHNORM_HPP

#include <mlpack/prereqs.hpp>
#include "layer.hpp"

namespace mlpack {
namespace ann /** Artificial Neural Network. */ {

/**
 * Declaration of the Batch Normalization layer class. The layer transforms
 * the input data into zero mean and unit variance and then scales and shifts
 * the data by parameters, gamma and beta respectively. These parameters are
 * learnt by the network.
 *
 * If deterministic is false (training), the mean and variance over the batch is
 * calculated and the data is normalized. If it is set to true (testing) then
 * the mean and variance accrued over the training set is used.
 *
 * For more information, refer to the following paper,
 *
 * @code
 * @article{Ioffe15,
 *   author    = {Sergey Ioffe and
 *                Christian Szegedy},
 *   title     = {Batch Normalization: Accelerating Deep Network Training by
 *                Reducing Internal Covariate Shift},
 *   journal   = {CoRR},
 *   volume    = {abs/1502.03167},
 *   year      = {2015},
 *   url       = {http://arxiv.org/abs/1502.03167},
 *   eprint    = {1502.03167},
 * }
 * @endcode
 *
 * @tparam InputType Type of the input data (arma::colvec, arma::mat,
 *         arma::sp_mat or arma::cube).
 * @tparam OutputType Type of the output data (arma::colvec, arma::mat,
 *         arma::sp_mat or arma::cube).
 */
template <
  typename InputType = arma::mat,
  typename OutputType = arma::mat
>
class BatchNormType : public Layer<InputType, OutputType>
{
 public:
  //! Create the BatchNorm object.
  BatchNormType();

  /**
   * Create the BatchNorm layer object for a specified number of input units.
   *
   * @param size The number of input units / channels.
   * @param eps The epsilon added to variance to ensure numerical stability.
   * @param average Boolean to determine whether cumulative average is used for
   *                updating the parameters or momentum is used.
   * @param momentum Parameter used to to update the running mean and variance.
   */
  BatchNormType(const size_t size,
                const double eps = 1e-8,
                const bool average = true,
                const double momentum = 0.1);

  /**
   * Reset the layer parameters
   */
  void Reset();

  /**
   * Forward pass of the Batch Normalization layer. Transforms the input data
   * into zero mean and unit variance, scales the data by a factor gamma and
   * shifts it by beta.
   *
   * @param input Input data for the layer
   * @param output Resulting output activations.
   */
  void Forward(const InputType& input, OutputType& output);

  /**
   * Backward pass through the layer.
   *
   * @param input The input activations
   * @param gy The backpropagated error.
   * @param g The calculated gradient.
   */
  void Backward(const InputType& input,
                const InputType& gy,
                OutputType& g);

  /**
   * Calculate the gradient using the output delta and the input activations.
   *
   * @param input The input activations
   * @param error The calculated error
   * @param gradient The calculated gradient.
   */
  void Gradient(const InputType& /* input */,
                const InputType& error,
                OutputType& gradient);

  //! Clone the BatchNormType object. This handles polymorphism correctly.
  BatchNormType* Clone() const { return new BatchNormType(*this); }

  //! Get the parameters.
  OutputType const& Parameters() const { return weights; }
  //! Modify the parameters.
  OutputType& Parameters() { return weights; }

  //! Get the output parameter.
  OutputType const& OutputParameter() const { return outputParameter; }
  //! Modify the output parameter.
  OutputType& OutputParameter() { return outputParameter; }

  //! Get the delta.
  OutputType const& Delta() const { return delta; }
  //! Modify the delta.
  OutputType& Delta() { return delta; }

  //! Get the gradient.
  OutputType const& Gradient() const { return gradient; }
  //! Modify the gradient.
  OutputType& Gradient() { return gradient; }

  //! Get the value of deterministic parameter.
  bool const& Deterministic() const { return deterministic; }
  //! Modify the value of deterministic parameter.
  bool& Deterministic() { return deterministic; }

  //! Get the mean over the training data.
  OutputType const& TrainingMean() const { return runningMean; }
  //! Modify the mean over the training data.
  OutputType& TrainingMean() { return runningMean; }

  //! Get the variance over the training data.
  OutputType const& TrainingVariance() const { return runningVariance; }
  //! Modify the variance over the training data.
  OutputType& TrainingVariance() { return runningVariance; }

  //! Get the number of input units / channels.
  size_t InputSize() const { return size; }

  //! Get the epsilon value.
  double Epsilon() const { return eps; }

  //! Get the momentum value.
  double Momentum() const { return momentum; }

  //! Get the average parameter.
  bool Average() const { return average; }

  //! Get size of weights.
  size_t WeightSize() const { return 2 * size; }

  /**
   * Serialize the layer
   */
  template<typename Archive>
  void serialize(Archive& ar, const uint32_t /* version */);

 private:
  //! Locally-stored number of input units.
  size_t size;

  //! Locally-stored epsilon value.
  double eps;

  //! If true use average else use momentum for computing running mean
  //! and variance
  bool average;

  //! Locally-stored value for momentum.
  double momentum;

  //! Variable to keep track of whether we are in loading or saving mode.
  bool loading;

  //! Locally-stored scale parameter.
  OutputType gamma;

  //! Locally-stored shift parameter.
  OutputType beta;

  //! Locally-stored mean object.
  OutputType mean;

  //! Locally-stored variance object.
  OutputType variance;

  //! Locally-stored parameters.
  OutputType weights;

  /**
   * If true then mean and variance over the training set will be considered
   * instead of being calculated over the batch.
   */
  bool deterministic;

  //! Locally-stored running mean/variance counter.
  size_t count;

  //! Locally-stored value for average factor which used to update running
  //! mean and variance.
  double averageFactor;

  //! Locally-stored mean object.
  OutputType runningMean;

  //! Locally-stored variance object.
  OutputType runningVariance;

  //! Locally-stored gradient object.
  OutputType gradient;

  //! Locally-stored delta object.
  OutputType delta;

  //! Locally-stored output parameter object.
  OutputType outputParameter;

  //! Locally-stored normalized input.
  arma::cube normalized;

  //! Locally-stored zero mean input.
  arma::cube inputMean;
}; // class BatchNorm

// Standard Batch Norm layer.
typedef BatchNormType<arma::mat, arma::mat> BatchNorm;

} // namespace ann
} // namespace mlpack

// Include the implementation.
#include "batch_norm_impl.hpp"

#endif
