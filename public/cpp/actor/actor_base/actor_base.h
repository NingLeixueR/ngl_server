/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "ttab_servers.h"
#include "handle_pram.h"
#include "nactortype.h"
#include "localtime.h"
#include "nscript.h"
#include "ntimer.h"
#include "ngroup.h"
#include "nguid.h"
#include "type.h"
#include "ukcp.h"

#include <memory>
#include <vector>
#include <atomic>
#include <list>
#include <set>
#include <map>

namespace ngl
{
	class actor_base;
	class ndb_component;
	class ndbclient_base;
	class actor_manage_dbclient;

	template <typename TDBTAB>
	struct data_modified;

	struct actorparmbase
	{
		ENUM_ACTOR		m_type				= nguid::none_type();				// actor类型
		i16_area		m_area				= tab_self_area;					// 区服
		i32_actordataid m_id				= nguid::none_actordataid();		// 数据id
		bool			m_manage_dbclient	= false;							// 是否有数据库依赖
		enscript		m_enscript			= enscript_none;					// 脚本支持
	};

	//# actor的状态
	enum actor_stat
	{
		actor_stat_init,  // 初始化阶段
		actor_stat_free,  // 状态为空闲
		actor_stat_list,  // 繁忙-在等待线程的actor队列
		actor_stat_run,	  // 繁忙-在线程中执行任务
		actor_stat_close, // 关闭状态
	};

	//# 消息的简单封装
	template <typename T>
	struct message;

	class nready
	{
	public:
		enum enum_ready
		{
			e_ready_all		= 0xFFFFFFFF,			// 检查所有数据是否加载完成
			e_ready_null	= 0x00000000,			// 不需要检测是否加载完成
			e_ready_db		= 0x00000001,			// 数据库数据加载情况
			e_ready_nsp		= 0x00000002,			// nsp数据同步情况
			e_ready_custom	= 0x00008000,			// 自定义ready方法
		};
	private:
		std::map<int32_t, std::function<bool()>> m_readyfun;
	public:
		//# 是否就绪
		bool is_ready(int32_t aready = e_ready_all);

		//# 设置就绪函数
		void set_ready(int32_t aready, const std::function<bool()>& afun);

		//# 设置自定义就绪函数
		void set_readybycustom(int anumber, const std::function<bool()>& afun);
	};

	class actor_base
	{
		actor_base() = delete;
		actor_base(const actor_base&) = delete;
		actor_base& operator=(const actor_base&) = delete;
	private:
		nguid										m_guid = nguid::make();			// actor guid
		std::unique_ptr<actor_manage_dbclient>		m_dbclient = nullptr;			// dbclient组件管理
		bool										m_isload = false;				// 数据是否加载完成
		std::map<pbdb::ENUM_DB, ndb_component*>		m_dbcomponent;					// dbclient组件

		//# 间隔一段时间发起的全员(所有actor)广播
		//# 可以在这个广播里推送一些需要处理的任务,例如 保存数据
		//# 推送全员广播的 单位(毫秒)
		static int									m_broadcast;
		//# 推送广播的定时器id
		static int									m_broadcasttimer;
		//# 是否接收广播消息
		bool										m_isbroadcast = false;
		ngroup										m_group;
		nready										m_ready;
	public:
		explicit actor_base(const actorparmbase& aparm);

		nready&			ready();

#pragma region db
		//# 获取actor_manage_dbclient实例
		using ptr_manage_dbc = std::unique_ptr<actor_manage_dbclient>;
		ptr_manage_dbc& get_actor_manage_dbclient();

		//# 设置db_component组件
		void			set_db_component(ndb_component* acomponent);

		//# 初始化数捿在数据加载完成后)
		void			db_component_init_data();

		//# 初始化db_component
		void			init_db_component(bool acreate);

		//# 添加dbclient
		void			add_dbclient(ndbclient_base* adbclient, i64_actorid aid);

		//# 向actor_db发送数据请求后的返回
		//# DBTYPE 数据类型
		//# TDBTAB 数据结构
		//# TACTOR 持有该数据表的actor
		template <pbdb::ENUM_DB DBTYPE, typename TDBTAB, typename TACTOR>
		bool handle(const message<np_actordb_load_response<DBTYPE, TDBTAB>>& adata);
#pragma endregion 

#pragma region virtual_function
		virtual ~actor_base();

		//# 初始化
		//# 一般actor对象会在其重载虚函数
		//# 让dbclient与actor对象进行绑定
		virtual void init() {}

		//# 获取actor状态
		virtual actor_stat get_activity_stat() = 0;

		//# 设置actor状态
		virtual void set_activity_stat(actor_stat astat) = 0;

		//# 检查任务列表是否为空
		virtual bool list_empty() = 0;

		//# 进行任务
		virtual void actor_handle(i32_threadid athreadid) = 0;

		//# 添加任务
		virtual void push(handle_pram& apram) = 0;

		//# 执行handle之后调用
		virtual void handle_after(handle_pram&) {}

		//# 派生actor重载此函数会在数据加载完成后调用
		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat) {}

		//# 删除actor时候会被调用
		virtual void release() = 0;

		//# 移除actor前一刻调用
		virtual void erase_actor_before() {}
#pragma endregion 

		//# 保存dbclient
		virtual void save();

		//# 是否为单例
		bool is_single();

		//# 获取actor guid
		nguid& guid();

		//# 获取actor guid i64_actorid
		i64_actorid id_guid();

		//# 获取actor guid的数据id
		i32_actordataid id();

		//# 获取actor guid的区服id
		i16_area area();

		//# 获取actor guid的actor type
		ENUM_ACTOR type();

		//# 移除actor自身
		virtual void erase_actor();

		//# 移除指定actor
		static void erase_actor(const nguid& aguid);

		//# 给actor自身添加任务
		void push_task_id(handle_pram& apram);

		//# 向指定actor添加任务
		static void push_task_id(const nguid& aguid, handle_pram& apram);
		static void push_task_id(const std::set<i64_actorid>& asetguid, handle_pram& apram);

		//# 给指定类型的actor添加任务
		static void push_task_type(ENUM_ACTOR atype, handle_pram& apram);

#pragma region nscript
	private:
		//# 对脚本语言的支持
		void* m_script = nullptr;
		enscript m_enscript = enscript_none;					// 脚本支持
	public:
		//# actor是否使用脚本
		bool nscript_using();

		//# 通知脚本db数据加载完毕
		bool nscript_db_loadfinish();

		template <typename T>
		struct nscript_data_csv
		{
			using TDATA = T;
			const std::map<int32_t, T>& data;

			explicit nscript_data_csv(const std::map<int32_t, T>& adata) :
				data(adata)
			{}

			dprotocol(nscript_data_csv<T>, data)
		};

		template <typename T>
		struct nscript_data_db
		{
			using TDATA = T;
			std::map<int64_t, T*> data;

			dprotocol(nscript_data_db<T>, data)
		};

		template <typename T>
		struct nscript_data_nsp
		{
			using TDATA = T;
			const std::map<int64_t, T>& data;
			explicit nscript_data_nsp(const std::map<int64_t, T>& adata) :
				data(adata)
			{}

			dprotocol(nscript_data_nsp<T>, data)
		};

		//# 向脚本压入数捿1、csv数据 2、db数据 3、nsp数据)
		// parm aname			数据名称
		// parm asource			数据来源(csv,db,nsp)
		// parm adata			压入的数据
		// parm aedit			是否可以在脚本中修改
		template <typename T>
		bool nscript_data_push(const char* asource, const T& adata, bool aedit/* = false*/)
		{
			if (!nscript_using())
			{
				return false;
			}
			return nscript_manage::data_push(
				m_enscript, m_script, tools::type_name<typename T::TDATA>().c_str(), asource, adata, aedit
			);
		}

		//# 告诉脚本数据被删除了
		// parm aname			数据名称
		// parm adataid			数据id
		template <typename T>
		bool nscript_data_del(int64_t adataid)
		{
			if (!nscript_using())
			{
				return false;
			}
			return nscript_manage::data_del(m_enscript, m_script, tools::type_name<T>().c_str(), adataid);
		}

		//# 检查数据是否被修改
		template <typename T>
		bool nscript_data_checkout(int64_t adataid, T& adata)
		{
			if (!nscript_using())
			{
				return false;
			}
			return nscript_manage::data_checkout(m_enscript, m_script, tools::type_name<T>().c_str(), adataid, adata);
		}

		template <typename T>
		bool nscript_data_checkout(std::map<int64_t, T>& adata)
		{
			if (!nscript_using())
			{
				return false;
			}
			return nscript_manage::data_checkout(m_enscript, m_script, tools::type_name<T>().c_str(), adata);
		}

		template <typename T>
		bool nscript_data_checkdel(int64_t adataid)
		{
			if (!nscript_using())
			{
				return false;
			}
			return nscript_manage::data_checkdel(m_enscript, m_script, tools::type_name<T>().c_str(), adataid);
		}

		template <typename T>
		bool nscript_data_checkdel(std::vector<int64_t>& adeldata)
		{
			if (!nscript_using())
			{
				return false;
			}
			return nscript_manage::data_checkdel(m_enscript, m_script, tools::type_name<T>().c_str(), adeldata);
		}

		template <typename T>
		bool nscript_handle(const T& adata)
		{
			if (!nscript_using())
			{
				return false;
			}
			return nscript_manage::handle(m_enscript, m_script, tools::type_name<T>().c_str(), adata);
		}
#pragma endregion 

#pragma region net
		//# 生成包
		template <typename T>
		static std::shared_ptr<pack> net_pack(T& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		//# 发送数据到指定服务器
		template <typename T>
		static bool send_server(i32_serverid aserverid, T& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		//# 向一组服务器发送数据
		template <typename T>
		static bool send_server(const std::set<i32_serverid>& aserverids, T& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		//# 发送pack到指定服务器
		template <typename T>
		static bool sendpack_server(i32_serverid aserverid, std::shared_ptr<pack>& apack);

		//# 通过proto结构名称与json消息体构造包
		static std::shared_ptr<pack> jsonpack(const std::string& apbname, const std::string& ajson, i64_actorid aactorid, i64_actorid arequestactorid);

		//# 给指定连接发送数据
		template <typename T>
		static bool sendpack_session(i32_sessionid asession, std::shared_ptr<pack>& apack);

		//# 给指定连接发送数据
		template <typename T>
		static bool send(i32_sessionid asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid);
#pragma endregion 

#pragma region kcp
	private:
		struct kcpport
		{
			std::map<pbnet::ENUM_KCP, int16_t> m_data;
		};
		std::map<i32_serverid, kcpport> m_kcpindex;
	public:
		void set_kcpindex(i32_serverid aserverid, pbnet::ENUM_KCP aenum, int16_t akcpindex)
		{
			m_kcpindex[aserverid].m_data[aenum] = akcpindex;
		}

		int16_t kcpindex(i32_serverid aserverid, pbnet::ENUM_KCP aenum)
		{
			auto itor = m_kcpindex.find(aserverid);
			if (itor == m_kcpindex.end())
			{
				return -1;
			}
			auto itor2 = itor->second.m_data.find(aenum);
			if (itor2 == itor->second.m_data.end())
			{
				return -1;
			}
			return itor2->second;
		}

		int16_t kcpindex(int16_t aservertid, int16_t atcount, pbnet::ENUM_KCP aenum)
		{
			int32_t lserverid = nnodeid::nodeid(aservertid, atcount);
			return kcpindex(lserverid, aenum);
		}

	public:
		//# 通过udp.kcp发送数据
		template <typename T>
		static bool send_kcp(i64_actorid aactorid, T& adata, int16_t auport = 0);

		template <typename T>
		static bool send_kcp(const std::set<i64_actorid>& aactorids, T& adata, int16_t auport = 0);

		template <typename T>
		static bool send_kcp(const pack* apack, T& adata);

		//# 发起kcp连接
		bool connect_kcp(int16_t anum, const std::string& aip, i16_port aprot, i64_actorid aactoridserver, std::string& akcpsession);
#pragma endregion 

#pragma region send_client
		static i64_actorid actorclient_guid();

		//# 根据actor_role.guidid给所在客户端发送数据
		template <typename T>
		static void send_client(i64_actorid aid, const T& adata, ENET_PROTOCOL aprotocol = ENET_TCP)
		{
			std::set<i64_actorid> lids = { aid };
			send_client(lids, adata, aprotocol);
		}

		template <typename T>
		static void send_client(const std::set<i64_actorid>& aids, const T& adata, ENET_PROTOCOL aprotocol = ENET_TCP)
		{
			auto pro = std::make_shared<np_actor_forward<T, forward_g2c<T>>>();
			pro->m_data.m_protocol = aprotocol;
			pro->m_data.m_data = adata;
			for (i64_actorid aactorid : aids)
			{
				nguid lguid(aactorid);
				pro->m_data.m_uid.push_back(lguid.actordataid());
				pro->m_data.m_area.push_back(lguid.area());
			}
			handle_pram lpram = handle_pram::create<T>(nguid::make(), nguid::make(), pro);
			push_task_id(actorclient_guid(), lpram);
		}

		template <typename T>
		static void send_client(const std::vector<i64_actorid>& aids, const T& adata, ENET_PROTOCOL aprotocol = ENET_TCP)
		{
			std::set<i64_actorid> lids(aids.cbegin(), aids.cend());
			send_client(lids, adata, aprotocol);
		}

		//# 向所有客户端发送消息
		template <typename T>
		static void send_client(const T& adata, ENET_PROTOCOL aprotocol = ENET_TCP)
		{
			std::set<i32_serverid>& lgatewayids = sysconfig::gatewayids();
			if (lgatewayids.empty())
			{
				return;
			}
			auto pro = std::make_shared<np_actor_forward<T, forward_g2c<T>>>();
			pro->m_data.m_protocol = aprotocol;
			pro->m_data.m_data = adata;
			nguid lguid(nguid::make());
			pro->m_data.m_uid.push_back(lguid.actordataid());
			pro->m_data.m_area.push_back(lguid.area());
			send_server(lgatewayids, *pro, nguid::make(), nguid::make());
		}

		//# 往指定区服所有客户端发送消息
		template <typename T>
		static void send_clientbyarea(i16_area aarea, const T& adata, ENET_PROTOCOL aprotocol = ENET_TCP)
		{
			auto pro = std::make_shared<np_actor_forward<T, forward_g2c<T>>>();
			pro->m_data.m_protocol = aprotocol;
			pro->m_data.m_data = adata;
			ttab_servers::instance().foreach_server(GATEWAY, aarea, [&pro](const tab_servers* atab)
				{
					send_server(atab->m_id, *pro, nguid::make(), nguid::make());
				});
		}
#pragma endregion

#pragma region send_actor
		//# 向指定actor发送pack
		static void send_actor_pack(const nguid& aguid, const std::shared_ptr<pack>& adata)
		{
			handle_pram lpram = handle_pram::create_pack(aguid, nguid::make(), adata);
			push_task_id(aguid, lpram);
		}

		template <typename T, bool IS_SEND = true>
		static void send_actor(ENUM_ACTOR atype, const std::shared_ptr<T>& adata)
		{
			handle_pram lpram = handle_pram::create<T, IS_SEND>(nguid::make_self(atype), nguid::make(), adata);
			lpram.m_forwardtype = true;
			push_task_type(atype, lpram);
		}

		//# 发送数据到指定的actor
		template <typename T, bool IS_SEND = true>
		static void send_actor(const nguid& aguid, const nguid& arequestguid, const std::shared_ptr<T>& adata)
		{
			handle_pram lpram = handle_pram::create<T, IS_SEND>(aguid, arequestguid, adata);
			push_task_id(aguid, lpram);
		}

		//# 发送数据到指定的actor
		template <typename T, bool IS_SEND = true>
		static void send_actor(
			const nguid& aguid, const nguid& arequestguid, const std::shared_ptr<T>& adata, const std::function<void()>& afailfun
		)
		{
			handle_pram lpram = handle_pram::create<T, IS_SEND>(aguid, arequestguid, adata, afailfun);
			push_task_id(aguid, lpram);
		}

		template <typename T, bool IS_SEND = true>
		static void send_actor(const std::set<i64_actorid>& asetguid, const nguid& arequestguid, const std::shared_ptr<T>& adata)
		{
			if (!asetguid.empty())
			{
				handle_pram lpram = handle_pram::create<T, IS_SEND>(asetguid, arequestguid, adata);
				push_task_id(asetguid, lpram);
			}
		}
#pragma endregion

		//# 方便调试打印协议
		template <typename T>
		void handle_print(const message<T>& adata)
		{
			if constexpr (is_protobuf_message<T>::value)
			{
				tools::print_json(*adata.get_data(), true);
			}
			else
			{
				njson_write ljson;
				njson::write(ljson, "message", *adata.get_data());
				std::string lstr = ljson.get();
				log_error()->print("{}", lstr);
			}
		}
#pragma region group
		//# 创建一个群发分绿可以指定ActorType,主要是为了区分客户端与普通actor)
		int32_t create_group(ENUM_ACTOR atype = ACTOR_NONE);
		
		//# 移除一个分绿
		void remove_group(int32_t agroupid);
		
		//# 将成员加入某个群发分绿
		bool add_group_member(int32_t agroupid, i64_actorid amember);
		
		//# 将成员从某个群发分组中移除
		void remove_group_member(int32_t agroupid, i64_actorid amember);
		
		//# 获取group id中的actor列表
		const std::set<i64_actorid>* get_group(int32_t agroupid);

		//# 给一组成员发送消息
		template <typename T>
		bool send_group(int agroupid, std::shared_ptr<T>& adata)
		{
			std::set<i64_actorid>* lset = get_group(agroupid);
			if(lset == nullptr)
			{
				return false;
			}
			std::set<i64_actorid> lclients;
			std::set<i64_actorid> lactors;
			for (i64_actorid aactor : *lset)
			{
				if ((ENUM_ACTOR)nguid::type(aactor) != ACTOR_ROBOT)
				{
					lactors.insert(aactor);
				}
				else
				{
					lclients.insert(aactor);
				}
			}
			if (!lclients.empty())
			{
				send_client(lclients, adata, this);
			}
			if (!lactors.empty())
			{
				send_actor(lactors, adata);
			}
			return true;
		}
#pragma endregion 
		
#pragma region broadcast
		//# 设置定时任务参数
		int32_t set_timer(const np_timerparm& aparm);

		//# 是否支持广播
		bool isbroadcast();

		//# 设置是否支持广播
		void set_broadcast(bool aisbroadcast);

		//# 启动广播定时器
		static void start_broadcast();
#pragma endregion 

		//# actor_base::create 
		//# 构造actor对象会自动被调用
		template <typename TDerived>
		static void first_nregister([[maybe_unused]] ENUM_ACTOR atype)
		{
			static std::atomic lfirst = true;
			if (lfirst.exchange(false))
			{
				TDerived::nregister();
			}
		}

		//# 用于创建非单例actor
		static std::shared_ptr<actor_base> create(ENUM_ACTOR atype, i16_area aarea, i32_actordataid aid, void* aparm = nullptr);
	};

	template <typename T>
	class actor_instance
	{
	public:
		static T& instance();
	};
}//namespace ngl

namespace ngl
{
	template <typename T>
	void tprotocol::tforward::func(int32_t aprotocolnum)
	{
		info* lptemp = tcustoms<true>::func<T>(aprotocolnum);
		if (lptemp != nullptr)
		{
			lptemp->m_toclient[enscript_lua] = [](int64_t aactorid, void* aL)->bool
				{
					auto L = (lua_State*)(aL);
					typename T::BASE_TYPE pro;
					if (!ngl::nlua_stack::stack_pop(L, pro))
					{
						return false;
					}
					actor_base::send_client(aactorid, pro, this);
					return true;
				};
			lptemp->m_toactor[enscript_lua] = [](int64_t aactorid, void* aL)->bool
				{
					auto L = (lua_State*)(aL);
					typename T::BASE_TYPE pro;
					if (!ngl::nlua_stack::stack_pop(L, pro))
					{
						return false;
					}
					actor_base::send_actor(aactorid, nguid::make(), pro);
					return true;
				};
		}
	}

	template <enscript SCRIPT>
	struct tcustoms_send
	{
		template <typename TX>
		static bool send_actor([[maybe_unused]] int64_t aactorid, [[maybe_unused]] void* aL)
		{
			return true;
		}
	};

	template <>
	struct tcustoms_send<enscript_lua>
	{
		template <typename TX>
		static bool send_actor(int64_t aactorid, void* aL)
		{
			auto L = (lua_State*)(aL);
			auto pro = std::make_shared<TX>();
			if (!ngl::nlua_stack::stack_pop(L, *pro))
			{
				return false;
			}
			actor_base::send_actor(aactorid, nguid::make(), pro);
			return true;
		}
	};

	template <bool SCRIPT>
	template <typename T>
	tprotocol::info* tprotocol::tcustoms<SCRIPT>::func(int32_t aprotocolnum /*= -1*/, int8_t ahigh /*= 0*/)
	{
		info* linfo = funcx<T>(aprotocolnum, ahigh);
		if (linfo != nullptr)
		{
			if constexpr (SCRIPT)
			{
				linfo->m_toactor[enscript_lua] = std::bind_front(tcustoms_send<enscript_lua>::send_actor<T>);
			}
		}
		return linfo;
	}
}//namespace ngl