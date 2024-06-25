#pragma once

#include "ndb_modular.h"
#include <algorithm>

namespace ngl
{
	//# 订阅/发布[数据副本](Subscription/Publishing)
	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename TDATA>
	class nsp_server
	{
		nsp_server() = delete;
		nsp_server(const nsp_server&) = delete;
		nsp_server& operator=(const nsp_server&) = delete;

		// # 发布列表	key/value(需要同步给谁/同步哪些数据:nguid::make()为全部)
		static std::map<i64_actorid, std::set<i64_actorid>>		m_publishlist; 
		static ndb_modular<ENUMDB, TDATA, TDerived>*			m_dbmodule;
	public:
		static void init(ndb_modular<ENUMDB, TDATA, TDerived>* adbmodule)
		{
			m_dbmodule = adbmodule;
			// # 订阅
			actor::register_actor_s<
				EPROTOCOL_TYPE_CUSTOM,
				TDerived,
				np_channel_register<TDATA>
			>([](TDerived* apTDerived, message<np_channel_register<TDATA>>& adata)
				{
					auto& recv = *adata.get_data();
					m_publishlist[recv.m_actorid] = recv.m_dataid;
					auto pro = std::make_shared<np_channel_register_reply<TDATA>>();
					pro->m_actorid = recv.m_actorid;
					m_dbmodule->actor()->send_actor(recv.m_actorid, pro);
					m_dbmodule->log_error()->print(
						"nsp_server register {}#{} actorid{} dataid{}",
						tools::type_name<TDerived>(),
						tools::type_name<TDATA>(),
						nguid(recv.m_actorid),
						recv.m_dataid
					);
				});

			actor::register_actor_s<
				EPROTOCOL_TYPE_CUSTOM,
				TDerived,
				np_channel_data<TDATA>
			>([](TDerived* apTDerived, message<np_channel_data<TDATA>>& adata)
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
					std::shared_ptr<np_channel_data<TDATA>> pro = adata.get_shared_data();
					publish(lactorset, pro);
				});
				
		}

		static void publish(
			const std::set<i64_actorid>& aactoridset, 
			std::shared_ptr<np_channel_data<TDATA>>& apro
		)
		{
			m_dbmodule->log_error()->print(
				"nsp_server publish {}:{}:{}",
				tools::type_name<TDerived>(),
				tools::type_name<TDATA>(),
				aactoridset
			);			
			for (std::pair<const i64_actorid, std::set<i64_actorid>>& lpair : 
				m_publishlist)
			{
				if (!lpair.second.empty())
				{
					std::vector<i64_actorid> lvec;
					std::set_intersection(
						aactoridset.begin(), aactoridset.end(),
						aactoridset.begin(), aactoridset.end(),
						lvec.begin()
					);
					if (lvec.empty())
						continue;
				}
				actor::static_send_actor(lpair.first, nguid::make(), apro);
			}
		}

		static void publish(
			i64_actorid aactorid, 
			std::shared_ptr<np_channel_data<TDATA>>& apro
		)
		{
			m_dbmodule->log_error()->print(
				"nsp_server publish {}:{}:{}",
				tools::type_name<TDerived>(),
				tools::type_name<TDATA>(),
				aactorid
			);
			data_modified<TDATA>* lp = m_dbmodule->find(aactorid);
			if (lp == nullptr)
				return;
			for (std::pair<const i64_actorid, std::set<i64_actorid>>& lpair : 
				m_publishlist)
			{
				if (lpair.second.empty() || lpair.second == aactorid)
				{
					actor::static_send_actor(lpair.first, nguid::make(), apro);
				}
			}
		}
	};

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename TDATA>
	std::map<i64_actorid, std::set<i64_actorid>> nsp_server<ENUMDB, TDerived, TDATA>::m_publishlist;

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename TDATA>
	ndb_modular<ENUMDB, TDATA, TDerived>* nsp_server<ENUMDB, TDerived, TDATA>::m_dbmodule;

}