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

		TDerived* m_actor = nullptr;
		nsp_callback<T> m_callback;

		std::function<void(int64_t, const T&, bool)>					m_changedatafun;	// [�ص�] �����ݷ����仯
		std::function<void(int64_t)>									m_deldatafun;		// [�ص�] �����ݱ�ɾ��
		std::function<void()>											m_loadfinishfun;	// [�ص�] ���ݼ������

		std::map<i16_area, i64_actorid>									m_nspserver;
		std::map<i16_area, bool>										m_register;

		enp_channel														m_type;				// ����
		//[[ ֻ�� <m_type == enp_channel_readpart> ����������Ч
		std::set<i64_actorid>											m_ids;				// ֻ����Щ����
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
			return *lpread;
		}

		static type_nsp_read& instance_readpart(TDerived* aactor, const std::set<i64_actorid>& aids)
		{
			auto lpread = std::make_shared<type_nsp_read>();
			lpread->m_type = enp_channel_readpart;
			lpread->m_actor = aactor;
			lpread->m_ids = aids;
			nsp_instance<type_nsp_read>::init(aactor->id_guid(), lpread);
			lpread->init();
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
			auto itor = m_data.find(adataid);
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

		template <typename TX>
		static void msg_info(TX& adata);

		bool is_care(i64_actorid adataid);

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
			// ��������
			nsp_instance<type_nsp_read>::template register_handle<TDerived, np_channel_data<T>>();
			// ����Ƿ���nsp������ע��
			nsp_instance<type_nsp_read>::template register_handle<TDerived, np_channel_check<T>>();
			// ����ע��ظ�
			nsp_instance<type_nsp_read>::template register_handle<TDerived, np_channel_read_register_reply<T>>();

			std::set<i16_area> lareaset;
			ttab_servers::instance().get_arealist_nonrepet(nconfig::m_nodeid, lareaset);

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
						msg_info(*pro);
						actor::send_actor(lactorid, nguid::make(), pro);
					}
				}; twheel::wheel().addtimer(lparm);
			}
		}
	}

	template <typename TDerived, typename TACTOR, typename T>
	template <typename TX>
	void nsp_read<TDerived, TACTOR, T>::msg_info(TX& adata)
	{
		adata.m_msg = std::format(
			"{}:{}:{}"
			, tools::type_name<TDerived>()
			, tools::type_name<TACTOR>()
			, tools::type_name<T>()
		);
	}

	template <typename TDerived, typename TACTOR, typename T>
	bool nsp_read<TDerived, TACTOR, T>::is_care(i64_actorid adataid)
	{
		return m_type == enp_channel_readall || m_ids.contains(adataid);
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_read<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_data<T>>& adata)
	{
		const np_channel_data<T>* recv = adata.get_data();

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
	void nsp_read<TDerived, TACTOR, T>::handle(TDerived*, const message<np_channel_check<T>>& adata)
	{
		auto lprecv = adata.get_data();
		if (m_register[lprecv->m_area])
		{
			twheel::wheel().removetimer(lprecv->m_timer);
			return;
		}
		auto pro = std::make_shared<np_channel_read_register<T>>();
		pro->m_actorid = m_actor->id_guid();
		pro->m_type = m_type;
		pro->m_readids = m_ids;
		log_error()->print(
			"nsp_client register: {} -> {}"
			, nguid(pro->m_actorid)
			, nguid(m_nspserver[lprecv->m_area])
		);
		msg_info(*pro);
		actor::send_actor(m_nspserver[lprecv->m_area], nguid::make(), pro);
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_read<TDerived, TACTOR, T>::handle(TDerived*, const message<np_channel_read_register_reply<T>>& adata)
	{
		const np_channel_read_register_reply<T>* recv = adata.get_data();
		if (m_actor->id_guid() != recv->m_actorid)
		{
			tools::no_core_dump();
		}
		m_register[nguid::area(recv->m_actorid)] = true;
		m_node_fieldnumbers = recv->m_node_fieldnumbers;
		return;
	}
}//namespace ngl