#ifndef NOISE_TREE_SIMULATOR
#define NOISE_TREE_SIMULATOR

#include <cppsim/state.hpp>
#include <cppsim/gate_factory.hpp>
#include <cppsim/gate_merge.hpp>
#include <cppsim/circuit.hpp>

class NoiseTreeSimulator
{
private:
	std::vector<QuantumCircuit *> circuits;
	std::vector<QuantumState *> intermediate_states;

	std::vector<UINT> results;

	std::vector<int> shots;

	QuantumStateBase *initial_state;

	int count;

	void recurExecute(const int curDepth);

public:
	/**
	 * \~japanese-en
	 * コンストラクタ。
	 *
	 * NoiseSimulatorを作成する。
	 * @param[in] init_circuit
	 * @param[in] prob ノイズが乗る確率
	 * @param[in] init_state 最初の状態。指定されなかった場合は0で初期化される。
	 * @return NoiseSimulatorのインスタンス
	 */
	NoiseTreeSimulator(const QuantumCircuit *init_circuit, const double prob1, const double prob2, const std::vector<int> shots_given, const std::vector<int> lengths, const QuantumState *init_state = NULL);

	/**
	 * \~japanese-en
	 * デストラクタ。このとき、NoiseSimulatorが保持しているcircuitとinitial_stateは解放される。
	 */
	virtual ~NoiseTreeSimulator();

	/**
	 * \~japanese-en
	 *
	 * サンプリングを行い、結果を配列で返す。
	 * @param[in] sample_count 行うsamplingの回数
	 */
	virtual std::vector<UINT> execute();
};

#endif