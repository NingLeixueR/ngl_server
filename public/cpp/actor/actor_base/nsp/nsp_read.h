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

#include "nsp.h"

#include <ranges>

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
		nsp_callback<T>													m_call;

		// # 负责管理向[nsp server]注册本结点的状态与管理数据加载状态
		nsp_regload m_regload;

		// # "哪些结点"关注"哪些数据"
		care_data m_caredata;

		// # "哪些结点"关注"哪些数据字段"
		operator_field m_operator_field;

		std::map<i64_actorid, T>										m_data;

	public:
		static type_nsp_read& inst(i64_actorid aactorid)
		{
			return *nsp_instance<type_nsp_read>::nclient(aactorid);
		}

		static type_nsp_read& inst_readall(TDerived* aactor)
		{
			auto lpread = std::make_shared<type_nsp_read>();
			lpread->m_readall = true;
			lpread->m_actor = aactor;
			nsp_instance<type_nsp_read>::init(aactor->id_guid(), lpread);

			lpread->m_caredata.init(true);

			lpread->init();
			log_error_net()->print("nsp_read::instance_readall( actor({}) )", nguid(aactor->id_guid()));
			return *lpread;
		}

		static type_nsp_read& inst_readpart(TDerived* aactor, const std::set<i64_actorid>& aids)
		{
			auto lpread = std::make_shared<type_nsp_read>();
			lpread->m_readall = false;
			lpread->m_actor = aactor;
			for (i64_actorid dataid : aids)
			{
				lpread->m_operator_field.add_field(nguid::type(aactor->id_guid()), dataid, epb_field_read);
			}
			nsp_instance<type_nsp_read>::init(aactor->id_guid(), lpread);

			lpread->m_caredata.init(aids);

			lpread->init();
			log_error_net()->print("nsp_read::instance_readpart( actor({}) : {} )", nguid(aactor->id_guid()), aids);
			return *lpread;
		}

		static i64_actorid to_actorid(i64_actorid adataid)
		{
			return nguid::make_type(adataid, nactor_type<TACTOR>::type());
		}

		void set_changedatafun(const std::function<void(int64_t, const T&, bool)>& afun)
		{
			m_call.set_changedatafun(afun);
		}

		void set_deldatafun(const std::function<void(int64_t)>& afun)
		{
			m_call.set_deldatafun(afun);
		}

		void set_loadfinishfun(const std::function<void()>& afun)
		{
			m_call.set_loadfinishfun(afun);
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

		void handle(TDerived* aactor, const message<np_channel_data<T>>& adata);

		void handle(TDerived*, const message<np_channel_check<T>>& adata);

		void handle(TDerived*, const message<np_channel_read_register_reply<T>>& adata);

		void exit()
		{
			{//发送给server
				auto pro = std::make_shared<np_channel_exit<T>>();
				pro->m_actorid = m_actor->id_guid();
				m_regload.foreach_nspser([&pro](i16_area aarea, i64_actorid aactorid)
					{
						actor::send_actor(aactorid, nguid::make(), pro);
					});
			}
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
		m_regload.init(TACTOR::actorid());

		if (m_isregister.exchange(false))
		{
			// 更新数据
			nsp_instance<type_nsp_read>::template register_handle<TDerived, np_channel_data<T>>();
			// 检查是否向nsp服务器注册
			nsp_instance<type_nsp_read>::template register_handle<TDerived, np_channel_check<T>>();
			// 处理注册回复
			nsp_instance<type_nsp_read>::template register_handle<TDerived, np_channel_read_register_reply<T>>();

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
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_read<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_data<T>>& adata)
	{
		const np_channel_data<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_read", aactor, recv);

		bool lfirstsynchronize = recv->m_firstsynchronize;
		for (const auto& apair : recv->m_data)
		{
			if (m_caredata.is_care(apair.first))
			{
				m_operator_field.field_copy(nguid::type(apair.first), apair.second, m_data[apair.first]);
				m_call.changedatafun(apair.first, m_data[apair.first], lfirstsynchronize);
			}
		}

		for (int64_t dataid : recv->m_deldata)
		{
			if (m_caredata.is_care(dataid))
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
					if (m_caredata.is_care(apair.first))
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
		auto pro = std::make_shared<np_channel_read_register<T>>();
		pro->m_actorid = m_actor->id_guid();
		pro->m_readall = m_caredata.is_readall();
		if (!pro->m_readall)
		{
			pro->m_readids = m_caredata.readids();
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
			"nsp_client register: {} -> {}", nguid(pro->m_actorid), nguid(lnspserid)
		);
		nsp_handle_print<TDerived>::template msg_info<TACTOR>(*pro);
		actor::send_actor(lnspserid, nguid::make(), pro);
	}

	template <typename TDerived, typename TACTOR, typename T>
	void nsp_read<TDerived, TACTOR, T>::handle(TDerived* aactor, const message<np_channel_read_register_reply<T>>& adata)
	{
		const np_channel_read_register_reply<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_read", aactor, recv);
		
		m_regload.set_register(recv->m_actorid);
		m_operator_field.set_field(recv->m_node_fieldnumbers);
		return;
	}
}//namespace ngl