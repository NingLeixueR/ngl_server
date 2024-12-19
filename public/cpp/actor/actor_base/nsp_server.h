#pragma once

#include "ndb_modular.h"
#include <algorithm>

namespace ngl
{
	//# ����/����[���ݸ���]
	//# nsp_server����ַ�����,�����ݷ����仯
	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename TDATA>
	class nsp_server
	{
		nsp_server() = delete;
		nsp_server(const nsp_server&) = delete;
		nsp_server& operator=(const nsp_server&) = delete;

		// # �����б�	key/value(��Ҫͬ����˭/ͬ����Щ����:nguid::make()Ϊȫ��)
		static std::map<i64_actorid, std::set<i64_actorid>>		m_publishlist; 
		static ndb_modular<ENUMDB, TDATA, TDerived>*			m_dbmodule;

		static void publish(const std::set<i64_actorid>& aactoridset, std::shared_ptr<np_channel_data<TDATA>>& apro)
		{
			std::ranges::for_each(m_publishlist, [&aactoridset,&apro](const auto& apair)
				{
					if (!apair.second.empty())
					{
						std::vector<i64_actorid> lvec;
						std::set_intersection(
							aactoridset.begin(), aactoridset.end(),
							apair.second.begin(), apair.second.end(),
							lvec.begin()
						);
						if (lvec.empty())
							return;
					}
					actor::static_send_actor(apair.first, nguid::make(), apro);
				});
		}

		static void publish(i64_actorid aactorid, std::shared_ptr<np_channel_data<TDATA>>& apro)
		{
			data_modified<TDATA>* lp = m_dbmodule->find(aactorid);
			if (lp == nullptr)
				return;
			std::ranges::for_each(m_publishlist, [aactorid, &apro](const auto& lpair)
				{
					if (!lpair.second.empty())
					{
						return;
					}
					if (lpair.second.contains(aactorid) == false)
					{
						return;
					}
					actor::static_send_actor(lpair.first, nguid::make(), apro);
				});
		}

	public:
		static void init(ndb_modular<ENUMDB, TDATA, TDerived>* adbmodule)
		{
			m_dbmodule = adbmodule;
			// # ����ע�ᴦ��
			actor::register_actor_s<
				EPROTOCOL_TYPE_CUSTOM, TDerived, np_channel_register<TDATA>
			>([](TDerived*, message<np_channel_register<TDATA>>& adata)
				{
					auto& recv = *adata.get_data();
					m_publishlist[recv.m_actorid] = recv.m_dataid;
					auto pro = std::make_shared<np_channel_register_reply<TDATA>>();
					pro->m_actorid = m_dbmodule->actorbase()->id_guid();
					log_error()->print("nsp_server.np_channel_register reply {}", nguid(recv.m_actorid));
					m_dbmodule->actor()->send_actor(recv.m_actorid, pro);
					// # ͬ����Ҫ������
					sync(recv.m_actorid);
				});

			// # �������ݱ��޸�
			actor::register_actor_s<
				EPROTOCOL_TYPE_CUSTOM, TDerived, np_channel_data<TDATA>
			>([](TDerived*, message<np_channel_data<TDATA>>& adata)
				{
					auto& recv = *adata.get_data();
					std::map<int64_t, TDATA>& lmap = *recv.m_data.m_data;
					std::set<i64_actorid> lactorset;
					for (std::pair<const int64_t, TDATA>& lpair : lmap)
					{
						// # m_dbmodule->get:���ݲ����ھʹ���
						data_modified<TDATA>* lp = m_dbmodule->get(lpair.first);
						lp->get() = lpair.second;
						lactorset.insert(lpair.first);
					}
					std::shared_ptr<np_channel_data<TDATA>> pro = adata.get_shared_data();
					publish(lactorset, pro);
				});
		}

		static void sync(i64_actorid aactor)
		{
			if (m_publishlist.empty())
				return;
			std::set<i64_actorid>* lpset = tools::findmap(m_publishlist, aactor);
			if (lpset == nullptr)
			{
				return;
			}

			auto pro = std::make_shared<np_channel_data<TDATA>>();
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

		// # ���ݼ�����ɺ�����ͬ����������
		static void loadfish_sync()
		{
			if (m_publishlist.empty())
				return;
			std::ranges::for_each(m_publishlist, [](const auto& apair)
				{
					sync(apair.first);
				});
		}

		// # ���ݱ��
		// # 1����������void publish(i64_actorid aactorid)
		// # 2��ͨ��np_channel_data<TDATA>Э���޸����ݺ���Զ�����publish�ַ�
		// # �˺���������������Ҫ��
		static void publish(i64_actorid aactorid)
		{
			data_modified<TDATA>* lp = m_dbmodule->find(aactorid);
			if (lp == nullptr)
				return;
			auto pro = std::make_shared<np_channel_data<TDATA>>();
			pro->m_data.make();
			(*pro->m_data.m_data)[aactorid] = lp->getconst();
			publish(aactorid, pro);
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
					continue;
				lmap[iactorid] = lp->getconst();
			}
			publish(aactoridset, pro);
		}
	};

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename TDATA>
	std::map<i64_actorid, std::set<i64_actorid>> nsp_server<ENUMDB, TDerived, TDATA>::m_publishlist;

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename TDATA>
	ndb_modular<ENUMDB, TDATA, TDerived>* nsp_server<ENUMDB, TDerived, TDATA>::m_dbmodule;

}//namespace ngl