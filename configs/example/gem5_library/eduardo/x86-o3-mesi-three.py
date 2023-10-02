import os
import argparse

from gem5.utils.requires import requires
from gem5.components.memory.single_channel import SingleChannelDDR3_1600
from gem5.components.cachehierarchies.ruby.mesi_three_level_cache_hierarchy import MESIThreeLevelCacheHierarchy
from gem5.components.processors.custom_processor import CustomProcessor
from gem5.coherence_protocol import CoherenceProtocol
from gem5.isas import ISA
from gem5.components.processors.cpu_types import CPUTypes
from gem5.resources.resource import Resource,AbstractResource,DiskImageResource,CheckpointResource
from gem5.simulate.simulator import Simulator
from gem5.simulate.exit_event import ExitEvent
from gem5.components.boards.x86_two_disks import TwoDisksX86Board

import importlib

##########################
# Add and Parse options
##########################

parser = argparse.ArgumentParser(
    description="Configuration"
)

parser.add_argument(
    "--cpu",
    type=str,
    help="CPU to use",
    default=1,
)

parser.add_argument(
    "--num-cpus",
    type=int,
    help="Num of cores to simulate",
    default=1,
)

parser.add_argument(
    "--kernel",
    type=str,
    required=True,
    help="Kernel binary to be used",
)

parser.add_argument(
    "--main-disk",
    type=str,
    required=True,
    help="OS Disk",
)

parser.add_argument(
    "--main-disk-partition",
    type=str,
    default="1",
    help="Which partition (main disk) should be loaded",
)

parser.add_argument(
    "--secondary-disk",
    type=str,
    required=True,
    help="Benchmark Disk",
)

parser.add_argument(
    "--secondary-disk-partition",
    type=str,
    default="1",
    help="Which partition (secondary disk) should be loaded",
)

parser.add_argument(
    "--checkpoint-dir",
    type=str,
    default="",
    help="Checkpoint directory",
)

parser.add_argument(
    "--checkpoint-num",
    type=int,
    default=0,
    help="Which checkpoint should be loaded 0 1 2 ...",
)

parser.add_argument(
    "--checkpoint",
    type=str,
    default="",
    help="Specific checkpoint to load",
)

parser.add_argument(
    "--l1i_size",
    type=str,
    default="",
    help="",
)

parser.add_argument(
    "--l1i_assoc",
    type=str,
    default="",
    help="",
)

parser.add_argument(
    "--l1d_size",
    type=str,
    default="",
    help="",
)

parser.add_argument(
    "--l1d_assoc",
    type=str,
    default="",
    help="",
)

parser.add_argument(
    "--l2_size",
    type=str,
    default="",
    help="",
)

parser.add_argument(
    "--l2_assoc",
    type=str,
    default="",
    help="",
)

parser.add_argument(
    "--l3_size",
    type=str,
    default="",
    help="",
)

parser.add_argument(
    "--l3_assoc",
    type=str,
    default="",
    help="",
)

parser.add_argument(
    "--l3_banks",
    type=int,
    default=1,
    help="",
)



args = parser.parse_args()

##########################
# Requiements to run
##########################

requires(
    isa_required=ISA.X86,
    coherence_protocol_required=CoherenceProtocol.MESI_THREE_LEVEL,
    kvm_required=False,
)

##########################
# Disks and Scripts
##########################

kernel=AbstractResource(
    local_path=args.kernel,
)

disk=DiskImageResource(
    local_path=args.main_disk,
    root_partition=args.main_disk_partition,
)
sec_disk=DiskImageResource(
    local_path=args.secondary_disk,
    root_partition=args.secondary_disk_partition,
)

# Find the newest checkpoint
ckpt = None

if args.checkpoint != "":
    ckpt=CheckpointResource(args.checkpoint)
    print(f"Using Checkpoint: {args.checkpoint}");
    
else:
    if args.checkpoint_dir != "":
        
        def checkpoint_filter(d):
            if (d[-1] == 't' and d[-2] == 'p' and d[-3] == 'c' and d[-4] == '.' and d[-5] == '5' and d[-6] == 'm'):
                return True
            return False

        def absoluteFilePaths(directory):
            for dirpath,_,filenames in os.walk(directory):
                for f in filenames:
                    yield os.path.abspath(os.path.join(dirpath, f))

        ckpt_dir=args.checkpoint_dir
        ckpts = list(filter(checkpoint_filter,absoluteFilePaths(ckpt_dir)))
        ckpts.sort()
        
        print(f"Found Checkpoints: {ckpts}")
        
        ckpt=CheckpointResource(local_path=ckpts[args.checkpoint_num][:-6])
        print(f"Using Checkpoint: {ckpts[args.checkpoint_num][:-6]}");

##########################
# Architecture
##########################

cache_hierarchy = MESIThreeLevelCacheHierarchy (
    l1i_size=args.l1i_size,
    l1i_assoc=args.l1i_assoc,
    l1d_size=args.l1d_size,
    l1d_assoc=args.l1d_assoc,
    l2_size=args.l2_size,
    l2_assoc=args.l2_assoc,
    l3_size=args.l3_size,
    l3_assoc=args.l3_assoc,
    num_l3_banks=args.l3_banks,
)

custom_cpu = getattr(importlib.import_module("cores.x86." + args.cpu), args.cpu + "_CPU")
processor = CustomProcessor(
    isa=ISA.X86,
    cputype=custom_cpu,
    num_cores=args.num_cpus,
)

memory = SingleChannelDDR3_1600(size="3GiB")

board = TwoDisksX86Board(
    clk_freq="1GHz",
    processor=processor,
    memory=memory,
    cache_hierarchy=cache_hierarchy,
    secondary_disk=sec_disk,
    root_disk_name="/dev/hda"
)

board.set_kernel_disk_workload(
    kernel=kernel,
    disk_image=disk,
    checkpoint=ckpt,
)

##########################
# Run the simulator
##########################

simulator = Simulator(
    board=board,
    full_system=True,    
)

simulator.run()

print(
    "Exiting @ tick {} because {}.".format(
        simulator.get_current_tick(), simulator.get_last_exit_event_cause()
    )
)
