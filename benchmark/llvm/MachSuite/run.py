import os
import typing

from .. import llvm_common
from ... import common


def arguments(directory: str) -> [str]:
    return [os.path.join(directory, "input.data"), os.path.join(directory, "check.data")]


def main(args: typing.Any):
    base_directory = os.path.dirname(__file__)
    llvm_common.run(
        base_directory,
        args.j,
        llvm_common.get_llvm_prefix() + "/MachSuite",
        arguments,
        common.backend_parsing(args.b),
        common.args_to_component(args),
        args.b
    )


if __name__ == '__main__':
    main(common.parse_jit_args())