#include "nsp_write.h"


namespace ngl
{
	template <typename TDerived, typename TACTOR, typename T>
	nsp_write<TDerived, TACTOR, T>& nsp_write<TDerived, TACTOR, T>::instance(i64_actorid aactorid)
	{
		return *nsp_instance<nsp_write<TDerived, TACTOR, T>>::nclient(aactorid);
	}

	template <typename TDerived, typename TACTOR, typename T>
	nsp_write<TDerived, TACTOR, T>& nsp_write<TDerived, TACTOR, T>::instance_writeall(TDerived* aactor, const std::set<int32_t>& afieldnumbers)
	{
		auto lpwrite = std::make_shared<nsp_write<TDerived, TACTOR, T>>();
		lpwrite->m_actor = aactor;
		nsp_instance<nsp_write<TDerived, TACTOR, T>>::init(aactor->id_guid(), lpwrite);

		lpwrite->m_care.init(false);

		lpwrite->init();
		return *lpwrite;
	}

	template <typename TDerived, typename TACTOR, typename T>
	nsp_write<TDerived, TACTOR, T>& nsp_write<TDerived, TACTOR, T>::instance_writepart(
		TDerived* aactor
		, const std::set<int32_t>& areadfieldnumbers
		, const std::set<int32_t>& awritefieldnumbers
		, const std::set<i64_actorid>& areadids
		, const std::set<i64_actorid>& awriteids
	)
	{
		auto lpwrite = std::make_shared<nsp_write<TDerived, TACTOR, T>>();
		lpwrite->m_actor = aactor;
		lpwrite->m_care.init(areadids, awriteids);
		for (i64_actorid dataid : areadfieldnumbers)
		{
			lpwrite->m_operator_field.add_field(nguid::type(aactor->id_guid()), dataid, epb_field_read);
		}
		for (i64_actorid dataid : awritefieldnumbers)
		{
			lpwrite->m_operator_field.add_field(nguid::type(aactor->id_guid()), dataid, epb_field_write);
		}
		nsp_instance<nsp_write<TDerived, TACTOR, T>>::init(aactor->id_guid(), lpwrite);

		lpwrite->m_care.init(areadids, awriteids);

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

		if (m_isregister.exchange(false))
		{
			// 更新数据
			nsp_instance<nsp_write<TDerived, TACTOR, T>>::template register_handle<TDerived, np_channel_data<T>>();
			// 检查是否向nsp服务器注册
			nsp_instance<nsp_write<TDerived, TACTOR, T>>::template register_handle<TDerived, np_channel_check<T>>();
			// 处理注册回复
			nsp_instance<nsp_write<TDerived, TACTOR, T>>::template register_handle<TDerived, np_channel_register_reply<T>>();
			// 接收结点信息
			nsp_instance<nsp_write<TDerived, TACTOR, T>>::template register_handle<TDerived, np_channel_dataid_sync<T>>();

			nsp_instance<nsp_write<TDerived, TACTOR, T>>::template register_handle<TDerived, np_channel_exit<T>>();			
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
		m_changeids.insert(to_actorid(adataid));
		return &m_data[adataid];
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
		nsp_instance<nsp_write<TDerived, TACTOR, T>>::exit(m_actor->id_guid());
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_write<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_data<T>>& adata)
	{
		const np_channel_data<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_write", aactor, recv);

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
		pro->m_read = true;
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
		std::ranges::copy(
			*lmapfieldtype | std::views::keys, std::inserter(pro->m_fieldnumbers, pro->m_fieldnumbers.begin())
		);

		i64_actorid lnspserid = m_regload.nspserid(recv->m_area);
		log_error()->print(
			"nsp_write register: {} -> {}", nguid(pro->m_actorid), nguid(lnspserid)
		);
		nsp_handle_print<TDerived>::template msg_info<TACTOR>(*pro);
		actor::send_actor(lnspserid, nguid::make(), pro);
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_write<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_register_reply<T>>& adata)
	{
		const np_channel_register_reply<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_write", aactor, recv);

		m_regload.set_register(recv->m_actorid);
		m_operator_field.set_field(recv->m_node_fieldnumbers);

		m_nodereadalls = recv->m_nodereadalls;
		m_nodewritealls = recv->m_nodewritealls;

		for (const auto& [_nodeid, _care] : recv->m_care)
		{
			m_othercare[_nodeid].init(_care);
		}
		return;
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_write<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_dataid_sync<T>>& adata)
	{
		const np_channel_dataid_sync<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_write", aactor, recv);
	
		m_operator_field.set_field(nguid::area(recv->m_actorid), recv->m_fieldnumbers);

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