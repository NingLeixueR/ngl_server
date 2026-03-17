/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Declares interfaces for sql.

#pragma once

#include "tools/log/nlog.h"

#include <cstddef>
#include <functional>
#include <cstdint>
#include <limits>
#include <list>

namespace ngl
{
	class dbuff
	{
		dbuff(const dbuff&) = delete;
		dbuff& operator=(const dbuff&) = delete;
		dbuff() = delete;

		std::unique_ptr<char[]> m_buff = nullptr;
		int32_t m_buffsize = 0;
		int32_t m_pos = 0;
	public:

		dbuff(int32_t abuffsize):
			m_buff(std::make_unique<char[]>(abuffsize)),
			m_buffsize(abuffsize),
			m_pos(0)
		{}

		~dbuff() = default;

		char* buff()
		{
			return m_buff.get();
		}

		int32_t buffsize()
		{
			return m_buffsize;
		}

		int32_t& pos()
		{
			return m_pos;
		}
	};

	class db_buff
	{
		db_buff(const db_buff&) = delete;
		db_buff& operator=(const db_buff&) = delete;

		enum
		{
			e_buffsize = 10 * 1024 * 1024, // Default inline buffer for common DB rows.
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
				return m_mallocbuff->buff();
			}
			return m_buff->buff();
		}

		inline int32_t buffsize()
		{
			if (m_mallocbuff != nullptr)
			{
				return m_mallocbuff->buffsize();
			}
			return m_buff->buffsize();
		}

		inline int32_t pos()
		{
			if (m_mallocbuff != nullptr)
			{
				return m_mallocbuff->pos();
			}
			return m_buff->pos();
		}

		inline void reset()noexcept
		{
			if (m_mallocbuff != nullptr)
			{
				m_mallocbuff = nullptr;
			}
			m_buff->pos() = 0;
		}

		template <typename T>
		inline bool do_binary(T& adata, dbuff* abuff)
		{
			ngl::ser::serialize_push lserialize(abuff->buff(), abuff->buffsize());
			if (ngl::ser::nserialize::push(&lserialize, adata))
			{
				abuff->pos() = lserialize.pos();
				return true;
			}
			return false;
		}

		template <typename T>
		inline bool serialize(bool isbinary, T& adata)
		{
			if(isbinary)
			{
				// Try the shared inline buffer first; fall back to a right-sized
				// heap buffer for unusually large blobs.
				if (!do_binary(adata, m_buff.get()))
				{
					log_error()->print("do_binary fail T=[{}:{}]", tools::type_name<T>(), adata.mid());
					ngl::ser::serialize_byte lbyte;
					ngl::ser::nserialize::bytes(&lbyte, adata);
					m_mallocbuff = std::make_shared<dbuff>(lbyte.pos());
					if (!do_binary(adata, m_mallocbuff.get()))
					{
						log_error()->print("do_binary fail T=[{}:{}]", tools::type_name<T>(), adata.mid());
						return false;
					}
				}
			}
			else
			{
				// Text mode keeps protobuf rows in JSON so they stay readable in
				// external tools and manual SQL inspection.
				std::string ltemp;
				if constexpr (is_protobuf_message<T>::value)
				{
					if (!tools::proto2json(adata, ltemp))
					{
						return false;
					}
				}
				else
				{
					if (!tools::custom2json(adata, ltemp))
					{
						return false;
					}
				}
				const std::size_t lneed = ltemp.size() + 1;
				if (lneed > static_cast<std::size_t>(std::numeric_limits<int32_t>::max()))
				{
					log_error()->print("db_buff::serialize fail size={}", lneed);
					return false;
				}
				const int32_t lneedpos = static_cast<int32_t>(lneed);

				if (lneed > static_cast<std::size_t>(m_buff->buffsize()))
				{
					m_mallocbuff = std::make_shared<dbuff>(lneedpos);
					memcpy(m_mallocbuff->buff(), ltemp.c_str(), lneed);
					m_mallocbuff->pos() = lneedpos;
				}
				else
				{
					memcpy(m_buff->buff(), ltemp.c_str(), lneed);
					m_buff->pos() = lneedpos;
				}
			}
			return true;
		}

		template <typename T>
		inline bool unserialize(bool isbinary, T& adata, const char* abuff, std::size_t alen)
		{
			if (isbinary)
			{
				if (alen > static_cast<std::size_t>(std::numeric_limits<int32_t>::max()))
				{
					log_error()->print("db_buff::unserialize fail size={}", alen);
					return false;
				}
				ngl::ser::serialize_pop lserialize(abuff, static_cast<int32_t>(alen));
				return ngl::ser::nserialize::pop(&lserialize, adata);
			}
			else
			{
				if constexpr (is_protobuf_message<T>::value)
				{
					return tools::json2proto(abuff, adata);
				}
				else
				{
					return tools::json2custom(abuff, adata);
				}
			}
		}
	};
}// namespace ngl
