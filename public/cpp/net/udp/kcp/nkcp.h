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

#include "actor_base.h"
#include "asio_kcp.h"
#include "ukcp.h"

namespace ngl
{
	class nkcp
	{
		std::map<i16_port, ukcp*> m_kcpnet;
		int16_t m_kcpindex;
	public:
		enum
		{
			enets_robot_tid = 100,
			enets_robot_tcount = 10,
			enets_kcp_tid = 1000,
			enets_kcp_tcount = 1000,
		};

		static nkcp& instance()
		{
			static nkcp ltemp;
			return ltemp;
		}

		i16_port kcp_port(int32_t atid, int16_t atcount, pbnet::ENUM_KCP aenum);

		// # 获取实例 
		ukcp* kcp(i16_port auport);
		ukcp* serkcp(pbnet::ENUM_KCP aenum, int16_t atcount);

		// # robot 创建随机端口
		i16_port create_kcp();

		// # server 创建指定端口
		i16_port create_kcp(pbnet::ENUM_KCP aenum);
	};
}//namespace ngl

namespace ngl
{
	template <typename T>
	bool actor_base::kcp_send(i64_actorid aactorid, T& adata, i16_port auport/* = 0*/)
	{
		std::set<i64_actorid> lactorids = { aactorid };
		return kcp_send(lactorids, adata, auport);
	}

	template <typename T>
	bool actor_base::kcp_send(const std::set<i64_actorid>& aactorids, T& adata, i16_port auport/* = 0*/)
	{
		ukcp* lpukcp = nkcp::instance().kcp(auport);
		if (lpukcp == nullptr)
		{
			return false;
		}
		return lpukcp->send(aactorids, adata);
	}
}//namespace ngl