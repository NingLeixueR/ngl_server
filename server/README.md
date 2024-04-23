# 目录简介

## ./idl
	* csv表格工具项目，用来根据.idl配置生成对应的表格文件，与加载表格的源文件

## ./node
	* 服务器进程

## ./makeproto
	* 根据db.proto文件生成[create_db.sql]文件
	* 根据net.proto文件生成[net_protocol.xml与netserver_protocol.xml]文件,自动关联结构体为其提供协议号

## ./rebuild
  * 遍历public/cpp文件夹生成[ub.cpp],用于加速编译
