from pathlib import Path
from tabulate import tabulate
import argparse
import logging
import pathlib

def get_property(smb_file: pathlib.Path, baseline_file: pathlib.Path, property_name: str) -> dict[str, float]:
    """Extracts a property value from baseline and smb stats files.
    
    Returns a dictionary with structure: { property_name: { baseline: value, smb: value } }
    """
    result: dict[str, float] = {}
    
    # Get baseline value (from -stats-no-smb.txt)
    with open(baseline_file, 'r') as f:
        for line in f:
            if line.startswith(property_name):
                result["baseline"] = float(line.split()[1])
                break
    
    # Get smb value (from -stats-smb.txt)
    with open(smb_file, 'r') as f:
        for line in f:
            if line.startswith(property_name):
                result["smb"] = float(line.split()[1])
                break
    
    return result

NUM_INSTS="simInsts"
IPC="system.cpu.ipc"
INSERTED_LOADS="system.cpu.MemDepUnit__0.insertedLoads"
FALSE_DEPS="system.cpu.MemDepUnit__0.falseDependencies"
MASCOT_NDEP_MISPREDICTIONS="system.cpu.MemDepUnit__0.mascotNDepMispredictions"
MASCOT_MDP_MISPREDICTIONS="system.cpu.MemDepUnit__0.mascotMDPMispredictions"
MASCOT_SMB_MISPREDICTIONS="system.cpu.MemDepUnit__0.mascotSMBMispredictions"
BYPASSED="system.cpu.rename.bypassedLoads"
BYPASSED_VALUE_CHECK_FAILED="system.cpu.commit.bypassedLoadValueCheckViolation"
BYPASSED_MEM_ORDER_VIOLATION="system.cpu.lsq0.bypassedLoadMemOrderViolation"

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    _ = parser.add_argument('-v', '--verbose', action='store_true', help='Enable verbose output')
    args = parser.parse_args()

    logging.basicConfig(
        level=logging.DEBUG if args.verbose else logging.WARNING,
        format='%(message)s'
    )

    benchmark_properties: dict[str, dict[str, dict[str, float]]] = {}

    script_dir = Path(__file__).resolve().parent
    for filename in script_dir.glob("*-stats-smb.txt"):
        logging.debug(f"Found SMB stats file: {filename}")
        benchmark_name = filename.stem.replace("-stats-smb", "")

        script_dir = Path(__file__).resolve().parent
        smb_file = script_dir / f"{benchmark_name}-stats-smb.txt"
        if not smb_file.exists():
            logging.warning(f"Could not open SMB stats file for {benchmark_name}")
            continue
        baseline_file = script_dir / f"{benchmark_name}-stats-no-smb.txt"
        if not baseline_file.exists():
            logging.warning(f"Could not open NO-SMB stats file for {benchmark_name}")
            continue

        ipc = get_property(smb_file, baseline_file, IPC)
        logging.debug(f"Extracted IPC for {benchmark_name}: {ipc}")

        insertedLoads = get_property(smb_file, baseline_file, INSERTED_LOADS)
        logging.debug(f"Extracted insertedLoads for {benchmark_name}: {insertedLoads}")

        bypassed = get_property(smb_file, baseline_file, BYPASSED)
        logging.debug(f"Extracted bypassedLoads for {benchmark_name}: {bypassed}")

        bypassedValueCheckFailed = get_property(smb_file, baseline_file, BYPASSED_VALUE_CHECK_FAILED)
        logging.debug(f"Extracted bypassedValueCheckFailed for {benchmark_name}: {bypassedValueCheckFailed}")

        bypassMemOrderViolation = get_property(smb_file, baseline_file, BYPASSED_MEM_ORDER_VIOLATION)
        logging.debug(f"Extracted bypassedMemOrderViolation for {benchmark_name}: {bypassMemOrderViolation}")

        num_insts = get_property(smb_file, baseline_file, NUM_INSTS)
        logging.debug(f"Extracted bypassedMemOrderViolation for {benchmark_name}: {bypassMemOrderViolation}")

        false_deps = get_property(smb_file, baseline_file, FALSE_DEPS)
        logging.debug(f"Extracted bypassedMemOrderViolation for {benchmark_name}: {bypassMemOrderViolation}")

        mascot_ndep_mispredictions = get_property(smb_file, baseline_file, MASCOT_NDEP_MISPREDICTIONS)
        mascot_mdp_mispredictions = get_property(smb_file, baseline_file, MASCOT_MDP_MISPREDICTIONS)
        mascot_smb_mispredictions = get_property(smb_file, baseline_file, MASCOT_SMB_MISPREDICTIONS)

        benchmark_properties[benchmark_name] = {
            "bypassedLoads": bypassed,
            "insertedLoads": insertedLoads,
            "ipc": ipc,
            "bypassedValueCheckFailed": bypassedValueCheckFailed,
            "bypassedMemOrderViolation": bypassMemOrderViolation,
            "numInsts": num_insts,
            "falseDeps": false_deps,
            "mascotNDepMispredictions": mascot_ndep_mispredictions,
            "mascotMDPMispredictions": mascot_mdp_mispredictions,
            "mascotSMBMispredictions": mascot_smb_mispredictions,
        }

    logging.info("\nBypassed percentage for each run:")
    
    table1 = []
    table2 = []
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

        table1.append([
            benchmark[:benchmark.find('-')],
            f"{bypassed_percentage:.4f}%",
            f"{value_check_failed_percentage:.4f}%",
            f"{mem_order_violation_percentage:.4f}%",
            f"{ipc_percentage:.4f}",
        ])

        phast_mpki = properties["falseDeps"]["baseline"] / properties["numInsts"]["baseline"] * 1000
        mascot_mpki = properties["falseDeps"]["smb"] / properties["numInsts"]["smb"] * 1000
        mascot_ndep_mpki = properties["mascotNDepMispredictions"].get("smb", 0) / properties["numInsts"]["smb"] * 1000
        mascot_mdp_mpki = properties["mascotMDPMispredictions"].get("smb", 0) / properties["numInsts"]["smb"] * 1000
        mascot_smb_mpki = properties["mascotSMBMispredictions"].get("smb", 0) / properties["numInsts"]["smb"] * 1000

        table2.append([
            benchmark[:benchmark.find('-')],
            f"{phast_mpki:.4f}",
            f"{mascot_mpki:.4f}",
            f"{mascot_ndep_mpki:.4f}",
            f"{mascot_mdp_mpki:.4f}",
            f"{mascot_smb_mpki:.4f}",
        ])
    
    headers = ["Benchmark", "Bypassed Loads %", "Value Mismatch %", "Mem Violations %", "IPC Ratio"]
    print(tabulate(table1, headers=headers, tablefmt="grid"), end="\n\n")

    headers = [
        "Benchmark",
        "PHAST False Dep MPKI",
        "MASCOT False Dep MPKI",
        "MASCOT NDEP Miss MPKI",
        "MASCOT MDP Miss MPKI",
        "MASCOT SMB Miss MPKI",
    ]
    print(tabulate(table2, headers=headers, tablefmt="grid"), end="\n\n")
