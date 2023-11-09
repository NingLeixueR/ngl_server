# ngl
Actor based c++distributed game server framework
基于Actor的c++分布式游戏服务器框架
# 目录
  * /3part                    		此目录用于存放第三方库(bcp裁剪的boost、cryptopp、libcurl、hiredis、protobuf、mysql)
  * /idl                      		csv表的生成代码生成工具(对应目录/tools/public/idl/csvtable.idl)
  * /public                   		库的源代码目录
  * /public/actor             		actor模式的实现
  * /public/logic             		简单的使用例子
  * /public/net               		网络相关(只支持tcp和ws)
  * /public/protocol          		生成的协议相关的文件
  * /public/serialize         		主要是tcp分包，包头，序列化
  * /public/tools             		一些简单封装的工具(curl,dbmysql,log,xml,csv,json,md5......)
  * /server                   		cmake项目文件,一些main.cpp等
  * /tools                    		此目录中有一些脚本文件 如rename_node.bat将生成的node.exe文件分别重命名,以方便在windwos任务管理器中查看.
  * /tools/public/idl         		idl文件目录
  * /tools/public/protocolbuff    	proto文件目录,用于定义与客户端交互的协议

# 杂谈
  * 源代码中.h暴露的接口中大量使用了impl用来隐藏和尽量避免成员变量的暴露,让使用.h的人只关心暴露出来的接口
  * 不要尝试编译libcore,libcore仅仅是为了在写代码时对单个cpp文件进行编译用来测试代码是否存在问题
  * libcorekkk是include 所有.cpp文件以加快编译速度
 
# 
  


