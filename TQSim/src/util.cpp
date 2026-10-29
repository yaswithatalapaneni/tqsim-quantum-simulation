// std libs
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <regex>
#include <time.h>
#include <map>
#include <cmath>
#include <regex>

// qulacs libs
#include <cppsim/circuit.hpp>
#include <cppsim/gate_factory.hpp>

// own header file
#include "util.hpp"
using namespace std;

// string basis_gates_depolar[] = {"cx", "id", "u1", "u2", "u3"};
// string basis_gates_melburn[] = {"x", "cx", "id", "rz", "sx"};

vector<string> split(const string &s, char delim)
{
    vector<string> elems;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim))
    {
        elems.push_back(item);
    }
    return elems;
}

std::vector<int> get_qubits(std::string str)
{
    std::regex reg("\\[(.*?)\\]");
    std::vector<int> numbers;
    // Used when your searching a string
    std::smatch matches;

    // Determines if there is a match and match
    // results are returned in matches
    while (std::regex_search(str, matches, reg))
    {
        // add the indices
        numbers.push_back(std::stoi(matches.str(1)));
        // Eliminate the previous match and create
        // a new string to search
        str = matches.suffix().str();
    }
    return numbers;
}

double get_value(std::string str)
{
    vector<string> numbers = split(str, '*');

    if (numbers.size() == 1)
    {
        return std::stod(numbers[0]);
    }
    else
    {
        return std::stod(numbers[0]) * std::stod(numbers[1]);
    }
}

std::vector<double> get_theta_values(std::string str)
{
    // replace the pi with a decimal value
    string s = regex_replace(str, regex("pi"), "3.14159");

    std::regex reg("\\((.*?)\\)");
    std::vector<double> numbers;

    std::smatch matches;
    std::regex_search(s, matches, reg);
    std::string values_str = matches.str(1);

    std::stringstream ss(values_str);

    std::string token;

    while (std::getline(ss, token, ','))
    {
        vector<string> integers = split(token, '/');

        if (integers.size() == 1)
        {
            numbers.push_back(get_value(integers[0]));
        }
        else
        {
            numbers.push_back(get_value(integers[0]) / get_value(integers[1]));
        }
    }

    return numbers;
}

QuantumCircuit *readCircuit(const char *file_name)
{
    ifstream infile(file_name);

    string line;

    int n = 0;

    while (getline(infile, line))
    {
        vector<string> strs = split(line, ' ');
        if (strs[0].compare("qreg") == 0)
        {
            n = get_qubits(strs[1])[0];
            break;
        }
    }

    if (n == 0)
    {
        printf("Can't find file: %s\n", file_name);
        exit(-1);
    }

    QuantumCircuit *circuit = new QuantumCircuit(n);

    while (getline(infile, line))
    {
        // cout << line << endl;

        string instrc = line.substr(0, 2);
        vector<int> qregs = get_qubits(line);

        if (instrc.compare("cx") == 0)
        {
            circuit->add_gate(gate::CNOT(qregs[0], qregs[1]));

            // cout << " CX"
            //      << " " << qregs[0] << " " << qregs[1] << endl;
        }
        else
        {
            vector<double> thetas = get_theta_values(line);

            if (instrc.compare("u1") == 0)
            {
                circuit->add_gate(gate::U1(qregs[0], thetas[0]));

                // cout << " U1"
                //      << " " << qregs[0] << " " << thetas[0] << endl;
            }
            else if (instrc.compare("u2") == 0)
            {
                circuit->add_gate(gate::U2(qregs[0], thetas[0], thetas[1]));

                // cout << " U2"
                //      << " " << qregs[0] << " " << thetas[0] << " " << thetas[1] << endl;
            }
            else if (instrc.compare("u3") == 0)
            {
                circuit->add_gate(gate::U3(qregs[0], thetas[0], thetas[1], thetas[2]));

                // cout << " U3"
                //      << " " << qregs[0] << " " << thetas[0] << " " << thetas[1] << " " << thetas[2] << endl;
            }
            else
            {
                // cout << "unrecognized qasm instruction: "
                //      << line << endl;
            }
        }
        // cout << endl;
    }

    infile.close();

    return circuit;
}

std::map<UINT, UINT> outcomesToFrequency(vector<UINT> outcomes)
{
    map<UINT, UINT> outcome_frequency;

    for (int i = 0; i < outcomes.size(); i++)
    {
        if (outcome_frequency.count(outcomes[i]) == 0)
            outcome_frequency.insert(pair<int, int>(outcomes[i], 1));

        else
            outcome_frequency[outcomes[i]]++;
    }

    return outcome_frequency;
}

UINT myPow(UINT x, UINT p)
{
    if (p == 0)
        return 1;
    if (p == 1)
        return x;

    UINT tmp = myPow(x, p / 2);
    if (p % 2 == 0)
        return tmp * tmp;
    else
        return x * tmp * tmp;
}

double chiLoss(std::vector<UINT> dist1, std::vector<UINT> dist2, int nqubit)
{
    std::map<UINT, UINT> map1 = outcomesToFrequency(dist1);
    std::map<UINT, UINT> map2 = outcomesToFrequency(dist2);

    double result = 0.0;
    for (int i = 0; i < (1 << nqubit); i++)
    {
        if (map1.count(i) != 0 || map2.count(i) != 0)
        {
            result += myPow(map1[i] - map2[i], 2) / (double)(map1[i] + map2[i]);
        }
    }
    return result;
}

double norm(std::map<UINT, UINT> map)
{
    double result = 0;

    auto it = map.begin();

    while (it != map.end())
    {
        result += myPow(it->second, 2);

        it++;
    }
    return sqrt(result);
}

double stateFidelity(std::vector<UINT> dist1, std::vector<UINT> dist2, int nqubit)
{
    std::map<UINT, UINT> map1 = outcomesToFrequency(dist1);
    std::map<UINT, UINT> map2 = outcomesToFrequency(dist2);

    double norm1 = norm(map1);
    double norm2 = norm(map2);

    double result = 0;

    for (int i = 0; i < (1 << nqubit); i++)
    {
        if (map1.count(i) != 0 && map2.count(i) != 0)
        {
            result += (map1[i] / norm1) * (map2[i] / norm2);
        }
    }

    return pow(result, 2);
}

char *getCmdOption(char **begin, char **end, const std::string &option)
{
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char **begin, char **end, const std::string &option)
{
    return std::find(begin, end, option) != end;
}

void printStats(std::vector<double> v)
{
    double sum = std::accumulate(v.begin(), v.end(), 0.0);
    double mean = sum / v.size();

    double sq_sum = std::inner_product(v.begin(), v.end(), v.begin(), 0.0);
    double stdev = std::sqrt(sq_sum / v.size() - mean * mean);

    printf("mean is: %.4f, stdev is: %.4f\n\n", mean, stdev);
}

double normFidelity(std::vector<UINT> ideal, std::vector<UINT> output, int nqubit)
{
    std::map<UINT, UINT> freq_ideal = outcomesToFrequency(ideal);
    std::map<UINT, UINT> freq_output = outcomesToFrequency(output);

    int sum_ideal = ideal.size();
    int sum_output = output.size();

    double prob_uni = sqrt((double)1 / myPow(2, nqubit));

    double f_ideal_out = 0.0;
    double f_ideal_uni = 0.0;

    for (int i = 0; i < (1 << nqubit); i++)
    {
        if (freq_ideal.count(i) != 0)
        {
            f_ideal_uni += sqrt(freq_ideal[i] / (double)sum_ideal) * prob_uni;

            if (freq_output.count(i) != 0)
            {
                f_ideal_out += sqrt(freq_ideal[i] / (double)sum_ideal) * sqrt(freq_output[i] / (double)sum_output);
            }
        }
    }

    f_ideal_uni *= f_ideal_uni;
    f_ideal_out *= f_ideal_out;

    // printf("f_ideal_uni: %f, f_ideal_out: %f\n", f_ideal_uni, f_ideal_out);

    return (f_ideal_out - f_ideal_uni) / (1 - f_ideal_uni);
}