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
#pragma once
#include "nsp_server.h"

namespace ngl
{
	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::init(ndb_modular<ENUMDB, T, TDerived>* adbmodule)
	{
		m_dbmodule = adbmodule;
		m_operator_field.init(false);

		// # 订阅注册处理
		actor::register_actor_s<TDerived, np_channel_register<T>>(
			e_ready_db, [](TDerived* aactor, const message<np_channel_register<T>>& adata)
			{
				nsp_server<ENUMDB, TDerived, T>::handle(aactor, adata);
			}
		);
		// # 订阅数据被修改
		actor::register_actor_s<TDerived, np_channel_data<T>>(
			e_ready_db, [](TDerived* aactor, const message<np_channel_data<T>>& adata)
			{
				nsp_server<ENUMDB, TDerived, T>::handle(aactor, adata);
			}
		);
		// # 退出订阅
		actor::register_actor_s<TDerived, np_channel_exit<T>>(
			e_ready_db, [](TDerived* aactor, const message<np_channel_exit<T>>& adata)
			{
				nsp_server<ENUMDB, TDerived, T>::handle(aactor, adata);
			}
		);
	}

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::channel_register_reply(i64_actorid aactorid)
	{
		auto pro = std::make_shared<np_channel_register_reply<T>>();
		pro->m_actorid = m_dbmodule->get_actor()->id_guid();
		pro->m_nodereadalls = m_nodereadalls;
		pro->m_nodewritealls = m_nodewritealls;
		for (const auto& [_nodeid, _care] : m_care)
		{
			if (aactorid == _nodeid)
			{
				continue;
			}
			pro->m_care[_nodeid] = _care.get_core();
		}
		pro->m_node_fieldnumbers = m_operator_field.field_numbers();
		actor::send_actor(aactorid, nguid::make(), pro);
	}

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::channel_channel_data(i64_actorid aactorid, const np_channel_register<T>* recv)
	{
		i64_actorid lnspserver = m_dbmodule->get_actor()->id_guid();
		std::function<std::shared_ptr<np_channel_data<T>>()> lmalloc = [lnspserver]()->std::shared_ptr<np_channel_data<T>>
			{
				auto pro = std::make_shared<np_channel_data<T>>();
				pro->m_actorid = lnspserver;
				pro->m_firstsynchronize = true;
				return pro;
			};
		auto pro = lmalloc();

		int32_t lindex = 0;
		if (recv->m_all)
		{
			m_dbmodule->foreach([&pro, &lindex, &lmalloc, aactorid](const data_modified<T>& adata)
				{
					const T& ldata = *adata.getconst();
					pro->m_data[ldata.mid()] = ldata;
					++lindex;
					if (lindex % esend_maxcount == 0)
					{
						actor::send_actor(aactorid, nguid::make(), pro);
						pro = lmalloc();
					}
				});
		}
		else
		{
			auto lfun = [&pro, &lindex, &lmalloc, aactorid](const std::set<i64_actorid>& aids)
				{
					for (i64_actorid id : aids)
					{
						data_modified<T>* lpmodifieddata = m_dbmodule->find(id);
						if (lpmodifieddata == nullptr)
						{
							continue;
						}
						const T* lpdata = lpmodifieddata->getconst();
						if (lpdata == nullptr)
						{
							continue;
						}
						pro->m_data[lpdata->mid()] = *lpdata;
						++lindex;
						if (lindex % esend_maxcount == 0)
						{
							actor::send_actor(aactorid, nguid::make(), pro);
							pro = lmalloc();
						}
					}
				};

			lfun(recv->m_readids);
			lfun(recv->m_writeids);
		}

		if (lindex % esend_maxcount == 0)
		{
			pro = lmalloc();
		}
		pro->m_recvfinish = true;
		actor::send_actor(aactorid, nguid::make(), pro);
	}

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::channel_dataid_sync(i64_actorid aactorid, const np_channel_register<T>* recv)
	{
		auto pro = std::make_shared<np_channel_dataid_sync<T>>();
		pro->m_actorid = aactorid;
		pro->m_read = recv->m_read;
		pro->m_all = recv->m_all;
		if (!recv->m_all)
		{
			pro->m_readpart = recv->m_readids;
			pro->m_writepart = recv->m_writeids;
		}
		pro->m_field = recv->m_field;

		std::set<i64_nodeid> lnodes;
		std::ranges::for_each(m_nodereadalls, [&lnodes](i64_nodeid aid) { lnodes.insert(aid); });
		std::ranges::for_each(m_nodewritealls, [&lnodes](i64_nodeid aid) { lnodes.insert(aid); });
		actor::send_actor(lnodes, nguid::make(), pro);
	}

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::handle(TDerived*, const message<np_channel_register<T>>& adata)
	{
		const np_channel_register<T>* recv = adata.get_data();
		i64_actorid lactorid = recv->m_actorid;

		if (m_care.contains(lactorid))
		{
			return;
		}

		if (m_nodewritealls.contains(lactorid))
		{
			return;
		}

		if (m_nodereadalls.contains(lactorid))
		{
			return;
		}

		i16_actortype ltype = nguid::type(lactorid);
		if (recv->m_read)
		{
			if (recv->m_all)
			{
				care_data& lcare = m_care[lactorid];
				lcare.init(true);
				m_nodereadalls.insert(lactorid);
			}
			else
			{
				care_data& lcare = m_care[lactorid];
				lcare.init(recv->m_readids);
			}
		}
		else
		{
			if (recv->m_all)
			{
				care_data& lcare = m_care[lactorid];
				lcare.init(false);
				m_nodereadalls.insert(lactorid);
			}
			else
			{
				care_data& lcare = m_care[lactorid];
				lcare.init(recv->m_readids, recv->m_writeids);
			}
		}
		
		m_operator_field.set_field(ltype, recv->m_field);
		m_nodepart.insert(lactorid);

		channel_register_reply(lactorid);
		channel_channel_data(lactorid, recv);
		channel_dataid_sync(lactorid, recv);
	}

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::handle(TDerived*, const message<np_channel_exit<T>>& adata)
	{
		const np_channel_exit<T>* recv = adata.get_data();
		i64_actorid lactorid = recv->m_actorid;

		m_nodepart.erase(lactorid);
		m_nodewritealls.erase(lactorid);
		m_nodereadalls.erase(lactorid);
		m_care.erase(lactorid);
	}

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::handle(TDerived*, const message<np_channel_data<T>>& adata)
	{
		const np_channel_data<T>* recv = adata.get_data();
		i64_actorid lactorid = recv->m_actorid;

		nsp_handle_print<TDerived>::print("nsp_server", m_dbmodule->nactor(), recv);
		if (!m_care.contains(lactorid))
		{
			tools::no_core_dump();
			return;
		}
		care_data& lcare = m_care[lactorid];

		i16_actortype ltype = nguid::type(lactorid);
		for (const auto& [ _dataid, _data] : recv->m_data)
		{
			if (!lcare.is_write(_dataid))
			{
				tools::no_core_dump();
				return;
			}
			data_modified<T>& ldata = m_dbmodule->get(_dataid);
			m_operator_field.field_copy(ltype, _data, *ldata.get(), true);
		}
		for (i64_actorid dataid : recv->m_deldata)
		{
			if (!lcare.is_write(dataid))
			{
				tools::no_core_dump();
				return;
			}
			m_dbmodule->erase(dataid);
		}
	}
}//namespace ngl
