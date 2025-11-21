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

#include "ndb_modular.h"
#include "threadtools.h"
#include "pb_field.h"
#include "tools.h"
#include "type.h"
#include "nsp.h"

namespace ngl
{
	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	class nsp_server
	{
		// # 可操作的"数据字段"
		static operator_field						m_operator_field;
		// # 哪些结点关注哪些数据
		static std::map<i64_actorid, care_data>		m_care;
		// # 读全部数据的结点
		static std::set<i64_nodeid>					m_nodereadalls;
		// # 写全部数据的结点
		static std::set<i64_nodeid>					m_nodewritealls;
		// # 读部分数据的结点
		static std::set<i64_nodeid>					m_nodepart;
		// # 数据
		static ndb_modular<ENUMDB, T, TDerived>*	m_dbmodule;
		// # 结点区服列表
		static std::set<i16_area>					m_areaset;
	public:
		using tnsp_server = nsp_server<ENUMDB, TDerived, T>;

		enum
		{
			esend_maxcount = 100,
		};

		// # 订阅注册处理
		static void init(ndb_modular<ENUMDB, T, TDerived>* adbmodule);

		// # 注册回复
		static void channel_register_reply(i64_actorid aactorid);

		// # 注册回复数据
		static void channel_channel_data(i64_actorid aactorid, const np_channel_register<T>* recv);

		// # 注册同步其他结点信息
		static void channel_dataid_sync(i64_actorid aactorid, const np_channel_register<T>* recv);

		// # 结点注册
		static void handle(TDerived*, const message<np_channel_register<T>>& adata);

		// # 结点退出
		static void handle(TDerived*, const message<np_channel_exit<T>>& adata);

		// # 结点修改数据
		static void handle(TDerived*, const message<np_channel_data<T>>& adata);
	};

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	operator_field nsp_server<ENUMDB, TDerived, T>::m_operator_field;

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	std::map<i64_actorid, care_data> nsp_server<ENUMDB, TDerived, T>::m_care;

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	std::set<i64_nodeid> nsp_server<ENUMDB, TDerived, T>::m_nodereadalls;

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	std::set<i64_nodeid> nsp_server<ENUMDB, TDerived, T>::m_nodewritealls;

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	std::set<i64_nodeid> nsp_server<ENUMDB, TDerived, T>::m_nodepart;

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	ndb_modular<ENUMDB, T, TDerived>* nsp_server<ENUMDB, TDerived, T>::m_dbmodule;

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	std::set<i16_area> nsp_server<ENUMDB, TDerived, T>::m_areaset;
}//namespace ngl