#pragma once

#include "nsp_server.h"

namespace ngl
{
	template <typename TDerived, typename TACTOR, typename T>
	class nsp_write
	{
	public:
		using type_nsp_write = nsp_write<TDerived, TACTOR, T>;
	private:
		static std::atomic<bool>										m_isregister;

		TDerived* m_actor = nullptr;
		
		nsp_callback<T> m_call;

		std::map<i16_area, i64_actorid>									m_nspserver;
		std::map<i16_area, bool>										m_register;

		enp_channel														m_type;				// 类型
		//[[ 只有 <m_type == enp_channel_readpart> 下面数据有效
		std::set<i64_actorid>											m_ids;				// 可写哪些数据
		//]]

		std::map<i16_actortype, std::set<i32_fieldnumber>>				m_node_fieldnumbers;
		std::map<i64_actorid, T>										m_data;

		std::set<int32_t>												m_fieldnumbers;
		std::set<i64_nodeid> m_nodereadalls;							// 读全部数据的结点
		std::set<i64_nodeid> m_nodewritealls;							// 写全部数据的结点
		//// 部分读/写
		std::map<i64_dataid, std::map<i64_nodeid, enp_channel>> m_part;

		
	public:
		static type_nsp_write& instance(i64_actorid aactorid)
		{
			return *nsp_instance<type_nsp_write>::nclient(aactorid);
		}

		static type_nsp_write& instance_writeall(
			TDerived* aactor
			, const std::set<int32_t>& afieldnumbers
		)
		{
			auto lpwrite = std::make_shared<type_nsp_write>();
			lpwrite->m_type = enp_channel_writeall;
			lpwrite->m_actor = aactor;
			lpwrite->m_fieldnumbers = afieldnumbers;
			nsp_instance<type_nsp_write>::init(aactor->id_guid(), lpwrite);
			lpwrite->register_handles();
			return *lpwrite;
		}

		static type_nsp_write& instance_writepart(
			TDerived* aactor
			, const std::set<i64_actorid>& aids
			, const std::set<int32_t>& afieldnumbers
		)
		{
			auto lpwrite = std::make_shared<type_nsp_write>();
			lpwrite->m_type = enp_channel_writepart;
			lpwrite->m_actor = aactor;
			lpwrite->m_fieldnumbers = afieldnumbers; 
			lpwrite->m_ids = aids;
			nsp_instance<type_nsp_write>::init(aactor->id_guid(), lpwrite);
			lpwrite->register_handles();
			return *lpwrite;
		}

		void register_handles();

		void set_changedatafun(int64_t aid, const std::function<void(int64_t, const T&, bool)>& afun)
		{
			m_call.set_changedatafun(aid, afun);
		}

		void set_deldatafun(int64_t aid, const std::function<void(int64_t)>& afun)
		{
			m_call.set_deldatafun(aid, afun);
		}

		void set_loadfinishfun(int64_t aid, const std::function<void()>& afun)
		{
			m_call.set_loadfinishfun(aid, afun);
		}

		template <typename TX>
		static void msg_info(TX& adata);

		std::set<i64_dataid> m_changeids;

		T* add(i64_dataid adataid)
		{
			m_changeids.insert(adataid);
			return &m_data[adataid];
		}

		T* get(i64_dataid adataid)
		{
			auto itor = m_data.find(adataid);
			if (itor == m_data.end())
			{
				return nullptr;
			}
			m_changeids.insert(adataid);
			return &itor->second;
		}

		const T* getconst(i64_dataid adataid)
		{
			auto itor = m_data.find(adataid);
			if (itor == m_data.end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		std::map<i64_actorid, T>& get_map()
		{
			std::transform(m_data.begin(), m_data.end(), std::inserter(m_changeids, m_changeids.end()),
				[](const auto& pair) { return pair.first; });
			return m_data;
		}

		const std::map<i64_actorid, T>& get_mapconst()
		{
			return m_data;
		}

		void change()
		{
			{
				auto pro = std::make_shared<np_channel_data<T>>();
				std::set<i64_nodeid> lnodes;
				lnodes.insert(m_nodereadalls.begin(), m_nodereadalls.end());
				lnodes.insert(m_nodewritealls.begin(), m_nodewritealls.end());
				pro->m_firstsynchronize = false;
				pro->m_recvfinish = true;
				for (i64_dataid dataid : m_changeids)
				{
					if (!m_data.contains(dataid))
					{
						continue;
					}
					pb_field::copy(m_data[dataid], &pro->m_data[dataid], m_fieldnumbers);
				}
				actor::send_actor(lnodes, nguid::make(), pro);
			}

			{
				for (i64_dataid dataid : m_changeids)
				{
					auto pro = std::make_shared<np_channel_data<T>>();
					pro->m_firstsynchronize = false;
					pro->m_recvfinish = true;
					if (!m_data.contains(dataid))
					{
						continue;
					}
					if (!m_part.contains(dataid))
					{
						continue;
					}
					pb_field::copy(m_data[dataid], &pro->m_data[dataid], m_fieldnumbers);
					std::set<i64_nodeid> lnodes;
					for (const auto& item : m_part[dataid])
					{
						lnodes.insert(item.first);
					}
					actor::send_actor(lnodes, nguid::make(), pro);
				}
			}
			
		}

		bool is_care(i64_actorid adataid);

		void handle(TDerived* aactor, const message<np_channel_data<T>>& adata);

		void handle(TDerived*, const message<np_channel_check<T>>& adata);

		void handle(TDerived*, const message<np_channel_write_register_reply<T>>& adata);

		void exit()
		{
			nsp_instance<type_nsp_write>::exit(m_actor->id_guid());
		}
	};


	template <typename TDerived, typename TACTOR, typename T>
	std::atomic<bool> nsp_write<TDerived, TACTOR, T>::m_isregister = true;
}//namespace ngl


namespace ngl
{
	template <typename TDerived, typename TACTOR, typename T>
	void nsp_write<TDerived, TACTOR, T>::register_handles()
	{
		if (m_isregister.exchange(false))
		{
			// 更新数据
			nsp_instance<type_nsp_write>::template register_handle<TDerived, np_channel_data<T>>();
			// 检查是否向nsp服务器注册
			nsp_instance<type_nsp_write>::template register_handle<TDerived, np_channel_check<T>>();
			// 处理注册回复
			nsp_instance<type_nsp_write>::template register_handle<TDerived, np_channel_write_register_reply<T>>();
		}
	}

	template <typename TDerived, typename TACTOR, typename T>
	template <typename TX>
	void nsp_write<TDerived, TACTOR, T>::msg_info(TX& adata)
	{
		adata.m_msg = std::format(
			"{}:{}:{}"
			, tools::type_name<TDerived>()
			, tools::type_name<TACTOR>()
			, tools::type_name<T>()
		);
	}

	template <typename TDerived, typename TACTOR, typename T>
	bool nsp_write<TDerived, TACTOR, T>::is_care(i64_actorid adataid)
	{
		return m_type == enp_channel_writeall || m_ids.contains(adataid);
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_write<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_data<T>>& adata)
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
				m_call.changedatafun(apair.first, apair.second, lfirstsynchronize);
			}
		}

		for (int64_t dataid : recv->m_deldata)
		{
			if (is_care(dataid))
			{
				m_data.erase(dataid);
				m_call.deldatafun(dataid);
			}
		}
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_write<TDerived, TACTOR, T>::handle(TDerived*, const message<np_channel_check<T>>& adata)
	{
		auto lprecv = adata.get_data();
		if (m_register[lprecv->m_area])
		{
			twheel::wheel().removetimer(lprecv->m_timer);
			return;
		}
		auto pro = std::make_shared<np_channel_write_register<T>>();
		pro->m_actorid = m_actor->id_guid();
		pro->m_type = m_type;
		pro->m_writeids = m_ids;
		pro->m_fieldnumbers = m_fieldnumbers;

		log_error()->print(
			"nsp_write register: {} -> {}"
			, nguid(pro->m_actorid)
			, nguid(m_nspserver[lprecv->m_area])
		);
		msg_info(*pro);
		actor::send_actor(m_nspserver[lprecv->m_area], nguid::make(), pro);
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_write<TDerived, TACTOR, T>::handle(TDerived*, const message<np_channel_write_register_reply<T>>& adata)
	{
		const np_channel_write_register_reply<T>* recv = adata.get_data();
		if (m_actor->id_guid() != recv->m_actorid)
		{
			tools::no_core_dump();
		}
		m_register[nguid::area(recv->m_actorid)] = true;
		m_node_fieldnumbers = recv->m_node_fieldnumbers;

		m_nodereadalls = recv->m_nodereadalls;
		m_nodewritealls = recv->m_nodewritealls;
		m_part = recv->m_part;

		return;
	}
}//namespace ngl