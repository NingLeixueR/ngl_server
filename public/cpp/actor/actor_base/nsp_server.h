#pragma once

#include "ndb_modular.h"

namespace ngl
{
	//# 订阅/发布[数据副本](Subscription/Publishing)
	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename TDATA>
	class nsp_server
	{
		static std::set<i64_actorid> m_publishlist; // 发布列表
		static ndb_modular<ENUMDB, TDATA, TDerived>* m_dbmodule;
	public:
		static void init(ndb_modular<ENUMDB, TDATA, TDerived>* adbmodule)
		{
			m_dbmodule = adbmodule;
			// # 订阅
			actor::register_actor_s<
				EPROTOCOL_TYPE_CUSTOM,
				TDerived,
				np_channel_register
			>([](TDerived* apTDerived, np_channel_register& ainfo)
				{
					m_publishlist.insert(ainfo.m_actorid);
					auto pro = std::make_shared<np_channel_register_reply>();
					pro->m_actorid = ainfo.m_actorid;
					m_dbmodule->actor()->send_actor(ainfo.m_actorid, pro);
					m_dbmodule->log_error()->print(
						"nsp_server register {}:{}:{}",
						tools::type_name<TDerived>(),
						tools::type_name<TDATA>(),
						nguid(ainfo.m_actorid)
					);
				});
		}

		static void publish(const std::vector<i64_actorid>& aactoridvec)
		{
			m_dbmodule->log_error()->print(
				"nsp_server publish {}:{}:{}",
				tools::type_name<TDerived>(),
				tools::type_name<TDATA>(),
				aactoridvec
			);

			auto pro = std::make_shared<np_channel_data<TDATA>>();
			pro->m_data.make();
			std::map<int64_t, TDATA>& lmap = *pro->m_data.m_data;
			for (i64_actorid aactorid : aactoridvec)
			{
				data_modified<TDATA>* lp = m_dbmodule->find(aactorid);
				if (lp == nullptr)
					continue;
				lmap[aactorid] = lp->getconst();
			}
			publish(pro);
		}

		static void publish(i64_actorid aactorid)
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
			auto pro = std::make_shared<np_channel_data<TDATA>>();
			pro->m_data.m_data[aactorid] = lp->getconst();
			publish(pro);
		}

		static void publish(std::shared_ptr<np_channel_data<TDATA>>& apro)
		{
			for (i64_actorid aactor : m_publishlist)
			{
				actor::static_send_actor(aactor, nguid::make(), apro);
			}
		}
	};

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename TDATA>
	std::set<i64_actorid> nsp_server<ENUMDB, TDerived, TDATA>::m_publishlist;

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename TDATA>
	ndb_modular<ENUMDB, TDATA, TDerived>* nsp_server<ENUMDB, TDerived, TDATA>::m_dbmodule;

}