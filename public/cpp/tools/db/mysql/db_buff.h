#pragma once

#include "nlog.h"

#include <functional>
#include <cstdint>
#include <list>

namespace ngl
{
	struct dbuff
	{
		dbuff(const dbuff&) = delete;
		dbuff& operator=(const dbuff&) = delete;
		dbuff() = delete;

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
			e_buffsize = 10 * 1024 * 1024, // µ¥blobÉÏÏÞ 10M
		};
		dbuff* m_buff;
		dbuff* m_mallocbuff;
	public:
		inline db_buff() :
			m_buff(new dbuff(e_buffsize))
			, m_mallocbuff(nullptr)
		{}

		inline char* buff()
		{
			if (m_mallocbuff != nullptr)
			{
				return m_mallocbuff->m_buff;
			}
			return m_buff->m_buff;
		}

		inline int32_t buffsize()
		{
			if (m_mallocbuff != nullptr)
			{
				return m_mallocbuff->m_buffsize;
			}
			return m_buff->m_buffsize;
		}

		inline int32_t pos()
		{
			if (m_mallocbuff != nullptr)
			{
				return m_mallocbuff->m_pos;
			}
			return m_buff->m_pos;
		}

		inline void reset()
		{
			if (m_mallocbuff != nullptr)
			{
				m_mallocbuff = nullptr;
			}
			m_buff->m_pos = 0;
		}

		template <typename T>
		inline bool do_serialize(T& adata, dbuff* abuff)
		{
			ngl::serialize lserialize(m_buff->m_buff, m_buff->m_buffsize);
			if (lserialize.push(adata))
			{
				m_buff->m_pos = lserialize.byte();
				return true;
			}
			return false;
		}

		template <typename T>
		inline void serialize(T& adata)
		{
			if (do_serialize(adata, m_buff))
			{
				return;
			}

			if (adata.m_isbinary)
			{
				int32_t lbytes = adata.m_data->ByteSize();
				if (lbytes > e_buffsize)
				{
					if (m_mallocbuff != nullptr)
					{
						delete m_mallocbuff;
					}
					m_mallocbuff = new dbuff(lbytes);
					if (do_serialize(adata, m_mallocbuff))
					{
						return;
					}
					reset();
				}
			}
			Throw("malloc_buff.serialize fail");
		}
	};
}// namespace ngl