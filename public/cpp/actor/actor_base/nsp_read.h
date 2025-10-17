#pragma once

#include "nsp_server.h"

namespace ngl
{
	template <typename TDerived, typename TACTOR, typename T>
	class nsp_read
	{
	public:
		using type_nsp_read = nsp_read<TDerived, TACTOR, T>;
	private:

		static std::atomic<bool>										m_isregister;
		
		TDerived*														m_actor = nullptr;
		nsp_callback<T>													m_callback;

		std::function<void(int64_t, const T&, bool)>					m_changedatafun;	// [回调] 当数据发生变化
		std::function<void(int64_t)>									m_deldatafun;		// [回调] 当数据被删除
		std::function<void()>											m_loadfinishfun;	// [回调] 数据加载完成

		std::map<i16_area, i64_actorid>									m_nspserver;
		std::map<i16_area, bool>										m_register;

		enp_channel														m_type;				// 类型
		//[[ 只有 <m_type == enp_channel_readpart> 下面数据有效
		std::set<i64_actorid>											m_ids;				// 只读哪些数据
		//]]

		std::map<i16_actortype, std::set<i32_fieldnumber>>				m_node_fieldnumbers;
		std::map<i64_actorid, T>										m_data;
	public:
		static type_nsp_read& instance(i64_actorid aactorid)
		{
			return *nsp_instance<type_nsp_read>::nclient(aactorid);
		}

		static type_nsp_read& instance_readall(TDerived* aactor)
		{
			auto lpread = std::make_shared<type_nsp_read>();
			lpread->m_type = enp_channel_readall;
			lpread->m_actor = aactor;
			nsp_instance<type_nsp_read>::init(aactor->id_guid(), lpread);
			lpread->init();
			log_error_net()->print("nsp_read::instance_readall( actor({}) )", nguid(aactor->id_guid()));
			return *lpread;
		}

		static i64_actorid to_actorid(i64_actorid adataid)
		{
			return nguid::make_type(adataid, nactor_type<TACTOR>::type());
		}

		static type_nsp_read& instance_readpart(TDerived* aactor, const std::set<i64_actorid>& aids)
		{
			auto lpread = std::make_shared<type_nsp_read>();
			lpread->m_type = enp_channel_readpart;
			lpread->m_actor = aactor;
			for (i64_actorid dataid : aids)
			{
				lpread->m_ids.insert(to_actorid(dataid));
			}
			nsp_instance<type_nsp_read>::init(aactor->id_guid(), lpread);
			lpread->init();
			log_error_net()->print("nsp_read::instance_readpart( actor({}) : {} )", nguid(aactor->id_guid()), aids);
			return *lpread;
		}

		void set_changedatafun(int64_t aid, const std::function<void(int64_t, const T&, bool)>& afun)
		{
			m_callback.set_changedatafun(aid, afun);
		}

		void set_deldatafun(int64_t aid, const std::function<void(int64_t)>& afun)
		{
			m_callback.set_deldatafun(aid, afun);
		}

		void set_loadfinishfun(int64_t aid, const std::function<void()>& afun)
		{
			m_callback.set_loadfinishfun(aid, afun);
		}

		void init();

		const T* getconst(i64_dataid adataid)
		{
			auto itor = m_data.find(to_actorid(adataid));
			if (itor == m_data.end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		const std::map<i64_actorid, T>& get_mapconst()
		{
			return m_data;
		}

		bool is_care(i64_actorid adataid)const;

		void handle(TDerived* aactor, const message<np_channel_data<T>>& adata);

		void handle(TDerived*, const message<np_channel_check<T>>& adata);

		void handle(TDerived*, const message<np_channel_read_register_reply<T>>& adata);

		void exit()
		{
			nsp_instance<type_nsp_read>::exit(m_actor->id_guid());
		}
	};


	template <typename TDerived, typename TACTOR, typename T>
	std::atomic<bool> nsp_read<TDerived, TACTOR, T>::m_isregister = true;
}//namespace ngl

namespace ngl
{
	template <typename TDerived, typename TACTOR, typename T>
	void nsp_read<TDerived, TACTOR, T>::init()
	{
		if (m_isregister.exchange(false))
		{
			// 更新数据
			nsp_instance<type_nsp_read>::template register_handle<TDerived, np_channel_data<T>>();
			// 检查是否向nsp服务器注册
			nsp_instance<type_nsp_read>::template register_handle<TDerived, np_channel_check<T>>();
			// 处理注册回复
			nsp_instance<type_nsp_read>::template register_handle<TDerived, np_channel_read_register_reply<T>>();

			std::set<i16_area> lareaset;
			ttab_servers::instance().get_arealist_nonrepet(nconfig::m_tid, lareaset);

			auto ltype = (ENUM_ACTOR)nguid::type(TACTOR::actorid());
			for (i16_area area : lareaset)
			{
				m_nspserver[area] = nguid::make(ltype, area, nguid::none_actordataid());
				m_register[area] = false;
			}

			i64_actorid lactorid = m_actor->id_guid();
			for (std::pair<const i16_area, bool>& item : m_register)
			{
				i16_area larea = item.first;
				wheel_parm lparm
				{
					.m_ms = 1000,
					.m_intervalms = [](int64_t) {return 10000; } ,
					.m_count = 0x7fffffff,
					.m_fun = [larea, lactorid](const wheel_node* anode)
					{
						auto pro = std::make_shared<np_channel_check<T>>(
							np_channel_check<T>{
								.m_timer = anode->m_timerid,
								.m_area = larea,
							});
						nsp_handle_print<TDerived>::template msg_info<TACTOR>(*pro);
						actor::send_actor(lactorid, nguid::make(), pro);
					}
				}; twheel::wheel().addtimer(lparm);
			}
		}
	}

	template <typename TDerived, typename TACTOR, typename T>
	bool nsp_read<TDerived, TACTOR, T>::is_care(i64_actorid adataid)const
	{
		return m_type == enp_channel_readall || m_ids.contains(to_actorid(adataid));
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_read<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_data<T>>& adata)
	{
		const np_channel_data<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_read", aactor, recv);

		bool lfirstsynchronize = recv->m_firstsynchronize;

		for (const auto& apair : recv->m_data)
		{
			if (is_care(apair.first))
			{
				if (lfirstsynchronize)
				{
					m_data[apair.first] = apair.second;
				}
				else
				{
					pb_field::copy(apair.second, &m_data[apair.first], m_node_fieldnumbers[nguid::type(apair.first)]);
				}
				if (m_changedatafun != nullptr)
				{
					m_changedatafun(apair.first, apair.second, lfirstsynchronize);
				}
			}
		}

		for (int64_t dataid : recv->m_deldata)
		{
			if (is_care(dataid))
			{
				m_data.erase(dataid);
				if (m_deldatafun != nullptr)
				{
					m_deldatafun(dataid);
				}
			}
		}
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_read<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_check<T>>& adata)
	{
		const np_channel_check<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_read", aactor, recv);

		if (m_register[recv->m_area])
		{
			twheel::wheel().removetimer(recv->m_timer);
			return;
		}
		auto pro = std::make_shared<np_channel_read_register<T>>();
		pro->m_actorid = m_actor->id_guid();
		pro->m_type = m_type;
		pro->m_readids = m_ids;
		log_error()->print(
			"nsp_client register: {} -> {}"
			, nguid(pro->m_actorid)
			, nguid(m_nspserver[recv->m_area])
		);
		nsp_handle_print<TDerived>::template msg_info<TACTOR>(*pro);
		actor::send_actor(m_nspserver[recv->m_area], nguid::make(), pro);
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_read<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_read_register_reply<T>>& adata)
	{
		const np_channel_read_register_reply<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_read", aactor, recv);

		if (m_actor->id_guid() != recv->m_actorid)
		{
			tools::no_core_dump();
		}
		m_register[nguid::area(recv->m_actorid)] = true;
		m_node_fieldnumbers = recv->m_node_fieldnumbers;
		return;
	}
}//namespace ngl