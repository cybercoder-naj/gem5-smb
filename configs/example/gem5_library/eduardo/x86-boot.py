import argparse

from gem5.utils.requires import requires
from gem5.components.memory.single_channel import SingleChannelDDR3_1600
from gem5.components.cachehierarchies.classic.no_cache import NoCache
from gem5.components.processors.simple_processor import SimpleProcessor
from gem5.coherence_protocol import CoherenceProtocol
from gem5.isas import ISA
from gem5.components.processors.cpu_types import CPUTypes
from gem5.resources.resource import Resource,AbstractResource,DiskImageResource,CheckpointResource
from gem5.simulate.simulator import Simulator
from gem5.simulate.exit_event import ExitEvent
from gem5.components.boards.x86_two_disks import TwoDisksX86Board

##########################
# Add and Parse options
##########################

parser = argparse.ArgumentParser(
    description="Configuration"
)

parser.add_argument(
    "--num-cpus",
    type=int,
    help="Num of cores to simulate",
    default=1,
)

parser.add_argument(
    "--boot-with-kvm",
    action="store_true",
    default=False,
    help="Boot using KVM CPU instead",
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
    "--script",
    type=str,
    required=True,
    help="Script to launch the benchmark",
)

args = parser.parse_args()

##########################
# Requiements to run
##########################

requires(
    isa_required=ISA.X86,
    kvm_required=args.boot_with_kvm,
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
script=args.script

##########################
# Architecture
##########################

cache_hierarchy = NoCache()

processor = SimpleProcessor(
    isa=ISA.X86,
    cpu_type=(CPUTypes.KVM if args.boot_with_kvm else CPUTypes.ATOMIC),
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
    readfile=script,
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
