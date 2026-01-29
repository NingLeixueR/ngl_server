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

#include "nlog.h"

#include <functional>
#include <cstdint>
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
			e_buffsize = 10 * 1024 * 1024, // 单blob上限 10M
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

		inline void reset()
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
		inline bool do_binary(T& adata)
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
		inline void serialize(T& adata, bool isbinary)
		{
			if(isbinary)
			{
				if (!do_binary(adata))
				{
					log_error()->print("do_binary fail T=[{}:{}]", tools::type_name<T>(), adata.mid());
					return;
				}
			}
			else
			{
				std::string ltemp;
				if constexpr (is_protobuf_message<T>::value)
				{
					tools::proto2json(adata, ltemp);
				}
				else
				{
					tools::custom2json(adata, ltemp);
				}
				if (ltemp.size() > m_buff->buffsize())
				{
					m_mallocbuff = std::make_shared<dbuff>(ltemp.size() + 1);
					memcpy(m_mallocbuff->buff(), ltemp.c_str(), ltemp.size() + 1);
					m_mallocbuff->pos() = (int32_t)ltemp.size() + 1;
				}
				else
				{
					memcpy(m_buff->buff(), ltemp.c_str(), ltemp.size() + 1);
					m_buff->pos() = (int32_t)ltemp.size() + 1;
				}
			}
			return;
		}
	};
}// namespace ngl