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
	class nsp_write
	{
		static std::atomic<bool>										m_isregister;

		TDerived* m_actor = nullptr;
		
		// # 自动注册handle协议处理
		nsp_callback<T>													m_call;

		// # 负责管理向[nsp server]注册本结点的状态与管理数据加载状态
		nsp_regload m_regload;

		// # 本结点关注"哪些数据"
		care_data m_care;

		// # 其他结点 关注哪些数据
		std::map<i64_actorid, care_data> m_othercare;

		// # "哪些类型的结点"关注"哪些数据字段"
		operator_field m_operator_field;
		std::set<i64_nodeid> m_nodereadalls;		// 读全部数据的结点
		std::set<i64_nodeid> m_nodewritealls;		// 写全部数据的结点


		std::map<i64_actorid, T>										m_data;
		// # 其他结点/当本结点exit()时需要通知哪些结点(1、所有nsp_server结点。2、所有nsp_write结点)
		std::set<i64_actorid> m_exit;

		// 被改变的数据列表
		std::set<i64_dataid>											m_changeids;
		// 被删除的数据列表
		std::set<i64_dataid>											m_delids;
	public:
		static nsp_write<TDerived, TACTOR, T>& instance(i64_actorid aactorid);

		static nsp_write<TDerived, TACTOR, T>& instance_writeall(
			TDerived* aactor
			, const std::set<i32_fieldnumber>& areadfieldnumbers
			, const std::set<i32_fieldnumber>& awritefieldnumbers);

		static nsp_write<TDerived, TACTOR, T>& instance_writepart(
			TDerived* aactor
			, const std::set<i32_fieldnumber>& areadfieldnumbers			/* 只读哪些字段 */
			, const std::set<i32_fieldnumber>& awritefieldnumbers			/* 读/写哪些字段 */
			, const std::set<i64_actorid>& areadids							/* 只读哪些数据 */
			, const std::set<i64_actorid>& awriteids						/* 读/写哪些数据 */
		);

		static i64_actorid to_actorid(i64_actorid adataid);

		void init();

		void set_changedatafun(const std::function<void(int64_t, const T&, bool)>& afun);

		void set_deldatafun(const std::function<void(int64_t)>& afun);

		void set_loadfinishfun(const std::function<void()>& afun);

		T* add(i64_dataid adataid);

		T* get(i64_dataid adataid);

		const T* getconst(i64_dataid adataid);

		////////////////////// 不应该提供这个方法(防止误用频繁更改数据) ////////////////////////
		// 应该使用[get_mapconst]配合[get]方法使用
		//std::map<i64_actorid, T>& get_map()
		//{
		//	std::transform(m_data.begin(), m_data.end(), std::inserter(m_changeids, m_changeids.end()),
		//		[](const auto& pair) { return pair.first; });
		//	return m_data;
		//}
		///////

		const std::map<i64_actorid, T>& get_mapconst();

		void change()
		{
			if (!m_changeids.empty())
			{

				std::map<i32_fieldnumber, epb_field>* lpfield = m_operator_field.get_field(nguid::area(m_actor->id_guid()));

				{
					auto pro = std::make_shared<np_channel_data<T>>();
					pro->m_actorid = m_actor->id_guid();
					std::set<i64_nodeid> lnodes;
					lnodes.insert(m_nodereadalls.begin(), m_nodereadalls.end());
					lnodes.insert(m_nodewritealls.begin(), m_nodewritealls.end());
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
						pb_field::copy(m_data[dataid], &pro->m_data[dataid], *lpfield);
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
						pb_field::copy(m_data[dataid], &pro->m_data[dataid], *lpfield);
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

		void handle(TDerived* aactor, const message<np_channel_data<T>>& adata);

		void handle(TDerived*, const message<np_channel_check<T>>& adata);

		void handle(TDerived*, const message<np_channel_register_reply<T>>& adata);

		void handle(TDerived*, const message<np_channel_dataid_sync<T>>& adata);

		void handle(TDerived*, const message<np_channel_exit<T>>& adata);

		void exit();
	};

	template <typename TDerived, typename TACTOR, typename T>
	std::atomic<bool> nsp_write<TDerived, TACTOR, T>::m_isregister = true;
}//namespace ngl
