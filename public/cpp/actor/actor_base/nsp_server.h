#pragma once

#include "ndb_modular.h"
#include <algorithm>
#include <ranges>

namespace ngl
{
	//# 订阅/发布[数据副本]
	//# nsp_server负责分发数据,当数据发生变化
	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename TDATA>
	class nsp_server
	{
	public:
		using tnsp_server = nsp_server<ENUMDB, TDerived, TDATA>;
	private:
		nsp_server() = delete;
		nsp_server(const nsp_server&) = delete;
		nsp_server& operator=(const nsp_server&) = delete;

		// # 发布列表	key/value(需要同步给谁/同步哪些数据:nguid::make()为全部)
		static std::map<i64_actorid, std::set<i64_actorid>>		m_publishlist; 
		static ndb_modular<ENUMDB, TDATA, TDerived>*			m_dbmodule;

		static void publish_foreach(
			const std::set<i64_actorid>& aactoridset, 
			std::shared_ptr<np_channel_data<TDATA>>& apro, 
			const std::pair<const i64_actorid, std::set<i64_actorid>>& apair
		)
		{
			if (!apair.second.empty())
			{
				std::vector<i64_actorid> lvec;
				std::ranges::set_intersection(aactoridset, apair.second, std::back_inserter(lvec));
				if (lvec.empty())
				{
					return;
				}
			}
			actor::static_send_actor(apair.first, nguid::make(), apro);
		}

		static void publish(const std::set<i64_actorid>& aactoridset, bool afirstsynchronize, std::shared_ptr<np_channel_data<TDATA>>& apro)
		{
			apro->m_firstsynchronize = afirstsynchronize;
			auto lforeachfun = std::bind_front(&tnsp_server::publish_foreach, std::ref(aactoridset), std::ref(apro));
			std::ranges::for_each(m_publishlist, lforeachfun);
		}

		static void publish(i64_actorid aactorid, bool afirstsynchronize, std::shared_ptr<np_channel_data<TDATA>>& apro)
		{
			apro->m_firstsynchronize = afirstsynchronize;
			std::set<i64_actorid> lactoridset = { aactorid };
			auto lforeachfun = std::bind_front(&tnsp_server::publish_foreach, std::ref(lactoridset), std::ref(apro));
			std::ranges::for_each(m_publishlist, lforeachfun);
		}

		static void channel_register(TDerived*, message<np_channel_register<TDATA>>& adata)
		{
			if (m_dbmodule->actorbase() == nullptr)
			{
				log_error()->print("nsp_server::channel_register fail m_dbmodule->actorbase() == nullptr");
				return;
			}
			auto& recv = *adata.get_data();
			if (m_publishlist.contains(recv.m_actorid))
			{
				log_error()->print("nsp_server::channel_register fail recv.m_actorid=[{}]", nguid(recv.m_actorid));
			}
			m_publishlist[recv.m_actorid] = recv.m_dataid;
			auto pro = std::make_shared<np_channel_register_reply<TDATA>>();
			pro->m_actorid = m_dbmodule->actorbase()->id_guid();
			pro->m_publishlist = m_publishlist;
			log_error()->print("nsp_server.np_channel_register reply {}", nguid(recv.m_actorid));

			std::ranges::for_each(m_publishlist, [&pro](const auto& apair)
				{
					actor::static_send_actor(apair.first, nguid::make(), pro);
				});
			
			// # 同步需要的数据
			sync(recv.m_actorid);
		}

		static void channel_exit(TDerived*, message<np_channel_exit<TDATA>>& adata)
		{
			if (m_dbmodule->actorbase() == nullptr)
			{
				log_error()->print("nsp_server::channel_exit fail m_dbmodule->actorbase() == nullptr");
				return;
			}
			auto& recv = *adata.get_data();
			m_publishlist.erase(recv.m_actorid);
			log_error()->print("nsp_server.np_channel_exit {}", nguid(recv.m_actorid));

			auto pro = std::make_shared<np_channel_register_reply<TDATA>>();
			pro->m_actorid = m_dbmodule->actorbase()->id_guid();
			pro->m_publishlist = m_publishlist;
			std::ranges::for_each(m_publishlist, [&pro](const auto& apair)
				{
					actor::static_send_actor(apair.first, nguid::make(), pro);
				});
		}

		static void channel_data(TDerived*, message<np_channel_data<TDATA>>& adata)
		{
			auto& recv = *adata.get_data();
			std::map<int64_t, TDATA>& lmap = *recv.m_data.m_data;
			std::set<i64_actorid> lactorset;
			for (std::pair<const int64_t, TDATA>& lpair : lmap)
			{
				// # m_dbmodule->get:数据不存在就创建
				data_modified<TDATA>* lp = m_dbmodule->get(lpair.first);
				lp->get() = lpair.second;
				lactorset.insert(lpair.first);
			}
			//std::shared_ptr<np_channel_data<TDATA>> pro = adata.get_shared_data();
			//publish(lactorset, false, pro);
		}

	public:
		static void init(ndb_modular<ENUMDB, TDATA, TDerived>* adbmodule)
		{
			m_dbmodule = adbmodule;
			// # 订阅注册处理
			actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, np_channel_register<TDATA>>(std::bind_front(&tnsp_server::channel_register));

			// # 订阅数据被修改
			actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, np_channel_data<TDATA>>(std::bind_front(&tnsp_server::channel_data));

			// # 退出订阅
			actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, np_channel_exit<TDATA>>(std::bind_front(&tnsp_server::channel_exit));
		}

		static void sync(i64_actorid aactor)
		{
			if (m_publishlist.empty())
			{
				return;
			}
			std::set<i64_actorid>* lpset = tools::findmap(m_publishlist, aactor);
			if (lpset == nullptr)
			{
				return;
			}

			auto pro = std::make_shared<np_channel_data<TDATA>>();
			pro->m_firstsynchronize = true;
			pro->m_data.make();
			std::map<int64_t, TDATA>& lmap = *pro->m_data.m_data;
			if (lpset->empty())
			{
				std::map<nguid, data_modified<TDATA>>& ldatamap = m_dbmodule->data();
				for (const auto& lpair : ldatamap)
				{
					lmap[lpair.first] = lpair.second.getconst();
				}
			}
			else
			{
				for (i64_actorid actorid : *lpset)
				{
					data_modified<TDATA>* lp = m_dbmodule->get(actorid);
					lmap[lp->getconst().m_id()] = lp->getconst();
				}
			}
			if (!lmap.empty())
			{
				actor::static_send_actor(aactor, nguid::make(), pro);
			}
		}

		// # 数据加载完成后主动同步给订阅者
		static void loadfish_sync()
		{
			if (m_publishlist.empty())
			{
				return;
			}
			std::ranges::for_each(m_publishlist, [](const auto& apair)
				{
					sync(apair.first);
				});
		}

		// # 数据变更
		// # 1、主动调用void publish(i64_actorid aactorid)
		// # 2、通过np_channel_data<TDATA>协议修改数据后会自动调用publish分发
		// # 此函数是主动调用需要的
		static void publish(i64_actorid aactorid)
		{
			data_modified<TDATA>* lp = m_dbmodule->find(aactorid);
			if (lp == nullptr)
			{
				return;
			}
			auto pro = std::make_shared<np_channel_data<TDATA>>();
			pro->m_data.make();
			(*pro->m_data.m_data)[aactorid] = lp->getconst();
			publish(aactorid, false, pro);
		}

		static void publish(const std::set<i64_actorid>& aactoridset)
		{
			auto pro = std::make_shared<np_channel_data<TDATA>>();
			pro->m_data.make();
			std::map<int64_t, TDATA>& lmap = *pro->m_data.m_data;
			for (i64_actorid iactorid : aactoridset)
			{
				data_modified<TDATA>* lp = m_dbmodule->find(iactorid);
				if (lp == nullptr)
				{
					continue;
				}
				lmap[iactorid] = lp->getconst();
			}
			publish(aactoridset, false, pro);
		}
	};

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename TDATA>
	std::map<i64_actorid, std::set<i64_actorid>> nsp_server<ENUMDB, TDerived, TDATA>::m_publishlist;

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename TDATA>
	ndb_modular<ENUMDB, TDATA, TDerived>* nsp_server<ENUMDB, TDerived, TDATA>::m_dbmodule;

}//namespace ngl