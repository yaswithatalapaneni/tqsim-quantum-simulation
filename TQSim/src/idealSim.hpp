#ifndef IDEAL_SIMULATOR
#define IDEAL_SIMULATOR

#include <cppsim/state.hpp>
#include <cppsim/gate_factory.hpp>
#include <cppsim/gate_merge.hpp>
#include <cppsim/circuit.hpp>

class IdealSimulator
{
private:
    QuantumCircuit *circuit;
    QuantumStateBase *initial_state;

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
    IdealSimulator(const QuantumCircuit *init_circuit, const QuantumState *init_state = NULL);

    /**
     * \~japanese-en
     * デストラクタ。このとき、NoiseSimulatorが保持しているcircuitとinitial_stateは解放される。
     */
    virtual ~IdealSimulator();

    /**
     * \~japanese-en
     *
     * サンプリングを行い、結果を配列で返す。
     * @param[in] sample_count 行うsamplingの回数
     */
    virtual std::vector<UINT> execute(const UINT sample_count);
};
#endif