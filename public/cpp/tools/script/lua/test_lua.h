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
#include "serialize_lua.h"
#include "ndefine.h"

namespace ngl
{
	// # 测试

	struct k0
	{
		int m_v1;
		double m_v2;
		std::string m_v3;

		dprotocol(k0, m_v1, m_v2, m_v3)
	};

	struct k1
	{
		int m_v1;
		double m_v2;
		std::string m_v3;
		std::vector<int32_t> m_v4;
		std::map<int32_t, std::string> m_v5;
		std::map<std::string, std::string> m_v6;
		std::map<std::string, k0> m_v7;

		dprotocol(k1, m_v1, m_v2, m_v3, m_v4, m_v5, m_v6, m_v7)
	};

	struct k2
	{
		int m_v1;
		k1 m_v2;

		dprotocol(k2, m_v1, m_v2)
	};
}//namespace ngl