/**
 * @file methods/ann/ffn.hpp
 * @author Marcus Edel
 * @author Shangtong Zhang
 *
 * Definition of the FFN class, which implements feed forward neural networks.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_METHODS_ANN_FFN_HPP
#define MLPACK_METHODS_ANN_FFN_HPP

#include <mlpack/prereqs.hpp>

#include "init_rules/network_init.hpp"

#include <mlpack/methods/ann/layer/layer.hpp>
#include <mlpack/methods/ann/init_rules/random_init.hpp>
#include <mlpack/methods/ann/loss_functions/negative_log_likelihood.hpp>
#include <mlpack/methods/ann/layer/layer_traits.hpp>
#include <ensmallen.hpp>

namespace mlpack {
namespace ann /** Artificial Neural Network. */ {

/**
 * Implementation of a standard feed forward network.
 *
 * @tparam OutputLayerType The output layer type used to evaluate the network.
 * @tparam InitializationRuleType Rule used to initialize the weight matrix.
 */
template<
    typename OutputLayerType = NegativeLogLikelihood<>,
    typename InitializationRuleType = RandomInitialization,
    typename InputType = arma::mat,
    typename OutputType = arma::mat>
class FFN
{
 public:
  //! Convenience typedef for the internal model construction.
  using NetworkType = FFN<OutputLayerType,
                          InitializationRuleType,
                          InputType,
                          OutputType>;

  /**
   * Create the FFN object.
   *
   * Optionally, specify which initialize rule and performance function should
   * be used.
   *
   * If you want to pass in a parameter and discard the original parameter
   * object, be sure to use std::move to avoid unnecessary copy.
   *
   * @param outputLayer Output layer used to evaluate the network.
   * @param initializeRule Optional instantiated InitializationRule object
   *        for initializing the network parameter.
   */
  FFN(OutputLayerType outputLayer = OutputLayerType(),
      InitializationRuleType initializeRule = InitializationRuleType());

  //! Copy constructor.
  FFN(const FFN&);

  //! Move constructor.
  FFN(FFN&&);

  //! Copy/move assignment operator.
  FFN& operator=(FFN);

  //! Destructor to release allocated memory.
  ~FFN();

  /**
   * Check if the optimizer has MaxIterations() parameter, if it does
   * then check if it's value is less than the number of datapoints
   * in the dataset.
   *
   * @tparam OptimizerType Type of optimizer to use to train the model.
   * @param optimizer optimizer used in the training process.
   * @param samples Number of datapoints in the dataset.
   */
  template<typename OptimizerType>
  typename std::enable_if<
      HasMaxIterations<OptimizerType, size_t&(OptimizerType::*)()>
      ::value, void>::type
  WarnMessageMaxIterations(OptimizerType& optimizer, size_t samples) const;

  /**
   * Check if the optimizer has MaxIterations() parameter, if it
   * doesn't then simply return from the function.
   *
   * @tparam OptimizerType Type of optimizer to use to train the model.
   * @param optimizer optimizer used in the training process.
   * @param samples Number of datapoints in the dataset.
   */
  template<typename OptimizerType>
  typename std::enable_if<
      !HasMaxIterations<OptimizerType, size_t&(OptimizerType::*)()>
      ::value, void>::type
  WarnMessageMaxIterations(OptimizerType& optimizer, size_t samples) const;

  /**
   * Train the feedforward network on the given input data using the given
   * optimizer.
   *
   * This will use the existing model parameters as a starting point for the
   * optimization. If this is not what you want, then you should access the
   * parameters vector directly with Parameters() and modify it as desired.
   *
   * If you want to pass in a parameter and discard the original parameter
   * object, be sure to use std::move to avoid unnecessary copy.
   *
   * @tparam OptimizerType Type of optimizer to use to train the model.
   * @tparam CallbackTypes Types of Callback Functions.
   * @param predictors Input training variables.
   * @param responses Outputs results from input training variables.
   * @param optimizer Instantiated optimizer used to train the model.
   * @param callbacks Callback function for ensmallen optimizer `OptimizerType`.
   *      See https://www.ensmallen.org/docs.html#callback-documentation.
   * @return The final objective of the trained model (NaN or Inf on error).
   */
  template<typename OptimizerType, typename... CallbackTypes>
  double Train(InputType predictors,
               InputType responses,
               OptimizerType& optimizer,
               CallbackTypes&&... callbacks);

  /**
   * Train the feedforward network on the given input data. By default, the
   * RMSProp optimization algorithm is used, but others can be specified
   * (such as ens::SGD).
   *
   * This will use the existing model parameters as a starting point for the
   * optimization. If this is not what you want, then you should access the
   * parameters vector directly with Parameters() and modify it as desired.
   *
   * If you want to pass in a parameter and discard the original parameter
   * object, be sure to use std::move to avoid unnecessary copy.
   *
   * @tparam OptimizerType Type of optimizer to use to train the model.
   * @param predictors Input training variables.
   * @tparam CallbackTypes Types of Callback Functions.
   * @param responses Outputs results from input training variables.
   * @param callbacks Callback function for ensmallen optimizer `OptimizerType`.
   *      See https://www.ensmallen.org/docs.html#callback-documentation.
   * @return The final objective of the trained model (NaN or Inf on error).
   */
  template<typename OptimizerType = ens::RMSProp, typename... CallbackTypes>
  double Train(InputType predictors,
               InputType responses,
               CallbackTypes&&... callbacks);

  /**
   * Predict the responses to a given set of predictors. The responses will
   * reflect the output of the given output layer as returned by the
   * output layer function.
   *
   * If you want to pass in a parameter and discard the original parameter
   * object, be sure to use std::move to avoid unnecessary copy.
   *
   * @param predictors Input predictors.
   * @param results Matrix to put output predictions of responses into.
   * @param batchSize Batch size to use for prediction.
   */
  void Predict(InputType predictors,
               OutputType& results,
               const size_t batchSize = 128);

  /**
   * Evaluate the feedforward network with the given predictors and responses.
   * This functions is usually used to monitor progress while training.
   *
   * @param predictors Input variables.
   * @param responses Target outputs for input variables.
   */
  template<typename PredictorsType, typename ResponsesType>
  double Evaluate(const PredictorsType& predictors,
                  const ResponsesType& responses);

  /**
   * Evaluate the feedforward network with the given parameters. This function
   * is usually called by the optimizer to train the model.
   *
   * @param parameters Matrix model parameters.
   */
  double Evaluate(const OutputType& parameters);

   /**
   * Evaluate the feedforward network with the given parameters, but using only
   * a number of data points. This is useful for optimizers such as SGD, which
   * require a separable objective function.
   *
   * Note that the network may return different results depending on the mode it
   * is in (see `SetNetworkMode()`).
   *
   * @param parameters Matrix model parameters.
   * @param begin Index of the starting point to use for objective function
   *        evaluation.
   * @param batchSize Number of points to be passed at a time to use for
   *        objective function evaluation.
   */
  double Evaluate(const OutputType& parameters,
                  const size_t begin,
                  const size_t batchSize);

  /**
   * Evaluate the feedforward network with the given parameters.
   * This function is usually called by the optimizer to train the model.
   * This just calls the overload of EvaluateWithGradient() with batchSize = 1.
   *
   * @param parameters Matrix model parameters.
   * @param gradient Matrix to output gradient into.
   */
  double EvaluateWithGradient(const OutputType& parameters,
                              OutputType& gradient);

   /**
   * Evaluate the feedforward network with the given parameters, but using only
   * a number of data points. This is useful for optimizers such as SGD, which
   * require a separable objective function.
   *
   * @param parameters Matrix model parameters.
   * @param begin Index of the starting point to use for objective function
   *        evaluation.
   * @param gradient Matrix to output gradient into.
   * @param batchSize Number of points to be passed at a time to use for
   *        objective function evaluation.
   */
  double EvaluateWithGradient(const OutputType& parameters,
                              const size_t begin,
                              OutputType& gradient,
                              const size_t batchSize);

  /**
   * Evaluate the gradient of the feedforward network with the given parameters,
   * and with respect to only a number of points in the dataset. This is useful
   * for optimizers such as SGD, which require a separable objective function.
   *
   * @param parameters Matrix of the model parameters to be optimized.
   * @param begin Index of the starting point to use for objective function
   *        gradient evaluation.
   * @param gradient Matrix to output gradient into.
   * @param batchSize Number of points to be processed as a batch for objective
   *        function gradient evaluation.
   */
  void Gradient(const OutputType& parameters,
                const size_t begin,
                OutputType& gradient,
                const size_t batchSize);

  /**
   * Shuffle the order of function visitation. This may be called by the
   * optimizer.
   */
  void Shuffle();

  /**
   * Add a new module to the model.
   *
   * @param args The layer parameter.
   */
  template <class LayerType, class... Args>
  void Add(Args... args)
  {
    network.push_back(new LayerType(args...));
    layerOutputs.push_back(OutputType());
    layerDeltas.push_back(OutputType());
    layerGradients.push_back(OutputType());
    inputDimensionsAreSet = false;
  }

  /**
   * Add a new module to the model.
   *
   * @param layer The Layer to be added to the model.
   */
  //! TODO: if weights are set in this layer, we should copy them and update our
  //cached parameters
  void Add(Layer<InputType, OutputType>* layer)
  {
    network.push_back(layer);
    layerOutputs.push_back(OutputType());
    layerDeltas.push_back(OutputType());
    layerGradients.push_back(OutputType());
    inputDimensionsAreSet = false;
  }

  //! Get the network model.
  const std::vector<Layer<InputType, OutputType>*>& Model() const
  {
    return network;
  }

  /**
   * Modify the network model.  Be careful!  If you change the structure of the
   * network or parameters for layers, its state may become invalid, so be sure
   * to call ResetParameters() afterwards.
   */
  std::vector<Layer<InputType, OutputType>*>& Model() { return network; }

  //! Return the number of separable functions (the number of predictor points).
  size_t NumFunctions() const { return responses.n_cols; }

  //! Return the initial point for the optimization.
  const OutputType& Parameters() const { return parameters; }
  //! Modify the initial point for the optimization.
  OutputType& Parameters() { return parameters; }

  //! Get the matrix of responses to the input data points.
  const InputType& Responses() const { return responses; }
  //! Modify the matrix of responses to the input data points.
  InputType& Responses() { return responses; }

  //! Get the matrix of data points (predictors).
  const InputType& Predictors() const { return predictors; }
  //! Modify the matrix of data points (predictors).
  InputType& Predictors() { return predictors; }

  /**
   * Reset the stored data of the network entirely.  This resets all weights of
   * each layer, and prepares the network to accept an input size of
   * `inputDimensionality` (if passed), or whatever input size has been set
   * with `InputDimensions()`.
   *
   * This also resets the mode of the network to prediction mode (not training
   * mode).  See `SetNetworkMode()` for more information.
   */
  void Reset(const size_t inputDimensionality = 0);

  /**
   * Set all the layers in the network to training mode, if `training` is
   * `true`, or set all the layers in the network to testing mode, if `training`
   * is `false`.
   */
  void SetNetworkMode(const bool training);

  //! Serialize the model.
  template<typename Archive>
  void serialize(Archive& ar, const uint32_t /* version */);

  /**
   * Perform the forward pass of the data in real batch mode.
   *
   * Forward and Backward should be used as a pair, and they are designed mainly
   * for advanced users. User should try to use Predict and Train unless those
   * two functions can't satisfy some special requirements.
   *
   * @param inputs The input data.
   * @param results The predicted results.
   */
  template<typename PredictorsType, typename ResponsesType>
  void Forward(const PredictorsType& inputs, ResponsesType& results);

  /**
   * Perform a partial forward pass of the data.
   *
   * This function is meant for the cases when users require a forward pass only
   * through certain layers and not the entire network.
   *
   * @param inputs The input data for the specified first layer.
   * @param results The predicted results from the specified last layer.
   * @param begin The index of the first layer.
   * @param end The index of the last layer.
   */
  template<typename PredictorsType, typename ResponsesType>
  void Forward(const PredictorsType& inputs ,
               ResponsesType& results,
               const size_t begin,
               const size_t end);

  /**
   * Perform the backward pass of the data in real batch mode.
   *
   * Forward and Backward should be used as a pair, and they are designed mainly
   * for advanced users. User should try to use Predict and Train unless those
   * two functions can't satisfy some special requirements.
   *
   * @param inputs Inputs of current pass.
   * @param targets The training target.
   * @param gradients Computed gradients.
   * @return Training error of the current pass.
   */
  template<typename PredictorsType,
           typename TargetsType,
           typename GradientsType>
  double Backward(const PredictorsType& inputs,
                  const TargetsType& targets,
                  GradientsType& gradients);

  /**
   * Set the logical dimensions of the input.
   *
   * TODO: better comment.  You would call this when you want to, e.g., pass an
   * n-dimensional tensor, so that you can specify each of those n dimensions.
   */
  // Note: we don't need to invalidate any caches, because any forward pass will
  // already check if the input dimensions have changed.
  std::vector<size_t>& InputDimensions() { return inputDimensions; }
  //! Get the logical dimensions of the input.
  const std::vector<size_t>& InputDimensions() const
  {
    // The user may change the input dimensions, so we will have to propagate
    // these changes to the network.
    inputDimensionsAreSet = false;
    return inputDimensions;
  }

 private:
  // Helper functions.

  /**
   * Prepare the network for the given data.
   * This function won't actually trigger training process.
   *
   * @param predictors Input data variables.
   * @param responses Outputs results from input data variables.
   */
  void ResetData(InputType predictors, InputType responses);

  //! Use the InitializationPolicy to initialize all the weights in the network.
  void InitializeWeights();

  //! Make the memory of each layer point to the right place, by calling
  //! SetWeightPtr() on each layer.
  void SetLayerMemory();

  /**
   * The Backward algorithm (part of the Forward-Backward algorithm). Computes
   * backward pass for module.
   */
  void Backward();

  /**
   * Iterate through all layer modules and update the the gradient using the
   * layer defined optimizer.
   */
  void Gradient(const InputType& input, OutputType& gradient);

  /**
   * Ensure that all the locally-cached information about the network is valid,
   * all parameter memory is initialized, and we can make forward and backward
   * passes.
   *
   * @param functionName Name of function to use if an exception is thrown.
   * @param inputDimensionality Given dimensionality of the input data.
   * @param setMode If true, the mode of the network will be set to the
   *     parameter given in `training`.  Otherwise the mode of the network is
   *     left unmodified.
   * @param training Mode to set the network to; `true` indicates the network
   *     should be set to training mode; `false` indicates testing mode.
   */
  void CheckNetwork(const std::string& functionName,
                    const size_t inputDimensionality,
                    const bool setMode = false,
                    const bool training = false);

  /**
   * Set the input and output dimensions of each layer in the network correctly.
   * The size of the input is taken, in case `inputDimensions` has not been set
   * otherwise (e.g. via `InputDimensions()`).
   */
  void UpdateDimensions(const std::string& functionName,
                        const size_t inputDimensionality);

  /**
   * Initialize memory to be used for storing the outputs of each layer, if
   * necessary.
   */
  void InitializeForwardPassMemory(const size_t batchSize);
  void InitializeBackwardPassMemory(const size_t batchSize);
  void InitializeGradientPassMemory(OutputType& gradient);

  /**
   * Swap the content of this network with given network.
   *
   * @param network Desired source network.
   */
  void Swap(FFN& network);

  //! Instantiated outputlayer used to evaluate the network.
  OutputLayerType outputLayer;

  //! Instantiated InitializationRule object for initializing the network
  //! parameter.
  InitializationRuleType initializeRule;

  //! Locally-stored model modules.
  std::vector<Layer<InputType, OutputType>*> network;

  /**
   * Matrix of (trainable) parameters.  Each weight here corresponds to a layer,
   * and each layer's `parameters` member is an alias pointing to parameters in
   * this matrix.
   *
   * Note: although each layer may have its own InputType and OutputType,
   * ensmallen optimization requires everything to be stored in one matrix
   * object, so we have chosen OutputType.  This could be made more flexible
   * with a "wrapper" class implementing the Armadillo API.
   */
  OutputType parameters;

  //! Dimensions of input data.
  std::vector<size_t> inputDimensions;

  //! The matrix of data points (predictors).  This member is empty, except
  //! during training---we must store a local copy of the training data since
  //! the ensmallen optimizer will not provide training data.
  InputType predictors;

  //! The matrix of responses to the input data points.  This member is empty,
  //! except during training.
  InputType responses;

  //! The current error for the backward pass.
  OutputType error;

  //! The current evaluation mode (training or testing).
  bool training;

  //! If true, each layer has its memory properly set for a forward/backward
  //! pass.
  bool layerMemoryIsSet;

  //! If true, each layer has its inputDimensions properly set, and
  //! `totalInputSize` and `totalOutputSize` are valid.
  bool inputDimensionsAreSet;

  //! Cached total number of input elements across all layers (for deltaMatrix
  //! and layerDeltas).
  size_t totalInputSize;
  //! Cached total number of output elements across all layers (for
  //! layerOutputMatrix and layerOutputs).
  size_t totalOutputSize;

  //! Locally-stored output parameter object.  This holds the results of
  //! Forward() for each layer, all in one matrix.
  OutputType layerOutputMatrix;
  //! Aliases to different parts of layerOutputMatrix, for convenience.
  //! layerOutputs[i] stores the results of Forward() for layer i.
  std::vector<OutputType> layerOutputs;

  //! Locally-stored delta object.
  OutputType deltaMatrix;
  std::vector<OutputType> layerDeltas;

  //! Aliases to different parts of the gradient, for convenience.
  //! gradientOutputs[i] stores the results of Gradient() for layer i.  These
  //! elements are only valid inside of Gradient().
  std::vector<OutputType> layerGradients;
}; // class FFN

// Utility function to make `m` an alias of the given memory at `newMem`, with a
// size of numElem x 1.
template<typename MatType>
void MakeAlias(MatType& m,
               typename MatType::elem_type* newMem,
               const size_t numRows,
               const size_t numCols);

} // namespace ann
} // namespace mlpack

// Include implementation.
#include "ffn_impl.hpp"

#endif
