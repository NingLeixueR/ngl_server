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

# 编译
  * 首先需要下载新版本的boost,boost编译并剪裁后将其放置在目录/3part/boost/下
  * 运行cmake 源路径选择主目录/ngl 生成目录/ngl/obj
  * 会生成sln项目,打开,首先项目中的libmycore是不需要编译的别浪费时间(libmycorekkk是他的替代主要是include所有cpp文件以加速编译)
  * makeproto工程主要是根据proto文件生成对应的sql文件和 协议结构与协议号关联的xml(我们就不需要大费心神关联协议号与协议体了)
  * rebuild工程主要是遍历/public下的所有cpp文件,生成ub.cpp让libmycorekkk加速编译
  * node是节点文件,包含逻辑的全部代码,在其中我们可以修改实例化哪些actor将一些逻辑进行分离和集中,我们游戏可能稳定运行了一段时间,要更新“新玩法”我们可以把新功能和玩法进行抽离，放到线上等起稳定了再将其集中,当然我们也能因为某个节点压力太大可以运用分离,将压力分散

# 使用
  * 如何定义一个actor?
	```
	定义一个actor_xxx 需要继承 actor
	在构造函数中我们需要简单配置一下actor,比如actor的类型ENUM_ACTOR,区服id,dataid,是否需要从数据库中加载数据,消息处理权重(数字,每一次轮训最多处理多少消息)
	virtual void init()中需要actor_xxx与dbclient相关联
	static void actor_register(); 需要注册actor_xxx要处理的消息
	例如 ：
	register_actor<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_xxx>(false,
				null<pbnet::PROBUFF_NET_ROLE_LOGIN>
			);
	在actor_xxx中注册了proto消息 pbnet::PROBUFF_NET_ROLE_LOGIN需要处理 
	此时我们需要实现对应的消息  bool actor_xxx::handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_ROLE_LOGIN& adata)
	参数i32_threadid athread是线程id  一般用不到
	参数const std::shared_ptr<pack>& apack 是经过网络的原生包 一般用不到
	参数pbnet::PROBUFF_NET_ROLE_LOGIN& adata 是我们需要处理的消息

	我们也可以实现bool timer_handle(i32_threadid athread, const std::shared_ptr<pack>& apack, timerparm& adata);方法 用来处理定时任务
	这时我们同样需要在static void actor_register();中注册 调用actor_xxx::register_timer<actor_role>();即可

	
	[可以查看actor_role](https://https://github.com/NingLeixueR/ngl/tree/main/public/actor/actor_logic/game/actor_role.h)

	```


# 杂谈
  * 源代码中.h暴露的接口中大量使用了impl用来隐藏和尽量避免成员变量的暴露,让使用.h的人只关心暴露出来的接口
  * 不要尝试编译libcore,libcore仅仅是为了在写代码时对单个cpp文件进行编译用来测试代码是否存在问题
  * libcorekkk是include 所有.cpp文件以加快编译速度
 
# 
  


