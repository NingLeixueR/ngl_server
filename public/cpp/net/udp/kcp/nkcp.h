#pragma once

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