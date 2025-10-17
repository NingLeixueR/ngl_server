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
		std::set<i64_nodeid>											m_nodereadalls;		// 读全部数据的结点
		std::set<i64_nodeid>											m_nodewritealls;	// 写全部数据的结点
		//// 部分读/写
		std::map<i64_dataid, std::map<i64_nodeid, enp_channel>>			m_part;
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
			lpwrite->init();
			return *lpwrite;
		}

		static i64_actorid to_actorid(i64_actorid adataid)
		{
			return nguid::make_type(adataid, nactor_type<TACTOR>::type());
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
			for (i64_actorid dataid : aids)
			{
				lpwrite->m_ids.insert(to_actorid(dataid));
			}
			nsp_instance<type_nsp_write>::init(aactor->id_guid(), lpwrite);
			lpwrite->init();
			return *lpwrite;
		}

		void init();

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

		std::set<i64_dataid> m_changeids;

		T* add(i64_dataid adataid)
		{
			m_changeids.insert(to_actorid(adataid));
			return &m_data[adataid];
		}

		T* get(i64_dataid adataid)
		{
			auto itor = m_data.find(to_actorid(adataid));
			if (itor == m_data.end())
			{
				return nullptr;
			}
			m_changeids.insert(to_actorid(adataid));
			return &itor->second;
		}

		const T* getconst(i64_dataid adataid)
		{
			auto itor = m_data.find(to_actorid(adataid));
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
				nsp_handle_print<TDerived>::template msg_info<TACTOR>(*pro);
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
					for (const auto& [lkey, lvalue] : m_part[dataid])
					{
						lnodes.insert(lkey);
					}
					nsp_handle_print<TDerived>::template msg_info<TACTOR>(*pro);
					actor::send_actor(lnodes, nguid::make(), pro);
				}
			}
		}

		bool is_care(i64_actorid adataid)const;

		void handle(TDerived* aactor, const message<np_channel_data<T>>& adata);

		void handle(TDerived*, const message<np_channel_check<T>>& adata);

		void handle(TDerived*, const message<np_channel_write_register_reply<T>>& adata);

		void handle(TDerived*, const message<np_channel_dataid_sync<T>>& adata);

		void exit()
		{
			// 通知其他写结点，本结点退除
			auto pro = std::make_shared<np_channel_dataid_sync<T>>();
			pro->m_actorid = m_actor->id_guid();
			pro->m_add = false;
			pro->m_type = m_type;
			if (m_type == enp_channel_readpart || m_type == enp_channel_writepart)
			{
				pro->m_part = m_ids;
			}

			std::set<i64_nodeid> lnodeids;
			lnodeids.insert(m_nodewritealls.begin(), m_nodewritealls.end());
			for (const auto& item1 : m_part)
			{
				for (const auto& item2 : item1.second)
				{
					if (item2.second == enp_channel_writepart)
					{
						lnodeids.insert(item2.first);
					}
				}				
			}
			actor::send_actor(lnodeids, nguid::make(), pro);

			nsp_instance<type_nsp_write>::exit(m_actor->id_guid());
		}
	};

	template <typename TDerived, typename TACTOR, typename T>
	std::atomic<bool> nsp_write<TDerived, TACTOR, T>::m_isregister = true;
}//namespace ngl

namespace ngl
{
	template <typename TDerived, typename TACTOR, typename T>
	void nsp_write<TDerived, TACTOR, T>::init()
	{
		if (m_isregister.exchange(false))
		{
			// 更新数据
			nsp_instance<type_nsp_write>::template register_handle<TDerived, np_channel_data<T>>();
			// 检查是否向nsp服务器注册
			nsp_instance<type_nsp_write>::template register_handle<TDerived, np_channel_check<T>>();
			// 处理注册回复
			nsp_instance<type_nsp_write>::template register_handle<TDerived, np_channel_write_register_reply<T>>();
			// 处理注册回复
			nsp_instance<type_nsp_write>::template register_handle<TDerived, np_channel_dataid_sync<T>>();
		}

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
			}; 
			twheel::wheel().addtimer(lparm);
		}
	}

	template <typename TDerived, typename TACTOR, typename T>
	bool nsp_write<TDerived, TACTOR, T>::is_care(i64_actorid adataid)const
	{
		return m_type == enp_channel_writeall || m_ids.contains(to_actorid(adataid));
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_write<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_data<T>>& adata)
	{
		const np_channel_data<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_write", aactor, recv);

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
	void nsp_write<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_check<T>>& adata)
	{
		const np_channel_check<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_write", aactor, recv);

		if (m_register[recv->m_area])
		{
			twheel::wheel().removetimer(recv->m_timer);
			return;
		}
		auto pro = std::make_shared<np_channel_write_register<T>>();
		pro->m_actorid = m_actor->id_guid();
		pro->m_type = m_type;
		pro->m_writeids = m_ids;
		pro->m_fieldnumbers = m_fieldnumbers;

		nsp_handle_print<TDerived>::template msg_info<TACTOR>(*pro);
		actor::send_actor(m_nspserver[recv->m_area], nguid::make(), pro);
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_write<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_write_register_reply<T>>& adata)
	{
		const np_channel_write_register_reply<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_write", aactor, recv);

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

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_write<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_dataid_sync<T>>& adata)
	{
		const np_channel_dataid_sync<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_write", aactor, recv);

		if (recv->m_add)
		{
			if (recv->m_fieldnumbers.empty() && (recv->m_type == enp_channel_writeall || recv->m_type == enp_channel_writepart))
			{
				tools::no_core_dump();
			}
			if (recv->m_type == enp_channel_readpart || recv->m_type == enp_channel_writepart)
			{
				for (i64_dataid dataid : recv->m_part)
				{
					m_part[dataid][recv->m_actorid] = recv->m_type;
				}
			}
			else
			{
				if (recv->m_type == enp_channel_readall)
				{
					m_nodereadalls.insert(recv->m_actorid);
				}
				else if(recv->m_type == enp_channel_writeall)
				{
					m_nodewritealls.insert(recv->m_actorid);
				}
			}
			m_node_fieldnumbers[nguid::type(recv->m_actorid)] = recv->m_fieldnumbers;
		}
		else
		{
			if (recv->m_type == enp_channel_readpart || recv->m_type == enp_channel_writepart)
			{
				if (recv->m_part.empty())
				{
					tools::no_core_dump();
				}
				for (i64_dataid dataid : recv->m_part)
				{
					m_part[dataid].erase(recv->m_actorid);
				}
			}
			else
			{
				if (recv->m_type == enp_channel_readall)
				{
					m_nodereadalls.erase(recv->m_actorid);
				}
				else if (recv->m_type == enp_channel_writeall)
				{
					m_nodewritealls.erase(recv->m_actorid);
				}
			}
		}
	}
}//namespace ngl