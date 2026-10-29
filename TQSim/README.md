# Artifact Evaluation of TQSim

TQSim is a tree-based reuse-focused noisy quantum circuit simulator that uses Qulacs as its backend. This guide walks you through installation, compilation, and result verification.

## Dependencies

### C++ Libraries
- C++ Compiler
- Boost ≥1.71.0
- CMake ≥3.0

### Python Packages
- Python ≥3.7
- tqdm
- matplotlib
- numpy

## Installation Steps

### 1. Download and Compile Qulacs
```git
git clone https://github.com/qulacs/qulacs.git
cd qulacs
./script/build_gcc.sh
cd ..
```

### 2. Download and Compile TQSim
```
git clone https://github.com/meng-ubc/TQSim.git
cd TQSim
git checkout AE
mkdir out
make
```

**Note:** TQSim and Qulacs must be in the same directory as the makefile uses relative paths to locate the Qulacs library.

## Running Experiments

The paper evaluates 48 benchmark circuits across 8 classes with 6 circuits each. The table below shows the number of qubits for each benchmark:

| Benchmark Class | Index 0 | Index 1 | Index 2 | Index 3 | Index 4 | Index 5 |
|----------------|---------|---------|---------|---------|---------|---------|
| adder          | 4       | 4       | 4       | 10      | 10      | 10      |
| bv             | 6       | 8       | 10      | 12      | 14      | 16      |
| mul            | 13      | 15      | 15      | 15      | 15      | 25      |
| qaoa           | 6       | 8       | 9       | 11      | 13      | 15      |
| qft            | 8       | 10      | 12      | 14      | 16      | 18      |
| qpe            | 4       | 6       | 9       | 9       | 11      | 16      |
| qsc            | 8       | 9       | 10      | 12      | 15      | 16      |
| qv             | 10      | 12      | 14      | 16      | 18      | 20      |

### Running All Benchmarks
```
cd ae_scripts
python get_results.py
```

By default, the script limits circuits to 13 qubits maximum, which runs 30 of the 48 benchmarks while ensuring at least one circuit from each class is executed. Results are saved to `results.json`.

### Generating Plots
```
python plot.py
```

This produces two figures that replicate results from the paper:
- `Figure_11_speedup.png`: Reproduces Figure 11 (a)-(h)
- `Figure_14_fidelity.png`: Reproduces Figure 14

## Running Individual Benchmarks

To run specific larger benchmarks, use:
```
python get_results.py [class] [index]
```

Example (14-qubit QFT circuit):
```
python get_results.py qft 3
```

This will update `results.json` with the new results, which can then be visualized using `plot.py`.