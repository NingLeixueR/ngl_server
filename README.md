[![Build C++](https://github.com/NingLeixueR/ngl/actions/workflows/cmake-unbutu.yml/badge.svg)](https://github.com/NingLeixueR/ngl/actions/workflows/cmake-unbutu.yml)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=NingLeixueR_ngl&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=NingLeixueR_ngl)
## ngl
* 基于Actor的c++游戏服务器框架


## 依赖的第三方库
  * <b>非boost独立版本的asio</b>
  * <b>curl</b>
  * <b>hiredis</b>
  * <b>mysql connect/c</b>
  * <b>lua-5.4.2</b>
  * <b>lua-cjson-2.1.0</b>

## 目录
  * <b>ngl/public/cpp</b><br/>cpp服务器源码
  * <b>ngl/server</b><br/>项目源文件

## 协议支持
  * tcp
  * kcp

## Linux CentOs编译
  * 编译安装gcc (GCC) 13.1.0
  * 编译安装protobuf 3.12.2
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

## 例子游戏
  * [猜数字](https://github.com/NingLeixueR/ngl_server/wiki/%E4%BE%8B%E5%AD%90%E6%B8%B8%E6%88%8F%E2%80%90%E7%8C%9C%E6%95%B0%E5%AD%97)

## 更多介绍
  * [wiki](https://github.com/NingLeixueR/ngl_server/wiki)
  * [deepwiki](https://deepwiki.com/NingLeixueR/ngl_server)

## QQ群
  * 56257994