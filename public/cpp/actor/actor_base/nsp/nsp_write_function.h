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
#include "nsp_write.h"


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
		
		lpwrite->m_operator_field.template add_field<T>(nguid::type(aactor->id_guid()), areadfieldnumbers, awritefieldnumbers);

		lpwrite->init();
		return *lpwrite;
	}

	template <typename TDerived, typename TACTOR, typename T>
	nsp_write<TDerived, TACTOR, T>& nsp_write<TDerived, TACTOR, T>::instance_writepart(
		TDerived* aactor, const std::set<i32_fieldnumber>& areadfieldnumbers, const std::set<i32_fieldnumber>& awritefieldnumbers, const std::set<i64_actorid>& areadids, const std::set<i64_actorid>& awriteids
	)
	{
		auto lpwrite = std::make_shared<nsp_write<TDerived, TACTOR, T>>();
		lpwrite->m_actor = aactor;
		nsp_instance<nsp_write<TDerived, TACTOR, T>>::init(aactor->id_guid(), lpwrite);

		lpwrite->m_operator_field.init(false);

		std::set<i64_actorid> lreadids;
		std::ranges::for_each(areadids, [&lreadids](i64_actorid areadid)
			{
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
		log_error_net()->print("nsp_read::instance_readpart( actor({}) : {} : {})", nguid(aactor->id_guid()), areadids,  awriteids);
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
		m_regload.init(TACTOR::actorid());

		m_regload.foreach_nspser([this](i16_area aarea, i64_actorid aactorid)
			{
				m_exit.insert(aactorid);
			});

		if (m_isregister.exchange(false))
		{
			
			nsp_instance<nsp_write<TDerived, TACTOR, T>>::template register_handle<
				TDerived
				, np_channel_data<T>					// 更新数据
				, np_channel_check<T>					// 检查是否向nsp服务器注册
				, np_channel_register_reply<T>			// 处理注册回复
				, np_channel_dataid_sync<T>				// 接收结点信息
				, np_channel_exit<T>					// 结点退出				
			>();		
		}

		i64_actorid lactorid = m_actor->id_guid();
		m_regload.foreach_nspser([lactorid](i16_area aarea, i64_actorid aactorid)
			{
				wheel_parm lparm
				{
					.m_ms = 1 * localtime::MILLISECOND,
					.m_intervalms = [](int64_t) {return 10 * localtime::MILLISECOND; } ,
					.m_count = 0x7fffffff,
					.m_fun = [aarea, lactorid](const wheel_node* anode)
					{
						auto pro = std::make_shared<np_channel_check<T>>(
							np_channel_check<T>{
								.m_timer = anode->m_timerid,
								.m_area = aarea,
							});
						nsp_handle_print<TDerived>::template msg_info<TACTOR>(*pro);
						actor::send_actor(lactorid, nguid::make(), pro);
					}
				};
				twheel::wheel().addtimer(lparm);
			});
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
		auto itor = m_data.find(to_actorid(adataid));
		if (itor == m_data.end())
		{
			return nullptr;
		}
		m_changeids.insert(to_actorid(adataid));
		return &itor->second;
	}

	template <typename TDerived, typename TACTOR, typename T>
	const T* nsp_write<TDerived, TACTOR, T>::getconst(i64_dataid adataid)
	{
		auto itor = m_data.find(to_actorid(adataid));
		if (itor == m_data.end())
		{
			return nullptr;
		}
		return &itor->second;
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
			i16_actortype ltype = nguid::type(m_actor->id_guid());
			{
				auto pro = std::make_shared<np_channel_data<T>>();
				pro->m_actorid = m_actor->id_guid();
				std::set<i64_nodeid> lnodes;
				std::ranges::for_each(m_nodereadalls, [&lnodes](i64_nodeid aid) { lnodes.insert(aid); });
				std::ranges::for_each(m_nodewritealls, [&lnodes](i64_nodeid aid) { lnodes.insert(aid); });
				m_regload.foreach_nspser([&lnodes](i16_area, i64_actorid aactorid)
					{
						lnodes.insert(aactorid);
					});
				pro->m_firstsynchronize = false;
				pro->m_recvfinish = true;
				for (i64_dataid dataid : m_changeids)
				{
					if (!m_data.contains(dataid))
					{
						continue;
					}
					m_operator_field.field_copy(ltype, m_data[dataid], pro->m_data[dataid], true);
				}

				for (i64_dataid dataid : m_delids)
				{
					pro->m_deldata.push_back(dataid);
				}

				nsp_handle_print<TDerived>::template msg_info<TACTOR>(*pro);
				lnodes.erase(m_actor->id_guid());
				actor::send_actor(lnodes, nguid::make(), pro);
			}

			{
				for (i64_dataid dataid : m_changeids)
				{
					std::set<i64_nodeid> lnodes;
					auto pro = std::make_shared<np_channel_data<T>>();
					pro->m_firstsynchronize = false;
					pro->m_recvfinish = true;
					m_operator_field.field_copy(ltype, m_data[dataid], pro->m_data[dataid], true);
					for (const auto& [_nodeid, _caredata] : m_othercare)
					{
						if (_caredata.is_care(dataid))
						{
							lnodes.insert(_nodeid);
						}
					}
					lnodes.erase(m_actor->id_guid());
					actor::send_actor(lnodes, nguid::make(), pro);
				}

				for (i64_dataid dataid : m_delids)
				{
					std::set<i64_nodeid> lnodes;
					auto pro = std::make_shared<np_channel_data<T>>();
					pro->m_firstsynchronize = false;
					pro->m_recvfinish = true;
					pro->m_deldata.push_back(dataid);
					for (const auto& [_nodeid, _caredata] : m_othercare)
					{
						if (_caredata.is_care(dataid))
						{
							lnodes.insert(_nodeid);
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
			// 通知其他写结点，本结点退除
			auto pro = std::make_shared<np_channel_dataid_sync<T>>();
			pro->m_actorid = m_actor->id_guid();
			pro->m_read = false;
			pro->m_all = false;
			if (!pro->m_all)
			{
				pro->m_readpart = m_care.readids();
				pro->m_writepart = m_care.writeids();
			}
			// 发送给其他结点
			std::set<i64_nodeid> lnodes;
			std::ranges::copy(m_othercare | std::views::keys, std::inserter(lnodes, lnodes.begin()));
			lnodes.insert(m_nodewritealls.begin(), m_nodewritealls.end());
			m_regload.foreach_nspser([&lnodes](i16_area, i64_actorid aactorid)
				{
					lnodes.insert(aactorid);
				});
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
		i16_actortype ltypesource = nguid::type(recv->m_actorid);
		i16_actortype ltypetarget = nguid::type(m_actor->id_guid());
		for (const auto& apair : recv->m_data)
		{
			if (m_care.is_care(apair.first))
			{
				if (lfirstsynchronize)
				{
					m_operator_field.field_copy(ltypetarget, apair.second, m_data[apair.first], true);
				}
				else
				{
					m_operator_field.field_copy(ltypesource, ltypetarget, apair.second, m_data[apair.first], true);
				}
				m_call.changedatafun(apair.first, m_data[apair.first], lfirstsynchronize);
			}
		}

		for (int64_t dataid : recv->m_deldata)
		{
			if (m_care.is_care(dataid))
			{
				if (m_actor->nscript_using())
				{
					m_actor->template nscript_data_del<T>(dataid);
				}
				m_data.erase(dataid);
				m_call.deldatafun(dataid);
			}
		}

		if (lfirstsynchronize)
		{
			if (recv->m_recvfinish)
			{
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
				std::map<i64_actorid, T> ldata;
				for (const auto& apair : recv->m_data)
				{
					if (m_care.is_care(apair.first))
					{
						ldata[apair.first] = m_data[apair.first];
					}
				}
				if (!ldata.empty())
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
			twheel::wheel().removetimer(recv->m_timer);
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
		nsp_handle_print<TDerived>::template msg_info<TACTOR>(*pro);
		actor::send_actor(lnspserid, nguid::make(), pro);
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_write<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_register_reply<T>>& adata)
	{
		const np_channel_register_reply<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_write", aactor, recv);

		m_regload.set_register(nguid::area(recv->m_actorid));
		m_operator_field.set_field(recv->m_node_fieldnumbers);

		m_nodereadalls = recv->m_nodereadalls;
		m_nodewritealls = recv->m_nodewritealls;

		m_exit.insert(recv->m_nodewritealls.begin(), recv->m_nodewritealls.end());

		for (const auto& [_nodeid, _care] : recv->m_care)
		{
			m_exit.insert(_nodeid);
			m_othercare[_nodeid].init(_care);
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

		m_nodewritealls.erase(lactorid);
		m_nodereadalls.erase(lactorid);
		m_othercare.erase(lactorid);
	}
}//namespace ngl