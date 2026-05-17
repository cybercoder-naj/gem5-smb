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
        insertedLoads = get_property(smb_file, baseline_file, INSERTED_LOADS)
        bypassed = get_property(smb_file, baseline_file, BYPASSED)
        bypassedValueCheckFailed = get_property(smb_file, baseline_file, BYPASSED_VALUE_CHECK_FAILED)
        bypassMemOrderViolation = get_property(smb_file, baseline_file, BYPASSED_MEM_ORDER_VIOLATION)
        num_insts = get_property(smb_file, baseline_file, NUM_INSTS)
        false_deps = get_property(smb_file, baseline_file, FALSE_DEPS)
        total_mem_order= get_property(smb_file, baseline_file, TOTAL_MEM_ORDER_VIOLATIONS)
        predictsNDep = get_property(smb_file, baseline_file, MASCOT_NDEP_PREDICTIONS)
        predictsMDP = get_property(smb_file, baseline_file, MASCOT_MDP_PREDICTIONS)
        predictsSMB = get_property(smb_file, baseline_file, MASCOT_SMB_PREDICTIONS)

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
            "predictsNDep": predictsNDep,
            "predictsSMB": predictsSMB,
            "predictsMDP": predictsMDP,
            "totalMemOrder": total_mem_order
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

        dependencePrediction = (properties["predictsSMB"]["smb"] + properties["predictsMDP"]["smb"]) / insertedLoads * 100
        memOrderViolations = (properties["totalMemOrder"]["smb"]) / insertedLoads * 100

        table1.append([
            benchmark[:benchmark.find('-')],
            f"{dependencePrediction:.4f}%",
            f"{memOrderViolations:.4f}%",
            f"{ipc_percentage:.4f}",
        ])

        phast_mpki = (properties["falseDeps"]["baseline"] + properties["totalMemOrder"]["baseline"]) / properties["numInsts"]["baseline"] * 1000
        mascot_mpki = (properties["falseDeps"]["smb"] + properties["totalMemOrder"]["smb"]) / properties["numInsts"]["smb"] * 1000
        # mascot_ndep = properties["mascotNDepMispredictions"]["smb"]
        # mascot_mdp = properties["mascotMDPMispredictions"]["smb"]
        # mascot_smb = properties["mascotSMBMispredictions"]["smb"]
        
        mpki_percentage = abs(phast_mpki - mascot_mpki) / phast_mpki * 100

        table2.append([
            benchmark[:benchmark.find('-')],
            f"{phast_mpki:.4f}",
            f"{mascot_mpki:.4f}",
            f"{'+' if mascot_mpki > phast_mpki else '-'}{mpki_percentage:.4f}%"
            # f"{mascot_ndep:.4f}",
            # f"{mascot_mdp:.4f}",
            # f"{mascot_smb:.4f}",
        ])
    
    headers = ["Benchmark", "Loads Stalled %", "Mem Violations %", "IPC Ratio"]
    print(tabulate(table1, headers=headers, tablefmt="grid"), end="\n\n")

    headers = [
        "Benchmark",
        "PHAST MPKI",
        "MASCOT MPKI",
        "% change",
        # "Mascot NDEP violations",
        # "Mascot MDP violations",
        # "Mascot SMB violations",
    ]
    print(tabulate(table2, headers=headers, tablefmt="grid"), end="\n\n")
