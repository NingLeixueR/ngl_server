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

#include "xmlinfo.h"
#include "type.h"
#include "nlog.h"

#include <functional>
#include <string>
#include <map>

namespace ngl
{
	struct xarg_protocols
	{
		struct info
		{
			int32_t m_client = 0;
			std::string m_name;
			int32_t m_number = 0;

			DXMLSERIALIZE(config, true, m_client, m_name, m_number)
		};
		std::vector<info> m_config;

		DXMLSERIALIZE(con, false, m_config)
	};

	class xmlprotocol
	{
		static std::map<std::string, int32_t> m_protocol;

		static void read(const char* axml);
	public:
		// # 从xml中加载数据
		static void load();

		// # 获取协议号
		static int32_t protocol(const std::string& aname);

		// # makeproto工具调用 所以不需要索引
		static int32_t free_protocol();
	};
}// namespace ngl