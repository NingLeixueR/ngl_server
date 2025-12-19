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

#include "threadtools.h"
#include "pack.h"

namespace ngl
{
	class node_pack
	{
		node_pack() = delete;

		std::shared_ptr<pack>	m_pack		= nullptr;
		std::shared_ptr<void>	m_packvoid	= nullptr;
		bool					m_ispack	= false;
		i32_sessionid			m_sessionid = 0;
	public:
		node_pack(i32_sessionid asessionid, std::shared_ptr<pack>& apack) :
			m_pack(apack),
			m_ispack(true),
			m_sessionid(asessionid)
		{}

		node_pack(i32_sessionid asessionid, std::shared_ptr<void>& apack) :
			m_packvoid(apack),
			m_ispack(false),
			m_sessionid(asessionid)
		{}

		~node_pack()
		{}

		inline bool is_pack()
		{
			return m_ispack;
		}

		inline i32_sessionid sessionid()
		{
			return m_sessionid;
		}

		inline std::shared_ptr<pack>& get_pack()
		{
			return m_pack;
		}

		inline std::shared_ptr<void>& get_voidpack()
		{
			return m_packvoid;
		}

		inline pack* get()
		{
			if (m_ispack)
			{
				return m_pack.get();
			}
			else
			{
				return (pack*)m_packvoid.get();
			}
		}
	};
}// namespace ngl
