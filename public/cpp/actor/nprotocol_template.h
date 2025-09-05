#pragma once

#include "threadtools.h"
#include "nactortype.h"
#include "nserialize.h"
#include "localtime.h"
#include "sysconfig.h"
#include "ndefine.h"
#include "nguid.h"
#include "db.pb.h"
#include "tools.h"
#include "pack.h"
#include "type.h"
#include "xml.h"

#include "example.pb.h"
#include "net.pb.h"

#include <source_location>
#include <iostream>
#include <sstream>
#include <format>

namespace ngl
{
	// ---- [actor db client -> actor db server]
	// ---- 从db server加载数据
	// 从db server加载数据
	template <pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_load
	{
		nguid m_id = -1;

		def_protocol(np_actordb_load, m_id)
	};

	template <pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_load_response
	{
		//protobuf_data<std::map<nguid, T>>	m_data;
		std::map<nguid, T>	m_data;
		bool		m_stat = true;
		bool		m_over = true;

		const std::map<nguid, T>& data()const
		{
			return m_data;
		}

		def_protocol(actor_db_load_response<T>, m_stat, m_data, m_over)
	};

	// ---- [actor db server -> actor db client]
	// ---- 保存数据
	// 从db server加载数据
	template <pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_save
	{
		//protobuf_data<std::map<nguid, T>>	m_data;
		std::map<nguid, T > m_data;

		void add(const nguid& akey, const T& avalue)
		{
			m_data.insert(std::make_pair(akey, avalue));
		}

		bool empty()
		{
			return m_data.empty();
		}

		def_protocol(actor_db_save<T>, m_data)
	};

	// 从db server删除数据
	template <pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_delete
	{
		std::vector<int64_t> m_data;

		dprotocol(actor_db_delete<T>, m_data)
	};

	// db cache list  保存缓存列表
	enum enum_cache_list
	{
		enum_clist_save = 1,
		enum_clist_del = 2,
	};
	template <typename T>
	struct np_actortime_db_cache
	{
		enum_cache_list			m_type;
		std::set<i64_actorid>	m_ls;

		dprotocol(actor_time_db_cache<T>, m_ls)
	};

	// ---- 模块间转发
	template <typename T>
	struct np_actormodule_forward
	{
	private:
		int64_t				m_identifier = 0;
		std::shared_ptr<T>	m_data;
	public:
		using BASE_TYPE = T;

		def_protocol(np_actormodule_forward<T>, m_identifier, *m_data)
		def_nlua_function2("m_identifier", m_identifier, "m_data", *m_data)

		np_actormodule_forward(int64_t aidentifier, const std::shared_ptr<T>& adata) :
			m_identifier(aidentifier),
			m_data(adata)
		{
		}

		np_actormodule_forward(int64_t aidentifier, const T& adata) :
			m_identifier(aidentifier),
			m_data(std::make_shared<T>(adata))
		{
		}

		explicit np_actormodule_forward(int64_t aidentifier) :
			m_identifier(aidentifier),
			m_data(std::make_shared<T>())
		{
		}

		np_actormodule_forward() :
			m_identifier(0),
			m_data(std::make_shared<T>())
		{
		}

		T* data()const
		{
			return m_data.get();
		}

		int64_t identifier()const
		{
			return m_identifier;
		}

		void set_identifier(int64_t aidentifier)
		{
			m_identifier = aidentifier;
		}
	};

	template <typename TDATA>
	using mforward = np_actormodule_forward<TDATA>;

	template <typename T>
	bool tools::proto2json(const mforward<T>& adata, std::string& json)
	{
		const T* ldata = adata.data();
		if (ldata == nullptr)
		{
			return false;
		}
		std::string ltemp;
		if (!proto2json(*ldata, ltemp))
		{
			return false;
		}
		json = std::format(R"({{"msg":"{}","identifier":"{}","data":{}}})", tools::type_name<T>(), adata.identifier(), ltemp);
		return true;
	}

	struct forward
	{
		int32_t m_bufflen = 0;
		const char* m_buff = nullptr;
	};

	namespace ser
	{
		template <>
		struct serialize_format<forward>
		{
			static bool push(serialize_push* aserialize, const forward& adata)
			{
				if (!serialize_format<int32_t>::push(aserialize, adata.m_bufflen))
				{
					return false;
				}
				return aserialize->basetype((void*)adata.m_buff, adata.m_bufflen);
			}

			static bool pop(serialize_pop* aserialize, forward& adata)
			{
				if (!serialize_format<int32_t>::pop(aserialize, adata.m_bufflen))
				{
					return false;
				}
				if (adata.m_bufflen > (aserialize->len() - aserialize->pos()))
				{
					return false;
				}
				adata.m_buff = &aserialize->buff()[aserialize->pos()];
				return true;
			}

			static void bytes(serialize_byte* aserialize, const forward& adata)
			{
				serialize_format<int32_t>::bytes(aserialize, adata.m_bufflen);
				aserialize->move_pos(adata.m_bufflen);
			}
		};
	}

	template <typename T>
	struct forward_c2g
	{
		T m_data;
	};

	namespace ser
	{
		template <typename T>
		struct serialize_format<forward_c2g<T>>
		{
			static bool push(serialize_push* aserialize, const forward_c2g<T>& adata)
			{
				return ngl::ser::nserialize::push(aserialize, adata.m_data);
			}

			static bool pop(serialize_pop* aserialize, forward_c2g<T>& adata)
			{
				return ngl::ser::nserialize::pop(aserialize, adata.m_data);
			}

			static void bytes(serialize_byte* aserialize, const forward_c2g<T>& adata)
			{
				ngl::ser::nserialize::bytes(aserialize, adata.m_data);
			}
		};
	}

	template <typename T>
	struct forward_g2c
	{
		std::vector<int16_t> m_area;
		std::vector<int32_t> m_uid;
		T m_data;
	};

	namespace ser
	{
		template <typename T>
		struct serialize_format<forward_g2c<T>>
		{
			static bool push(serialize_push* aserialize, const forward_g2c<T>& adata)
			{
				return ngl::ser::nserialize::push(aserialize, adata.m_area, adata.m_uid, adata.m_data);
			}

			static bool pop(serialize_pop* aserialize, forward_g2c<T>& adata)
			{
				return ngl::ser::nserialize::pop(aserialize, adata.m_area, adata.m_uid, adata.m_data);
			}

			static void bytes(serialize_byte* aserialize, const forward_g2c<T>& adata)
			{
				ngl::ser::nserialize::bytes(aserialize, adata.m_area, adata.m_uid, adata.m_data);
			}
		};
	}


	template <typename T, typename Y>
	struct np_actor_forward
	{
		Y m_data;

		dprotocol(np_actor_forward, m_data)
	};
	
	// # 群发数据给其他actor
	template <typename T>
	struct np_mass_actor
	{
		std::set<i64_actorid> m_actorids;
	private:
		std::shared_ptr<T> m_data;
	public:
		np_mass_actor() :
			m_data(std::make_shared<T>())
		{}

		np_mass_actor(std::shared_ptr<T>& adata) :
			m_data(adata)
		{}

		T* data()const
		{
			return m_data.get();
		}

		void set_actorids(std::set<i64_actorid>& aactorids)
		{
			m_actorids = aactorids;
		}

		void add_actorid(i64_actorid aactorid)
		{
			m_actorids.insert(aactorid);
		}

		def_jsonfunction_parm("m_actorids", m_actorids, "m_data", *m_data)
		def_protocol(np_mass_actor, m_actorids, *m_data)
		def_nlua_function2("m_actorids", m_actorids, "m_data", *m_data)
	};


	// ---- actor 切换进程
	//ACTOR_SPROCESS_ROLE
	template <typename T>
	struct np_actorswitch_process
	{
		i64_actorid		m_actor = 0;
		i32_serverid	m_serverid = 0;
		i32_serverid	m_toserverid = 0;
		T				m_pram;

		dprotocol(np_actorswitch_process, m_actor, m_serverid, m_toserverid, m_pram);
	};
		
	template <typename TDATA>
	struct np_channel_register
	{
		std::string m_msg;										// 调试查看信息
		i64_actorid	m_actorid		= nguid::make();			// 子节点id
		bool m_onlyread				= true;						// 是否只读
		std::set<i64_actorid> m_dataid;							// 关注哪些数据,结点可读可写

		dprotocol(np_channel_register, m_msg, m_actorid, m_onlyread, m_dataid)
	};

	template <typename TDATA>
	struct np_channel_register_reply
	{
		std::string m_msg;											// 调试查看信息
		i64_actorid m_actorid;										// 子节点id
		std::set<i64_actorid> m_onlyreads;							// 只读全部数据
		std::set<i64_actorid> m_writealls;							// 读/写全部数据
		// m_publishlist_write.first:<结点id>
		// m_publishlist_write.second:<读写的数据id列表>
		std::map<i64_actorid, std::set<i64_actorid>>			m_publishlist;

		dprotocol(np_channel_register_reply, m_msg, m_actorid, m_onlyreads, m_writealls, m_publishlist)
	};

	template <typename TDATA>
	struct np_channel_dataid_sync
	{
		std::string m_msg;										// 调试查看信息
		i64_actorid m_actorid = 0;								// 异变的子节点id
		bool m_add = true;										// 增加还是删除
		// add
		// [
		bool m_onlyread = true;									// 是否只读
		std::set<i64_actorid> m_dataid;							// 关注哪些数据,结点可读可写
		// ]

		dprotocol(np_channel_dataid_sync, m_msg, m_actorid, m_add, m_onlyread, m_dataid)
	};

	template <typename TDATA>
	struct np_channel_exit
	{
		std::string				m_msg;							// 调试查看信息
		i64_actorid				m_actorid;
		dprotocol(np_channel_exit, m_msg, m_actorid)
	};

	template <typename TDATA>
	struct np_channel_data
	{
		std::string m_msg;									// 调试查看信息
		bool m_firstsynchronize = false;					// 首次同步
		bool m_recvfinish = false;
		//protobuf_data<std::map<int64_t, TDATA>> m_data;		// 1、数据同步2、数据修改3、数据增加
		std::map<int64_t, TDATA> m_data;					// 1、数据同步2、数据修改3、数据增加
		std::vector<int64_t> m_deldata;						// 数据被删除
		def_protocol(np_channel_data<TDATA>, m_msg, m_firstsynchronize, m_recvfinish, m_data)
	};

	template <typename TDATA>
	struct np_channel_check
	{
		std::string m_msg;									// 调试查看信息
		int64_t m_timer;
		i16_area m_area;
		dprotocol(np_channel_check, m_msg, m_timer, m_area)
	};


	struct np_gm
	{
		std::string m_json;

		dprotocol(np_gm, m_json)
	};

	struct np_gm_response
	{
		std::string m_json;

		dprotocol(np_gm_response, m_json)
	};

	struct np_testlua
	{
		std::string m_name;
		std::map<int, std::string> m_data;

		dprotocol(np_testlua, m_name, m_data)
	};
}//namespace ngl