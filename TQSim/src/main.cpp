// #include <cppsim/state.hpp>
// #include <cppsim/gate_factory.hpp>
#include <cppsim/circuit.hpp>

#include "noiseTreeSimulator.hpp"
#include "baselineSimulator.hpp"
#include "idealSim.hpp"
#include "util.hpp"

#include <time.h>
#include <iostream>
#include <fstream>

// stats calculation
#include <algorithm>
#include <numeric>

#include <iostream>
#include <fstream>

int main(int argc, char **argv)
{
    QuantumCircuit *circuit = NULL;
    std::vector<int> shots;
    std::vector<int> lengths;

    int total_shots = 1;

    // Getting the filename of the testing circuit, abort if not given
    if (cmdOptionExists(argv, argv + argc, "-c"))
    {
        char *circuit_str = getCmdOption(argv, argv + argc, "-c");

        circuit = readCircuit(circuit_str);

        printf("Running tests with circuit: %s\n", circuit_str);
    }
    else
    {
        printf("circuits not given, abort!\n\n");
        return 0;
    }

    // Getting the shots required for the test, abort if not provided
    if (cmdOptionExists(argv, argv + argc, "-s"))
    {
        char *shots_str = getCmdOption(argv, argv + argc, "-s");

        char delim[] = "-";

        char *ptr = strtok(shots_str, delim);

        while (ptr != NULL)
        {
            int temp = std::stoi(ptr);

            shots.push_back(temp);
            total_shots *= temp;
            // printf("%d\n", temp);

            ptr = strtok(NULL, delim);
        }
    }
    else
    {
        printf("shots not given, abort!\n\n");
        return 0;
    }

    // Getting the lengths of the subcircuits, abort if not provided
    if (cmdOptionExists(argv, argv + argc, "-l"))
    {
        char *lengths_str = getCmdOption(argv, argv + argc, "-l");

        char delim[] = "-";

        char *ptr = strtok(lengths_str, delim);

        while (ptr != NULL)
        {
            int temp = std::stoi(ptr);

            lengths.push_back(temp);
            // printf("%d\n", temp);
            ptr = strtok(NULL, delim);
        }
    }
    else
    {
        printf("subcirc lengths not given, abort!\n\n");
        return 0;
    }

    double probs[2];

    if (cmdOptionExists(argv, argv + argc, "-p"))
    {
        // Do stuff
        char *probabilities = getCmdOption(argv, argv + argc, "-p");

        char delim[] = "/";

        char *ptr = strtok(probabilities, delim);

        int index = 0;

        while (ptr != NULL && index < 2)
        {
            probs[index++] = std::atof(ptr);

            ptr = strtok(NULL, delim);
        }
    }
    else
    {
        probs[0] = 0.001;
        probs[1] = 0.015;
    }

    // initialize both baseline and tree_sim noisy simulators
    NoiseTreeSimulator tqsim(circuit, probs[0], probs[1], shots, lengths);
    BaselineSimulator baseline_simulator(circuit, probs[0], probs[1]);
    IdealSimulator ideal_simulator(circuit);

    // Run the test
    if (cmdOptionExists(argv, argv + argc, "-t"))
    {
        printf("Doing test\n");

        // Getting the BR
        auto start = std::chrono::high_resolution_clock::now();

        std::vector<UINT> br = baseline_simulator.execute(total_shots);

        auto elapsed = std::chrono::high_resolution_clock::now() - start;

        long long baseline_time = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

        // Getting the TR
        start = std::chrono::high_resolution_clock::now();

        std::vector<UINT> tr = tqsim.execute();

        elapsed = std::chrono::high_resolution_clock::now() - start;

        long long tqsim_time = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

        // Print the execution speedup results
        printf("Baselie Simulation time: %lld(ms), Tree Simulation time: %lld(ms)\n", baseline_time / 1000, tqsim_time / 1000);
        printf("Speedups: %.4f\n\n", baseline_time / (double)tqsim_time);

        // Getting IR
        std::vector<UINT> ir = ideal_simulator.execute(total_shots);

        // Calculate the Normalized fidelities
        double ref_norm_fidelity = normFidelity(ir, br, circuit->qubit_count);
        double output_norm_fidelity = normFidelity(ir, tr, circuit->qubit_count);

        // Print the results
        printf("REF normalized fidelity: %.4f\n\n", ref_norm_fidelity);
        printf("EXP normalized fidelity: %.4f, diff: %.4f%c\n\n", output_norm_fidelity, fabs(ref_norm_fidelity - output_norm_fidelity) * 100, '%');

        printf("%lld\n%lld\n%f\n%f\n%f\n", baseline_time / 1000, tqsim_time / 1000, baseline_time / (double)tqsim_time, ref_norm_fidelity, output_norm_fidelity);
    }

    // free the resources allocated
    delete circuit;
    return 0;
}