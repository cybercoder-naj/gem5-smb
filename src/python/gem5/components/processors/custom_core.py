from typing import Optional
from ...utils.requires import requires
from .base_cpu_core import BaseCPUCore
from .cpu_types import CPUTypes
from ...isas import ISA
from ...utils.requires import requires
from ...runtime import get_runtime_isa
import importlib
import platform


class CustomCore(BaseCPUCore):

    def __init__(
        self, cputype, core_id: int, isa: Optional[ISA] = None
    ):

        # If the ISA is not specified, we infer it via the `get_runtime_isa`
        # function.
        if isa:
            requires(isa_required=isa)
            isa = isa
        else:
            isa = get_runtime_isa()

        super().__init__(
            core=cputype(),
            isa=isa,
        )
        self.core.cpu_id = core_id

        self._cpu_type = CPUTypes.O3

    def get_type(self) -> CPUTypes:
        return CPUTypes.O3
