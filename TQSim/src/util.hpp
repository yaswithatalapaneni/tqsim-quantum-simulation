#ifndef TREE_SIM_UTIL
#define TREE_SIM_UTIL

#include <cppsim/circuit.hpp>

QuantumCircuit *readCircuit(const char *file_name);
double chiLoss(std::vector<UINT> dist1, std::vector<UINT> dist2, int nqubit);
double stateFidelity(std::vector<UINT> dist1, std::vector<UINT> dist2, int nqubit);
double normFidelity(std::vector<UINT> ideal, std::vector<UINT> output, int nqubit);

// methods used in main.cpp
char *getCmdOption(char **begin, char **end, const std::string &option);
bool cmdOptionExists(char **begin, char **end, const std::string &option);
void printStats(std::vector<double> v);

#endif