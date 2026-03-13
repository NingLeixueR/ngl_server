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
#include "actor/actor_base/nmanage_dbclient.h"
#include "actor/actor_base/actor_base.h"
#include "actor/actor_base/ndbclient.h"

namespace ngl
{
	void nmanage_dbclient::foreach_function(const std::function<void(ndbclient_base*)>& afun)
	{
		for (auto& [_enumdb, _npdbclient] : m_dbclientmap)
		{
			afun(_npdbclient);
		}
	}

	void nmanage_dbclient::add(ndbclient_base* adbclient, const nguid& aid)
	{
		if (m_typedbclientmap.contains(adbclient->type()))
		{
			tools::no_core_dump();
			return;
		}
		m_typedbclientmap.insert(std::make_pair(adbclient->type(), adbclient));
		init(adbclient, m_actor, aid);
	}

	void nmanage_dbclient::set_loadfinish_function(const std::function<void(pbdb::ENUM_DB, enum_dbstat)>& afun)
	{
		m_loadfinishfun = afun;
	}

	void nmanage_dbclient::init(ndbclient_base* adbclient, actor_base* aactor, const nguid& aid)
	{
		adbclient->init(this, aactor, aid);
	}

	bool nmanage_dbclient::on_load_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (m_typedbclientmap.empty())
		{
			log_error()->print("on_load_finish m_typedbclientmap.empty()");
			return false;
		}

		m_loadfinishfun(atype, astat);

		for (auto itor = m_typedbclientmap.begin(); itor != m_typedbclientmap.end();)
		{
			if (!itor->second->isload())
			{
				++itor;
				continue;
			}
			m_dbclientmap.insert(std::make_pair(itor->first, itor->second));
			itor = m_typedbclientmap.erase(itor);
		}

		if (!m_typedbclientmap.empty())
		{
			log_error()->print("on_load_finish !m_typedbclientmap.empty()");
			return false;
		}

		m_actor->db_component_init_data();

		// 1、将数据修改为[裁剪修改]
		for (auto& [_enumdb, _npdbclient] : m_dbclientmap)
		{
			_npdbclient->clear_modified();
		}

		// 2、数据压倒脚本中
		nscript_push_data();

		// 3、做一些初始化之类的工作,并且需要的话将其发送给客户端
		// 3.1 c++内部操作
		m_loadfinishfun(pbdb::ENUM_DB_ALL, astat);
		// 3.2 通知脚本
		m_actor->nscript_db_loadfinish();
		return true;
	}

	bool nmanage_dbclient::isloadfinish()const
	{
		return m_typedbclientmap.empty();
	}

	void nmanage_dbclient::save()
	{
		static std::function<void(ndbclient_base*)> lfun = [](ndbclient_base* ap)
			{
				ap->savedb();
			};
		foreach_function(lfun);
	}

	void nmanage_dbclient::del()
	{
		static std::function<void(ndbclient_base*)> lfun = [](ndbclient_base* ap)
			{
				ap->deldb();
			};
		foreach_function(lfun);
	}

	void nmanage_dbclient::nscript_push_data()
	{
		static std::function<void(ndbclient_base*)> lfun = [](ndbclient_base* ap)
			{
				ap->nscript_push_data();
			};
		foreach_function(lfun);
	}
}//namespace ngl