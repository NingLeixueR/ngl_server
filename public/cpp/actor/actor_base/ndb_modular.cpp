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
#include "ndb_modular.h"

namespace ngl
{
	ndb_component::ndb_component(pbdb::ENUM_DB aenum) :
		m_type(aenum)
	{}

	void ndb_component::set_dbclient(ndbclient_base* andbclient)
	{
		m_dbclient = andbclient;
	}

	//# 获取ndbclient基类
	ndbclient_base* ndb_component::get_dbclient()
	{
		return m_dbclient;
	}

	void ndb_component::set(actor_base* aactor)
	{
		m_actor = aactor;
		m_actor->set_db_component(this);
	}

	actor_base* ndb_component::get_actor()
	{
		return m_actor;
	}

	i64_actorid ndb_component::get_actorid()const
	{
		return m_id;
	}

	pbdb::ENUM_DB ndb_component::type()const
	{
		return m_type;
	}

	actor_base* ndb_component::actorbase()
	{
		return m_actor;
	}

	void ndb_component::set_actorid(i64_actorid aactorid)
	{
		m_id = aactorid;
	}

	void ndb_component::related_actorid()
	{
		m_id = m_actor->id_guid();
	}

	void ndb_component::init()
	{
		related_actorid();
		m_actor->add_dbclient(m_dbclient, m_id);
	}

	void ndb_component::create()
	{
		m_dbclient->create(m_id);
	}

	ndbclient_base* ndb_component::dbclientbase()
	{
		return m_dbclient;
	}

	void ndb_component::init_data() 
	{
	}
}//namespace ngl