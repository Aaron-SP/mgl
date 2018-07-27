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
#ifndef _MGL_TEST_NEURAL_NET_AUGMENTED_MGL_
#define _MGL_TEST_NEURAL_NET_AUGMENTED_MGL_

#include <min/nn.h>
#include <min/nneat.h>
#include <min/test.h>
#include <min/vec.h>
#include <stdexcept>

bool test_neural_net_augmented()
{
    bool out = true;
    std::mt19937 gen(static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    min::net_rng<double> rng;

    // 3X3 Problems
    {
        min::vector<double, 3> in;
        in[0] = 3.0;
        in[1] = 4.0;
        in[2] = 5.0;
        min::nneat<double, 3, 3> net;
        min::nneat<double, 3, 3> net2;
        min::nneat<double, 3, 3> net3;
        min::vector<double, 3> output;
        min::vector<double, 3> cached_output;

        // Set topology constants
        net.set_topology_constants(1, 3, 3, 5);
        net2.set_topology_constants(1, 3, 3, 5);
        net3.set_topology_constants(1, 3, 3, 5);

        // Test default calculate
        net.set_input(in);
        output = net.calculate();
        out = out && test(0.5, output[0], 1E-4, "Failed neat default output");
        out = out && test(0.5, output[1], 1E-4, "Failed neat default output");
        out = out && test(0.5, output[2], 1E-4, "Failed neat default output");

        // Add three connections to nodes
        net.add_connection(0, 3, 1.0);
        net.add_connection(1, 4, 1.0);
        net.add_connection(2, 5, 1.0);

        // Test adding same connection twice no duplicate
        net.add_connection(0, 3, 1.0);
        net.add_connection(1, 4, 1.0);
        net.add_connection(2, 5, 1.0);

        // Test add connection
        output = net.calculate();
        out = out && test(0.9525, output[0], 1E-4, "Failed neat add connection 1");
        out = out && test(0.9820, output[1], 1E-4, "Failed neat add connection 1");
        out = out && test(0.9933, output[2], 1E-4, "Failed neat add connection 1");

        // Test removing connection and adding it again
        net.remove_connection(0, 3);
        net.remove_connection(1, 4);
        net.remove_connection(2, 5);

        output = net.calculate();
        out = out && test(0.5, output[0], 1E-4, "Failed neat remove connection");
        out = out && test(0.5, output[1], 1E-4, "Failed neat remove connection");
        out = out && test(0.5, output[2], 1E-4, "Failed neat remove connection");

        net.add_connection(0, 3, 1.0);
        net.add_connection(1, 4, 1.0);
        net.add_connection(2, 5, 1.0);

        // Test add connection
        output = net.calculate();
        out = out && test(0.9525, output[0], 1E-4, "Failed neat add connection 2");
        out = out && test(0.9820, output[1], 1E-4, "Failed neat add connection 2");
        out = out && test(0.9933, output[2], 1E-4, "Failed neat add connection 2");

        // Test faulty connection from output to input
        net.add_connection(3, 0, 1.0);
        net.add_connection(4, 1, 1.0);
        net.add_connection(5, 2, 1.0);

        // Test no change
        output = net.calculate();
        out = out && test(0.9525, output[0], 1E-4, "Failed neat add connection 2");
        out = out && test(0.9820, output[1], 1E-4, "Failed neat add connection 2");
        out = out && test(0.9933, output[2], 1E-4, "Failed neat add connection 2");

        // Test add node between
        net.add_node_between(0, 3);
        net.add_node_between(1, 4);
        net.add_node_between(2, 5);

        // Test topology change
        output = net.calculate();
        out = out && test(0.7216, output[0], 1E-4, "Failed neat add node between 1");
        out = out && test(0.7275, output[1], 1E-4, "Failed neat add node between 1");
        out = out && test(0.7297, output[2], 1E-4, "Failed neat add node between 1");

        // Test valid topology change
        net.add_connection(6, 7, 1.0);

        // Test create connection
        output = net.calculate();
        out = out && test(0.7216, output[0], 1E-4, "Failed neat add node between 2");
        out = out && test(0.7296, output[1], 1E-4, "Failed neat add node between 2");
        out = out && test(0.7297, output[2], 1E-4, "Failed neat add node between 2");

        // Test operator=
        net2 = net;

        // Test faulty add_node, new nodes must be between output nodes
        net2.add_node_between(6, 7);
        net2.add_node_between(7, 8);

        // Test node between
        output = net.calculate();
        out = out && test(0.7216, output[0], 1E-4, "Failed neat add node between 2");
        out = out && test(0.7296, output[1], 1E-4, "Failed neat add node between 2");
        out = out && test(0.7297, output[2], 1E-4, "Failed neat add node between 2");

        // Test mutate
        for (size_t i = 0; i < 30; i++)
        {
            net2.mutate(gen, rng);
        }

        // Test different calculation
        output = net2.calculate();
        out = out && not_test(0.7216, output[0], 1E-4, "Failed neat random mutate");
        out = out && not_test(0.7296, output[1], 1E-4, "Failed neat random mutate");
        out = out && not_test(0.7297, output[2], 1E-4, "Failed neat random mutate");

        // Test randomize
        net2.randomize(gen, rng);

        // Test different calculation
        output = net2.calculate();
        out = out && not_test(0.5, output[0], 1E-4, "Failed neat randomize");
        out = out && not_test(0.5, output[1], 1E-4, "Failed neat randomize");
        out = out && not_test(0.5, output[2], 1E-4, "Failed neat randomize");

        // Test inbreeding
        cached_output = output;
        net2 = min::nneat<double, 3, 3>::breed(net2, net2);
        output = net2.calculate();
        out = out && test(cached_output[0], output[0], 1E-4, "Failed neat calculate inbreeding");
        out = out && test(cached_output[1], output[1], 1E-4, "Failed neat calculate inbreeding");
        out = out && test(cached_output[2], output[2], 1E-4, "Failed neat calculate inbreeding");

        // Test breeding takes first parent as base
        net3 = min::nneat<double, 3, 3>::breed(net2, net);
        output = net3.calculate();
        out = out && not_test(cached_output[0], output[0], 1E-4, "Failed neat calculate breeding");
        out = out && not_test(cached_output[1], output[1], 1E-4, "Failed neat calculate breeding");
        out = out && not_test(cached_output[2], output[2], 1E-4, "Failed neat calculate breeding");

        // Test serialize neural net
        size_t net_size = net.get_nodes();
        cached_output = net.calculate();
        std::vector<double> data = net.serialize();
        out = out && test(51, data.size(), "Failed neat serialize");

        // Test deserialize neural net
        net2.deserialize(data);
        net2.set_input(in);
        output = net2.calculate();
        out = out && test(cached_output[0], output[0], 1E-4, "Failed neat deserialize calculate 1");
        out = out && test(cached_output[1], output[1], 1E-4, "Failed neat deserialize calculate 1");
        out = out && test(cached_output[2], output[2], 1E-4, "Failed neat deserialize calculate 1");

        // Test node size match
        out = out && test(net2.get_nodes(), net_size, "Failed neat serialize node size match 1");

        // Test serialize neural net3
        const size_t connections = net3.get_connections();
        net_size = net3.get_nodes();
        cached_output = net3.calculate();
        data = net3.serialize();

        // Test connection count
        out = out && not_test(7, connections, "Failed neat serialize connection count");

        // Test deserialize neural net3
        net2.deserialize(data);
        net2.set_input(in);
        output = net2.calculate();
        out = out && test(cached_output[0], output[0], 1E-4, "Failed neat deserialize calculate 2");
        out = out && test(cached_output[1], output[1], 1E-4, "Failed neat deserialize calculate 2");
        out = out && test(cached_output[2], output[2], 1E-4, "Failed neat deserialize calculate 2");

        // Test node size match
        out = out && test(net2.get_nodes(), net_size, "Failed neat serialize node size match 2");

        // Test connection count
        out = out && test(connections, net2.get_connections(), "Failed neat deserialize connection count");
    }

    return out;
}

#endif
