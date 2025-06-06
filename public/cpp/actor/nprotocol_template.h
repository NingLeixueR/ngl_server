#pragma once

#include "threadtools.h"
#include "nactortype.h"
#include "serialize.h"
#include "localtime.h"
#include "serialize.h"
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

		def_portocol(np_actordb_load, m_id)
	};

	template <pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_load_response
	{
		protobuf_data<std::map<nguid, T>>	m_data;
		bool		m_stat = true;
		bool		m_over = true;

		const std::map<nguid, T>& data()const
		{
			return *m_data.m_data;
		}

		def_portocol(actor_db_load_response<T>, m_stat, m_data, m_over)
	};

	// ---- [actor db server -> actor db client]
	// ---- 保存数据
	// 从db server加载数据
	template <pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_save
	{
		protobuf_data<std::map<nguid, T>>	m_data;

		void add(const nguid& akey, const T& avalue)
		{
			if (m_data.m_data == nullptr)
				m_data.make();
			m_data.m_data->insert(std::make_pair(akey, avalue));
		}

		bool empty()
		{
			if (m_data.m_data == nullptr)
				return true;
			return m_data.m_data->empty();
		}

		def_portocol(actor_db_save<T>, m_data)
	};

	// 从db server删除数据
	template <pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_delete
	{
		std::vector<int64_t> m_data;

		def_portocol(actor_db_delete<T>, m_data)
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

		def_portocol(actor_time_db_cache<T>, m_ls)
	};

	// ---- 模块间转发
	template <typename T>
	struct np_actormodule_forward
	{
	private:
		int64_t				m_identifier = 0;
		std::shared_ptr<T>	m_data;
	public:
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
			m_data(nullptr)
		{
		}

		np_actormodule_forward() :
			m_identifier(0),
			m_data(nullptr)
		{
		}

		T* add_data()
		{
			if (m_data == nullptr)
			{
				m_data = std::make_shared<T>();
			}
			return m_data.get();
		}

		const T* data()const
		{
			if (m_data == nullptr)
				return nullptr;
			return m_data.get();
		}

		int64_t identifier()const
		{
			return m_identifier;
		}

		static const char* name()
		{
			return "actor_module_forward";
		}

		inline bool push(ngl::serialize& ser)const
		{
			if (ser.push(m_identifier) == false)
			{
				return false;
			}
			if (m_data->SerializeToArray(&ser.buff()[ser.byte()], ser.len() - ser.byte()) == false)
			{
				return false;
			}
			return true;
		}

		inline bool SerializeToArray(void* data, int size) const
		{
			ngl::serialize lserialize((char*)data, size);
			return push(lserialize);
		}

		inline bool pop(ngl::unserialize& ser)
		{
			if (ser.pop(m_identifier) == false)
			{
				return false;
			}
			if (m_data == nullptr)
			{
				m_data = std::make_shared<T>();
			}
			return m_data->ParseFromArray(&ser.buff()[ser.byte()], ser.len() - ser.byte());
		}

		inline bool ParseFromArray(const void* data, int size)
		{
			ngl::unserialize lunserialize((const char*)data, size);
			return pop(lunserialize);
		}

		inline int bytes(ngl::serialize_bytes& abytes)const
		{
			int lbytes = abytes.bytes(m_identifier);
			return lbytes += m_data->ByteSize();
		}

		inline int ByteSize()const
		{
			ngl::serialize_bytes lserialize_bytes;
			return bytes(lserialize_bytes);
		}
	};

	template <typename TDATA>
	using mforward = np_actormodule_forward<TDATA>;

	template <typename T, EPROTOCOL_TYPE PROTYPE, bool ISUSING, typename TREAL>
	struct np_actor_forward
	{
		using BASE_TYPE = T;
		static const bool isusing = ISUSING;

		const T* get_data()const
		{
			return nullptr;
		}

		std::shared_ptr<T> get_shared()const
		{
			return nullptr;
		}
	};

	template <typename T, EPROTOCOL_TYPE PROTYPE>
	struct np_actor_forward<T, PROTYPE, true, ngl::forward>
	{
		using BASE_TYPE = T;
		static const bool				isusing = true;
		std::vector<i32_actordataid>	m_uid;
		std::vector<i16_area>			m_area;
		std::shared_ptr<pack>			m_recvpack;

		np_actor_forward() = default;

		explicit np_actor_forward(const np_actor_forward<T, PROTYPE, false, ngl::forward>& adata) :
			m_recvpack(adata.m_recvpack)
		{
		}

		const T* get_data()const
		{
			return nullptr;
		}

		std::shared_ptr<T> get_shared()const
		{
			return nullptr;
		}

		def_portocol(np_actor_forward, m_uid, m_area/*, m_data*/)
	};

	template <typename T, EPROTOCOL_TYPE PROTYPE>
	struct np_actor_forward<T, PROTYPE, false, ngl::forward>
	{
		using BASE_TYPE = T;
		static const bool isusing = false;
		std::shared_ptr<pack> m_recvpack;

		np_actor_forward() = default;

		explicit np_actor_forward(const np_actor_forward<T, PROTYPE, true, ngl::forward>& adata) :
			m_recvpack(adata.m_recvpack)
		{}

		const T* get_data()const
		{
			return nullptr;
		}

		std::shared_ptr<T> get_shared()const
		{
			return nullptr;
		}

		def_portocol(np_actor_forward, /*m_uid, m_area,*/ /*m_data*/)
	};

	template <typename T>
	struct np_actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, true, T>
	{
		using BASE_TYPE = T;
		static const bool				isusing = true;
		std::vector<i32_actordataid>	m_uid;
		std::vector<i16_area>			m_area;
	private:
		protobuf_data<T, true> m_data;
	public:
		void set_data(const std::shared_ptr<T>& adata)
		{
			m_data.m_data = adata;
		}

		const T* get_data()const
		{
			return m_data.m_data.get();
		}

		std::shared_ptr<T> get_shared()const
		{
			return m_data.m_data;
		}

		np_actor_forward() = default;

		explicit np_actor_forward(const np_actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, false, T>& adata)
			:m_uid(adata.m_uid), m_area(adata.m_area), m_data(adata.m_data)
		{}

		def_portocol(np_actor_forward, m_uid, m_area, m_data)
	};

	template <typename T>
	struct np_actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, false, T>
	{
		using BASE_TYPE = T;
		static const bool				isusing = false;
		std::vector<i32_actordataid>	m_uid;
		std::vector<i16_area>			m_area;
	private:
		protobuf_data<T, true> m_data;
	public:
		void set_data(std::shared_ptr<T>& adata)
		{
			m_data.m_data = adata;
		}

		const T* get_data()const
		{
			return m_data.m_data.get();
		}

		std::shared_ptr<T> get_shared()const
		{
			return m_data.m_data;
		}

		void make_data()
		{
			m_data.make();
		}

		np_actor_forward() = default;

		explicit np_actor_forward(const np_actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, true, T>& adata)
			:m_uid(adata.m_uid), m_area(adata.m_area), m_data(adata.m_data)
		{}

		def_portocol(np_actor_forward, m_uid, m_area, m_data)
	};

	template <typename T, bool ISUSING>
	struct np_actor_forward<T, EPROTOCOL_TYPE_CUSTOM, ISUSING, T>
	{
		using BASE_TYPE = T;
		static const bool				isusing = ISUSING;
		std::vector<i32_actordataid>	m_uid;
		std::vector<i16_area>			m_area;
	private:
		T m_data_;
		T* m_data;
	public:
		void set_data(T& adata)
		{
			m_data = &adata;
		}

		const T* get_data()const
		{
			return m_data != nullptr ? m_data : &m_data_;
		}

		np_actor_forward()
			:m_data(nullptr)
		{}

		explicit np_actor_forward(const np_actor_forward<T, EPROTOCOL_TYPE_CUSTOM, ISUSING ? false : true, T>& adata)
			:m_uid(adata.m_uid), m_area(adata.m_area), m_data(adata.m_data), m_data_(adata.m_data_)
		{}

		def_portocol(np_actor_forward, m_uid, m_area, m_data != nullptr ? *m_data : m_data_)
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

		def_portocol(np_actorswitch_process, m_actor, m_serverid, m_toserverid, m_pram);
	};

	template <typename TDATA>
	struct np_channel_register
	{
		i64_actorid				m_actorid;
		std::set<i64_actorid>	m_dataid;
		def_portocol(np_channel_register, m_actorid, m_dataid)
	};

	template <typename TDATA>
	struct np_channel_register_reply
	{
		i64_actorid m_actorid;
		std::map<i64_actorid, std::set<i64_actorid>> m_publishlist;// 其他副本结点(nsp_client)
		def_portocol(np_channel_register_reply, m_actorid, m_publishlist)
	};

	template <typename TDATA>
	struct np_channel_exit
	{
		i64_actorid				m_actorid;
		def_portocol(np_channel_exit, m_actorid)
	};

	template <typename TDATA>
	struct np_channel_data
	{
		bool m_firstsynchronize = false;					// 首次同步
		protobuf_data<std::map<int64_t, TDATA>> m_data;
		def_portocol(np_channel_data<TDATA>, m_firstsynchronize, m_data)
	};

	template <typename TDATA>
	struct np_channel_check
	{
		int64_t m_timer;
		i16_area m_area;
		def_portocol(np_channel_check, m_timer, m_area)
	};


	struct np_gm
	{
		std::string m_json;

		def_portocol(np_gm, m_json)
	};

	struct np_gm_response
	{
		std::string m_json;

		def_portocol(np_gm_response, m_json)
	};

}//namespace ngl