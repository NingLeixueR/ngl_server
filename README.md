# ngl
Actor based c++distributed game server framework
基于Actor的c++分布式游戏服务器框架

# 依赖的第三方库
  * bcp 剪裁的 boost
   ```
   ./bcp chrono ../boost_out
   ./bcp date_time ../boost_out
   ./bcp exception ../boost_out
   ./bcp filesystem ../boost_out
   ./bcp smart_ptr ../boost_out
   ./bcp system thread ../boost_out
   ./bcp bimap ../boost_out
   ./bcp crc ../boost_out
   ./bcp interprocess ../boost_out
   ./bcp pool ../boost_out
   ./bcp random.hpp ../boost_out
   ./bcp algorithm ../boost_out
   ./bcp random.hpp ../boost_out
   ./bcp property_tree ../boost_out
   ./bcp uuid ../boost_out
   ```
  * cryptopp
  * curl
  * hiredis
  * mysql connect/c
  * protobuf 3.13.0

# 目录
  * ngl/3part
  ```
    此目录用于存放第三方库
  ```
  * ngl/tools/public/idl
  ```
  csv表生成       		
  ```
  * ngl/tools/public/proto
  ```
  proto协议生成
  ```
  * ngl/tools/public/rebuild.bat
  ```  
   与cmake unity build类似的加速编译
   ```
  * ngl/public/cpp
  ```            		  
  cpp源码
  ```
  * ngl/public/csharp   
  ```            
  cs源码
  ```
  * ngl/server     
  ```     		        
  项目源文件
  ```

# Linux CentOs编译
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
