# tests 测试模块

`server/tests` 是项目的回归验证层，使用 GoogleTest，覆盖运行时、网络、序列化、工具、配置和部分性能基准。

## 当前测试组织方式

`server/tests/CMakeLists.txt` 采用按文件扫描的方式收集测试：

- 所有 `*_gtest.cpp` 自动加入测试目标
- 默认注册 `ngl_test_fast`
- 非性能用例通过 `--gtest_filter=-*Perf*` 跑 fast 套件
- 性能套件需要显式打开 `NGL_TEST_REGISTER_PERF`

## 模块职责

- 验证 `nglcore` 的启动与运行时行为
- 验证 `public/cpp` 的基础设施行为
- 验证网络、WS/WSS、KCP、序列化、配置解析等关键路径
- 在不影响 CI 稳定性的前提下保留性能基准

## 典型测试类型

- 启动烟测
  `smoke_gtest.cpp`
- 运行时和配置
  `runtime_gtest.cpp`、`xml_sysconfig_gtest.cpp`
- 网络
  `net_gtest.cpp`、`ws_gtest.cpp`
- 工具与序列化
  `tools_gtest.cpp`、`serialize_gtest.cpp`
- 性能
  `perf_gtest.cpp`

## 测试支撑文件

- `test_support.h`
  通用测试工具、临时目录、性能输出等
- `runtime_test_support.h`
  运行时配置与 `tab_servers` 初始化辅助

## 设计特点

- 测试范围较广，既覆盖 happy path，也覆盖失败和边界条件
- 部分性能测试支持环境变量缩放，方便本地和 CI 复用
- 非 Perf 测试默认是 CI 主链路的一部分

## 常见改动入口

- 增加新模块测试
  新建一个 `*_gtest.cpp`
- 新增测试辅助
  优先放在 `test_support.h` 或 `runtime_test_support.h`
- 调整 CTest 注册方式
  改 `server/tests/CMakeLists.txt`

## 维护建议

- 保持 fast 套件稳定，不要把慢测试混进默认回归路径。
- 性能测试和功能测试继续分层，避免 CI 上出现误报超时。
- 测试文件本身也应遵守命名与可读性规范，否则会反向放大维护成本。
