#include <cppsim/state.hpp>
#include <cppsim/gate_factory.hpp>
#include <cppsim/gate_merge.hpp>
#include <cppsim/circuit.hpp>
#include "idealSim.hpp"
/**
 * \~japanese-en 回路にノイズを加えてサンプリングするクラス
 */

IdealSimulator::IdealSimulator(const QuantumCircuit *init_circuit, const QuantumState *init_state)
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

    for (UINT i = 0; i < init_circuit->gate_list.size(); ++i)
    {
        circuit->add_gate_copy(init_circuit->gate_list[i]);
    }
}

IdealSimulator::~IdealSimulator()
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

std::vector<UINT> IdealSimulator::execute(const UINT sample_count)
{
    circuit->update_quantum_state(initial_state);

    std::vector<UINT> results;

    auto counts = initial_state->sampling(sample_count);

    for (int i = 0; i < counts.size(); i++)
    {
        results.push_back(counts[i]);
    }

    return results;
}