# ngl
Actor based c++distributed game server framework
基于Actor的c++分布式游戏服务器框架
# 目录
  * ngl/3part                    		此目录用于存放第三方库(bcp裁剪的boost、cryptopp、libcurl、hiredis、protobuf、mysql)
  * ngl/idl                      		csv表的生成代码生成工具(对应目录/tools/public/idl/csvtable.idl)
  * ngl/public                   		库的源代码目录
  * ngl/public/actor             		actor模式的实现
  * ngl/public/logic             		简单的使用例子
  * ngl/public/net               		网络相关(只支持tcp和ws)
  * ngl/public/protocol          		生成的协议相关的文件
  * ngl/public/tools/serialize         	主要是tcp分包，包头，序列化
  * ngl/public/tools             		一些简单封装的工具(curl,dbmysql,log,xml,csv,json,md5......)
  * ngl/server                   		cmake项目文件,一些main.cpp等
  * ngl/tools                    		此目录中有一些脚本文件 如rename_node.bat将生成的node.exe文件分别重命名,以方便在windwos任务管理器中查看.
  * ngl/tools/public/idl         		idl文件目录
  * ngl/tools/public/protocolbuff    	proto文件目录,用于定义与客户端交互的协议

# 编译
  * 首先需要下载新版本的boost,boost编译并剪裁后将其放置在目录/3part/boost/下
  * 运行cmake 源路径选择主目录/ngl 生成目录/ngl/obj
  * 会生成sln项目,打开,首先项目中的libmycore是不需要编译的别浪费时间(libmycorekkk是他的替代主要是include所有cpp文件以加速编译)
  * makeproto工程主要是根据proto文件生成对应的sql文件和 协议结构与协议号关联的xml(我们就不需要大费心神关联协议号与协议体了)
  * rebuild工程主要是遍历/public下的所有cpp文件,生成ub.cpp让libmycorekkk加速编译
  * node是节点文件,包含逻辑的全部代码,在其中我们可以修改实例化哪些actor将一些逻辑进行分离和集中,我们游戏可能稳定运行了一段时间,要更新“新玩法”我们可以把新功能和玩法进行抽离，放到线上等起稳定了再将其集中,当然我们也能因为某个节点压力太大可以运用分离,将压力分散
  * 在linux编译运行
  ** [root@localhost bin]# cat /etc/centos-release
  ** CentOS Linux release 7.9.2009 (Core)
  ** [root@localhost bin]# gcc --version
  ** gcc (GCC) 13.1.0
  * 在win编译运行
  ** vs2022 
# 使用
  * 如何定义一个actor?
	```cpp
	定义一个actor_xxx 需要继承 actor
	在构造函数中我们需要简单配置一下actor,比如actor的类型ENUM_ACTOR,区服id,dataid,是否需要从数据库中加载数据,消息处理权重(数字,每一次轮训最多处理多少消息)
	virtual void init()中需要actor_xxx与dbclient相关联
	static void actor_register(); 需要注册actor_xxx要处理的消息
	例如 ：
	register_actor<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_xxx>(false,
				dregister_fun_handle(actor_xxx, pbnet::PROBUFF_NET_ROLE_LOGIN>)
			);
	在actor_xxx中注册了proto消息 pbnet::PROBUFF_NET_ROLE_LOGIN需要处理 
	此时我们需要实现对应的消息  bool actor_xxx::handle(message<pbnet::PROBUFF_NET_ROLE_LOGIN>& adata)
	message.m_data	是我们需要处理的消息pbnet::PROBUFF_NET_ROLE_LOGIN
	adata.m_thread	是线程id  一般用不到
	adata.m_pack	是经过网络的原生包 一般用不到

	我们也可以实现bool timer_handle(message<timerparm>& adata);方法 用来处理定时任务
	这时我们同样需要在static void actor_register();中注册 调用actor_xxx::register_timer<actor_role>(&timer_handle);即可

	```
  * [非单例actor:actor_role](https://github.com/NingLeixueR/ngl/blob/main/public/actor/actor_logic/game/actor_role.h)
  *	[单例actor:actor_manage_role](https://github.com/NingLeixueR/ngl/blob/main/public/actor/actor_logic/game/actor_manage_role.h)

  * 数据的存储与加载
	```cpp
	ngl/tools/public/proto/db.proto中定义要存储的db数据
	数据统一以db_开头 例如：
	定义一个枚举
	enum ENUM_DB
	{
		ENUM_DB_ACCOUNT = 0;		// 账号数据
		ENUM_DB_ROLE = 1;			// 玩家数据
		ENUM_DB_BRIEF = 2;			// 玩家简要数据
		ENUM_DB_BAG = 3;			// 背包数据
		ENUM_DB_KEYVALUE = 4;		// 键值数据
		ENUM_DB_MAIL = 5;			// 邮件数据
		ENUM_DB_GUILD = 6;			// 工会数据
		ENUM_DB_NOTICE = 7;			// 公告数据
		ENUM_DB_RANKLIST = 8;		// 排行数据
		ENUM_DB_ACTIVITY = 9;		// 活动数据
		ENUM_DB_COUNT = 10;
	};
	// ENUM_DB_ACCOUNT,				// 账号数据
	message db_account
	{
		optional int64	m_id			= 1;				// 帐号id
		optional int64	m_roleid		= 2;				// 角色id
		optional string m_account		= 3;				// 账号
		optional string m_passworld		= 4;				// 密码
		optional int32	m_area			= 5;				// 区服id
	}

	这样我们就可以在node的db中增加数据加载(server_main.h)
	using actor_db_account = ngl::actor_db<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_ACCOUNT, pbdb::db_account>;
	actor_db_account::getInstance();

	我们还必须在tab_dbload.csv中指定db_account表的基本信息
	int32_t		string		string		bool						bool						int
	m_id		m_name		m_remarks	m_isloadall					m_network					m_sendmaxcount
	唯一id		名称		注释		是否需要启动加载全部数据	是否允许通过网络分发		单次最多发送数量
	version:0					
	1			DB_ACCOUNT	DB_ACCOUNT	1							1							100

	好了就差最后一步 定义加载后数据访问的途径
	using account_db_modular = db_modular<
		EPROTOCOL_TYPE_PROTOCOLBUFF,
		pbdb::ENUM_DB_ACCOUNT,
		pbdb::db_account,
		actor_login
	>;
	class account : public account_db_modular
	需要重载以下函数
	看你是加载一个实例还是加载全部实例 -1加载全部
	virtual void set_id()
	加载成功后会调用此函数  我们可以在其中对数据进行预处理
	virtual void initdata() 

	```
    * actor间消息交互
	```cpp
	1、自定义消息  例如 
	// ---- 通知服务器玩家账号验证通过
	struct actor_role_login
	{
		std::string m_session;
		i64_accountid m_accountid = 0;
		i64_actorid m_roleid = 0;
		i32_serverid m_gameid = 0;
		i32_serverid m_gatewayid = 0;
		i16_area m_area = 0;
		bool m_iscreate = false;
		def_portocol(actor_role_login, m_session, m_accountid, m_roleid, m_gameid, m_gatewayid, m_area, m_iscreate)
	};
	只需要定义一个消息体  然后def_portocol将成员变量依次填入，这样就actor就可以进行消息交互了 
	不过自定义协议需要绑定自增协议号，在init_protobuf.cpp中定义
	init_customs(EPROTOCOL_TYPE_CUSTOM
			, null<actor_role_login>
		);
	自定义消息可以使用模板，例如模块间转发消息
	template <typename T>
	struct actor_module_forward
	大部分自定义消息都定义在actor_protocol.h中，当然也可以随意定义在任何位置，但记得绑定自增协议号噢。

	2、proto消息交互 
	定义在net.proto文件中，不需要绑定自增协议号，makeproto工具会生成proto对应的xml（如net_protocol.xml）
	net_protocol.xml需要交给客户端，客户端需要根据协议名称自行进行协议号与消息体绑定
	```

# 杂谈
  * 源代码中.h暴露的接口中大量使用了impl用来隐藏和尽量避免成员变量的暴露,让使用.h的人只关心暴露出来的接口
  * 不要尝试编译libcore,libcore仅仅是为了在写代码时对单个cpp文件进行编译用来测试代码是否存在问题
  * libcorekkk是include 所有.cpp文件以加快编译速度
  * 近半年的修改未在linux上编译,一方面是不太方便，另一方面是懒,因为代码使用了部分c++20的内容所以对编译器版本有要求。 
  * QQ群 56257994 欢迎志同道合的小伙伴一起维护完善这个小项目 
# 
  


