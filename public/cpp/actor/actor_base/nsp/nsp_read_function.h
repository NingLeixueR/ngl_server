#include "nsp_read.h"

namespace ngl
{
	template <typename TDerived, typename TACTOR, typename T>
	nsp_read<TDerived, TACTOR, T>& nsp_read<TDerived, TACTOR, T>::instance(i64_actorid aactorid)
	{
		return *nsp_instance<nsp_read<TDerived, TACTOR, T>>::nclient(aactorid);
	}

	template <typename TDerived, typename TACTOR, typename T>
	nsp_read<TDerived, TACTOR, T>& nsp_read<TDerived, TACTOR, T>::instance_readall(TDerived* aactor)
	{
		auto lpread = std::make_shared<nsp_read<TDerived, TACTOR, T>>();
		lpread->m_actor = aactor;
		nsp_instance<nsp_read<TDerived, TACTOR, T>>::init(aactor->id_guid(), lpread);

		lpread->m_care.init(true);

		lpread->init();
		log_error_net()->print("nsp_read::instance_readall( actor({}) )", nguid(aactor->id_guid()));
		return *lpread;
	}

	template <typename TDerived, typename TACTOR, typename T>
	nsp_read<TDerived, TACTOR, T>& nsp_read<TDerived, TACTOR, T>::instance_readpart(TDerived* aactor, const std::set<i64_actorid>& aids)
	{
		auto lpread = std::make_shared<nsp_read<TDerived, TACTOR, T>>();
		lpread->m_actor = aactor;
		lpread->m_care.init(aids);
		for (i64_actorid dataid : aids)
		{
			lpread->m_operator_field.add_field(nguid::type(aactor->id_guid()), dataid, epb_field_read);
		}
		nsp_instance<nsp_read<TDerived, TACTOR, T>>::init(aactor->id_guid(), lpread);

		lpread->m_care.init(aids);

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

		if (m_isregister.exchange(false))
		{
			// 更新数据
			nsp_instance<nsp_read<TDerived, TACTOR, T>>::template register_handle<TDerived, np_channel_data<T>>();
			// 检查是否向nsp服务器注册
			nsp_instance<nsp_read<TDerived, TACTOR, T>>::template register_handle<TDerived, np_channel_check<T>>();
			// 处理注册回复
			nsp_instance<nsp_read<TDerived, TACTOR, T>>::template register_handle<TDerived, np_channel_register_reply<T>>();
			// 
			nsp_instance<nsp_read<TDerived, TACTOR, T>>::template register_handle<TDerived, np_channel_dataid_sync<T>>();

			nsp_instance<nsp_read<TDerived, TACTOR, T>>::template register_handle<TDerived, np_channel_exit<T>>();
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
		{//发送给nsp_server
			m_regload.foreach_nspser([&pro](i16_area aarea, i64_actorid aactorid)
				{
					actor::send_actor(aactorid, nguid::make(), pro);
				});
		}		
		{//发送给nsp_write
			actor::send_actor(m_exit, nguid::make(), pro);			
		}
		nsp_instance<nsp_read<TDerived, TACTOR, T>>::exit(m_actor->id_guid());
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_read<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_data<T>>& adata)
	{
		const np_channel_data<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_read", aactor, recv);

		bool lfirstsynchronize = recv->m_firstsynchronize;
		for (const auto& apair : recv->m_data)
		{
			if (m_care.is_care(apair.first))
			{
				m_operator_field.field_copy(nguid::type(apair.first), apair.second, m_data[apair.first]);
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
						actor_base::nscript_data_nsp<T> ltemp(m_data);
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
					actor_base::nscript_data_nsp<T> ltemp(m_data);
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
		std::map<i32_fieldnumber, epb_field>* lmapfieldtype = m_operator_field.get_field(nguid::type(pro->m_actorid));
		if (lmapfieldtype == nullptr)
		{
			tools::no_core_dump();
			return;
		}
		std::ranges::copy(
			*lmapfieldtype | std::views::keys, std::inserter(pro->m_fieldnumbers, pro->m_fieldnumbers.begin())
		);

		i64_actorid lnspserid = m_regload.nspserid(recv->m_area);
		log_error()->print(
			"nsp_read register: {} -> {}", nguid(pro->m_actorid), nguid(lnspserid)
		);
		nsp_handle_print<TDerived>::template msg_info<TACTOR>(*pro);
		actor::send_actor(lnspserid, nguid::make(), pro);
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_read<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_register_reply<T>>& adata)
	{
		const np_channel_register_reply<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_read", aactor, recv);

		m_regload.set_register(recv->m_actorid);
		m_operator_field.set_field(recv->m_node_fieldnumbers);

		std::ranges::copy(
			recv->m_care | std::views::keys, std::inserter(m_exit, m_exit.begin())
		);

		m_exit.insert(m_exit.m_nodewritealls);
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_read<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_dataid_sync<T>>& adata)
	{
		const np_channel_dataid_sync<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_read", aactor, recv);

		m_operator_field.set_field(nguid::area(recv->m_actorid), recv->m_fieldnumbers);
	}
}