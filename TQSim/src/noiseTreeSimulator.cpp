#include <cppsim/state.hpp>
#include <cppsim/gate_factory.hpp>
#include <cppsim/gate_merge.hpp>
#include <cppsim/circuit.hpp>
#include "noiseTreeSimulator.hpp"

NoiseTreeSimulator::NoiseTreeSimulator(const QuantumCircuit *init_circuit, const double prob1, const double prob2, const std::vector<int> shots_given, const std::vector<int> lengths, const QuantumState *init_state)
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

	// store the shots
	shots = shots_given;

	// generate subcircuits
	int main_circ_index = 0;

	for (int i = 0; i < lengths.size(); i++)
	{
		QuantumCircuit *subcirc = new QuantumCircuit(init_circuit->qubit_count);

		for (int j = 0; j < lengths[i]; j++)
		{
			subcirc->add_gate_copy(init_circuit->gate_list[main_circ_index]);

			std::vector<UINT> qubit_indices = init_circuit->gate_list[main_circ_index]->get_target_index_list();

			for (auto x : init_circuit->gate_list[main_circ_index]->get_control_index_list())
			{
				qubit_indices.push_back(x);
			}
			if (qubit_indices.size() == 1)
			{
				subcirc->add_gate(gate::DepolarizingNoise(qubit_indices[0], prob1));
			}
			else if (qubit_indices.size() == 2)
			{
				subcirc->add_gate(gate::TwoQubitDepolarizingNoise(qubit_indices[0], qubit_indices[1], prob2));
			}
			else
			{
				std::cerr << "Error: In NoiseSimulator gates must not over 2 qubits" << std::endl;
				std::cerr << "Added nothing on gate " << i << " ." << std::endl;
			}

			main_circ_index++;
		}

		circuits.push_back(subcirc);
		intermediate_states.push_back(new QuantumState(init_circuit->qubit_count));
	}
	intermediate_states.push_back(new QuantumState(init_circuit->qubit_count));
}

NoiseTreeSimulator::~NoiseTreeSimulator()
{
	delete initial_state;
	for (int i = 0; i < circuits.size(); i++)
	{
		delete circuits[i];
		delete intermediate_states[i];
	}
	// delete intermediate_states[intermediate_states.size() - 1];
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

void NoiseTreeSimulator::recurExecute(const int curDepth)
{
	if (curDepth == shots.size())
	{
		results.push_back((intermediate_states[curDepth]->sampling(1))[0]);
		count++;

		// if (count % 100 == 0)
		// {
		// 	std::cout << string_format("%-10s shot: %-5d", "Qtree", count) << '\r' << std::flush;
		// }
		return;
	}

	for (UINT i = 0; i < shots[curDepth]; ++i)
	{
		intermediate_states[curDepth + 1]->load(intermediate_states[curDepth]);

		circuits[curDepth]->update_quantum_state(intermediate_states[curDepth + 1]);

		recurExecute(curDepth + 1);
	}

	return;
}

std::vector<UINT> NoiseTreeSimulator::execute()
{
	results.clear();
	count = 0;
	intermediate_states[0]->load(initial_state);

	recurExecute(0);

	return results;
}
