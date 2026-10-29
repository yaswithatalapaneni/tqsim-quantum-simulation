import re
import os
import sys
import numpy as np
import math

from benchmark_list import benchmarks
from benchmark_setup import setup


def loadGates(filename):
    f = open(filename)
    lines = f.readlines()

    gate_arities = []
    qubits = 0

    for line in lines:
        if line.strip() == "":
            continue  # skip empty lines

        inst = re.split('\s+', line.strip())[0][0:2]

        if inst == 'qr':
            if line[8] == ']':
                qubits = int(line[7])
            else:
                qubits = int(line[7:9])
        elif inst == 'cx':
            gate_arities.append(2)
        elif inst[0] == 'u':
            gate_arities.append(1)
        elif inst == 'H ' or inst == 'H':
            gate_arities.append(1)
        elif inst == 'CN':
            gate_arities.append(2)

    f.close()
    return gate_arities, qubits


def firstCricNoisy(gate_arities, min_gates, single_error_rate, two_error_rate):
    gates = len(gate_arities) >> 1
    gates = min(gates, min_gates)

    overall_error_rate = 1.0

    for i in range(gates):
        if gate_arities[i] == 1:
            overall_error_rate *= (1 - single_error_rate)
        else:
            overall_error_rate *= (1 - two_error_rate)

    return (gates, 1 - overall_error_rate)


def minFirstShot(p, N):
    z = 2.576
    e = 0.04

    shots = z ** 2 * p * (1 - p) / e ** 2
    shots = shots / (1 + shots / N)

    return max(1, int(shots))  # prevent zero


def remainingGates(total_gates, min_gates, max_subcircs):
    gates = []

    while total_gates > 0 and len(gates) < max_subcircs - 1:
        gates.append(min(total_gates, min_gates))
        total_gates -= min_gates

    if total_gates > 0:
        topup = int(total_gates / len(gates)) if len(gates) > 0 else total_gates

        total_gates -= len(gates) * topup

        for i in range(len(gates)):
            gates[i] += topup

        if len(gates) > 0:
            gates[0] += total_gates

    return gates


def totalShots(shots):
    total_shots = 1
    for shot in shots:
        total_shots *= shot
    return total_shots


def arrToStr(arr):
    return '-'.join(str(x) for x in arr)


def appendCMD(command, new_cmd, args):
    command += ' ' + new_cmd + ' '
    if args != 'null':
        command += args
    return command


def minIndex(shots):
    values = []
    for i in range(len(shots)):
        total = 1
        for j in range(len(shots)):
            if i != j:
                total *= shots[j]
        values.append(total)
    return np.argmin(values)


def adjustShots(shots, total_shots):
    while totalShots(shots) < total_shots:
        shots[minIndex(shots)] += 1
    return shots


def get_command(benchmark_class, benchmark_index):

    filename = '../benchmark_circuits/' + benchmark_class + '/' + benchmarks[benchmark_class][benchmark_index] + '.txt'

    gate_arities, qubits = loadGates(filename)

    subcirc_gates = []
    local_setup = setup.copy()

    if benchmark_class == "shor":
        local_setup['total_shots'] = 1024

    first_gates, overall_error_rate = firstCricNoisy(
        gate_arities,
        local_setup['min_gates'],
        local_setup['single_error_rate'],
        local_setup['two_error_rate']
    )

    subcirc_gates.append(first_gates)

    shots = [minFirstShot(overall_error_rate, local_setup['total_shots'])]

    if shots[0] == 0:
        shots[0] = 1

    remaining_shots = local_setup['total_shots'] / shots[0]

    max_subcircs = min(local_setup['max_subcircs'], int(math.log(max(remaining_shots, 1), 2)) + 1)

    subcirc_gates += remainingGates(len(gate_arities) - subcirc_gates[0],
                                   local_setup['min_gates'],
                                   max_subcircs)

    for i in range(len(subcirc_gates) - 1):
        shots.append(max(1, min(int(remaining_shots ** (1 / max(len(subcirc_gates) - 1, 1))), shots[0])))

    index = 1

    while totalShots(shots) < local_setup['total_shots']:
        shots[index] += 1

        if totalShots(shots) >= local_setup['total_shots']:
            shots[index] -= 1
            adjustShots(shots, local_setup['total_shots'])
            break

        index += 1

        if index == len(shots):
            index = 0 if shots[0] <= shots[1] else 1

    command = './../out/tqsim-cpu'
    command = appendCMD(command, '-c', filename)
    command = appendCMD(command, '-s', arrToStr(shots))
    command = appendCMD(command, '-l', arrToStr(subcirc_gates))
    command = appendCMD(command, '-t', 'null')

    return command, qubits


if __name__ == '__main__':
    benchmark_class = str(sys.argv[1])
    benchmark_index = int(sys.argv[2])

    command = get_command(benchmark_class, benchmark_index)[0]
    os.system(command)
