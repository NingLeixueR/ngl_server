#pragma once

#include "ndb_modular.h"
#include <algorithm>
#include <ranges>

namespace ngl
{
	//# ����/����[���ݸ���]
	//# nsp_server����ַ�����,�����ݷ����仯
	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename TDATA>
	class nsp_server
	{
	public:
		using tnsp_server = nsp_server<ENUMDB, TDerived, TDATA>;
	private:
		nsp_server() = delete;
		nsp_server(const nsp_server&) = delete;
		nsp_server& operator=(const nsp_server&) = delete;

		// # �����б�	key/value(��Ҫͬ����˭/ͬ����Щ����:nguid::make()Ϊȫ��)
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
				auto itor = std::ranges::find_if(aactoridset, [&apair](i64_actorid aactorid)->bool
					{
						return apair.second.contains(aactorid);
					});
				if (itor == aactoridset.end())
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

		static void addremove_channel_dataid_sync(bool aadd, int64_t aacountid)
		{
			auto itor = m_publishlist.find(aacountid);
			if (itor == m_publishlist.end())
			{
				return;
			}
			auto pro = std::make_shared<np_channel_dataid_sync<TDATA>>();
			pro->m_actorid = m_dbmodule->actorbase()->id_guid();
			if (!aadd)
			{
				pro->m_deleteactorid = aacountid;
			}
			else
			{
				pro->m_adddataids = itor->second;
			}

			std::vector<int64_t> lvec;
			std::ranges::for_each(m_publishlist, [&pro, aacountid, &lvec](const auto& apair)
				{
					if (apair.first == aacountid)
					{
						return;
					}
					if (apair.second.empty())
					{
						lvec.push_back(apair.first);
					}
					else
					{
						std::set<int64_t> intersection;
						std::ranges::set_intersection(apair.second, pro->m_adddataids, std::inserter(intersection, intersection.begin()));
						if (!intersection.empty())
						{
							lvec.push_back(apair.first);
						}
					}
				});
			actor::static_send_actor(lvec, nguid::make(), pro);
			if (!aadd)
			{
				m_publishlist.erase(aacountid);
			}
			else
			{
				auto pro = std::make_shared<np_channel_register_reply<TDATA>>();
				pro->m_actorid = m_dbmodule->actorbase()->id_guid();
				pro->m_publishlist = m_publishlist;
				//log_error()->print("nsp_server.np_channel_register reply {}", nguid(recv.m_actorid));
				actor::static_send_actor(aacountid, nguid::make(), pro);
			}
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
			addremove_channel_dataid_sync(true, recv.m_actorid);

			// # ͬ����Ҫ������
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

			log_error()->print("nsp_server.np_channel_exit {}", nguid(recv.m_actorid));

			addremove_channel_dataid_sync(false, recv.m_actorid);
		}

		static void channel_data(TDerived*, message<np_channel_data<TDATA>>& adata)
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
			//std::shared_ptr<np_channel_data<TDATA>> pro = adata.get_shared_data();
			//publish(lactorset, false, pro);
		}

	public:
		static void init(ndb_modular<ENUMDB, TDATA, TDerived>* adbmodule)
		{
			m_dbmodule = adbmodule;
			// # ����ע�ᴦ��
			actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, np_channel_register<TDATA>>(std::bind_front(&tnsp_server::channel_register));

			// # �������ݱ��޸�
			actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, np_channel_data<TDATA>>(std::bind_front(&tnsp_server::channel_data));

			// # �˳�����
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

		// # ���ݼ�����ɺ�����ͬ����������
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

		// # ���ݱ��
		// # 1����������void publish(i64_actorid aactorid)
		// # 2��ͨ��np_channel_data<TDATA>Э���޸����ݺ���Զ�����publish�ַ�
		// # �˺���������������Ҫ��
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