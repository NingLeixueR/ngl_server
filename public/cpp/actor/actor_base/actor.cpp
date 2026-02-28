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
#include "actor/actor_base/actor_manage.h"
#include "actor/actor_base/ndbclient.h"
#include "actor/actor_base/actor.h"
#include "net/tcp/ntcp.h"

namespace ngl
{
	actor::actor(const actorparm& aparm) :
		actor_base(aparm.m_parm)
		, m_weight(aparm.m_weight)
		, m_timeout(aparm.m_timeout)
	{
		if (aparm.m_weight <= 0 || m_timeout <= 0)
		{
			tools::no_core_dump();
		}
		// 检查数据库依赖actorparmbase.m_manage_dbclient
		// 如果依赖数据库那么需要开启"是否支持广播"m_broadcast
		// 以便顺利保存数据修改
		if (aparm.m_parm.m_manage_dbclient && !aparm.m_broadcast)
		{
			log_error()->print("actorparm fail [m_parm.m_manage_dbclient && !m_broadcast]");
			actorparm lparm = aparm;
			lparm.m_broadcast = true;
			set_broadcast(true);
		}
		else
		{
			set_broadcast(aparm.m_broadcast);
		}
	}

	void actor::release()
	{
		lock_write(m_mutex);
		m_release = true;
		actor_handle(0x7fffffff);
		save();
		m_list.clear();
	}

	bool actor::list_empty()
	{
		lock_read(m_mutex);
		return m_list.empty() && m_hightlist.empty();
	}

	actor_stat actor::activity_stat()
	{
		lock_read(m_mutex);
		return m_stat;
	}

	void actor::set_activity_stat(actor_stat astat)
	{
		lock_write(m_mutex);
		m_stat = astat;
	}

	void actor::push(handle_pram& apram)
	{
		int8_t highvalue = tprotocol::highvalue(apram.m_enum);
		lock_write(m_mutex);
		if (highvalue <= 0)
		{
			m_list.push_back(apram);
		}
		else
		{
			m_hightlist[highvalue].push_back(apram);
		}
	}

	bool actor::ahandle(i32_threadid athreadid, handle_pram& aparm)
	{
		//Try
		//{
			nrfunbase* lprfun = m_actorfun;
			if (lprfun == nullptr)
			{
				tools::no_core_dump();
				return false;
			}
			if (lprfun->handle_switch(this, athreadid, aparm))
			{
				return true;
			}
			lprfun->notfindfun(this, athreadid, aparm);
			return true;
		//}Catch
		//return false;
	}

	void actor::actor_handle(i32_threadid athreadid)
	{
		std::list<handle_pram> locallist;
		std::map<int32_t, std::list<handle_pram>> localhightlist;
		{
			lock_write(m_mutex);
			m_hightlist.swap(localhightlist);
			m_list.swap(locallist);
		}

		if (!localhightlist.empty())
		{
			for (auto& [_hight, _list] : localhightlist)
			{
				for (auto& _harm : _list)
				{
					ahandle(athreadid, _harm);
				}
			}
			localhightlist.clear();
		}
		
		auto llistcount = (int32_t)locallist.size();
		if (m_weight < llistcount || llistcount >= 0x7F)
		{
			log_error()->print("actor::actor_handle({}) {}:[weight:{}/count:{}]", athreadid, nguid(id_guid()), m_weight, locallist.size());
		}
		time_t lbeg = localtime::gettimems();
		int32_t lcount = 0;
		int32_t lweight = m_weight;
		while (--lweight >= 0 && !locallist.empty())
		{
			if (m_release == false && localtime::timeout(lbeg, m_timeout))
			{
				break;
			}
			ahandle(athreadid, locallist.front());
			locallist.pop_front();
			++lcount;
		}
		if (!locallist.empty())
		{
			lock_write(m_mutex);
			m_list.insert(m_list.begin(), locallist.begin(), locallist.end());
		}
	}

	bool actor::handle_broadcast(const message<np_actor_broadcast>& adata)
	{
		// # 保存数据
		if (manage_dbclient() != nullptr)
		{
			manage_dbclient()->save();
			manage_dbclient()->del();
		}
		// # actor派生类自定义处理逻辑
		broadcast();
		return true;
	}

	bool actor::handle_close(const message<np_actor_close>&)
	{
		erase_actor();
		return true;
	}
}//namespace ngl