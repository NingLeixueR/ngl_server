# ngl

基于 Actor 模型的 C++ 游戏服务器框架

[![Windows Server 2022](https://github.com/NingLeixueR/ngl/actions/workflows/cmake-windows-conan.yml/badge.svg)](https://github.com/NingLeixueR/ngl/actions/workflows/cmake-windows-conan.yml)
[![Ubuntu 22.04](https://github.com/NingLeixueR/ngl/actions/workflows/cmake-linux-conan.yml/badge.svg)](https://github.com/NingLeixueR/ngl/actions/workflows/cmake-linux-conan.yml)
[![Ubuntu Latest + Conan Cache](https://github.com/NingLeixueR/ngl/actions/workflows/cmake-linux-conan-cache.yml/badge.svg)](https://github.com/NingLeixueR/ngl/actions/workflows/cmake-linux-conan-cache.yml)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=NingLeixueR_ngl&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=NingLeixueR_ngl)

---

## 项目简介
ngl 是一款基于 **Actor 模型** 的 C++ 游戏服务器框架，追求轻量、高效、易扩展。

## 依赖第三方库

### Conan 管理依赖
配置文件：[conanfile.txt](https://github.com/NingLeixueR/ngl_server/blob/main/conanfile.txt)
- **curl**
- **hiredis**
- **mysql-connector-c**
- **lua**
- **protobuf**（如需修改版本，需重新生成 .cc 文件）
- **libpq**

### 内嵌第三方库（无需包管理）
- **asio**（独立版本，不依赖 Boost）
- **cjson**
- **kcp**
- **tinyxml2**
- **utf8cpp**

## 目录结构
- `ngl/public/cpp`：C++ 服务器公共头文件与源码
- `ngl/server`：项目核心服务实现

## 协议支持
- TCP
- KCP

## 交流 QQ 群
- 群号：56257994