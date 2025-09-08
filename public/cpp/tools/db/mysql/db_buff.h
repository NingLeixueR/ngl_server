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
			e_buffsize = 10 * 1024 * 1024, // µ•blob…œœﬁ 10M
		};
		std::shared_ptr<dbuff> m_buff;
		std::shared_ptr<dbuff> m_mallocbuff;
	public:
		inline db_buff() :
			m_buff(std::make_shared<dbuff>(e_buffsize))
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
		inline bool do_binary(T& adata, dbuff* abuff)
		{
			ngl::ser::serialize_push lserialize(abuff->m_buff, abuff->m_buffsize);
			if (ngl::ser::nserialize::push(&lserialize, adata))
			{
				abuff->m_pos = lserialize.pos();
				return true;
			}
			return false;
		}

		template <typename T>
		inline bool do_serialize_binary(T& adata)
		{
			if (do_binary(adata, m_buff.get()))
			{
				return true;
			}
			ngl::ser::serialize_byte lbyte;
			ngl::ser::nserialize::bytes(&lbyte, adata);
			m_mallocbuff = std::make_shared<dbuff>(lbyte.pos());
			if (do_binary(adata, m_mallocbuff.get()))
			{
				return true;
			}
			return false;
		}



		template <typename T>
		inline bool do_serialize_json(T& adata)
		{
			std::string ltemp;
			if (!tools::proto2json(adata, ltemp))
			{
				return false;
			}
			if (ltemp.size() > m_buff->m_buffsize)
			{
				m_mallocbuff = std::make_shared<dbuff>(ltemp.size() + 1);
				memcpy(m_mallocbuff->m_buff, ltemp.c_str(), ltemp.size() + 1);
				m_mallocbuff->m_pos = ltemp.size() + 1;
			}
			else
			{
				memcpy(m_buff->m_buff, ltemp.c_str(), ltemp.size() + 1);
				m_buff->m_pos = ltemp.size() + 1;
			}
			return true;
		}

		template <typename T, bool ISBINARY>
		inline void serialize(T& adata)
		{
			if constexpr (ISBINARY)
			{
				if (!do_serialize_binary(adata))
				{
					log_error()->print("do_serialize_binary fail T=[{}:{}]", tools::type_name<T>(), adata.mid());
					return;
				}
			}
			else
			{
				if (!do_serialize_json(adata))
				{
					log_error()->print("do_serialize_json fail T=[{}:{}]", tools::type_name<T>(), adata.mid());
					return;
				}
			}
			return;
		}
	};
}// namespace ngl