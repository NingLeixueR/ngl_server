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

		TDerived*														m_actor = nullptr;
		
		// # 自动注册handle协议处理
		nsp_callback<T>													m_call;

		// # 负责管理向[nsp server]注册本结点的状态与管理数据加载状态
		nsp_regload														m_regload;

		// # 本结点关注"哪些数据"
		care_data														m_care;

		// # 其他结点 关注哪些数据
		std::map<i64_actorid, care_data>								m_othercare;

		// # "哪些类型的结点"关注"哪些数据字段"
		operator_field													m_operator_field;

		// # 读全部数据的结点
		std::set<i64_nodeid>											m_nodereadalls;

		// # 写全部数据的结点
		std::set<i64_nodeid>											m_nodewritealls;		

		std::map<i64_actorid, T>										m_data;

		// # 其他结点/当本结点exit()时需要通知哪些结点(1、所有nsp_server结点。2、所有nsp_write结点)
		std::set<i64_actorid>											m_exit;

		// 被改变的数据列表
		std::set<i64_dataid>											m_changeids;

		// 被删除的数据列表
		std::set<i64_dataid>											m_delids;
	public:
		// # 获取单例
		static nsp_write<TDerived, TACTOR, T>& instance(i64_actorid aactorid);

		// # [写全部数据]首充获取单例-需创建并初始化
		static nsp_write<TDerived, TACTOR, T>& instance_writeall(
			TDerived* aactor
			, const std::set<i32_fieldnumber>& areadfieldnumbers
			, const std::set<i32_fieldnumber>& awritefieldnumbers);

		// # [写部分数据]首充获取单例-需创建并初始化
		static nsp_write<TDerived, TACTOR, T>& instance_writepart(
			TDerived* aactor
			, const std::set<i32_fieldnumber>& areadfieldnumbers			/* 只读哪些字段 */
			, const std::set<i32_fieldnumber>& awritefieldnumbers			/* 读/写哪些字段 */
			, const std::set<i64_actorid>& areadids							/* 只读哪些数据 */
			, const std::set<i64_actorid>& awriteids						/* 读/写哪些数据 */
		);

		// # 替换数据guid的[actor_type]
		static i64_actorid to_actorid(i64_actorid adataid);

		// # 初始化
		void init();

		// # 设置数据发生变化的回调
		void set_changedatafun(const std::function<void(int64_t, const T&, bool)>& afun);

		// # 设置数据被删除的回调
		void set_deldatafun(const std::function<void(int64_t)>& afun);

		// # 设置数据全部加载成功的回调
		void set_loadfinishfun(const std::function<void()>& afun);

		// # 添加数据
		T* add(i64_dataid adataid);

		// # 获取指定数据(并自动设置其状态为已修改)
		T* get(i64_dataid adataid);

		// # 获取指定数据
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

		// # 卸载此nsp模块
		void exit();

		// # 消息结束调用
		void change();
		static void change(i64_actorid aactorid);

		// # 数据同步处理
		void handle(TDerived* aactor, const message<np_channel_data<T>>& adata);

		// # 向[nsp server]注册此模块
		void handle(TDerived*, const message<np_channel_check<T>>& adata);

		// # 向[nsp server]注册此模块的回复
		void handle(TDerived*, const message<np_channel_register_reply<T>>& adata);

		// # 同步新增的[operator_field]
		void handle(TDerived*, const message<np_channel_dataid_sync<T>>& adata);

		// # 结点退出
		void handle(TDerived*, const message<np_channel_exit<T>>& adata);
	};

	template <typename TDerived, typename TACTOR, typename T>
	std::atomic<bool> nsp_write<TDerived, TACTOR, T>::m_isregister = true;
}//namespace ngl
