from tqsim import get_command
import subprocess
from tqdm import tqdm
import json
import sys

def main():
    
    custom_testing = False
    
    if len(sys.argv) == 3:
        # If a class name is provided as an argument, use it
        classes = [sys.argv[1]]
        index_range = [int(sys.argv[2])]
        custom_testing = True
    else:
        # Otherwise, use the default classes and index range
        classes = ['adder', 'bv', 'mul', 'qaoa', 'qft', 'qpe', 'qsc', 'qv']
        index_range = range(6)
    
    max_qubits = 13

    results = {}

    for class_name in tqdm(classes):
        results[class_name] = {
            'speedup': {},
            'fidelity_difference': {}
        }
        
        for index in tqdm(index_range, leave=False):

            command, nqubits = get_command(class_name, index)
            
            if not custom_testing and nqubits > max_qubits:
                break # skip all the rest benchmarks in this class since they will be too slow

            output = subprocess.check_output(command, shell=True)
            with open("result.json", "a") as f:
                f.write(output.decode() + "\n")
            # Decode bytes to string
            output_str = output.decode("utf-8")

            # Split the output into lines and get the last three
            lines = output_str.strip().split("\n")[-3:]

            speedup = float(lines[-3])

            fidelity_difference = abs(float(lines[-2]) - float(lines[-1]))
            
            results[class_name]['speedup'][index] = speedup
            results[class_name]['fidelity_difference'][index] = fidelity_difference
    
    # check if the results file already exists
    try:
        with open('results.json', 'r') as f:
            existing_results = json.load(f)
            
        # merge the new results with the existing ones
        for class_name in results:
            if class_name not in existing_results:
                existing_results[class_name] = results[class_name]
            else:
                for index in results[class_name]['speedup']:
                    existing_results[class_name]['speedup'][index] = results[class_name]['speedup'][index]
                    existing_results[class_name]['fidelity_difference'][index] = results[class_name]['fidelity_difference'][index]
        # save the merged results
        results = existing_results    
    except FileNotFoundError:
        # if the file doesn't exist, just save the new results
        pass
    
    with open('results.json', 'w') as f:
        json.dump(results, f, indent=4)
            
    
if __name__ == '__main__':
    main()
