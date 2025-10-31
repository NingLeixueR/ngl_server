/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
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
		ENUM_ACTOR		m_type				= nguid::none_type();				// actor����
		i16_area		m_area				= tab_self_area;					// ����
		i32_actordataid m_id				= nguid::none_actordataid();		// ����id
		bool			m_manage_dbclient	= false;							// �Ƿ������ݿ�����
		enscript		m_enscript			= enscript_none;					// �ű�֧��
	};

	// # actor��״̬
	enum actor_stat
	{
		actor_stat_init,  // ��ʼ���׶�
		actor_stat_free,  // ״̬Ϊ����
		actor_stat_list,  // ��æ-�ڵȴ��̵߳�actor����
		actor_stat_run,	  // ��æ-���߳���ִ������
		actor_stat_close, // �ر�״̬
	};

	// # ��Ϣ�ļ򵥷�װ
	template <typename T>
	struct message;

	class actor_base
	{
		actor_base() = delete;
		actor_base(const actor_base&) = delete;
		actor_base& operator=(const actor_base&) = delete;
	private:
		nguid										m_guid = nguid::make();			// actor guid
		std::unique_ptr<actor_manage_dbclient>		m_dbclient = nullptr;			// dbclient�������
		bool										m_isload = false;				// �����Ƿ�������
		std::map<pbdb::ENUM_DB, ndb_component*>		m_dbcomponent;					// dbclient���
		i32_session									m_kcpsession = -1;				// kcp session

		//# ���һ��ʱ�䷢���ȫԱ(����actor)�㲥
		//# ����������㲥������һЩ��Ҫ���������,���� ��������
		//# ����ȫԱ�㲥�� ��λ(����)
		static int									m_broadcast;
		//# ���͹㲥�Ķ�ʱ��id
		static int									m_broadcasttimer;
		//# �Ƿ���չ㲥��Ϣ
		bool										m_isbroadcast = false;
		ngroup										m_group;
	public:
		explicit actor_base(const actorparmbase& aparm);

#pragma region db
		//# ��ȡactor_manage_dbclientʵ��
		using ptr_manage_dbc = std::unique_ptr<actor_manage_dbclient>;
		ptr_manage_dbc& get_actor_manage_dbclient();

		//# �Ƿ���Ҫ�����ݿ��������
		bool			isload();

		//# �Ƿ�������
		bool			isloadfinish();

		//# ����db_component���
		void			set_db_component(ndb_component* acomponent);

		//# ��ʼ�����������ݼ�����ɺ�)
		void			db_component_init_data();

		//# ��ʼ��db_component
		void			init_db_component(bool acreate);

		//# ���dbclient
		void			add_dbclient(ndbclient_base* adbclient, i64_actorid aid);

		//# ��actor_db�������������ķ���
		//# DBTYPE ��������
		//# TDBTAB ���ݽṹ
		//# TACTOR ���и����ݱ��actor
		template <pbdb::ENUM_DB DBTYPE, typename TDBTAB, typename TACTOR>
		bool handle(const message<np_actordb_load_response<DBTYPE, TDBTAB>>& adata);
#pragma endregion 

#pragma region virtual_function
		virtual ~actor_base();

		//# ��ʼ��
		//# һ��actor��������������麯��
		//# ��dbclient��actor������а�
		virtual void init() {}

		//# ��ȡactor״̬
		virtual actor_stat get_activity_stat() = 0;

		//# ����actor״̬
		virtual void set_activity_stat(actor_stat astat) = 0;

		//# ��������б��Ƿ�Ϊ��
		virtual bool list_empty() = 0;

		//# ��������
		virtual void actor_handle(i32_threadid athreadid) = 0;

		//# �������
		virtual void push(handle_pram& apram) = 0;

		//# ִ��handle֮�����
		virtual void handle_after(handle_pram&) {}

		//# ����actor���ش˺����������ݼ�����ɺ����
		virtual void loaddb_finish(bool adbishave) {}

		//# ɾ��actorʱ��ᱻ����
		virtual void release() = 0;

		//# �Ƴ�actorǰһ�̵���
		virtual void erase_actor_before() {}
#pragma endregion 

		//# ����dbclient
		virtual void save();

		//# �Ƿ�Ϊ����
		bool is_single();

		//# ��ȡactor guid
		nguid& guid();

		//# ��ȡactor guid i64_actorid
		i64_actorid id_guid();

		//# ��ȡactor guid������id
		i32_actordataid id();

		//# ��ȡactor guid������id
		i16_area area();

		//# ��ȡactor guid��actor type
		ENUM_ACTOR type();

		//# �Ƴ�actor����
		virtual void erase_actor();

		//# �Ƴ�ָ��actor
		static void erase_actor(const nguid& aguid);

		//# ��actor�����������
		void push_task_id(handle_pram& apram);

		//# ��ָ��actor�������
		static void push_task_id(const nguid& aguid, handle_pram& apram);
		static void push_task_id(const std::set<i64_actorid>& asetguid, handle_pram& apram);

		//# ��ָ�����͵�actor�������
		static void push_task_type(ENUM_ACTOR atype, handle_pram& apram);

#pragma region nscript
	private:
		//# �Խű����Ե�֧��
		void* m_script = nullptr;
		enscript m_enscript = enscript_none;					// �ű�֧��
	public:
		// # actor�Ƿ�ʹ�ýű�
		bool nscript_using();

		// # ֪ͨ�ű�db���ݼ������
		bool nscript_db_loadfinish();

		template <typename T>
		struct nscript_data_csv
		{
			using TDATA = T;
			const std::map<int32_t, T>& data;

			nscript_data_csv(const std::map<int32_t, T>& adata) :
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
			nscript_data_nsp(const std::map<int64_t, T>& adata) :
				data(adata)
			{}

			dprotocol(nscript_data_nsp<T>, data)
		};

		// # ��ű�ѹ������1��csv���� 2��db���� 3��nsp����)
		// parm aname			��������
		// parm asource			������Դ(csv,db,nsp)
		// parm adata			ѹ�������
		// parm aedit			�Ƿ�����ڽű����޸�
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

		// # ���߽ű����ݱ�ɾ����
		// parm aname			��������
		// parm adataid			����id
		template <typename T>
		bool nscript_data_del(int64_t adataid)
		{
			if (!nscript_using())
			{
				return false;
			}
			return nscript_manage::data_del(m_enscript, m_script, tools::type_name<T>().c_str(), adataid);
		}

		// # ��������Ƿ��޸�
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
		//# ���ɰ�
		template <typename T>
		static std::shared_ptr<pack> net_pack(T& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		//# �������ݵ�ָ��������
		template <typename T>
		static bool send_server(i32_serverid aserverid, T& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		//# ��һ���������������
		template <typename T>
		static bool send_server(const std::set<i32_serverid>& aserverids, T& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		//# ����pack��ָ��������
		template <typename T>
		static bool sendpack_server(i32_serverid aserverid, std::shared_ptr<pack>& apack);

		//# ͨ��proto�ṹ������json��Ϣ�幹���
		static std::shared_ptr<pack> jsonpack(const std::string& apbname, const std::string& ajson, i64_actorid aactorid, i64_actorid arequestactorid);

		//# ��ָ�����ӷ�������
		template <typename T>
		static bool sendpack_session(i32_sessionid asession, std::shared_ptr<pack>& apack);

		//# ��ָ�����ӷ�������
		template <typename T>
		static bool send(i32_sessionid asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid);
#pragma endregion 

#pragma region kcp
		//# ����udp.kcp session
		void set_kcpssion(i32_session asession);

		//# ��ȡudp.kcp session
		i32_session get_kcpssion();

		//# �Ƿ�֧��udp.kcp
		static bool iskcp();

		//# ͨ��udp.kcp��������
		template <typename T>
		bool sendkcp(T& adata, i64_actorid aactorid, int16_t asystemindex = 0);

		//# ͨ��udp.kcp��������
		template <typename T>
		static bool static_sendkcp(i32_sessionid asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid, int16_t asystemindex = 0);

		//# ͨ��udp.kcp��������
		template <typename T>
		static bool static_sendkcp(const std::vector<i32_sessionid>& asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid, int16_t asystemindex = 0);

		virtual const char* kcp_session();

		//# ����kcp����
		bool connect_kcp(int16_t anum, const std::string& aip, i16_port aprot);
#pragma endregion 

#pragma region send_client
		static i64_actorid actorclient_guid();
	private:
		template <typename T>
		static std::shared_ptr<np_actor_forward<T, forward_g2c<T>>> create_cpro(const std::shared_ptr<T>& adata)
		{
			auto pro = std::make_shared<np_actor_forward<T, forward_g2c<T>>>();
			pro->m_data.m_data = *adata;
			return pro;
		}

		template <typename T>
		static i64_actorid cpro_push_actorid(const std::shared_ptr<np_actor_forward<T, forward_g2c<T>>>& apro, i64_actorid aid)
		{
			nguid lguid(aid);
			apro->m_data.m_uid.push_back(lguid.actordataid());
			apro->m_data.m_area.push_back(lguid.area());
			return lguid.make_type(nguid::none_type());
		}
	public:
		//# ����actor_role.guidid�����ڿͻ��˷�������
		template <typename T>
		static void send_client(i64_actorid aid, const std::shared_ptr<T>& adata)
		{
			if (aid == nguid::make())
			{
				return;
			}
			auto pro = create_cpro(adata);
			cpro_push_actorid(pro, aid);
			handle_pram lpram = handle_pram::create<T>(nguid::make(), nguid::make(), pro);
			push_task_id(actorclient_guid(), lpram);
		}

		template <typename T, typename TCONTAINER>
		static void send_client(const TCONTAINER& asetids, const std::shared_ptr<T>& adata)
		{
			auto pro = create_cpro(adata);
			for (i64_actorid aactorid : asetids)
			{
				cpro_push_actorid(pro, aactorid);
			}
			handle_pram lpram = handle_pram::create<T>(nguid::make(), nguid::make(), pro);
			push_task_id(actorclient_guid(), lpram);
		}

		//# �����пͻ��˷�����Ϣ
		template <typename T>
		static void send_client(const std::shared_ptr<T>& adata)
		{
			std::set<i32_serverid>& lgatewayids = sysconfig::gatewayids();
			if (lgatewayids.empty())
			{
				return;
			}
			auto pro = create_cpro(adata);
			cpro_push_actorid(pro, nguid::make());
			send_server(lgatewayids, *pro, nguid::make(), nguid::make());
		}

		//# ��ָ���������пͻ��˷�����Ϣ
		template <typename T>
		static void send_client(i16_area aarea, const std::shared_ptr<T>& adata)
		{
			auto pro = create_cpro(adata);
			cpro_push_actorid(pro, nguid::make());
			ttab_servers::foreach_server(GATEWAY, aarea, [&pro](const tab_servers* atab)
				{
					send_server(atab->m_id, *pro, nguid::make(), nguid::make());
				});
		}
#pragma endregion

#pragma region send_actor
		//# ��ָ��actor����pack
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

		//# �������ݵ�ָ����actor
		template <typename T, bool IS_SEND = true>
		static void send_actor(const nguid& aguid, const nguid& arequestguid, const std::shared_ptr<T>& adata)
		{
			handle_pram lpram = handle_pram::create<T, IS_SEND>(aguid, arequestguid, adata);
			push_task_id(aguid, lpram);
		}

		//# �������ݵ�ָ����actor
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

		// # ������Դ�ӡЭ��
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
		//# ����һ��Ⱥ�����̿���ָ��ActorType,��Ҫ��Ϊ�����ֿͻ�������ͨactor)
		int32_t create_group(ENUM_ACTOR atype = ACTOR_NONE);
		
		//# �Ƴ�һ������
		void remove_group(int32_t agroupid);
		
		//# ����Ա����ĳ��Ⱥ������
		bool add_group_member(int32_t agroupid, i64_actorid amember);
		
		//# ����Ա��ĳ��Ⱥ���������Ƴ�
		void remove_group_member(int32_t agroupid, i64_actorid amember);
		
		//# ��ȡgroup id�е�actor�б�
		const std::set<i64_actorid>* get_group(int32_t agroupid);

		//# ��һ���Ա������Ϣ
		template <typename T>
		bool send_group(int agroupid, std::shared_ptr<T>& adata)
		{
			std::set<i64_actorid>* lset = get_group(agroupid);
			if(lset == nullptr)
			{
				return false;
			}
			std::set<i64_actorid> lclient;
			std::set<i64_actorid> lactors;
			for (i64_actorid aactor : *lset)
			{
				if ((ENUM_ACTOR)nguid::type(aactor) != ACTOR_ROBOT)
				{
					lactors.insert(aactor);
				}
				else
				{
					lclient.insert(aactor);
				}
			}
			if (!lclient.empty())
			{
				send_client(lclient, adata);
			}
			if (!lactors.empty())
			{
				send_actor(lactors, adata);
			}
			return true;
		}
#pragma endregion 
		
#pragma region broadcast
		//# ���ö�ʱ�������
		int32_t set_timer(const np_timerparm& aparm);

		//# �Ƿ�֧�ֹ㲥
		bool isbroadcast();

		//# �����Ƿ�֧�ֹ㲥
		void set_broadcast(bool aisbroadcast);

		//# �����㲥��ʱ��
		static void start_broadcast();
#pragma endregion 

		//# actor_base::create 
		//# ����actor������Զ�������
		template <typename TDerived>
		static void first_nregister(ENUM_ACTOR atype)
		{
			static std::atomic<bool> lfirst = true;
			if (lfirst.exchange(false))
			{
				TDerived::nregister();
			}
		}

		//# ���ڴ����ǵ���actor
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
					lua_State* L = (lua_State*)(aL);
					auto pro = std::make_shared<typename T::BASE_TYPE>();
					if (!ngl::nlua_stack::stack_pop(L, *pro))
					{
						return false;
					}
					actor_base::send_client(aactorid, pro);
					return true;
				};
			lptemp->m_toactor[enscript_lua] = [](int64_t aactorid, void* aL)->bool
				{
					lua_State* L = (lua_State*)(aL);
					auto pro = std::make_shared<typename T::BASE_TYPE>();
					if (!ngl::nlua_stack::stack_pop(L, *pro))
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
		static bool send_actor(int64_t aactorid, void* aL)
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
			lua_State* L = (lua_State*)(aL);
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
				linfo->m_toactor[enscript_lua] = std::bind(
					&tcustoms_send<enscript_lua>::send_actor<T>
					, std::placeholders::_1
					, std::placeholders::_2
				);
			}
		}
		return linfo;
	}
}//namespace ngl