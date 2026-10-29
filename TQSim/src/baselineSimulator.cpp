#include <cppsim/state.hpp>
#include <cppsim/gate_factory.hpp>
#include <cppsim/gate_merge.hpp>
#include <cppsim/circuit.hpp>
#include "baselineSimulator.hpp"
/**
 * \~japanese-en 回路にノイズを加えてサンプリングするクラス
 */

BaselineSimulator::BaselineSimulator(const QuantumCircuit *init_circuit, const double prob_single, const double prob_two, const QuantumState *init_state)
{

    if (init_state == NULL)
    {
        // initialize with zero state if not provided.
        initial_state = new QuantumState(init_circuit->qubit_count);
        initial_state->set_zero_state();
    }
    else
    {
        // initialize with init_state if provided.
        initial_state = init_state->copy();
    }

    circuit = new QuantumCircuit(init_circuit->qubit_count);
    UINT n = init_circuit->gate_list.size();
    for (UINT i = 0; i < n; ++i)
    {
        circuit->add_gate_copy(init_circuit->gate_list[i]);
        std::vector<UINT> qubit_indices = init_circuit->gate_list[i]->get_target_index_list();
        for (auto x : init_circuit->gate_list[i]->get_control_index_list())
        {
            qubit_indices.push_back(x);
        }
        if (qubit_indices.size() == 1)
        {
            circuit->add_gate(gate::DepolarizingNoise(qubit_indices[0], prob_single));
        }
        else if (qubit_indices.size() == 2)
        {
            circuit->add_gate(gate::TwoQubitDepolarizingNoise(qubit_indices[0], qubit_indices[1], prob_two));
        }
        else
        {
            std::cerr << "Error: In BaselineSimulator gates must not over 2 qubits" << std::endl;
            std::cerr << "Added nothing on gate " << i << " ." << std::endl;
        }
    }
}

BaselineSimulator::~BaselineSimulator()
{
    delete initial_state;
    delete circuit;
}

#include <memory>
#include <string>
#include <stdexcept>

template <typename... Args>
std::string string_format(const std::string &format, Args... args)
{
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
    if (size_s <= 0)
    {
        throw std::runtime_error("Error during formatting.");
    }
    auto size = static_cast<size_t>(size_s);
    auto buf = std::make_unique<char[]>(size);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

std::vector<UINT> BaselineSimulator::execute(const UINT sample_count)
{
    std::vector<UINT> result;
    UINT temp;

    // std::cout<< "Baseline simulation starts" << std::endl;
    QuantumState sampling_state(initial_state->qubit_count);
    for (UINT i = 0; i < sample_count; ++i)
    {
        sampling_state.load(initial_state);
        circuit->update_quantum_state(&sampling_state);
        // std::cout << temp << std::endl;
        result.push_back(sampling_state.sampling(1)[0]);

        // if (i % 100 == 0)
        // {
        //     std::cout << string_format("%-10s shot: %-5d", "Baseline", i) << '\r' << std::flush;
        // }
    }

    // std::cout<< "Baseline simulation finished!" << std::endl;
    return result;
}