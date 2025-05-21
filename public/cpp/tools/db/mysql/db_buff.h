#pragma once

#include "nlog.h"

#include <functional>
#include <cstdint>
#include <list>

namespace ngl
{
	struct dbuff
	{
		char* m_buff;
		int32_t m_buffsize;
		int32_t m_pos;

		dbuff(int32_t abuffsize):
			m_buff(new char[abuffsize]),
			m_buffsize(abuffsize),
			m_pos(0)
		{

		}

		~dbuff()
		{
			delete []m_buff;
		}
	};

	class db_buff
	{
		db_buff(const db_buff&) = delete;
		db_buff& operator=(const db_buff&) = delete;

		enum
		{
			e_buffsize = 10 * 1024 * 1024, // µ•blob…œœﬁ 10M
		};
		std::shared_ptr<dbuff> m_buff;
	public:
		inline db_buff() :
			m_buff(std::make_shared<dbuff>(e_buffsize))
		{}

		inline char* buff()
		{
			return m_buff->m_buff;
		}

		inline int32_t buffsize()
		{
			return m_buff->m_buffsize;
		}

		inline int32_t pos()
		{
			return m_buff->m_pos;
		}

		inline void reset()
		{
			m_buff->m_pos = 0;
		}

		template <typename T>
		inline void serialize(T& adata)
		{
			ngl::serialize lserialize(m_buff->m_buff, m_buff->m_buffsize);
			if (lserialize.push(adata))
			{
				m_buff->m_pos = lserialize.byte();
				return;
			}
			Throw("malloc_buff.serialize fail");
		}
	};
}// namespace ngl