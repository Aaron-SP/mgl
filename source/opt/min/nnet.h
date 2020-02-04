/* Copyright [2013-2018] [Aaron Springstroh, Minimal Graphics Library]

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#ifndef _MGL_NEURAL_NET_FIXED_MGL_
#define _MGL_NEURAL_NET_FIXED_MGL_

#include <algorithm>
#include <functional>
#include <iostream>
#include <min/nn.h>
#include <min/vec.h>
#include <vector>

namespace min
{

template <typename T>
class nnode
{
  private:
    std::vector<T> _weights;
    std::vector<T> _delta_weights;
    mutable std::vector<T> _inputs;
    T _bias;
    mutable T _sum;
    mutable T _output;
    T _delta;

    inline static void range(T &weight)
    {
        constexpr T _low_range = -1E6;
        constexpr T _high_range = 1E6;
// BUG HERE IN MSVC COMPILER
#if _MSC_VER && !__INTEL_COMPILER
        const T min1 = (weight < _high_range) ? weight : _high_range;
        weight = (min1 > _low_range) ? min1 : _low_range;
#else
        weight = std::max(_low_range, std::min(weight, _high_range));
#endif
    }
    inline static T transfer_deriv_identity(const T output)
    {
        return 1.0;
    }
    inline static T transfer_identity(const T input)
    {
        return input;
    }
    inline static T transfer_deriv_relu(const T output)
    {
        const T arg = -1.0 * output;
        if (arg > 20.0)
        {
            return 0.0;
        }
        else if (arg < -20.0)
        {
            return 1.0;
        }

        return 1.0 / (1.0 + std::exp(arg));
    }
    inline static T transfer_relu(const T input)
    {
        return std::log(1.0 + std::exp(input));
    }
    inline static T transfer_deriv_sigmoid(const T output)
    {
        return output * (1.0 - output);
    }
    inline static T transfer_sigmoid(const T input)
    {
        const T arg = -1.0 * input;
        if (arg > 20.0)
        {
            return 0.0;
        }
        else if (arg < -20.0)
        {
            return 1.0;
        }

        return 1.0 / (1.0 + std::exp(arg));
    }
    inline static T transfer_deriv_tanh(const T output)
    {
        return 1.0 - (output * output);
    }
    inline static T transfer_tanh(const T input)
    {
        const T arg = -2.0 * input;
        if (arg > 20.0)
        {
            return 0.0;
        }
        else if (arg < -20.0)
        {
            return 1.0;
        }

        return (2.0 / (1.0 + std::exp(arg))) - 1.0;
    }
    inline void backprop(const T step_size)
    {
        // Calculate the delta sum term
        const size_t size = _inputs.size();
        for (size_t i = 0; i < size; i++)
        {
            _delta_weights[i] = (_delta * _weights[i]);
        }

        // Calculate step by step size
        const T step = step_size * _delta;

        // Update weights
        for (size_t i = 0; i < size; i++)
        {
            // Update weights
            _weights[i] -= (step * _inputs[i]);
        }

        // Update bias
        _bias -= step;
    }
    inline void zero() const
    {
        _sum = _bias;
        _output = 0.0;
    }

  public:
    nnode(const size_t size)
        : _weights(size, 1.0), _delta_weights(size, 0), _inputs(size, 0),
          _bias(0.0), _sum(0.0), _output(0.0), _delta(0.0) {}
    inline void reset(std::mt19937 &gen, min::net_rng<T> &ran)
    {
        // Randomize weights
        for (T &w : _weights)
        {
            w = ran.random(gen);
        }

        // Zero out weights vectors
        std::fill(_delta_weights.begin(), _delta_weights.end(), 0.0);
        std::fill(_inputs.begin(), _inputs.end(), 0.0);

        // Reset node values
        _bias = ran.random(gen);
        _sum = 0.0;
        _output = 0.0;
        _delta = 0.0;
    }
    inline nnode<T> &operator*=(const nnode<T> &n)
    {
        const size_t size = n._weights.size();
        for (size_t i = 0; i < size; i++)
        {
            // average weights
            _weights[i] = (_weights[i] + n._weights[i]) * 0.5;

            // Check for weight overflow
            range(_weights[i]);
        }

        // average bias
        _bias = (_bias + n._bias) * 0.5;

        // Check for bias overflow
        range(_bias);

        return *this;
    }
    inline void backprop_identity(const T propagated, const T step_size)
    {
        // propagated = sum(dk * Wjk) or if last layer (Ok - tk)
        // dj = 1.0 * propagated
        _delta = transfer_deriv_identity(_output) * propagated;

        // Calculate steps for weights and biases
        backprop(step_size);
    }
    inline void backprop_relu(const T propagated, const T step_size)
    {
        // propagated = sum(dk * Wjk) or if last layer (Ok - tk)
        // dj = 1.0/(1.0+exp(-x)) * propagated
        _delta = transfer_deriv_relu(_output) * propagated;

        // Calculate steps for weights and biases
        backprop(step_size);
    }
    inline void backprop_sigmoid(const T propagated, const T step_size)
    {
        // propagated = sum(dk * Wjk) or if last layer (Ok - tk)
        // dj = (Oj) * (1.0 - Oj) * propagated
        _delta = transfer_deriv_sigmoid(_output) * propagated;

        // Calculate steps for weights and biases
        backprop(step_size);
    }
    inline void backprop_tanh(const T propagated, const T step_size)
    {
        // propagated = sum(dk * Wjk) or if last layer (Ok - tk)
        // dj = (1.0 - (Oj * Oj)) * propagated
        _delta = transfer_deriv_tanh(_output) * propagated;

        // Calculate steps for weights and biases
        backprop(step_size);
    }
    inline void calculate_identity() const
    {
        // Calculate transfer
        _output = transfer_identity(_sum);
    }
    inline void calculate_relu() const
    {
        // Calculate transfer
        _output = transfer_relu(_sum);
    }
    inline void calculate_sigmoid() const
    {
        // Calculate transfer
        _output = transfer_sigmoid(_sum);
    }
    inline void calculate_tanh() const
    {
        // Calculate transfer
        _output = transfer_tanh(_sum);
    }
    inline T delta(const size_t index) const
    {
        // dk * Wjk in backprop
        return _delta_weights[index];
    }
    inline T get_bias() const
    {
        return _bias;
    }
    inline size_t get_inputs() const
    {
        return _weights.size();
    }
    inline const std::vector<T> &get_weights() const
    {
        return _weights;
    }
    inline std::vector<T> &get_weights()
    {
        return _weights;
    }
    inline void mutate(std::mt19937 &gen, min::net_rng<T> &ran)
    {
        // Calculate a mutation type
        const unsigned r = ran.random_int(gen);

        // calculate random weight index
        const unsigned index = ran.random_int(gen) % _weights.size();

        // Mutate the node based on type
        if (r % 2 == 0)
        {
            // Mutate the weight with mult
            _weights[index] *= ran.mutation(gen);
        }
        else if (r % 3 == 0)
        {
            // Mutate the bias with mult
            _bias += ran.mutation(gen);
        }
        else if (r % 5 == 0)
        {
            // Mutate the weight with add
            _weights[index] += ran.mutation(gen);
        }
        else if (r % 7 == 0)
        {
            // Mutate the bias with add
            _bias *= ran.mutation(gen);
        }
        else if (r % 11 == 0)
        {
            // Assign random values
            _weights[index] = ran.random(gen);
            _bias = ran.random(gen);
        }

        // Check for weight and bias overflow
        range(_weights[index]);
        range(_bias);
    }
    inline T output() const
    {
        return _output;
    }
    inline void reset() const
    {
        // Reset the node
        zero();
    }
    inline void set_bias(const T bias)
    {
        _bias = bias;
    }
    inline void sum(const T input, const size_t index) const
    {
        // Store input for later
        _inputs[index] = input;

        // Sum input
        _sum += input * _weights[index];
    }
};

template <typename T>
class nnlayer
{
  private:
    std::vector<nnode<T>> _nodes;
    size_t _inputs;

  public:
    nnlayer(const size_t size, const size_t inputs) : _nodes(size, inputs), _inputs(inputs) {}
    inline size_t size() const
    {
        return _nodes.size();
    }
    inline size_t inputs() const
    {
        return _inputs;
    }
    inline nnode<T> &operator[](const size_t n)
    {
        return _nodes[n];
    }
    inline const nnode<T> &operator[](const size_t n) const
    {
        return _nodes[n];
    }
};

template <typename T, size_t IN, size_t OUT>
class nnet
{
  private:
    mutable vector<T, IN> _input;
    mutable vector<T, OUT> _output;
    std::vector<nnlayer<T>> _layers;
    bool _final;
    bool _linear_output;

    inline void backprop(const std::function<void(nnode<T> &, const T, const T)> &back, const vector<T, OUT> &set_point, const T step_size)
    {
        // We assume the network is in calculated state

        // If we are in a valid state
        if (_layers.size() >= 2)
        {
            // Do backprop for last layer first
            const size_t last = _layers.size() - 1;

            // Check that last layer is the appropriate size
            const size_t last_size = _layers[last].size();
            if (last_size != OUT)
            {
                throw std::runtime_error("nnet: backprop invalid output dimension");
            }

            // If we want a linear output node
            if (_linear_output)
            {
                // For all nodes in last layer
                for (size_t i = 0; i < last_size; i++)
                {
                    const T error = _layers[last][i].output() - set_point[i];

                    // Do backprop for node in last layer
                    _layers[last][i].backprop_identity(error, step_size);
                }
            }
            else
            {
                // For all nodes in last layer
                for (size_t i = 0; i < last_size; i++)
                {
                    const T error = _layers[last][i].output() - set_point[i];

                    // Do backprop for node in last layer
                    back(_layers[last][i], error, step_size);
                }
            }

            // For all internal layers, iterating backwards
            const size_t layers = _layers.size();
            for (size_t i = 1; i < layers; i++)
            {
                size_t current = last - i;
                const size_t nodes = _layers[current].size();
                for (size_t j = 0; j < nodes; j++)
                {
                    // For all nodes in layer, calculate delta summation
                    T sum = 0.0;
                    const size_t size_out = _layers[current + 1].size();
                    for (size_t k = 0; k < size_out; k++)
                    {
                        sum += _layers[current + 1][k].delta(j);
                    }

                    // Do backprop for this node
                    back(_layers[current][j], sum, step_size);
                }
            }
        }
        else
        {
            throw std::runtime_error("nnet: can't backprop, not enough layers");
        }
    }
    inline vector<T, OUT> calculate(const std::function<void(const nnode<T> &)> &calc) const
    {
        if (_final)
        {
            // If we added any layers
            if (_layers.size() >= 2)
            {
                // For each node in first layer
                const size_t in_nodes = _layers[0].size();
                for (size_t i = 0; i < in_nodes; i++)
                {
                    // Reset the layer node
                    _layers[0][i].reset();

                    // Map input to first layer of net
                    for (size_t j = 0; j < IN; j++)
                    {
                        // Calculate sum of inputs
                        _layers[0][i].sum(_input[j], j);
                    }
                }

                // Do N-1 propagations from first layer
                const size_t layers = _layers.size() - 1;
                for (size_t i = 0; i < layers; i++)
                {
                    // For all nodes in out_layer
                    const size_t size_out = _layers[i + 1].size();
                    for (size_t j = 0; j < size_out; j++)
                    {
                        // Reset the layer node
                        _layers[i + 1][j].reset();

                        // For all nodes in in_layer
                        const size_t size_in = _layers[i].size();
                        for (size_t k = 0; k < size_in; k++)
                        {
                            calc(_layers[i][k]);
                            _layers[i + 1][j].sum(_layers[i][k].output(), k);
                        }
                    }
                }

                // Map last layer to output of net
                // Last layer is special and added during finalize so we can just grab the output value
                // From the last internal layer for the output

                // If we want a linear output node
                if (_linear_output)
                {
                    const nnlayer<T> &last = _layers.back();
                    for (size_t i = 0; i < OUT; i++)
                    {
                        last[i].calculate_identity();
                        _output[i] = last[i].output();
                    }
                }
                else
                {
                    const nnlayer<T> &last = _layers.back();
                    for (size_t i = 0; i < OUT; i++)
                    {
                        calc(last[i]);
                        _output[i] = last[i].output();
                    }
                }
            }
            else
            {
                throw std::runtime_error("nnet: can't calculate, not enough layers");
            }
        }
        else
        {
            throw std::runtime_error("nnet: can't calculate, must finalize net");
        }

        return _output;
    }
    inline void on_net(const std::function<void(nnode<T> &, const size_t, const size_t)> &f)
    {
        // For all net layers
        const size_t layers = _layers.size();
        for (size_t i = 0; i < layers; i++)
        {
            // For all layer nodes
            const size_t nodes = _layers[i].size();
            for (size_t j = 0; j < nodes; j++)
            {
                f(_layers[i][j], i, j);
            }
        }
    }
    inline void on_const_net(const std::function<void(const nnode<T> &, const size_t, const size_t)> &f) const
    {
        // For all net layers
        const size_t layers = _layers.size();
        for (size_t i = 0; i < layers; i++)
        {
            // For all layer nodes
            const size_t nodes = _layers[i].size();
            for (size_t j = 0; j < nodes; j++)
            {
                f(_layers[i][j], i, j);
            }
        }
    }

  public:
    nnet() : _final(false), _linear_output(false) {}
    inline void add_layer(const size_t size)
    {
        if (!_final)
        {
            // If first layer
            size_t inputs = IN;
            if (_layers.size() != 0)
            {
                // Size of last layer is number of inputs to next layer
                inputs = _layers.back().size();
            }

            // Zero initialize layer to zero
            _layers.emplace_back(size, inputs);
        }
        else
        {
            throw std::runtime_error("nnet: can't add layers to a finalized neural net");
        }
    }
    inline static nnet<T, IN, OUT> breed(const nnet<T, IN, OUT> &p1, const nnet<T, IN, OUT> &p2)
    {
        // Initialize dimensions with p1
        nnet<T, IN, OUT> out = p1;

        const auto f = [&p2](nnode<T> &node, const size_t i, const size_t j) {
            node *= p2._layers[i][j];
        };

        // Breed nets together
        out.on_net(f);

        return out;
    }
    inline void backprop_identity(const vector<T, OUT> &set_point, const T step_size = 0.1)
    {
        const auto f = [](nnode<T> &node, const T propagated, const T step) {
            node.backprop_identity(propagated, step);
        };

        backprop(f, set_point, step_size);
    }
    inline void backprop_relu(const vector<T, OUT> &set_point, const T step_size = 0.1)
    {
        const auto f = [](nnode<T> &node, const T propagated, const T step) {
            node.backprop_relu(propagated, step);
        };

        backprop(f, set_point, step_size);
    }
    inline void backprop_sigmoid(const vector<T, OUT> &set_point, const T step_size = 0.1)
    {
        const auto f = [](nnode<T> &node, const T propagated, const T step) {
            node.backprop_sigmoid(propagated, step);
        };

        backprop(f, set_point, step_size);
    }
    inline void backprop_tanh(const vector<T, OUT> &set_point, const T step_size = 0.1)
    {
        const auto f = [](nnode<T> &node, const T propagated, const T step) {
            node.backprop_tanh(propagated, step);
        };

        backprop(f, set_point, step_size);
    }
    inline vector<T, OUT> calculate_identity() const
    {
        const auto f = [](const nnode<T> &node) {
            node.calculate_identity();
        };

        return calculate(f);
    }
    inline vector<T, OUT> calculate_relu() const
    {
        const auto f = [](const nnode<T> &node) {
            node.calculate_relu();
        };

        return calculate(f);
    }
    inline vector<T, OUT> calculate_sigmoid() const
    {
        const auto f = [](const nnode<T> &node) {
            node.calculate_sigmoid();
        };

        return calculate(f);
    }
    inline vector<T, OUT> calculate_tanh() const
    {
        const auto f = [](const nnode<T> &node) {
            node.calculate_tanh();
        };

        return calculate(f);
    }
    inline static bool compatible(const nnet<T, IN, OUT> &p1, const nnet<T, IN, OUT> &p2)
    {
        // Test if nets are compatible
        if (p1._layers.size() != p2._layers.size())
        {
            throw std::runtime_error("nnet: can't breed incompatible neural nets, layers differ");
        }

        // Check net compatibility
        const size_t layers = p1._layers.size();
        for (size_t i = 0; i < layers; i++)
        {
            // For all nodes in in layer
            const size_t nodes = p1._layers[i].size();
            if (p2._layers[i].size() != nodes)
            {
                throw std::runtime_error("nnet: can't breed incompatible neural nets, nodes differ");
            }
        }

        return true;
    }
    inline const vector<T, IN> &get_input() const
    {
        return _input;
    }
    inline const std::vector<T> &get_weights(const size_t i, const size_t j) const
    {
        return _layers[i][j].get_weights();
    }
    inline void debug_weights(const size_t i, const size_t j) const
    {
        const size_t size = _layers[i][j].get_inputs();
        const std::vector<T> &w = _layers[i][j].get_weights();

        // Print out weights
        for (size_t k = 0; k < size; k++)
        {
            std::cout << "Node " << std::to_string(i) << ", " << std::to_string(j) << std::endl;
            std::cout << "Weight " << std::to_string(k) << ": " << w[k] << std::endl;
        }

        // Print out bias
        std::cout << "Bias " << _layers[i][j].get_bias() << std::endl;
    }
    inline void debug_connections() const
    {
        // Print out entire neural net
        const size_t layers = _layers.size();
        for (size_t i = 0; i < layers; i++)
        {
            const size_t nodes = _layers[i].size();
            for (size_t j = 0; j < nodes; j++)
            {
                debug_weights(i, j);
            }
        }
    }
    inline T get_output(const size_t i, const size_t j) const
    {
        return _layers[i][j].output();
    }
    inline void finalize()
    {
        if (!_final)
        {
            // Create output network layer with input count from last layer
            add_layer(OUT);
            _final = true;
        }
    }
    inline void set_linear_output(const bool mode)
    {
        _linear_output = mode;
    }
    inline void mutate(std::mt19937 &gen, min::net_rng<T> &ran)
    {
        // Calculate a random layer index
        const unsigned layer_index = ran.random_int(gen) % _layers.size();

        // Calculate a random node index
        const unsigned node_index = ran.random_int(gen) % _layers[layer_index].size();

        // Mutate this node
        _layers[layer_index][node_index].mutate(gen, ran);
    }
    inline void randomize(std::mt19937 &gen, min::net_rng<T> &ran)
    {
        const auto f = [&gen, &ran](nnode<T> &node, const size_t i, const size_t j) {
            // Randomize weights
            node.reset(gen, ran);
        };

        // Randomize the net
        on_net(f);
    }
    inline void reset()
    {
        // Clear layers
        _layers.clear();

        // Unfinalize the net
        _final = false;
    }
    inline void set_input(const vector<T, IN> &input) const
    {
        _input = input;
    }
    inline std::vector<T> serialize() const
    {
        std::vector<T> out;

        // Serial net dimensions
        out.push_back(static_cast<T>(IN));
        out.push_back(static_cast<T>(OUT));
        out.push_back(static_cast<T>(_layers.size()));

        // Serialize layer sizes
        const size_t size = _layers.size();
        for (size_t i = 0; i < size; i++)
        {
            const size_t nodes = _layers[i].size();
            out.push_back(static_cast<T>(nodes));
        }

        // Serialize net data
        const auto f = [&out](const nnode<T> &node, const size_t i, const size_t j) {
            // Serialize all weights
            const size_t size = node.get_inputs();
            const std::vector<T> &weights = node.get_weights();
            for (size_t k = 0; k < size; k++)
            {
                out.push_back(weights[k]);
            }

            // Serialize bias
            out.push_back(node.get_bias());
        };

        // Randomize the net
        on_const_net(f);

        return out;
    }
    inline void deserialize(const std::vector<T> &data)
    {
        // Unfinalize the net
        _final = false;

        // Use int here, in case someone feeds in garbage data
        // Check input size
        const int in = static_cast<int>(data[0]);
        if (in != IN)
        {
            throw std::runtime_error("nnet: can't deserialize, expected input '" + std::to_string(IN) + "' but got '" + std::to_string(in) + "'");
        }

        // Check output size
        const int out = static_cast<int>(data[1]);
        if (out != OUT)
        {
            throw std::runtime_error("nnet: can't deserialize, expected output '" + std::to_string(OUT) + "' but got '" + std::to_string(out) + "'");
        }

        // Clear the layers
        _layers.clear();
        const int size = static_cast<int>(data[2]);

        // Check last layer size special case
        const int last = static_cast<int>(data[2 + size]);
        if (last != OUT)
        {
            throw std::runtime_error("nnet: can't deserialize, expected last size '" + std::to_string(OUT) + "' but got '" + std::to_string(last) + "'");
        }

        // Count bytes
        size_t count = 0;
        int inputs = 0;
        for (int i = 0; i < size; i++)
        {
            // Number of nodes in layer
            const int length = static_cast<int>(data[3 + i]);
            if (length <= 0)
            {
                throw std::runtime_error("nnet: invalid layer size");
            }

            // Add new layer
            this->add_layer(length);

            // Count data members in nodes
            if (i == 0)
            {
                // Bias and one input
                count += length * (IN + 1);
                inputs = length;
            }
            else
            {
                // Bias and inputs per node in layer
                count += length * (inputs + 1);
                inputs = length;
            }
        }

        // Check that the number of nodes makes sense
        const size_t left = data.size() - (size + 3);
        if (count != left)
        {
            throw std::runtime_error("nnet: can't deserialize node mismatch");
        }

        // Starting index, assign values to net
        size_t index = 3 + size;
        const auto f = [&data, &index](nnode<T> &node, const size_t i, const size_t j) {
            // Create a new node
            const size_t size = node.get_inputs();
            node = nnode<T>(size);

            // Copy weights from input stream
            std::vector<T> &weights = node.get_weights();
            for (size_t k = 0; k < size; k++)
            {
                weights[k] = data[index];
                index++;
            }

            // Set the node bias
            node.set_bias(data[index]);
            index++;
        };

        // Randomize the net
        on_net(f);

        // Finalize this network
        _final = true;
    }
};
}

#endif
