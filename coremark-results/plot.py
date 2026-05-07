from pathlib import Path
from tabulate import tabulate
import argparse
import logging

def get_property(benchmark_name, property_name):
    """Extracts a property value from baseline and smb stats files.
    
    Returns a dictionary with structure: { property_name: { baseline: value, smb: value } }
    """
    script_dir = Path(__file__).resolve().parent
    result = {}
    
    # Get baseline value (from -stats-no-smb.txt)
    baseline_file = script_dir / f"/home/nj421/gem5-smb/m5out/{benchmark_name}/no-smb/stats.txt"
    if baseline_file.exists():
        with open(baseline_file, 'r') as f:
            for line in f:
                if line.startswith(property_name):
                    result["baseline"] = float(line.split()[1])
                    break
    else:
        logging.warning(f"Baseline stats file not found for {benchmark_name}")
        result["baseline"] = 0.0
    
    # Get smb value (from -stats-smb.txt)
    smb_file = script_dir / f"/home/nj421/gem5-smb/m5out/{benchmark_name}/smb/stats.txt"
    if smb_file.exists():
        with open(smb_file, 'r') as f:
            for line in f:
                if line.startswith(property_name):
                    result["smb"] = float(line.split()[1])
                    break
    else:
        logging.warning(f"SMB stats file not found for {benchmark_name}")
        result["smb"] = 0.0
    
    return result

IPC="system.cpu.ipc"
INSERTED_LOADS="system.cpu.MemDepUnit__0.insertedLoads"
BYPASSED="system.cpu.rename.bypassedLoads"
BYPASSED_VALUE_CHECK_FAILED="system.cpu.commit.bypassedLoadValueCheckViolation"
BYPASSED_MEM_ORDER_VIOLATION="system.cpu.lsq0.bypassedLoadMemOrderViolation"

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-v', '--verbose', action='store_true', help='Enable verbose output')
    args = parser.parse_args()

    logging.basicConfig(
        level=logging.DEBUG if args.verbose else logging.WARNING,
        format='%(message)s'
    )

    benchmark_properties = {}

    script_dir = Path(__file__).resolve().parent
    for filename in script_dir.glob("*-stats-smb.txt"):
        logging.debug(f"Found SMB stats file: {filename}")
        benchmark_name = filename.stem.replace("-stats-smb", "")

        ipc = get_property(benchmark_name, IPC)
        logging.debug(f"Extracted IPC for {benchmark_name}: {ipc}")

        insertedLoads = get_property(benchmark_name, INSERTED_LOADS)
        logging.debug(f"Extracted insertedLoads for {benchmark_name}: {insertedLoads}")

        bypassed = get_property(benchmark_name, BYPASSED)
        logging.debug(f"Extracted bypassedLoads for {benchmark_name}: {bypassed}")

        bypassedValueCheckFailed = get_property(benchmark_name, BYPASSED_VALUE_CHECK_FAILED)
        logging.debug(f"Extracted bypassedValueCheckFailed for {benchmark_name}: {bypassedValueCheckFailed}")

        bypassMemOrderViolation = get_property(benchmark_name, BYPASSED_MEM_ORDER_VIOLATION)
        logging.debug(f"Extracted bypassedMemOrderViolation for {benchmark_name}: {bypassMemOrderViolation}")

        benchmark_properties[benchmark_name] = {
            "bypassedLoads": bypassed,
            "insertedLoads": insertedLoads,
            "ipc": ipc,
            "bypassedValueCheckFailed": bypassedValueCheckFailed,
            "bypassedMemOrderViolation": bypassMemOrderViolation
        }

    logging.info("\nBypassed percentage for each run:")
    
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

        mem_order_violation = properties["bypassedMemOrderViolation"]["smb"]
        mem_order_violation_percentage = (mem_order_violation / bypassed) * 100 if bypassed > 0 else 0

        table_data.append([
            benchmark,
            f"{bypassed_percentage:.4f}%",
            f"{value_check_failed_percentage:.4f}%",
            f"{mem_order_violation_percentage:.4f}%",
            f"{ipc_percentage:.4f}"
        ])
    
    headers = ["Benchmark", "Bypassed Loads %", "Value Check Failed %", "Memory Order Violations %", "IPC (Ratio to Baseline)"]
    print(tabulate(table_data, headers=headers, tablefmt="grid"))
