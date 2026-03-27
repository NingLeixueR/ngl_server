/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Declares interfaces for nsp.

#pragma once

#include "actor/actor_base/nsp/nsp_write.h"


namespace ngl
{
	template <typename TDerived, typename TACTOR, typename T>
	nsp_write<TDerived, TACTOR, T>& nsp_write<TDerived, TACTOR, T>::instance(i64_actorid aactorid)
	{
		return *nsp_instance<nsp_write<TDerived, TACTOR, T>>::nclient(aactorid, true);
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_write<TDerived, TACTOR, T>::change(i64_actorid aactorid)
	{
		auto lp = nsp_instance<nsp_write<TDerived, TACTOR, T>>::nclient(aactorid, false);
		if (lp != nullptr)
		{
			lp->change();
		}
	}

	template <typename TDerived, typename TACTOR, typename T>
	nsp_write<TDerived, TACTOR, T>& nsp_write<TDerived, TACTOR, T>::instance_writeall(
		TDerived* aactor, const std::set<i32_fieldnumber>& areadfieldnumbers, const std::set<i32_fieldnumber>& awritefieldnumbers
	)
	{
		auto lpwrite = std::make_shared<nsp_write<TDerived, TACTOR, T>>();
		lpwrite->m_actor = aactor;
		nsp_instance<nsp_write<TDerived, TACTOR, T>>::init(aactor->id_guid(), lpwrite);

		lpwrite->m_operator_field.init(false);
		lpwrite->m_care.init(false);
		
		// Writer-side permissions include both readable fields and writable fields.
		lpwrite->m_operator_field.template add_field<T>(nguid::type(aactor->id_guid()), areadfieldnumbers, awritefieldnumbers);

		lpwrite->init();
		return *lpwrite;
	}

	template <typename TDerived, typename TACTOR, typename T>
	nsp_write<TDerived, TACTOR, T>& nsp_write<TDerived, TACTOR, T>::instance_writepart(
		TDerived* aactor
		, const std::set<i32_fieldnumber>& areadfieldnumbers
		, const std::set<i32_fieldnumber>& awritefieldnumbers
		, const std::set<i64_actorid>& areadids
		, const std::set<i64_actorid>& awriteids
	)
	{
		auto lpwrite = std::make_shared<nsp_write<TDerived, TACTOR, T>>();
		lpwrite->m_actor = aactor;
		nsp_instance<nsp_write<TDerived, TACTOR, T>>::init(aactor->id_guid(), lpwrite);

		lpwrite->m_operator_field.init(false);

		std::set<i64_actorid> lreadids;
		std::ranges::for_each(areadids, [&lreadids](i64_actorid areadid)
			{
				// Data ids are retagged with the DB owner actor type used by the NSP server.
				lreadids.insert(nsp_write<TDerived, TACTOR, T>::to_actorid(areadid));
			});
		std::set<i64_actorid> lwriteids;
		std::ranges::for_each(awriteids, [&lwriteids](i64_actorid areadid)
			{
				lwriteids.insert(nsp_write<TDerived, TACTOR, T>::to_actorid(areadid));
			});

		lpwrite->m_care.init(lreadids, lwriteids);
		
		lpwrite->m_operator_field.template add_field<T>(nguid::type(aactor->id_guid()), areadfieldnumbers, awritefieldnumbers);

		lpwrite->init();
		log_error_net()->print("nsp_write::instance_writepart( actor({}) : {} : {})", nguid(aactor->id_guid()), areadids,  awriteids);
		return *lpwrite;
	}

	template <typename TDerived, typename TACTOR, typename T>
	i64_actorid nsp_write<TDerived, TACTOR, T>::to_actorid(i64_actorid adataid)
	{
		return nguid::make_type(adataid, nactor_type<TACTOR>::type());
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_write<TDerived, TACTOR, T>::init()
	{
		// Use the registered actor enum instead of TACTOR::actorid() so NSP templates do
		// not require the DB owner actor to be a complete type at every instantiation site.
		m_regload.init(nguid::make_self(nactor_type<TACTOR>::type()));

			m_regload.foreach_nspser([this](i16_area, i64_actorid aactorid)
				{
					m_exit.insert(aactorid);
				}
			);

		if (m_isregister.exchange(false))
		{
			// One registration per concrete writer type is enough because instances are
			// keyed by actor id in nsp_instance<>.
			nsp_instance<nsp_write<TDerived, TACTOR, T>>::template register_handle<
				TDerived
				, np_channel_data<T>					// Data
				, np_channel_check<T>					// Whethertonspserverregister
				, np_channel_register_reply<T>			// Handleregisterresponse
				, np_channel_dataid_sync<T>				// Nodeinfo
				, np_channel_exit<T>					// Nodeexit
			>();		
		}

		i64_actorid lactorid = m_actor->id_guid();
			m_regload.foreach_nspser([lactorid](i16_area aarea, i64_actorid)
				{
					tools::wheel_parm lparm
					{
					.m_ms = 1 * tools::time::MILLISECOND,
					.m_intervalms = [](int64_t) {return 10 * tools::time::MILLISECOND; } ,
					.m_count = 0x7fffffff,
					.m_fun = [aarea, lactorid](const tools::wheel_node* anode)
					{
						// Periodically retry registration until the target NSP server replies.
						auto pro = std::make_shared<np_channel_check<T>>(
							np_channel_check<T>{
								.m_timer = anode->m_timerid,
								.m_area = aarea,
							});
						nsp_handle_print<TDerived>::template msg_info<TACTOR>(*pro);
						actor::send_actor(lactorid, nguid::make(), pro);
					}
				};
				tools::twheel::wheel().addtimer(lparm);
			}
		);
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_write<TDerived, TACTOR, T>::set_changedatafun(const std::function<void(int64_t, const T&, bool)>& afun)
	{
		m_call.set_changedatafun(afun);
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_write<TDerived, TACTOR, T>::set_deldatafun(const std::function<void(int64_t)>& afun)
	{
		m_call.set_deldatafun(afun);
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_write<TDerived, TACTOR, T>::set_loadfinishfun(const std::function<void()>& afun)
	{
		m_call.set_loadfinishfun(afun);
	}

	template <typename TDerived, typename TACTOR, typename T>
	T* nsp_write<TDerived, TACTOR, T>::add(i64_dataid adataid)
	{
		i64_actorid ldataid = to_actorid(adataid);
		m_changeids.insert(ldataid);
		T* lpdata = &m_data[ldataid];
		lpdata->set_mid(ldataid);
		return lpdata;
	}

	template <typename TDerived, typename TACTOR, typename T>
	T* nsp_write<TDerived, TACTOR, T>::get(i64_dataid adataid)
	{
		i64_actorid ldataid = to_actorid(adataid);
		auto lpdata = tools::findmap(m_data, ldataid);
		if (lpdata != nullptr)
		{
			m_changeids.insert(ldataid);
		}		
		return lpdata;
	}

	template <typename TDerived, typename TACTOR, typename T>
	const T* nsp_write<TDerived, TACTOR, T>::getconst(i64_dataid adataid)
	{
		return tools::findmap(m_data, to_actorid(adataid));
	}

	template <typename TDerived, typename TACTOR, typename T>
	const std::map<i64_actorid, T>& nsp_write<TDerived, TACTOR, T>::get_mapconst()
	{
		return m_data;
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_write<TDerived, TACTOR, T>::change()
	{
		if (!m_changeids.empty())
		{
			const i16_actortype ltype = nguid::type(m_actor->id_guid());
			{
				// First send a combined packet to peers that subscribe to all rows.
				auto pro = std::make_shared<np_channel_data<T>>();
				pro->m_actorid = m_actor->id_guid();
				std::set<i64_nodeid> lnodes;
				std::ranges::for_each(m_nodereadalls, [&lnodes](i64_nodeid aid) 
					{ 
						lnodes.insert(aid); 
					}
				);
				std::ranges::for_each(m_nodewritealls, [&lnodes](i64_nodeid aid) 
					{ 
						lnodes.insert(aid);
					}
				);
				m_regload.foreach_nspser([&lnodes](i16_area, i64_actorid aactorid)
					{
						lnodes.insert(aactorid);
					}
				);
				pro->m_firstsynchronize = false;
				pro->m_recvfinish = true;
				for (const i64_dataid ldataid : m_changeids)
				{
					const T* lpdata = tools::findmap(m_data, ldataid);
					if (lpdata == nullptr)
					{
						continue;
					}
					// Field filtering happens here so each peer sees only allowed columns.
					T& ldst = pro->m_data[ldataid];
					m_operator_field.field_copy(ltype, *lpdata, ldst, true);
				}

				for (const i64_dataid ldataid : m_delids)
				{
					pro->m_deldata.emplace_back(ldataid);
				}

				nsp_handle_print<TDerived>::template msg_info<TACTOR>(*pro);
				lnodes.erase(m_actor->id_guid());
				actor::send_actor(lnodes, nguid::make(), pro);
			}

			{
				// Then send per-row packets to peers that subscribe only to specific rows.
				for (const i64_dataid ldataid : m_changeids)
				{
					T* lpdata = tools::findmap(m_data, ldataid);
					if (lpdata == nullptr)
					{
						continue;
					}
					std::set<i64_nodeid> lnodes;
					auto pro = std::make_shared<np_channel_data<T>>();
					pro->m_actorid = m_actor->id_guid();
					pro->m_firstsynchronize = false;
					pro->m_recvfinish = true;
					T& ldst = pro->m_data[ldataid];
					m_operator_field.field_copy(ltype, *lpdata, ldst, true);
					for (const auto& [lnodeid, lcare] : m_othercare)
					{
						if (lcare.is_care(ldataid))
						{
							lnodes.insert(lnodeid);
						}
					}
					lnodes.erase(m_actor->id_guid());
					actor::send_actor(lnodes, nguid::make(), pro);
				}

				for (const i64_dataid ldataid : m_delids)
				{
					std::set<i64_nodeid> lnodes;
					auto pro = std::make_shared<np_channel_data<T>>();
					pro->m_actorid = m_actor->id_guid();
					pro->m_firstsynchronize = false;
					pro->m_recvfinish = true;
					pro->m_deldata.emplace_back(ldataid);
					for (const auto& [lnodeid, lcare] : m_othercare)
					{
						if (lcare.is_care(ldataid))
						{
							lnodes.insert(lnodeid);
						}
					}
					lnodes.erase(m_actor->id_guid());
					actor::send_actor(lnodes, nguid::make(), pro);
				}
			}
			m_changeids.clear();
			m_delids.clear();
		}
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_write<TDerived, TACTOR, T>::exit()
	{
		{
			// Notify node, node
			auto pro = std::make_shared<np_channel_dataid_sync<T>>();
			pro->m_actorid = m_actor->id_guid();
			pro->m_read = false;
			pro->m_all = false;
			if (!pro->m_all)
			{
				pro->m_readpart = m_care.readids();
				pro->m_writepart = m_care.writeids();
			}
			// Sendto node
			std::set<i64_nodeid> lnodes;
			std::ranges::copy(m_othercare | std::views::keys, std::inserter(lnodes, lnodes.begin()));
			lnodes.insert(m_nodewritealls.begin(), m_nodewritealls.end());
			m_regload.foreach_nspser([&lnodes](i16_area, i64_actorid aactorid)
				{
					lnodes.insert(aactorid);
				}
			);
			actor::send_actor(lnodes, nguid::make(), pro);
		}

		{
			auto pro = std::make_shared<np_channel_exit<T>>();
			pro->m_actorid = m_actor->id_guid();
			actor::send_actor(m_exit, nguid::make(), pro);
		}
		nsp_instance<nsp_write<TDerived, TACTOR, T>>::exit(m_actor->id_guid());
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_write<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_data<T>>& adata)
	{
		const np_channel_data<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_write", aactor, recv);

		bool lfirstsynchronize = recv->m_firstsynchronize;
		const i16_actortype ltypesource = nguid::type(recv->m_actorid);
		const i16_actortype ltypetarget = nguid::type(m_actor->id_guid());
		bool lchanged = false;
		for (const auto& [lguid, lsrc] : recv->m_data)
		{
			if (m_care.is_care(lguid))
			{
				lchanged = true;
				T& ldst = m_data[lguid];
				if (lfirstsynchronize)
				{
					// The first sync is already aligned to the writer's field layout.
					m_operator_field.field_copy(ltypetarget, lsrc, ldst, true);
				}
				else
				{
					// Incremental updates may come from another node type, so remap fields.
					m_operator_field.field_copy(ltypesource, ltypetarget, lsrc, ldst, true);
				}
				m_call.changedatafun(lguid, ldst, lfirstsynchronize);
			}
		}

		for (const int64_t ldataid : recv->m_deldata)
		{
			if (m_care.is_care(ldataid))
			{
				if (m_actor->nscript_using())
				{
					m_actor->template nscript_data_del<T>(ldataid);
				}
				m_data.erase(ldataid);
				m_call.deldatafun(ldataid);
			}
		}

		if (lfirstsynchronize)
		{
			if (recv->m_recvfinish)
			{
				// Each area streams independently; load completion fires after all areas finish.
				m_regload.set_loadfinish(nguid::area(recv->m_actorid));
				if (m_regload.is_loadfinish())
				{
					if (m_actor->nscript_using())
					{
						nscript_data_nsp<T> ltemp(m_data);
						m_actor->nscript_data_push("nsp", ltemp, true);
					}
					m_call.loadfinishfun();
				}
			}
		}
		else
		{
			if (m_actor->nscript_using())
			{
				if (lchanged)
				{
					nscript_data_nsp<T> ltemp(m_data);
					m_actor->nscript_data_push("nsp", ltemp, true);
				}
			}
		}
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_write<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_check<T>>& adata)
	{
		const np_channel_check<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_write", aactor, recv);

		if (m_regload.is_register(recv->m_area))
		{
			tools::twheel::wheel().removetimer(recv->m_timer);
			return;
		}
		auto pro = std::make_shared<np_channel_register<T>>();
		pro->m_actorid = m_actor->id_guid();
		pro->m_read = false;
		pro->m_all = m_care.is_readall();
		if (!pro->m_all)
		{
			pro->m_readids = m_care.readids();
			pro->m_writeids = m_care.writeids();
		}
		std::map<i32_fieldnumber, epb_field>* lmapfieldtype = m_operator_field.get_field(nguid::type(pro->m_actorid));
		if (lmapfieldtype == nullptr)
		{
			tools::no_core_dump();
			return;
		}
		pro->m_field = *lmapfieldtype;

		i64_actorid lnspserid = m_regload.nspserid(recv->m_area);
		// Registration tells the NSP server which rows and which protobuf fields we want.
		nsp_handle_print<TDerived>::template msg_info<TACTOR>(*pro);
		actor::send_actor(lnspserid, nguid::make(), pro);
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_write<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_register_reply<T>>& adata)
	{
		const np_channel_register_reply<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_write", aactor, recv);

		m_regload.set_register(nguid::area(recv->m_actorid));
		// The reply carries other peers' subscription scopes so change() can target them.
		m_operator_field.set_field(recv->m_node_fieldnumbers);

		m_nodereadalls = recv->m_nodereadalls;
		m_nodewritealls = recv->m_nodewritealls;

		m_exit.insert(recv->m_nodewritealls.begin(), recv->m_nodewritealls.end());

		for (const auto& [lnodeid, lcare] : recv->m_care)
		{
			m_exit.insert(lnodeid);
			m_othercare[lnodeid].init(lcare);
		}
		return;
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_write<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_dataid_sync<T>>& adata)
	{
		const np_channel_dataid_sync<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_write", aactor, recv);
	
		i16_actortype ltype = nguid::type(recv->m_actorid);
		m_operator_field.set_field(ltype, recv->m_field);

		if (recv->m_read)
		{
			if (recv->m_all)
			{
				m_othercare[recv->m_actorid].init(true);
			}
			else
			{
				m_othercare[recv->m_actorid].init(recv->m_readpart);
			}
		}
		else
		{
			if (recv->m_all)
			{
				m_othercare[recv->m_actorid].init(false);
			}
			else
			{
				m_othercare[recv->m_actorid].init(recv->m_readpart, recv->m_writepart);
			}
			m_exit.insert(recv->m_actorid);
		}
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_write<TDerived, TACTOR, T>::handle(TDerived*, const message<np_channel_exit<T>>& adata)
	{
		const np_channel_exit<T>* recv = adata.get_data();
		i64_actorid lactorid = recv->m_actorid;

		// Once a peer exits, incremental broadcasts should stop targeting it.
		m_nodewritealls.erase(lactorid);
		m_nodereadalls.erase(lactorid);
		m_othercare.erase(lactorid);
	}
}//namespace ngl
