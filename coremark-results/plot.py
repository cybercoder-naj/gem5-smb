from pathlib import Path
from tabulate import tabulate
import argparse
import logging
import pathlib

def get_properties(file: pathlib.Path, props: list[str]) -> dict[str, float]:
    """Extracts properties from stats files.
    
    Returns a dictionary with structure: { property_name: value }
    """
    prop_values: dict[str, float] = {}
    
    # Get baseline value (from -stats-no-smb.txt)
    with open(file, 'r') as f:
        for line in f:
            parts = line.split()
            if len(parts) < 2:
                continue

            prop_name = parts[0]

            if prop_name in props:
                try:
                    prop_values[prop_name] = float(parts[1])
                except ValueError:
                    continue
    
    return prop_values

NUM_INSTS="simInsts"
IPC="system.cpu.ipc"
INSERTED_LOADS="system.cpu.MemDepUnit__0.insertedLoads"
FALSE_DEPS="system.cpu.MemDepUnit__0.falseDependencies"
MASCOT_NDEP_PREDICTIONS="system.cpu.MemDepUnit__0.predictsNDep"
MASCOT_SMB_PREDICTIONS="system.cpu.MemDepUnit__0.predictsSMB"
MASCOT_MDP_PREDICTIONS="system.cpu.MemDepUnit__0.predictsMDP"
MASCOT_NDEP_MISPREDICTIONS="system.cpu.MemDepUnit__0.ndepViolations"
MASCOT_MDP_MISPREDICTIONS="system.cpu.MemDepUnit__0.mdpViolations"
MASCOT_SMB_MISPREDICTIONS="system.cpu.MemDepUnit__0.smbViolations"
BYPASSED="system.cpu.rename.bypassedLoads"
BYPASSED_VALUE_CHECK_FAILED="system.cpu.commit.bypassedLoadValueCheckViolation"
BYPASSED_MEM_ORDER_VIOLATION="system.cpu.lsq0.bypassedLoadMemOrderViolation"
TOTAL_MEM_ORDER_VIOLATIONS="system.cpu.commit.memOrderViolationEvents"

properties = [
    NUM_INSTS,
    IPC,
    INSERTED_LOADS,
    FALSE_DEPS,
    MASCOT_MDP_PREDICTIONS,
    MASCOT_MDP_PREDICTIONS,
    MASCOT_SMB_PREDICTIONS,
    MASCOT_NDEP_MISPREDICTIONS,
    MASCOT_MDP_MISPREDICTIONS,
    MASCOT_SMB_MISPREDICTIONS,
    BYPASSED,
    BYPASSED_VALUE_CHECK_FAILED,
    BYPASSED_MEM_ORDER_VIOLATION,
    TOTAL_MEM_ORDER_VIOLATIONS
]

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

        baseline_stats = get_properties(baseline_file, properties) 
        smb_stats = get_properties(smb_file, properties) 

        benchmark_properties[benchmark_name] = {
            'baseline': baseline_stats,
            'smb': smb_stats
        }

    logging.info("\nBypassed percentage for each run:")
    
    table: list[list[str]] = []
    for benchmark, properties in benchmark_properties.items():
        insertedLoads = properties["smb"][INSERTED_LOADS]
        predictedSMB = properties["smb"][MASCOT_SMB_PREDICTIONS]
        bypassedLoads = properties["smb"][BYPASSED]
        smbViolations = properties["smb"][MASCOT_SMB_MISPREDICTIONS]
        smbSuccesses = bypassedLoads - smbViolations

        bypassPredPct = (predictedSMB / insertedLoads) * 100
        actualBypassPct = (bypassedLoads / predictedSMB) * 100
        successRate = (smbSuccesses / bypassedLoads) * 100
        violationRate = (smbViolations / bypassedLoads) * 100

        ipcPct = (properties["smb"][IPC] / properties["baseline"][IPC])

        table.append([
            benchmark,
            f"{bypassPredPct:.4f}%",
            f"{actualBypassPct:.4f}%",
            f"{successRate:.4f}%",
            f"{violationRate:.4f}%",
            f"{ipcPct:.4f}"
        ])

    
    headers = ["Benchmark", "SMB Pred %", "Actual bypass %", "Correct bypass %", "Violation %", "IPC Ratio"]
    print(tabulate(table, headers=headers, tablefmt="grid"), end="\n\n")
