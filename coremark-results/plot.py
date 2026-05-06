from pathlib import Path
from tabulate import tabulate

def get_property(benchmark_name, property_name):
    """Extracts a property value from baseline and smb stats files.
    
    Returns a dictionary with structure: { property_name: { baseline: value, smb: value } }
    """
    script_dir = Path(__file__).resolve().parent
    result = {}
    
    # Get baseline value (from -stats-no-smb.txt)
    baseline_file = script_dir / f"{benchmark_name}-stats-no-smb.txt"
    if baseline_file.exists():
        with open(baseline_file, 'r') as f:
            for line in f:
                if line.startswith(property_name):
                    result["baseline"] = float(line.split()[1])
                    break
    else:
        print(f"Warning: Baseline stats file not found for {benchmark_name}")
        result["baseline"] = 0.0
    
    # Get smb value (from -stats-smb.txt)
    smb_file = script_dir / f"{benchmark_name}-stats-smb.txt"
    if smb_file.exists():
        with open(smb_file, 'r') as f:
            for line in f:
                if line.startswith(property_name):
                    result["smb"] = float(line.split()[1])
                    break
    else:
        print(f"Warning: SMB stats file not found for {benchmark_name}")
        result["smb"] = 0.0
    
    return result

IPC="system.cpu.ipc"
INSERTED_LOADS="system.cpu.MemDepUnit__0.insertedLoads"
BYPASSED="system.cpu.rename.bypassedLoads"
BYPASSED_VALUE_CHECK_FAILED="system.cpu.commit.bypassedLoadValueCheckViolation"

if __name__ == "__main__":
    benchmark_properties = {}

    script_dir = Path(__file__).resolve().parent
    for filename in script_dir.glob("*-stats-smb.txt"):
        benchmark_name = filename.stem.replace("-stats-smb", "")

        ipc = get_property(benchmark_name, IPC)
        insertedLoads = get_property(benchmark_name, INSERTED_LOADS)
        bypassed = get_property(benchmark_name, BYPASSED)
        bypassedValueCheckFailed = get_property(benchmark_name, BYPASSED_VALUE_CHECK_FAILED)

        benchmark_properties[benchmark_name] = {
            "bypassedLoads": bypassed,
            "insertedLoads": insertedLoads,
            "ipc": ipc,
            "bypassedValueCheckFailed": bypassedValueCheckFailed
        }

    print("\nBypassed percentage for each run:")
    
    table_data = []
    for benchmark, properties in benchmark_properties.items():
        bypassed = properties["bypassedLoads"]["smb"]
        insertedLoads = properties["insertedLoads"]["smb"]
        if insertedLoads > 0:
            bypassed_percentage = (bypassed / insertedLoads) * 100
        else:
            bypassed_percentage = 0

        ipc_percentage = properties["ipc"]["smb"] / properties["ipc"]["baseline"] if properties["ipc"]["baseline"] > 0 else 0

        value_check_failed = properties["bypassedValueCheckFailed"]["smb"]
        value_check_failed_percentage = (value_check_failed / bypassed) * 100 if bypassed > 0 else 0
        
        table_data.append([
            benchmark,
            f"{bypassed_percentage:.4f}%",
            f"{value_check_failed_percentage:.4f}%",
            f"{ipc_percentage:.4f}"
        ])
    
    headers = ["Benchmark", "Bypassed Loads %", "Mispredicted %", "IPC (Ratio to Baseline)"]
    print(tabulate(table_data, headers=headers, tablefmt="grid"))
