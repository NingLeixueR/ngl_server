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


		static std::set<i64_actorid>							m_onlyreads;								// 只读全部数据
		static std::set<i64_actorid>							m_writealls;								// 读/写全部数据
		// m_publishlist_write.first:<结点id>
		// m_publishlist_write.second:<读写的数据id列表>
		static std::map<i64_actorid, std::set<i64_actorid>>		m_publishlist;

		static ndb_modular<ENUMDB, TDATA, TDerived>*			m_dbmodule;

		static void channel_register(TDerived*, message<np_channel_register<TDATA>>& adata)
		{
			if (m_dbmodule->actorbase() == nullptr)
			{
				log_error()->print("nsp_server::channel_register fail m_dbmodule->actorbase() == nullptr");
				return;
			}
			const np_channel_register<TDATA>* recv = adata.get_data();

			i64_actorid lactorid = recv->m_actorid;
			bool lonlyread = recv->m_onlyread;
			const std::set<i64_actorid>& ldataid = recv->m_dataid;

			if (m_onlyreads.contains(lactorid))
			{
				return;
			}

			if (m_writealls.contains(lactorid))
			{
				return;
			}

			if (m_publishlist.contains(lactorid))
			{
				return;
			}

			if (lonlyread)
			{
				m_onlyreads.insert(lactorid);
			}
			else
			{
				if (ldataid.empty())
				{
					m_writealls.insert(lactorid);
				}
				else
				{
					m_publishlist[lactorid] = ldataid;
				}
			}

			{
				auto pro = std::make_shared<np_channel_dataid_sync<TDATA>>();
				pro->m_actorid = lactorid;
				pro->m_add = true;
				pro->m_onlyread = lonlyread;
				pro->m_dataid = ldataid;

				std::set<i64_actorid> lset;
				// 发送给 m_writeall
				lset.insert(m_writealls.begin(), m_writealls.end());
				
				if (lonlyread || ldataid.empty())
				{
					for (const auto& item : m_publishlist)
					{
						lset.insert(item.first);
					}
				}
				else
				{
					for (const auto& item : m_publishlist)
					{
						if (std::ranges::find_if(item.second, [&ldataid](i64_actorid dataid)
							{
								return ldataid.contains(dataid);
							}) != item.second.end())
						{
							lset.insert(item.first);
						}
					}
				}
				lset.erase(lactorid);
				actor::static_masssend_actor(lset, nguid::make(), pro);
			}
			

			{//回复
				auto pro = std::make_shared<np_channel_register_reply<TDATA>>();
				pro->m_actorid = lactorid;
				pro->m_onlyreads = m_onlyreads;
				pro->m_writealls = m_writealls;
				pro->m_publishlist = m_publishlist;
				actor::static_send_actor(lactorid, nguid::make(), pro);
			}

			{
				auto pro = std::make_shared<np_channel_data<TDATA>>();
				pro->m_data.make();
				//ndb_modular<ENUMDB, TDATA, TDerived>*
				std::map<nguid, data_modified<TDATA>>& ldata = m_dbmodule->data();
				if (lonlyread || ldataid.empty())
				{
					for (const std::pair<const nguid, data_modified<TDATA>>& itempair : ldata)
					{
						pro->m_firstsynchronize = true;
						std::map<int64_t, TDATA>& lmapdata = *pro->m_data.m_data;
						i64_actorid ldataid = itempair.first;
						lmapdata[itempair.first] = itempair.second.getconst();
					}
				}
				else
				{
					for (i64_actorid dataid : ldataid)
					{
						auto itor = ldata.find(dataid);
						if (itor != ldata.end())
						{
							pro->m_firstsynchronize = true;
							(*pro->m_data.m_data)[dataid] = itor->second.getconst();
						}
					}
				}
				actor::static_send_actor(lactorid, nguid::make(), pro);
			}
		}

		static void channel_exit(TDerived*, message<np_channel_exit<TDATA>>& adata)
		{
			if (m_dbmodule->actorbase() == nullptr)
			{
				log_error()->print("nsp_server::channel_exit fail m_dbmodule->actorbase() == nullptr");
				return;
			}
			const np_channel_exit<TDATA>* recv = adata.get_data();

			log_error()->print("nsp_server.np_channel_exit {}", nguid(recv->m_actorid));

			auto pro = std::make_shared<np_channel_dataid_sync<TDATA>>();
			pro->m_actorid = recv->m_actorid;
			pro->m_add = false;

			// 移除的结点性质
			// 1、读/写全部数据  => 给全部[m_writealls + 全部part] 发送
			bool lwriteall = m_writealls.contains(recv->m_actorid);
			// 2、读全部数据 => 给全部[m_writealls + 全部part] 发送
			bool lreadeall = m_onlyreads.contains(recv->m_actorid);
			// 3、读写部分数据 => 给全部[m_writealls + part中相交的] 发送
			//auto itor = m_publishlist.find(recv->m_actorid);

			m_writealls.erase(recv->m_actorid);
			m_onlyreads.erase(recv->m_actorid);

			std::set<i64_actorid> lset;
			lset.insert(m_writealls.begin(), m_writealls.end());
			if (lwriteall || lreadeall)
			{
				for (const auto& item : m_publishlist)
				{
					lset.insert(item.first);
				}
			}
			else
			{
				auto itor = m_publishlist.find(recv->m_actorid);
				if (itor != m_publishlist.end())
				{
					for (const auto& item : m_publishlist)
					{
						if (item.first == recv->m_actorid)
						{
							continue;
						}
						std::set<i64_actorid> lvvset;
						std::ranges::set_intersection(itor->second, item.second, std::inserter(lvvset, lvvset.end()));
						if (!lvvset.empty())
						{
							lset.insert(item.first);
						}
					}
				}
			}

			m_publishlist.erase(recv->m_actorid);
			actor::static_send_actor(lset, nguid::make(), pro);
		}

		static void channel_data(TDerived*, message<np_channel_data<TDATA>>& adata)
		{
			auto& recv = *adata.get_data();
			std::map<int64_t, TDATA>& lmap = *recv.m_data.m_data;
			for (std::pair<const int64_t, TDATA>& lpair : lmap)
			{
				// # m_dbmodule->get:数据不存在就创建
				data_modified<TDATA>& ldata = m_dbmodule->get(lpair.first);
				ldata.get() = lpair.second;
			}
		}
	public:
		static void init(ndb_modular<ENUMDB, TDATA, TDerived>* adbmodule)
		{
			m_dbmodule = adbmodule;
			// # 订阅注册处理
			actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, false, np_channel_register<TDATA>>(std::bind_front(&tnsp_server::channel_register));

			// # 订阅数据被修改
			auto lchannel_data = std::bind_front(&tnsp_server::channel_data);
			actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, true, np_channel_data<TDATA>>(lchannel_data);
			actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, false, np_channel_data<TDATA>>(lchannel_data);

			// # 退出订阅
			actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, false, np_channel_exit<TDATA>>(std::bind_front(&tnsp_server::channel_exit));
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
					data_modified<TDATA>& ldata = m_dbmodule->get(actorid);
					lmap[ldata.getconst().m_id()] = ldata.getconst();
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
	};

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename TDATA>
	std::map<i64_actorid, std::set<i64_actorid>> nsp_server<ENUMDB, TDerived, TDATA>::m_publishlist;

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename TDATA>
	std::set<i64_actorid> nsp_server<ENUMDB, TDerived, TDATA>::m_onlyreads;

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename TDATA>
	std::set<i64_actorid> nsp_server<ENUMDB, TDerived, TDATA>::m_writealls;

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename TDATA>
	ndb_modular<ENUMDB, TDATA, TDerived>* nsp_server<ENUMDB, TDerived, TDATA>::m_dbmodule;

}//namespace ngl