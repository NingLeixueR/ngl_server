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
#include "nsp_read.h"

namespace ngl
{
	template <typename TDerived, typename TACTOR, typename T>
	nsp_read<TDerived, TACTOR, T>& nsp_read<TDerived, TACTOR, T>::instance(i64_actorid aactorid)
	{
		return *nsp_instance<nsp_read<TDerived, TACTOR, T>>::nclient(aactorid, true);
	}

	template <typename TDerived, typename TACTOR, typename T>
	nsp_read<TDerived, TACTOR, T>& nsp_read<TDerived, TACTOR, T>::instance_readall(TDerived* aactor, const std::set<i32_fieldnumber>& afieldnumbers)
	{
		auto lpread = std::make_shared<nsp_read<TDerived, TACTOR, T>>();
		lpread->m_actor = aactor;
		nsp_instance<nsp_read<TDerived, TACTOR, T>>::init(aactor->id_guid(), lpread);

		lpread->m_operator_field.init(false);
		lpread->m_care.init(true);
		
		lpread->m_operator_field.template add_field<T>(nguid::type(aactor->id_guid()), epb_field_read, afieldnumbers);

		lpread->init();
		log_error_net()->print("nsp_read::instance_readall( actor({}) )", nguid(aactor->id_guid()));
		return *lpread;
	}

	template <typename TDerived, typename TACTOR, typename T>
	nsp_read<TDerived, TACTOR, T>& nsp_read<TDerived, TACTOR, T>::instance_readpart(TDerived* aactor, const std::set<i32_fieldnumber>& afieldnumbers, const std::set<i64_actorid>& aids)
	{
		auto lpread = std::make_shared<nsp_read<TDerived, TACTOR, T>>();
		lpread->m_actor = aactor;
		nsp_instance<nsp_read<TDerived, TACTOR, T>>::init(aactor->id_guid(), lpread);

		lpread->m_operator_field.init(false);

		std::set<i64_actorid> lids;
		std::ranges::for_each(aids, [&lids](i64_actorid areadid)
			{
				lids.insert(nsp_write<TDerived, TACTOR, T>::to_actorid(areadid));
			});
		lpread->m_care.init(lids);

		lpread->m_operator_field.template add_field<T>(nguid::type(aactor->id_guid()), epb_field_read, afieldnumbers);

		lpread->init();
		log_error_net()->print("nsp_read::instance_readpart( actor({}) : {} )", nguid(aactor->id_guid()), aids);
		return *lpread;
	}

	template <typename TDerived, typename TACTOR, typename T>
	i64_actorid nsp_read<TDerived, TACTOR, T>::to_actorid(i64_actorid adataid)
	{
		return nguid::make_type(adataid, nactor_type<TACTOR>::type());
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_read<TDerived, TACTOR, T>::set_changedatafun(const std::function<void(int64_t, const T&, bool)>& afun)
	{
		m_call.set_changedatafun(afun);
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_read<TDerived, TACTOR, T>::set_deldatafun(const std::function<void(int64_t)>& afun)
	{
		m_call.set_deldatafun(afun);
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_read<TDerived, TACTOR, T>::set_loadfinishfun(const std::function<void()>& afun)
	{
		m_call.set_loadfinishfun(afun);
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_read<TDerived, TACTOR, T>::init()
	{
		m_regload.init(TACTOR::actorid());
		m_regload.foreach_nspser([this](i16_area aarea, i64_actorid aactorid)
			{
				m_exit.insert(aactorid);
			});

		if (m_isregister.exchange(false))
		{
			// 更新数据
			nsp_instance<nsp_read<TDerived, TACTOR, T>>::template register_handle<TDerived, np_channel_data<T>>();
			// 检查是否向nsp服务器注册
			nsp_instance<nsp_read<TDerived, TACTOR, T>>::template register_handle<TDerived, np_channel_check<T>>();
			// 处理注册回复
			nsp_instance<nsp_read<TDerived, TACTOR, T>>::template register_handle<TDerived, np_channel_register_reply<T>>();
			// 接收结点信息
			nsp_instance<nsp_read<TDerived, TACTOR, T>>::template register_handle<TDerived, np_channel_dataid_sync<T>>();
		}
		i64_actorid lactorid = m_actor->id_guid();
		m_regload.foreach_nspser([lactorid](i16_area aarea, i64_actorid aactorid)
			{
				wheel_parm lparm
				{
					.m_ms = 1000,
					.m_intervalms = [](int64_t) {return 10000; } ,
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
	const T* nsp_read<TDerived, TACTOR, T>::getconst(i64_dataid adataid)
	{
		auto itor = m_data.find(to_actorid(adataid));
		if (itor == m_data.end())
		{
			return nullptr;
		}
		return &itor->second;
	}

	template <typename TDerived, typename TACTOR, typename T>
	const std::map<i64_actorid, T>& nsp_read<TDerived, TACTOR, T>::get_mapconst()
	{
		return m_data;
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_read<TDerived, TACTOR, T>::exit()
	{
		auto pro = std::make_shared<np_channel_exit<T>>();
		pro->m_actorid = m_actor->id_guid();
		actor::send_actor(m_exit, nguid::make(), pro);
		nsp_instance<nsp_read<TDerived, TACTOR, T>>::exit(m_actor->id_guid());
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_read<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_data<T>>& adata)
	{
		const np_channel_data<T>* recv = adata.get_data();

		//nsp_handle_print<TDerived>::print("nsp_read", aactor, recv);

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
	void nsp_read<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_check<T>>& adata)
	{
		const np_channel_check<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_read", aactor, recv);

		if (m_regload.is_register(recv->m_area))
		{
			twheel::wheel().removetimer(recv->m_timer);
			return;
		}
		auto pro = std::make_shared<np_channel_register<T>>();
		pro->m_actorid = m_actor->id_guid();
		pro->m_read = true;
		pro->m_all = m_care.is_readall();
		if (!pro->m_all)
		{
			pro->m_readids = m_care.readids();
		}
		std::map<i32_fieldnumber, epb_field>* lmapfieldtype = m_operator_field.get_field(nguid::type(m_actor->id_guid()));
		if (lmapfieldtype == nullptr)
		{
			tools::no_core_dump();
			return;
		}
		pro->m_field = *lmapfieldtype;

		i64_actorid lnspserid = m_regload.nspserid(recv->m_area);
		log_error()->print("nsp_read register: {} -> {}", nguid(pro->m_actorid), nguid(lnspserid));
		nsp_handle_print<TDerived>::template msg_info<TACTOR>(*pro);
		actor::send_actor(lnspserid, nguid::make(), pro);
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_read<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_register_reply<T>>& adata)
	{
		const np_channel_register_reply<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_read", aactor, recv);

		m_regload.set_register(nguid::area(recv->m_actorid));
		m_operator_field.set_field(recv->m_node_fieldnumbers);

		std::ranges::for_each(recv->m_care, [this](const auto& apair)
			{
				m_exit.insert(apair.first);
			});
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_read<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_dataid_sync<T>>& adata)
	{
		const np_channel_dataid_sync<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_read", aactor, recv);

		i16_actortype ltype = nguid::type(recv->m_actorid);
		m_operator_field.set_field(ltype, recv->m_field);
	}
}