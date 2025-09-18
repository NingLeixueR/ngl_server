#pragma once

#include "threadtools.h"
#include "pack.h"

namespace ngl
{
	class node_pack
	{
		node_pack() = delete;

		std::shared_ptr<pack>	m_pack;
		std::shared_ptr<void>	m_packvoid;
		bool					m_ispack;
		i32_sessionid			m_sessionid;
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

		inline bool ispack()
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
