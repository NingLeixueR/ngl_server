#pragma once

#include "ndb_modular.h"
#include <algorithm>
#include <ranges>

namespace ngl
{
	//# ����/����[���ݸ���]
	//# nsp_server����ַ�����,�����ݷ����仯
	template <
		pbdb::ENUM_DB ENUMDB,	// ��������ö��
		typename TDerived,		// ���޵�actor
		typename TDATA			// ��������
	>
	class nsp_server
	{
	public:
		using tnsp_server = nsp_server<ENUMDB, TDerived, TDATA>;
		enum
		{
			ESEND_MAX_COUNT = 100,
		};
	private:
		nsp_server() = delete;
		nsp_server(const nsp_server&) = delete;
		nsp_server& operator=(const nsp_server&) = delete;

		static std::set<i64_actorid>							m_onlyreads;								// ֻ��ȫ������
		static std::set<i64_actorid>							m_writealls;								// ��/дȫ������
		// m_publishlist_write.first:<���id>
		// m_publishlist_write.second:<��д������id�б�>
		static std::map<i64_actorid, std::set<i64_actorid>>		m_publishlist;

		static ndb_modular<ENUMDB, TDATA, TDerived>*			m_dbmodule;

		template <typename TX>
		static void msg_info(TX& adata)
		{
			adata.m_msg = std::format("{}:{}", tools::type_name<TDerived>(), tools::type_name<TDATA>());
		}

		static void log(const char* amessage)
		{
			m_dbmodule->get_actor()->log_error()->print(
				"{}:{}"
				, tools::type_name<tnsp_server>()
				, amessage
			);
		}

		static void channel_register(TDerived*, message<np_channel_register<TDATA>>& adata)
		{
			if (m_dbmodule->actorbase() == nullptr)
			{
				log("channel_register fail");
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
				// ���͸� m_writeall
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
				msg_info(*pro);
				actor::send_actor(lset, nguid::make(), pro);
			}
			

			{//�ظ�
				auto pro = std::make_shared<np_channel_register_reply<TDATA>>();
				pro->m_actorid = lactorid;
				pro->m_onlyreads = m_onlyreads;
				pro->m_writealls = m_writealls;
				pro->m_publishlist = m_publishlist;
				msg_info(*pro);
				actor::send_actor(lactorid, nguid::make(), pro);
			}

			{
				auto pro = std::make_shared<np_channel_data<TDATA>>();
				std::map<nguid, data_modified<TDATA>>& ldata = m_dbmodule->data();
				if (lonlyread || ldataid.empty())
				{
					for (const std::pair<const nguid, data_modified<TDATA>>& itempair : ldata)
					{
						pro->m_firstsynchronize = true;
						std::map<int64_t, TDATA>& lmapdata = pro->m_data;

						data_modified_continue_getconst(lpddataconst, itempair.second);
						lmapdata[itempair.first] = *lpddataconst;
						if (lmapdata.size() >= ESEND_MAX_COUNT)
						{
							msg_info(*pro);
							actor::send_actor(lactorid, nguid::make(), pro);
							pro = std::make_shared<np_channel_data<TDATA>>();
						}
					}
					pro->m_recvfinish = true;
					msg_info(*pro);
					actor::send_actor(lactorid, nguid::make(), pro);
					return;
				}
				else
				{
					for (i64_actorid dataid : ldataid)
					{
						auto itor = ldata.find(dataid);
						if (itor != ldata.end())
						{
							pro->m_firstsynchronize = true;
							data_modified_continue_getconst(lpddataconst, itor->second);
							(pro->m_data)[dataid] = *lpddataconst;
						}
					}
				}
				msg_info(*pro);
				actor::send_actor(lactorid, nguid::make(), pro);
			}
		}

		static void channel_exit(TDerived*, message<np_channel_exit<TDATA>>& adata)
		{
			if (m_dbmodule->actorbase() == nullptr)
			{
				log("channel_exit fail");
				return;
			}
			const np_channel_exit<TDATA>* recv = adata.get_data();

			log(std::format("np_channel_exit {}", nguid(recv->m_actorid)).c_str());

			auto pro = std::make_shared<np_channel_dataid_sync<TDATA>>();
			pro->m_actorid = recv->m_actorid;
			pro->m_add = false;

			// �Ƴ��Ľ������
			// 1����/дȫ������  => ��ȫ��[m_writealls + ȫ��part] ����
			bool lwriteall = m_writealls.contains(recv->m_actorid);
			// 2����ȫ������ => ��ȫ��[m_writealls + ȫ��part] ����
			bool lreadeall = m_onlyreads.contains(recv->m_actorid);
			// 3����д�������� => ��ȫ��[m_writealls + part���ཻ��] ����
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
			msg_info(*pro);
			actor::send_actor(lset, nguid::make(), pro);
		}

		static void channel_data(TDerived*, message<np_channel_data<TDATA>>& adata)
		{
			const np_channel_data<TDATA>* recv = adata.get_data();
			const std::map<int64_t, TDATA>& lmap = recv->m_data;
			for (const auto& lpair : lmap)
			{
				// # m_dbmodule->get:���ݲ����ھʹ���
				data_modified<TDATA>& ldata = m_dbmodule->get(lpair.first);
				data_modified_continue_get(lpddata, ldata);
				*lpddata = lpair.second;
			}
			for (int64_t dataid : recv->m_deldata)
			{
				m_dbmodule->erase(dataid);
			}
		}
	public:
		static void init(ndb_modular<ENUMDB, TDATA, TDerived>* adbmodule)
		{
			m_dbmodule = adbmodule;
			// # ����ע�ᴦ��
			actor::register_actor_s<
				TDerived, np_channel_register<TDATA>
			>(std::bind_front(&tnsp_server::channel_register), true);

			// # �������ݱ��޸�
			actor::register_actor_s<
				TDerived, np_channel_data<TDATA>
			>(std::bind_front(&tnsp_server::channel_data), true);

			// # �˳�����
			actor::register_actor_s<
				TDerived, np_channel_exit<TDATA>
			>(std::bind_front(&tnsp_server::channel_exit), true);
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
			std::map<int64_t, TDATA>& lmap = pro->m_data;
			if (lpset->empty())
			{
				for (std::pair<const nguid, data_modified<TDATA>>& lpair : m_dbmodule->data())
				{
					data_modified_continue_getconst(lpddataconst, lpair.second);
					lmap[lpair.first] = *lpddataconst;
				}
			}
			else
			{
				for (i64_actorid actorid : *lpset)
				{
					data_modified<TDATA>& ldata = m_dbmodule->get(actorid);
					data_modified_continue_getconst(lpddataconst, ldata);
					lmap[lpddataconst->mid()] = *lpddataconst;
				}
			}
			if (!lmap.empty())
			{
				msg_info(*pro);
				actor::send_actor(aactor, nguid::make(), pro);
			}
		}

		// # ���ݼ�����ɺ�����ͬ����������
		static void loadfish_sync()
		{
			if (m_publishlist.empty())
			{
				return;
			}
			for (const auto& apair : m_publishlist)
			{
				sync(apair.first);
			}
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