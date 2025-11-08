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

#include "threadtools.h"
#include "example.pb.h"
#include "nactortype.h"
#include "nserialize.h"
#include "localtime.h"
#include "sysconfig.h"
#include "ndefine.h"
#include "net.pb.h"
#include "nguid.h"
#include "db.pb.h"
#include "tools.h"
#include "pack.h"
#include "type.h"
#include "xml.h"

#include <source_location>
#include <iostream>
#include <sstream>
#include <format>

namespace ngl
{
	// [db client -> db server]
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
		std::map<nguid, T>	m_data;
		bool		m_stat = true;
		bool		m_over = true;

		const std::map<nguid, T>& data()const
		{
			return m_data;
		}

		def_protocol(actor_db_load_response<T>, m_stat, m_data, m_over)
	};

	// [db client -> db server]
	// 保存数据
	template <pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_save
	{
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

	// 删除数据
	template <pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_delete
	{
		std::vector<int64_t> m_data;

		dprotocol(actor_db_delete<T>, m_data)
	};

	// 保存数据缓存列表
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

	// 模块间转发
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
	
	// 群发数据给其他actor
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

		explicit np_mass_actor(const std::shared_ptr<T>& adata) :
			m_data(adata)
		{}

		T* data()
		{
			return m_data.get();
		}

		std::shared_ptr<T> shared_data()
		{
			return m_data;
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


	// actor 切换进程
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


	enum enp_channel
	{
		enp_channel_readall		= 1,					// 只读全部
		enp_channel_writeall	= 2,					// 可写全部
		enp_channel_readpart	= 3,					// 只读部分
		enp_channel_writepart	= 4,					// 可写部分
	};
		
	// 只读注册
	template <typename TDATA>
	struct np_channel_read_register
	{
		using T = TDATA;
		std::string		m_msg;									// 调试查看信息
		i64_actorid		m_actorid = nguid::make();				// 子节点id
		enp_channel		m_type;									// 类型
		//[[ 只有 <m_type == enp_channel_readpart> 下面数据有效
		std::set<i64_actorid> m_readids;						// 只读哪些数据
		//]]
		// 结点只关心哪些字段编号
		std::set<i32_fieldnumber> m_fieldnumbers;

		dprotocol(np_channel_read_register, m_msg, m_actorid, m_type, m_readids, m_fieldnumbers)
	};

	template <typename TDATA>
	struct np_channel_write_register
	{
		using T = TDATA;
		std::string		m_msg;									// 调试查看信息
		i64_actorid		m_actorid = nguid::make();				// 子节点id
		enp_channel		m_type;									// 类型
		//[[ 只有 <m_type == enp_channel_writepart> 下面数据有效
		std::set<i64_actorid> m_writeids;						// 写哪些数据
		//]]
		std::set<int32_t> m_fieldnumbers;						// 可修改哪些字段编号

		dprotocol(np_channel_write_register, m_msg, m_actorid, m_type, m_writeids, m_fieldnumbers)
	};

	template <typename TDATA>
	struct np_channel_read_register_reply
	{
		using T = TDATA;
		std::string m_msg;											// 调试查看信息
		i64_actorid m_actorid;										// 子节点id
		// 结点可修改哪些字段编号
		std::map<i16_actortype, std::set<i32_fieldnumber>> m_node_fieldnumbers;

		dprotocol(np_channel_read_register_reply, m_msg, m_actorid, m_node_fieldnumbers)
	};


	template <typename TDATA>
	struct np_channel_write_register_reply
	{
		using T = TDATA;
		std::string m_msg;											// 调试查看信息
		i64_actorid m_actorid;										// 子节点id

		std::set<i64_nodeid> m_nodereadalls;						// 读全部数据的结点
		std::set<i64_nodeid> m_nodewritealls;						// 写全部数据的结点

		// 部分读/写
		std::map<i64_dataid, std::map<i64_nodeid, enp_channel>> m_part;

		// 结点可修改哪些字段编号
		std::map<i16_actortype, std::set<i32_fieldnumber>> m_node_fieldnumbers;

		dprotocol(np_channel_register_reply, m_msg, m_actorid, m_nodereadalls, m_nodewritealls, m_part, m_node_fieldnumbers)
	};

	template <typename TDATA>
	struct np_channel_dataid_sync
	{
		using T = TDATA;
		std::string m_msg;										// 调试查看信息
		i64_actorid m_actorid = 0;								// 异变的子节点id
		bool m_add = true;										// 增加还是删除
		enp_channel		m_type;									// 类型
		//if (m_type == enp_channel_readpart || m_type == enp_channel_writepart)
		//{
		// (部分读/写)数据被哪些结点关心
		std::set<i64_dataid> m_part;
		//}
		
		// add[
		std::set<i32_fieldnumber> m_fieldnumbers;
		// ]add

		dprotocol(np_channel_dataid_sync, m_msg, m_actorid, m_add, m_type, m_part, m_fieldnumbers)
	};

	template <typename TDATA>
	struct np_channel_exit
	{
		using T = TDATA;
		std::string				m_msg;							// 调试查看信息
		i64_actorid				m_actorid;
		dprotocol(np_channel_exit, m_msg, m_actorid)
	};

	template <typename TDATA>
	struct np_channel_data
	{
		using T = TDATA;
		std::string m_msg;									// 调试查看信息
		i64_nodeid m_actorid = 0;
		bool m_firstsynchronize = false;					// 首次同步
		bool m_recvfinish = false;
		std::map<int64_t, TDATA> m_data;					// 1、数据同步2、数据修改3、数据增加
		std::vector<int64_t> m_deldata;						// 数据被删除

		dprotocol(np_channel_data<TDATA>, m_msg, m_actorid, m_firstsynchronize, m_recvfinish, m_data, m_deldata)
	};

	template <typename TDATA>
	struct np_channel_check
	{
		using T = TDATA;
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