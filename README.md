[![Build C++](https://github.com/NingLeixueR/ngl/actions/workflows/cmake-unbutu.yml/badge.svg)](https://github.com/NingLeixueR/ngl/actions/workflows/cmake-unbutu.yml)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=NingLeixueR_ngl&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=NingLeixueR_ngl)
## ngl
* 基于Actor的c++游戏服务器框架

## 暂时正在对脚本语言进行支持,涉及脚本语言lua的库编译还没增加到actions中，可以回退到对脚本语言支持的版本

## 依赖的第三方库
  * <b>非boost独立版本的asio</b>
  * <b>curl</b>
  * <b>hiredis</b>
  * <b>mysql connect/c</b>
  * <b>protobuf 3.13.0</b>
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

## 增加对脚本语言的支持
  * 需要考虑的问题: 存库数据 c++与脚本语言的交互
  * 方案1:在actor中强制用户二选一，是使用脚本构造还是c++,这样数据只需要保存一份，但是这样麻烦的是每增加一种脚本语言的支持就需要完善全部存库操作
  * 方案2:actor的构造还是在c++中进行，存库数据会自动通过json传递给脚本语言，这样可以逻辑放在脚本语言，使用延迟加载的方式，封装get方法与getconst方法，当使用get自动设置标志位，当对端语言要使用前先检查
  标志位大概如下：
       ```
	   T get()
	   {
			if(检查标志位)
			{
				发生变化，从对端语言加载数据				
			}
			if(cpp)
			{//这步设置主要是因为定时保存数据是发生在cpp中
				cpp中设置发生变化
			}
			设置本语言对数据进行了修改
	   }
	   T getconst()
	   {
			if(检查标志位)
			{
				发生变化，从对端语言加载数据	
				if(cpp)
				{//这步设置主要是因为定时保存数据是发生在cpp中
					cpp中设置发生变化
				}				
			}
	   }
	   ```
	   



## 例子游戏
  * [猜数字](https://github.com/NingLeixueR/ngl_server/wiki/%E4%BE%8B%E5%AD%90%E6%B8%B8%E6%88%8F%E2%80%90%E7%8C%9C%E6%95%B0%E5%AD%97)

## 更多介绍
  * [wiki](https://github.com/NingLeixueR/ngl_server/wiki)
  * [deepwiki](https://deepwiki.com/NingLeixueR/ngl_server)

## QQ群
  * 56257994