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

#include "ndbclient.h"
#include "csvtable.h"
#include "nlog.h"

namespace ngl
{
	class ndb_component
	{
		ndb_component() = delete;
		ndb_component(const ndb_component&) = delete;
		ndb_component& operator=(const ndb_component&) = delete;

	private:
		actor_base*				m_actor		= nullptr;							// 宿主actor
		i64_actorid				m_id		= 0;								// 宿主actor id
		ndbclient_base*			m_dbclient	= nullptr;							// ndbclient基类
		pbdb::ENUM_DB			m_type		= pbdb::ENUM_DB::ENUM_DB_FAIL;		// 数据类型

	protected:
		explicit ndb_component(pbdb::ENUM_DB aenum);

		//# 设置ndbclient基类
		void set_dbclient(ndbclient_base* andbclient);

		//# 获取ndbclient基类
		inline ndbclient_base* get_dbclient();
	public:
		//# 设置宿主actor
		void				set(actor_base* aactor);

		// # 获取actor
		actor_base*			get_actor();

		//# 获取宿主actor id
		i64_actorid			get_actorid()const;

		//# 获取数据类型
		pbdb::ENUM_DB		type()const;

		//# 获取宿主actor
		actor_base*			actorbase();

		//# 服务于related_actorid,用于自定义actor id
		void				set_actorid(i64_actorid aactorid);

		//# 设置m_id关联m_actor
		virtual void		related_actorid();

		//# 设置m_actor关联m_dbclient
		void				init();

		//# 创建数据
		void				create();

		//# 获取m_dbclient
		ndbclient_base*		dbclientbase();

		//# 当数据全部加载后调用
		virtual void		init_data();
	};

	template <pbdb::ENUM_DB ENUM, typename TDATA, typename TACTOR>
	class ndb_modular : 
		public ndb_component
	{
		ndb_modular(const ndb_modular&) = delete;
		ndb_modular& operator=(const ndb_modular&) = delete;

	protected:
		ndbclient<ENUM, TDATA, TACTOR> m_data;

		inline ndb_modular():
			ndb_component(ENUM)
		{
			set_dbclient(&m_data);
		}

		virtual ~ndb_modular() = default;
	public:
		inline ndbclient<ENUM, TDATA, TACTOR>* dbclient()
		{ 
			return &m_data;
		}

		//# 遍历data
		inline void foreach(const std::function<void(const data_modified<TDATA>&)>& afun)
		{
			for (const auto& apair : data())
			{
				afun(apair.second);
			}
		}

		inline void foreach(const std::function<void(data_modified<TDATA>&)>& afun)
		{
			for (auto& apair : foreach_data())
			{
				afun(apair.second);
			}
		}

		//# 查找指定数据
		inline data_modified<TDATA>* find(const std::function<bool(const data_modified<TDATA>&)>& afun)
		{
			for (const auto& apair : data())
			{
				if (afun(apair.second))
				{
					return &get(apair.first);
				}
			}
			return nullptr;
		}

		//# 获取所有数据
		inline const std::map<nguid, data_modified<TDATA>>& data()
		{
			return m_data.get_data();
		}

		inline std::map<nguid, data_modified<TDATA>>& foreach_data()
		{
			return m_data.get_foreach_data();
		}

		//# 获取m_id对应的数据
		inline data_modified<TDATA>& get()
		{
			return get(get_actorid());
		}

		// # 查找指定数据
		inline data_modified<TDATA>* find(nguid aid)
		{
			if (!data().contains(aid))
			{
				return nullptr;
			}
			return &get(aid);
		}

		// # 与find类似(只是没有就添加)
		inline data_modified<TDATA>& get(nguid aid)
		{
			if (aid == nguid::make())
			{
				tools::no_core_dump();
			}
			if (get_actorid() != nguid::make() && get_actorid() != aid)
			{
				tools::no_core_dump();
			}
			return *m_data.get_data(aid);
		}

		// # 删除指定数据
		inline void erase(nguid aid)
		{
			get_actor_dbclient()->del(aid, true);
		}

		// # 返回保护数据
		inline data_modified<TDATA>* db()
		{ 
			return m_data.get_dbdata();
		}

		// # 获取宿主actor
		inline TACTOR* nactor()
		{ 
			return (TACTOR*)get_actor();
		}

		// # 创建数据
		inline void create()
		{
			m_id = m_actor->id_guid();
			m_data.set_id(m_id);
			m_data.set_actor(m_actor);
			m_data.add(m_id, TDATA());
		}

		// # 清空数据
		inline void clear()
		{
			get_actor_dbclient()->clear();
		}

		// # 当数据全部加载后调用
		void init_data() final;

		virtual void initdata() = 0;

		inline ndbclient<ENUM, TDATA, TACTOR>* get_actor_dbclient()
		{
			return (ndbclient<ENUM, TDATA, TACTOR>*)get_dbclient();
		}

		inline data_modified<TDATA>& add(i64_actorid aid, const TDATA& adbtab)
		{
			if (get_actorid() != nguid::make() && get_actorid() != aid)
			{
				tools::no_core_dump();
			}
			data_modified<TDATA>& ldata = get(aid);
			*ldata.get() = adbtab;
			return ldata;
		}

		inline void remove(int64_t aid)
		{
			get_actor_dbclient()->del(aid);
		}
	};
}//namespace ngl