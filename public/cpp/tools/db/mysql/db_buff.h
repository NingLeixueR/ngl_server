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
			e_buffsize = 102400,
			e_maxcount = 20,
		};
		int m_count = 1;
		std::shared_ptr<dbuff> m_buff;
	public:
		db_buff() :
			m_buff(std::make_shared<dbuff>(e_buffsize* 1))
		{}

		char* buff()
		{
			return m_buff->m_buff;
		}

		int32_t buffsize()
		{
			return m_buff->m_buffsize;
		}

		int32_t pos()
		{
			return m_buff->m_pos;
		}

		void reset()
		{
			m_buff->m_pos = 0;
		}

		template <typename T>
		int serialize(T& adata)
		{
			int lbyte = 0;
			std::function<bool(std::shared_ptr<dbuff>&)> lfun = [&adata, &lbyte](std::shared_ptr<dbuff>& abuff)->bool
				{
					abuff->m_pos = 0;
					ngl::serialize lserialize(abuff->m_buff, abuff->m_buffsize);
					if (lserialize.push(adata))
					{
						abuff->m_pos = lserialize.byte();
						return true;
					}
					return false;
				};
			if (malloc_function(lfun))
			{
				return lbyte;
			}
			Throw("malloc_buff.serialize fail");
		}

		bool malloc_function(const std::function<bool(std::shared_ptr<dbuff>&)>& afun);

	};
}// namespace ngl