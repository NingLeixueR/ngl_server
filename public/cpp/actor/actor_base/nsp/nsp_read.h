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
		using tnsp_read = nsp_read<TDerived, TACTOR, T>;
	private:
		static std::atomic<bool>										m_isregister;
				
		// # 持有此nsp模块的actor
		TDerived*														m_actor = nullptr;

		// # 自动注册handle协议处理
		nsp_callback<T>													m_call;

		// # 负责管理向[nsp server]注册本结点的状态与管理数据加载状态
		nsp_regload														m_regload;

		// # 本结点关注"哪些数据"
		care_data														m_care;

		// # "哪些结点"关注"哪些数据字段"
		operator_field													m_operator_field;

		// # 其他结点/当本结点exit()时需要通知哪些结点(1、所有nsp_server结点。2、所有nsp_write结点)
		std::set<i64_actorid>											m_exit;

		// # 副本数据
		std::map<i64_actorid, T>										m_data;
	public:
		// # 获取单例
		static nsp_read<TDerived, TACTOR, T>& instance(i64_actorid aactorid);

		// # [读全部数据]首充获取单例-需创建并初始化
		static nsp_read<TDerived, TACTOR, T>& instance_readall(TDerived* aactor, const std::set<i32_fieldnumber>& afieldnumbers);

		// # [读部分数据]首充获取单例-需创建并初始化
		static nsp_read<TDerived, TACTOR, T>& instance_readpart(
			TDerived* aactor, const std::set<i32_fieldnumber>& afieldnumbers, const std::set<i64_actorid>& aids
		);

		// # 替换数据guid的[actor_type]
		static i64_actorid to_actorid(i64_actorid adataid);

		// # 设置数据发生变化的回调
		void set_changedatafun(const std::function<void(int64_t, const T&, bool)>& afun);

		// # 设置数据被删除的回调
		void set_deldatafun(const std::function<void(int64_t)>& afun);

		// # 设置数据全部加载成功的回调
		void set_loadfinishfun(const std::function<void()>& afun);

		// # 初始化
		void init();

		// # 卸载此nsp模块
		void exit();

		// # 获取指定数据
		const T* getconst(i64_dataid adataid);

		// # 获取订阅的全部数据
		const std::map<i64_actorid, T>& get_mapconst();

		// # 数据同步处理
		void handle(TDerived* aactor, const message<np_channel_data<T>>& adata);

		// # 向[nsp server]注册此模块
		void handle(TDerived* aactor, const message<np_channel_check<T>>& adata);

		// # 向[nsp server]注册此模块的回复
		void handle(TDerived* aactor, const message<np_channel_register_reply<T>>& adata);

		// # 同步新增的[operator_field]
		void handle(TDerived* aactor, const message<np_channel_dataid_sync<T>>& adata);
	};

	template <typename TDerived, typename TACTOR, typename T>
	std::atomic<bool> nsp_read<TDerived, TACTOR, T>::m_isregister = true;
}//namespace ngl