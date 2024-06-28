[![Build C++](https://github.com/NingLeixueR/ngl/actions/workflows/cmake-unbutu.yml/badge.svg)](https://github.com/NingLeixueR/ngl/actions/workflows/cmake-unbutu.yml)
## ngl
* Actor based c++distributed game server framework
* 基于Actor的c++分布式游戏服务器框架

## 依赖的第三方库
  * <b>非boost独立版本的asio</b>
  * <b>curl</b>
  * <b>hiredis</b>
  * <b>mysql connect/c</b>
  * <b>protobuf 3.13.0</b>

## 目录
  * <b>ngl/3part</b><br/>此目录用于存放第三方库
  * <b>ngl/bin/idl</b><br/>csv表生成
  * <b>ngl/bin/proto</b><br/>proto协议生成
  * <b>ngl/bin/rebuild.bat</b><br/>与cmake unity build类似的加速编译
  * <b>ngl/public/cpp</b><br/>cpp服务器源码
  * <b>ngl/public/csharp</b><br/>cs客户端源码(主要包含消息、网络、csv的读取)
  * <b>ngl/server</b><br/>项目源文件

## 协议支持
  * tcp
  * ws(暂时移除)
  * kcp

## Linux CentOs编译
  * 编译安装gcc (GCC) 13.1.0
  * 编译安装protobuf 3.13.0
  * 下载新版本的boost,boost编译并剪裁后将其放置在目录/3part/boost/下
  * 其他第三方库自行  yum install 进行安装
  * 创建build目录，在build目录下cmake .., 然后make
  * 在linux编译运行
     ```
     [root@localhost bin]# cat /etc/centos-release
     CentOS Linux release 7.9.2009 (Core)
     [root@localhost bin]# gcc --version
     gcc (GCC) 13.1.0
     ```
  * 在win编译运行(vs2022)

## 更多介绍
  * [wiki](https://github.com/NingLeixueR/ngl/wiki)

## QQ群
  * 56257994
