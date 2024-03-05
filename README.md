## ngl
* Actor based c++distributed game server framework
* 基于Actor的c++分布式游戏服务器框架

## 依赖的第三方库
  * <b>bcp 剪裁的 boost</b>
    <br/>./bcp chrono ../boost_out
    <br/>./bcp date_time ../boost_out
    <br/>./bcp exception ../boost_out
    <br/>./bcp filesystem ../boost_out
    <br/>./bcp smart_ptr ../boost_out
    <br/>./bcp system thread ../boost_out
    <br/>./bcp bimap ../boost_out
    <br/>./bcp crc ../boost_out
    <br/>./bcp interprocess ../boost_out
    <br/>./bcp pool ../boost_out
    <br/>./bcp random.hpp ../boost_out
    <br/>./bcp algorithm ../boost_out
    <br/>./bcp random.hpp ../boost_out
    <br/>./bcp property_tree ../boost_out
    <br/>./bcp uuid ../boost_out
  * <b>cryptopp</b>
  * <b>curl</b>
  * <b>hiredis</b>
  * <b>mysql connect/c</b>
  * <b>protobuf 3.13.0</b>

## 目录
  * <b>ngl/3part</b><br/>此目录用于存放第三方库
  * <b>ngl/tools/public/idl</b><br/>csv表生成
  * <b>ngl/tools/public/proto</b><br/>proto协议生成
  * <b>ngl/tools/public/rebuild.bat</b><br/>与cmake unity build类似的加速编译
  * <b>ngl/public/cpp</b><br/>cpp源码
  * <b>ngl/public/csharp</b><br/>cs源码
  * <b>ngl/server</b><br/>项目源文件

## 协议支持
  * tcp
  * ws
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

## QQ群
  * 56257994
