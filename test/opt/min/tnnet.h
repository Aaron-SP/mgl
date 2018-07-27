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
#ifndef _MGL_TEST_NEURAL_NET_MGL_
#define _MGL_TEST_NEURAL_NET_MGL_

#include <min/nn.h>
#include <min/nnet.h>
#include <min/test.h>
#include <min/vec.h>

bool test_neural_net_fixed()
{
    bool out = true;
    std::mt19937 gen(static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    min::net_rng<double> rng;

    // 3X3 Problems
    {
        // Test nnet operations
        min::vector<double, 3> in;
        in[0] = 3.0;
        in[1] = 4.0;
        in[2] = 5.0;
        min::vector<double, 3> output;
        min::vector<double, 3> cached_output;
        min::nnet<double, 3, 3> net;
        min::nnet<double, 3, 3> net2;
        net.add_layer(3);
        net.add_layer(5);
        net.add_layer(4);
        net.finalize();

        // Test net calculation, should be zero
        net.set_input(in);
        output = net.calculate_sigmoid();

        // Test first layer of net
        out = out && test(1.0, net.get_output(0, 0), 1E-4, "Failed net calculate layer 1");
        out = out && test(1.0, net.get_output(0, 1), 1E-4, "Failed net calculate layer 1");
        out = out && test(1.0, net.get_output(0, 2), 1E-4, "Failed net calculate layer 1");

        // Test second layer of net
        out = out && test(0.9525, net.get_output(1, 0), 1E-4, "Failed net calculate layer 2");
        out = out && test(0.9525, net.get_output(1, 1), 1E-4, "Failed net calculate layer 2");
        out = out && test(0.9525, net.get_output(1, 2), 1E-4, "Failed net calculate layer 2");
        out = out && test(0.9525, net.get_output(1, 3), 1E-4, "Failed net calculate layer 2");
        out = out && test(0.9525, net.get_output(1, 4), 1E-4, "Failed net calculate layer 2");

        // Test third layer of net
        out = out && test(0.9915, net.get_output(2, 0), 1E-4, "Failed net calculate layer 3");
        out = out && test(0.9915, net.get_output(2, 1), 1E-4, "Failed net calculate layer 3");
        out = out && test(0.9915, net.get_output(2, 2), 1E-4, "Failed net calculate layer 3");
        out = out && test(0.9915, net.get_output(2, 3), 1E-4, "Failed net calculate layer 3");

        // Test last layer of net
        out = out && test(0.9814, net.get_output(3, 0), 1E-4, "Failed net calculate layer 4");
        out = out && test(0.9814, net.get_output(3, 1), 1E-4, "Failed net calculate layer 4");
        out = out && test(0.9814, net.get_output(3, 2), 1E-4, "Failed net calculate layer 4");

        out = out && test(0.9814, output[0], 1E-4, "Failed net calculate output");
        out = out && test(0.9814, output[1], 1E-4, "Failed net calculate output");
        out = out && test(0.9814, output[2], 1E-4, "Failed net calculate output");

        // Check operator=
        net2 = net;
        output = net2.calculate_sigmoid();
        out = out && test(0.9814, output[0], 1E-4, "Failed net calculate output copy");
        out = out && test(0.9814, output[1], 1E-4, "Failed net calculate output copy");
        out = out && test(0.9814, output[2], 1E-4, "Failed net calculate output copy");

        // Check neural nets are compatible
        const bool compatible = min::nnet<double, 3, 3>::compatible(net, net2);
        out = out && test(true, compatible, "Failed net compatible");

        // Breed the net with itself, output is same since weights are zero
        net2 = min::nnet<double, 3, 3>::breed(net, net2);
        output = net2.calculate_sigmoid();
        out = out && test(0.9814, output[0], 1E-4, "Failed net calculate output breed");
        out = out && test(0.9814, output[1], 1E-4, "Failed net calculate output breed");
        out = out && test(0.9814, output[2], 1E-4, "Failed net calculate output breed");

        // Try to randomize the net
        cached_output = output;
        net2.randomize(gen, rng);
        output = net2.calculate_sigmoid();
        out = out && not_test(cached_output[0], output[0], 1E-4, "Failed net calculate output random");
        out = out && not_test(cached_output[1], output[1], 1E-4, "Failed net calculate output random");
        out = out && not_test(cached_output[2], output[2], 1E-4, "Failed net calculate output random");

        // Test inbreeding
        cached_output = output;
        net2 = min::nnet<double, 3, 3>::breed(net2, net2);
        output = net2.calculate_sigmoid();
        out = out && test(cached_output[0], output[0], 1E-4, "Failed net calculate inbreeding");
        out = out && test(cached_output[1], output[1], 1E-4, "Failed net calculate inbreeding");
        out = out && test(cached_output[2], output[2], 1E-4, "Failed net calculate inbreeding");

        // Mutate the neural net, ensure RNG doesn't screw up our tests
        cached_output = output;
        net2.mutate(gen, rng);
        net2.mutate(gen, rng);
        net2.mutate(gen, rng);
        net2.mutate(gen, rng);
        net2.mutate(gen, rng);
        net2.mutate(gen, rng);
        net2.mutate(gen, rng);
        net2.mutate(gen, rng);
        output = net2.calculate_sigmoid();
        out = out && not_test(cached_output[0], output[0], 1E-4, "Failed net calculate output random breed mutate");
        out = out || not_test(cached_output[1], output[1], 1E-4, "Failed net calculate output random breed mutate");
        out = out || not_test(cached_output[2], output[2], 1E-4, "Failed net calculate output random breed mutate");

        // Test input is unchanged
        in = net2.get_input();
        out = out && test(3.0, in[0], 1E-4, "Failed net input unchanged");
        out = out && test(4.0, in[1], 1E-4, "Failed net input unchanged");
        out = out && test(5.0, in[2], 1E-4, "Failed net input unchanged");

        // Test serialize neural net
        std::vector<double> data = net2.serialize();
        out = out && test(78, data.size(), "Failed net serialize");

        // Cache old result
        cached_output = output;

        // Test deserialize neural net
        min::nnet<double, 3, 3> net3;
        net3.deserialize(data);
        net3.set_input(in);
        output = net3.calculate_sigmoid();

        out = out && test(cached_output[0], output[0], 1E-4, "Failed net deserialize calculate");
        out = out && test(cached_output[1], output[1], 1E-4, "Failed net deserialize calculate");
        out = out && test(cached_output[2], output[2], 1E-4, "Failed net deserialize calculate");
    }

    // 1X1 Problems linear model
    {
        // Test nnet regression convergence
        min::vector<double, 1> in;
        min::vector<double, 1> output;
        min::nnet<double, 1, 1> net;
        net.add_layer(1);
        net.finalize();

        in[0] = 2.0;
        net.set_input(in);
        output = net.calculate_identity();

        min::vector<double, 1> sp;
        sp[0] = 1.0;

        // Iteration N
        for (size_t i = 0; i < 10; i++)
        {
            net.backprop_identity(sp, 0.25);
            output = net.calculate_identity();
        }

        // Did we converge?
        out = out && test(1.0, output[0], 1E-4, "Failed neural net 1x1 training y=1");
    }
    {
        // Test nnet operations
        min::vector<double, 1> in;
        min::vector<double, 1> output;
        min::nnet<double, 1, 1> net;
        net.add_layer(1);
        net.finalize();

        // Test training x at [-1, 1]
        for (size_t i = 0; i < 20; i++)
        {
            // Calculate input
            const double x = (-1.0 + (i / 10.0));
            in[0] = x;
            net.set_input(in);
            net.calculate_identity();

            // Set set point value to train on
            min::vector<double, 1> sp;
            sp[0] = x;

            // Train input to be output
            net.backprop_identity(sp, 0.01);
        }

        // Test neural net sum inputs
        double total_error = 0.0;
        for (size_t i = 0; i < 2000; i++)
        {
            // Calculate input
            const double x = (-1.0 + (i / 1000.0));
            in[0] = x;
            net.set_input(in);
            output = net.calculate_identity();

            // test error rates
            const double sp = x;
            total_error += (output - sp).square_magnitude();
        }

        // Did we pass the fit test?
        out = out && test(0.0, total_error, 1E-4, "Failed neural net 1x1 training y=x");
    }
    // 1X1 Problems
    {
        // Test nnet operations
        min::vector<double, 1> in;
        min::vector<double, 1> output;
        min::nnet<double, 1, 1> net;
        net.add_layer(6);
        net.add_layer(6);
        net.add_layer(6);
        net.finalize();
        net.set_linear_output(true);
        min::mapper<double> m(-1.0, 1.0);

        // Test training sin(x)/x at x = [-1, 7]; y = [-1, 1]
        for (size_t i = 0; i < 24000; i++)
        {
            // Calculate input
            const double x = (-1.0 + (i / 3000.0));
            in[0] = x;
            net.set_input(in);
            net.calculate_relu();

            // Set set point value to train on
            min::vector<double, 1> sp;
            if (std::abs(x) >= 1E-6)
            {
                sp[0] = std::sin(x) / x;
            }
            else
            {
                sp[0] = 1.0;
            }

            // Train input to be output
            net.backprop_relu(sp, 0.001);
        }

        // Test neural net sin(x)/x inputs
        double total_error = 0.0;
        for (size_t i = 0; i < 1000; i++)
        {
            // Calculate input
            const double x = (-1.0 + (i / 125.0));
            in[0] = x;
            net.set_input(in);
            output = net.calculate_relu();

            // test error rates
            const double sp = (std::abs(x) >= 1E-6) ? std::sin(x) / x : 1.0;
            total_error += (output - sp).square_magnitude();
        }

        // This is still pretty far away from perfect, but good enough for a test
        out = out && test(true, (total_error > 359.0) && (total_error < 360.0), "Failed neural net 1x1 training sin(x) / x");

        // Test deserialize and serialize
        min::nnet<double, 1, 1> net2;

        // Test serialize neural net
        std::vector<double> data = net.serialize();
        out = out && test(110, data.size(), "Failed net 1X1 serialize");

        // Test deserialize neural net
        net2.deserialize(data);
        net2.set_input(in);
        net2.set_linear_output(true);

        // Test last neural net sin(x)/x inputs
        total_error = 0.0;
        for (size_t i = 0; i < 1000; i++)
        {
            // Calculate input
            const double x = (-1.0 + (i / 125.0));
            in[0] = x;
            net2.set_input(in);
            output = net2.calculate_relu();

            // test error rates
            const double sp = (std::abs(x) >= 1E-6) ? std::sin(x) / x : 1.0;
            total_error += (output - sp).square_magnitude();
        }

        // This is still pretty far away from perfect, but good enough for a test
        out = out && test(true, (total_error > 359.0) && (total_error < 360.0), "Failed neural net 1x1 training sin(x) / x deserialize");
    }

    // 2X2 Problems linear model
    {
        // Test nnet regression convergence
        min::vector<double, 2> in;
        min::vector<double, 2> output;
        min::nnet<double, 2, 2> net;
        net.add_layer(1);
        net.finalize();

        in[0] = 2.0;
        in[1] = 4.0;
        net.set_input(in);
        output = net.calculate_identity();

        min::vector<double, 2> sp;
        sp[0] = 5.0;
        sp[1] = 1.0;

        // Iteration N
        for (size_t i = 0; i < 45; i++)
        {
            net.backprop_identity(sp, 0.01);
            output = net.calculate_identity();
        }

        // Did we converge?
        out = out && test(5.0, output[0], 1E-4, "Failed neural net 1x1 training y=1");
        out = out && test(1.0, output[1], 1E-4, "Failed neural net 1x1 training y=1");
    }
    // 2X1 Problems linear model
    {
        // Test nnet regression convergence
        min::vector<double, 2> in;
        min::vector<double, 1> output;
        min::nnet<double, 2, 1> net;
        net.add_layer(1);
        net.finalize();

        in[0] = 2.0;
        in[1] = 4.0;
        net.set_input(in);
        output = net.calculate_identity();

        min::vector<double, 1> sp;
        sp[0] = 10.0;

        // Iteration N
        for (size_t i = 0; i < 35; i++)
        {
            net.backprop_identity(sp, 0.01);
            output = net.calculate_identity();
        }

        // Did we converge?
        out = out && test(10.0, output[0], 1E-4, "Failed neural net 2x1 training y=1");
    }
    {
        // Test nnet operations x+y=z
        min::vector<double, 2> in;
        min::vector<double, 1> output;
        min::nnet<double, 2, 1> net;
        net.add_layer(1);
        net.finalize();

        // Test training x at [-1, 1]
        for (size_t i = 0; i < 20; i++)
        {
            for (size_t j = 0; j < 20; j++)
            {
                // Calculate input
                const double x = (-1.0 + (i / 10.0));
                const double y = (-1.0 + (j / 10.0));
                in[0] = x;
                in[1] = y;
                net.set_input(in);

                // Set set point value to train on
                min::vector<double, 1> sp;
                sp[0] = x + y;

                // Do N iterations
                for (size_t k = 0; k < 35; k++)
                {
                    output = net.calculate_identity();

                    // Train input to be output
                    net.backprop_identity(sp, 0.01);
                }
            }
        }

        // Test neural net sum inputs
        double total_error = 0.0;
        for (size_t i = 0; i < 20; i++)
        {
            for (size_t j = 0; j < 20; j++)
            {
                // Calculate input
                const double x = (-1.0 + (i / 10.0));
                const double y = (-1.0 + (j / 10.0));
                in[0] = x;
                in[1] = y;
                net.set_input(in);
                output = net.calculate_identity();

                // test error rates
                const double sp = x + y;
                total_error += (output - sp).square_magnitude();
            }
        }

        // Did we pass the fit test?
        out = out && test(0.0, total_error, 1E-4, "Failed neural net 1x1 training z=x+y");
    }

    // return result
    return out;
}

#endif
