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
    
    table1: list[list[str]] = []
    table2: list[list[str]] = []
    table3: list[list[str]] = []
    for benchmark, properties in benchmark_properties.items():
        bypassed = properties["smb"][BYPASSED]
        insertedLoads = properties["smb"][INSERTED_LOADS]
        if insertedLoads > 0:
            bypassed_percentage = (bypassed / insertedLoads) * 100
        else:
            bypassed_percentage = 0

        ipc_percentage = properties["smb"][IPC] / properties["baseline"][IPC] if properties["baseline"][IPC] > 0 else 0

        table1.append([
            benchmark[:benchmark.find('-')],
            f"{bypassed_percentage:.4f}%",
            f"{ipc_percentage:.4f}",
        ])

        phast_mpki = (properties["baseline"][FALSE_DEPS] + properties["baseline"][TOTAL_MEM_ORDER_VIOLATIONS]) / properties["baseline"][NUM_INSTS] * 1000
        mascot_mpki = (properties["smb"][FALSE_DEPS] + properties["smb"][TOTAL_MEM_ORDER_VIOLATIONS]) / properties["smb"][NUM_INSTS] * 1000
        mpki_percentage = (mascot_mpki - phast_mpki) / phast_mpki * 100

        table2.append([
            benchmark[:benchmark.find('-')],
            f"{phast_mpki:.4f}",
            f"{mascot_mpki:.4f}",
            f"{mpki_percentage:.2f}%"
        ])

        predictedSMB = properties["smb"][MASCOT_SMB_PREDICTIONS]
        mispredictedSMB = properties["smb"][MASCOT_SMB_MISPREDICTIONS]
        correctSMBPredictions = bypassed - mispredictedSMB
        smbAccuracy = (correctSMBPredictions / bypassed) * 100
        table3.append([
            benchmark[:benchmark.find('-')],
            f"{predictedSMB}",
            f"{bypassed}",
            f"{correctSMBPredictions}",
            f"{mispredictedSMB}",
            f"{smbAccuracy:.2f}%"
        ])

    
    headers = ["Benchmark", "Bypassed Loads %", "IPC Ratio"]
    print(tabulate(table1, headers=headers, tablefmt="grid"), end="\n\n")

    headers = [
        "Benchmark",
        "PHAST MPKI",
        "MASCOT MPKI",
        "% improvement",
    ]
    print(tabulate(table2, headers=headers, tablefmt="grid"), end="\n\n")

    headers = [
        "Benchmark",
        "# SMB Predictions",
        "# Loads Bypassed",
        "# Correct Bypassing",
        "# Bypassing Violations",
        'Accuracy'
    ]
    print(tabulate(table3, headers=headers, tablefmt="grid"), end="\n\n")
