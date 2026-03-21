# ngl_server

基于 Actor 模型的 C++ 游戏服务器框架。

[![Windows Server 2022](https://github.com/NingLeixueR/ngl/actions/workflows/cmake-windows-conan.yml/badge.svg)](https://github.com/NingLeixueR/ngl/actions/workflows/cmake-windows-conan.yml)
[![Ubuntu 22.04](https://github.com/NingLeixueR/ngl/actions/workflows/cmake-linux-conan.yml/badge.svg)](https://github.com/NingLeixueR/ngl/actions/workflows/cmake-linux-conan.yml)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=NingLeixueR_ngl&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=NingLeixueR_ngl)

## 项目简介

`ngl_server` 面向游戏服务端场景，核心实现位于 `public/cpp` 与 `server` 两个目录，当前已经具备：

- Actor 模型驱动的服务节点与逻辑模块
- TCP / WebSocket / KCP 网络栈
- CSV / XML / Protobuf 配置与协议支持
- 基于 GoogleTest 的单元、运行时与性能测试
- Conan + CMake 的跨平台构建链路

## 目录结构

- `public/cpp`：公共 C++ 源码，包含网络、序列化、日志、配置、Actor 基础设施等
- `server/libnglcore`：服务端核心运行时库
- `server/node`：服务端进程入口
- `server/tests`：GoogleTest 测试集
- `server/idl`：IDL 相关工具与生成入口
- `server/makeproto`：协议与 SQL 生成工具
- `bin/configure`：运行配置、CSV 表和脚本

更细的服务端子目录说明见 [server/README.md](./server/README.md)。

## 依赖

### Conan 管理

依赖定义位于 [conanfile.py](./conanfile.py)：

- `boost/1.86.0`
- `behaviortree.cpp/4.9.0`
- `lua/5.4.6`
- `protobuf/3.21.12`
- `libcurl/8.9.1`
- `libmysqlclient/8.1.0`
- `hiredis/1.2.0`
- `libpq/17.7`
- `rapidjson/1.1.0`
- `gtest/1.14.0`（测试依赖）

Linux 下额外依赖 `libuuid/1.0.3`。

### 仓库内嵌

- `asio`
- `kcp`
- `tinyxml2`
- `utf8cpp`

## 构建

下面以 Windows + Conan 2 + Visual Studio 2022 为例：

```powershell
conan install . --output-folder=build/Debug --build=missing -s build_type=Debug
cmake --preset conan-default
cmake --build --preset conan-debug
```

如果希望把性能测试也注册进 `ctest`，在配置阶段额外打开：

```powershell
cmake -S . -B build/Debug `
  -DCMAKE_TOOLCHAIN_FILE=build/Debug/conan_toolchain.cmake `
  -DNGL_TEST_REGISTER_PERF=ON
cmake --build build/Debug --config Debug --target ngl_test -- /m:1
```

## 测试

默认 `ctest` 只跑快速回归套件，适合本地和 CI：

```powershell
ctest --test-dir build/Debug -C Debug --output-on-failure
```

直接运行完整测试可执行文件：

```powershell
.\bin\Debug\ngl_test.exe --gtest_filter=-*Perf*
```

性能测试支持通过环境变量缩放迭代次数，降低本地或 CI 波动：

```powershell
$env:NGL_TEST_PERF_SCALE='10'
.\bin\Debug\ngl_test.exe --gtest_filter=*Perf*
```

如需查看性能测试的详细输出，可额外设置：

```powershell
$env:NGL_TEST_VERBOSE='1'
```

## 协议支持

- TCP
- WebSocket / WSS
- KCP
