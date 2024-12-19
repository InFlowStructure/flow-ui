# flow-ui

[![CMake](https://github.com/InFlowStructure/flow-ui/actions/workflows/cmake.yml/badge.svg)](https://github.com/InFlowStructure/flow-ui/actions/workflows/cmake.yml)

A cross-platform C++ UI library for visualising the [flow-core](https://github.com/InFlowStructure/flow-core) library.

## Building

To build the project with CMake, simply run
```bash
cmake -B build
cmake --build build --parallel
```

To build tests, configure the build directory with the following
```bash
cmake -B build -Dflow-ui_BUILD_TESTS=ON
```

To use flow-core installed on your system instead of the submodule:
```bash
cmake -B build -Dflow-ui_USE_EXTERNAL_FLOW_CORE=ON
```

## Installing

To install, configure the cmake build as follows:
```bash
cmake -B build -Dflow-ui_INSTALL=ON
```
Then, simply build the project normally, then run one of
```bash
cmake --install build
```

> [!TIP]
> Try adding the `--config` flag with the appropriate build type.

## Dependencies

This project depends on the following open source projects:
- [spdlog](https://github.com/gabime/spdlog)
- [nativefiledialog-extended](https://github.com/btzy/nativefiledialog-extended.git)
- [imgui](https://github.com/pthom/imgui.git)
- [imgui-node-editor](https://github.com/pthom/imgui-node-editor.git)
- [hello_imgui](https://github.com/pthom/hello_imgui.git)
