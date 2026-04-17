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
// File overview: Template-based serialization engine mapping C++ types to binary wire format.

#pragma once

#include "actor/generated/pb/net.pb.h"
#include "tools/tools.h"

#include <google/protobuf/util/json_util.h>
#include <google/protobuf/message_lite.h>
#include <google/protobuf/descriptor.h>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <cstdint>
#include <cstring>
#include <memory>
#include <cstdio>
#include <limits>
#include <vector>
#include <string>
#include <utility>
#include <list>
#include <set>
#include <map>

namespace ngl
{
	namespace ser
	{
		// `serialize<char*>` writes bytes into a mutable buffer, while
		// `serialize<const char*>` reads bytes back out of an immutable buffer.
		template <typename TBUFF>
		class serialize
		{
			TBUFF m_buff;
			int32_t m_len = 0;
			int32_t m_pos = 0;
		public:
			serialize(TBUFF abuff, int32_t alen) :
				m_buff(abuff),
				m_len(alen),
				m_pos(0)
			{}

			inline TBUFF buff()
			{
				return m_buff;
			}

			inline int& pos()
			{
				return m_pos;
			}

			inline int& len()
			{
				return m_len;
			}

			inline int move_pos(int abytes)
			{
				m_pos += abytes;
				return m_pos;
			}

			virtual bool basetype(void* adata, int32_t abytes) = 0;
		};

		class serialize_byte
		{
			int32_t m_pos = 0;
		public:
			inline int32_t pos()
			{
				return m_pos;
			}

			inline int move_pos(int abytes)
			{
				m_pos += abytes;
				return m_pos;
			}
		};

		class serialize_push : 
			public serialize<char*>
		{
		public:
			serialize_push(char* abuff, int32_t alen) :
				serialize<char*>(abuff, alen)
			{}

			virtual bool basetype(void* adata, int32_t abytes)
			{
				const int32_t lpos = pos();
				const int32_t llen = len();
				if (abytes < 0 || lpos < 0 || llen < 0 || lpos > llen || abytes > (llen - lpos))
				{
					return false;
				}
				if (abytes == 0)
				{
					return true;
				}
				if (adata == nullptr)
				{
					return false;
				}
				if (buff() == nullptr)
				{
					return false;
				}
				memcpy(&buff()[lpos], adata, (size_t)abytes);
				move_pos(abytes);
				return true;
			}
		};

		class serialize_pop : 
			public serialize<const char*>
		{
		public:
			serialize_pop(const char* abuff, int32_t alen) :
				serialize<const char*>(abuff, alen)
			{}

			virtual bool basetype(void* adata, int32_t abytes)
			{
				const int32_t lpos = pos();
				const int32_t llen = len();
				if (abytes < 0 || lpos < 0 || llen < 0 || lpos > llen || abytes > (llen - lpos))
				{
					return false;
				}
				if (abytes == 0)
				{
					return true;
				}
				if (adata == nullptr)
				{
					return false;
				}
				if (buff() == nullptr)
				{
					return false;
				}
				memcpy(adata, &buff()[lpos], (size_t)abytes);
				move_pos(abytes);
				return true;
			}
		};

		template <typename T>
		struct serialize_format
		{
			static bool push(serialize_push* aser, const T& adata);
			static bool pop(serialize_pop* aser, T& adata);
			static void bytes(serialize_byte* aser, const T& adata);
		};

		template <typename T>
		struct serialize_format<T*>
		{
			static bool push(serialize_push* aser, const T* adata)
			{
				if (adata == nullptr)
				{
					return false;
				}
				return serialize_format<T>::push(aser, *adata);
			}

			static bool pop(serialize_pop* aser, T* adata)
			{
				if (adata == nullptr)
				{
					return false;
				}
				return serialize_format<T>::pop(aser, *adata);
			}

			static void bytes(serialize_byte* aser, const T* adata)
			{
				if (adata == nullptr)
				{
					return;
				}
				serialize_format<T>::bytes(aser, *adata);
			}
		};

		template <typename T>
		struct serialize_format<std::shared_ptr<T>>
		{
			static bool push(serialize_push* aser, const std::shared_ptr<T>& adata)
			{
				if (adata == nullptr)
				{
					return false;
				}
				return serialize_format<T>::push(aser, *adata);
			}

			static bool pop(serialize_pop* aser, std::shared_ptr<T>& adata)
			{
				if (adata == nullptr)
				{
					return false;
				}
				return serialize_format<T>::pop(aser, *adata);
			}

			static void bytes(serialize_byte* aser, const std::shared_ptr<T>& adata)
			{
				if (adata == nullptr)
				{
					return;
				}
				serialize_format<T>::bytes(aser, *adata);
			}
		};

		template <>
		struct serialize_format<int8_t>
		{
			static bool push(serialize_push* aser, const int8_t adata)
			{
				return aser->basetype((void*)&adata, sizeof(adata));
			}

			static bool pop(serialize_pop* aser, int8_t& adata)
			{
				return aser->basetype((void*)&adata, sizeof(adata));
			}

			static void bytes(serialize_byte* aser, const int8_t adata)
			{
				aser->move_pos(sizeof(adata));
			}
		};

		template <>
		struct serialize_format<int16_t>
		{
			static bool push(serialize_push* aser, const int16_t adata)
			{
				tools::parm<int16_t> lparm(adata);
				lparm.m_value = tools::transformlittle(lparm);
				return aser->basetype((void*)&lparm.m_value, sizeof(lparm.m_value));
			}

			static bool pop(serialize_pop* aser, int16_t& adata)
			{
				if (!aser->basetype((void*)&adata, sizeof(adata)))
				{
					return false;
				}
				tools::parm<int16_t> lparm(adata);
				adata = tools::transformlittle(lparm);
				return true;
			}

			static void bytes(serialize_byte* aser, const int16_t adata)
			{
				aser->move_pos(sizeof(adata));
			}
		};

		template <>
		struct serialize_format<int32_t>
		{
			static bool push(serialize_push* aser, const int32_t adata)
			{
				tools::parm<int32_t> lparm(adata);
				lparm.m_value = tools::transformlittle(lparm);
				return aser->basetype((void*)&lparm.m_value, sizeof(lparm.m_value));
			}

			static bool pop(serialize_pop* aser, int32_t& adata)
			{
				if (!aser->basetype((void*)&adata, sizeof(adata)))
				{
					return false;
				}
				tools::parm<int32_t> lparm(adata);
				adata = tools::transformlittle(lparm);
				return true;
			}

			static void bytes(serialize_byte* aser, const int32_t adata)
			{
				aser->move_pos(sizeof(adata));
			}
		};

		template <>
		struct serialize_format<int64_t>
		{
			static bool push(serialize_push* aser, const int64_t adata)
			{
				tools::parm<int64_t> lparm(adata);
				lparm.m_value = tools::transformlittle(lparm);
				return aser->basetype((void*)&lparm.m_value, sizeof(lparm.m_value));
			}

			static bool pop(serialize_pop* aser, int64_t& adata)
			{
				if (!aser->basetype((void*)&adata, sizeof(adata)))
				{
					return false;
				}
				tools::parm<int64_t> lparm(adata);
				adata = tools::transformlittle(lparm);
				return true;
			}

			static void bytes(serialize_byte* aser, const int64_t adata)
			{
				aser->move_pos(sizeof(adata));
			}
		};

		template <>
		struct serialize_format<uint8_t>
		{
			static bool push(serialize_push* aser, const uint8_t adata)
			{
				return serialize_format<int8_t>::push(aser, (int8_t)adata);
			}

			static bool pop(serialize_pop* aser, uint8_t& adata)
			{
				int8_t lvalue = 0;
				if (serialize_format<int8_t>::pop(aser, lvalue))
				{
					adata = (uint8_t)lvalue;
					return true;
				}
				return false;
			}

			static void bytes(serialize_byte* aser, const uint8_t adata)
			{
				serialize_format<int8_t>::bytes(aser, adata);
			}
		};

		template <>
		struct serialize_format<uint16_t>
		{
			static bool push(serialize_push* aser, const uint16_t adata)
			{
				tools::parm<uint16_t> lparm(adata);
				lparm.m_value = tools::transformlittle(lparm);
				return aser->basetype((void*)&lparm.m_value, sizeof(lparm.m_value));
			}

			static bool pop(serialize_pop* aser, uint16_t& adata)
			{
				if (!aser->basetype((void*)&adata, sizeof(adata)))
				{
					return false;
				}
				tools::parm<uint16_t> lparm(adata);
				adata = tools::transformlittle(lparm);
				return true;
			}

			static void bytes(serialize_byte* aser, const uint16_t adata)
			{
				serialize_format<int16_t>::bytes(aser, adata);
			}
		};

		template <>
		struct serialize_format<uint32_t>
		{
			static bool push(serialize_push* aser, const uint32_t adata)
			{
				tools::parm<uint32_t> lparm(adata);
				lparm.m_value = tools::transformlittle(lparm);
				return aser->basetype((void*)&lparm.m_value, sizeof(lparm.m_value));
			}

			static bool pop(serialize_pop* aser, uint32_t& adata)
			{
				if (!aser->basetype((void*)&adata, sizeof(adata)))
				{
					return false;
				}
				tools::parm<uint32_t> lparm(adata);
				adata = tools::transformlittle(lparm);
				return true;
			}

			static void bytes(serialize_byte* aser, const uint32_t adata)
			{
				serialize_format<int32_t>::bytes(aser, (int32_t)adata);
			}
		};

		template <>
		struct serialize_format<uint64_t>
		{
			static bool push(serialize_push* aser, const uint64_t adata)
			{
				tools::parm<uint64_t> lparm(adata);
				lparm.m_value = tools::transformlittle(lparm);
				return aser->basetype((void*)&lparm.m_value, sizeof(lparm.m_value));
			}

			static bool pop(serialize_pop* aser, uint64_t& adata)
			{
				if (!aser->basetype((void*)&adata, sizeof(adata)))
				{
					return false;
				}
				tools::parm<uint64_t> lparm(adata);
				adata = tools::transformlittle(lparm);
				return true;
			}

			static void bytes(serialize_byte* aser, const uint64_t adata)
			{
				serialize_format<int64_t>::bytes(aser, (int64_t)adata);
			}
		};

		template <>
		struct serialize_format<bool>
		{
			static bool push(serialize_push* aser, const bool adata)
			{
				int8_t lvalue = adata ? 1 : 0;
				return serialize_format<int8_t>::push(aser, lvalue);
			}

			static bool pop(serialize_pop* aser, bool& adata)
			{
				int8_t lvalue = 0;
				if (serialize_format<int8_t>::pop(aser, lvalue))
				{
					adata = lvalue == 0 ? false : true;
					return true;
				}
				return false;
			}

			static void bytes(serialize_byte* aser, const bool adata)
			{
				serialize_format<int8_t>::bytes(aser, adata ? 1 : 0);
			}
		};

		template <>
		struct serialize_format<float>
		{
			static bool push(serialize_push* aser, const float adata)
			{
				return aser->basetype((void*)&adata, sizeof(adata));
			}

			static bool pop(serialize_pop* aser, float& adata)
			{
				return aser->basetype((void*)&adata, sizeof(adata));
			}

			static void bytes(serialize_byte* aser, const float adata)
			{
				aser->move_pos(sizeof(adata));
			}
		};

		template <>
		struct serialize_format<double>
		{
			static bool push(serialize_push* aser, const double adata)
			{
				return aser->basetype((void*)&adata, sizeof(adata));
			}

			static bool pop(serialize_pop* aser, double& adata)
			{
				return aser->basetype((void*)&adata, sizeof(adata));
			}

			static void bytes(serialize_byte* aser, const double adata)
			{
				aser->move_pos(sizeof(adata));
			}
		};

		template <>
		struct serialize_format<std::string>
		{
			enum
			{
				eserialize_format_bytes = 102400,
			};
			static bool push(serialize_push* aser, const std::string& adata)
			{
				const int32_t lpos = aser->pos();
				if (adata.size() > eserialize_format_bytes || adata.size() > (size_t)std::numeric_limits<int32_t>::max())
				{
					return false;
				}
				const int32_t lsize = (int32_t)adata.size();
				const int32_t lremain = aser->len() - aser->pos();
				if (lremain < (int32_t)sizeof(int32_t) || lsize > (lremain - (int32_t)sizeof(int32_t)))
				{
					return false;
				}
				if (!serialize_format<int32_t>::push(aser, lsize))
				{
					aser->pos() = lpos;
					return false;
				}
				if (!aser->basetype((void*)adata.c_str(), lsize))
				{
					aser->pos() = lpos;
					return false;
				}
				return true;
			}

			static bool pop(serialize_pop* aser, std::string& adata)
			{
				int32_t lsize = 0;
				if (!serialize_format<int32_t>::pop(aser, lsize))
				{
					return false;
				}
				if (lsize < 0 || lsize > eserialize_format_bytes || lsize > (aser->len() - aser->pos()))
				{
					return false;
				}
				std::string ltemp;
				ltemp.resize(lsize);
				if (lsize > 0 && !aser->basetype((void*)ltemp.data(), lsize))
				{
					return false;
				}
				adata.swap(ltemp);
				return true;
			}

			static void bytes(serialize_byte* aser, const std::string& adata)
			{
				int32_t lsize = (int32_t)adata.size();
				serialize_format<int32_t>::bytes(aser, lsize);
				aser->move_pos(lsize);
			}
		};

		template <typename T>
		struct serialize_format<std::vector<T>>
		{
			static bool push(serialize_push* aser, const std::vector<T>& adata)
			{
				const int32_t lpos = aser->pos();
				if (adata.size() > (size_t)std::numeric_limits<int32_t>::max())
				{
					return false;
				}
				const int32_t lsize = (int32_t)adata.size();
				if (!serialize_format<int32_t>::push(aser, lsize))
				{
					return false;
				}
				for (auto& item : adata)
				{
					if (!serialize_format<T>::push(aser, item))
					{
						aser->pos() = lpos;
						return false;
					}
				}
				return true;
			}

			static bool pop(serialize_pop* aser, std::vector<T>& adata)
			{
				int32_t lsize = 0;
				if (!serialize_format<int32_t>::pop(aser, lsize))
				{
					return false;
				}
				if (lsize < 0)
				{
					return false;
				}
				std::vector<T> ltemp;
				ltemp.reserve((size_t)lsize);
				for (int32_t i = 0; i < lsize; ++i)
				{
					T lvalue;
					if (!serialize_format<T>::pop(aser, lvalue))
					{
						return false;
					}
					ltemp.push_back(std::move(lvalue));
				}
				adata.swap(ltemp);
				return true;
			}

			static void bytes(serialize_byte* aser, const std::vector<T>& adata)
			{
				serialize_format<int32_t>::bytes(aser, (int32_t)adata.size());
				for (auto& item : adata)
				{
					serialize_format<T>::bytes(aser, item);
				}
			}
		};

		template <typename T>
		struct serialize_format<std::set<T>>
		{
			static bool push(serialize_push* aser, const std::set<T>& adata)
			{
				const int32_t lpos = aser->pos();
				if (adata.size() > (size_t)std::numeric_limits<int32_t>::max())
				{
					return false;
				}
				const int32_t lsize = (int32_t)adata.size();
				if (!serialize_format<int32_t>::push(aser, lsize))
				{
					return false;
				}
				for (auto& item : adata)
				{
					if (!serialize_format<T>::push(aser, item))
					{
						aser->pos() = lpos;
						return false;
					}
				}
				return true;
			}

			static bool pop(serialize_pop* aser, std::set<T>& adata)
			{
				int32_t lsize = 0;
				if (!serialize_format<int32_t>::pop(aser, lsize))
				{
					return false;
				}
				if (lsize < 0)
				{
					return false;
				}
				std::set<T> lcontainer;
				for (int32_t i = 0; i < lsize; ++i)
				{
					T lvalue;
					if (!serialize_format<T>::pop(aser, lvalue))
					{
						return false;
					}
					const auto [_it, inserted] = lcontainer.insert(std::move(lvalue));
					if (!inserted)
					{
						return false;
					}
				}
				adata.swap(lcontainer);
				return true;
			}

			static void bytes(serialize_byte* aser, const std::set<T>& adata)
			{
				serialize_format<int32_t>::bytes(aser, (int32_t)adata.size());
				for (auto& item : adata)
				{
					serialize_format<T>::bytes(aser, item);
				}
			}
		};

		template <typename T>
		struct serialize_format<std::list<T>>
		{
			static bool push(serialize_push* aser, const std::list<T>& adata)
			{
				const int32_t lpos = aser->pos();
				if (adata.size() > (size_t)std::numeric_limits<int32_t>::max())
				{
					return false;
				}
				const int32_t lsize = (int32_t)adata.size();
				if (!serialize_format<int32_t>::push(aser, lsize))
				{
					return false;
				}
				for (auto& item : adata)
				{
					if (!serialize_format<T>::push(aser, item))
					{
						aser->pos() = lpos;
						return false;
					}
				}
				return true;
			}

			static bool pop(serialize_pop* aser, std::list<T>& adata)
			{
				int32_t lsize = 0;
				if (!serialize_format<int32_t>::pop(aser, lsize))
				{
					return false;
				}
				if (lsize < 0)
				{
					return false;
				}
				std::list<T> lcontainer;
				for (int32_t i = 0; i < lsize; ++i)
				{
					T lvalue;
					if (!serialize_format<T>::pop(aser, lvalue))
					{
						return false;
					}
					lcontainer.push_back(std::move(lvalue));
				}
				adata.swap(lcontainer);
				return true;
			}

			static void bytes(serialize_byte* aser, const std::list<T>& adata)
			{
				const int32_t lsize = adata.size() > static_cast<std::size_t>(std::numeric_limits<int32_t>::max())
					? std::numeric_limits<int32_t>::max()
					: static_cast<int32_t>(adata.size());
				serialize_format<int32_t>::bytes(aser, lsize);
				for (auto& item : adata)
				{
					serialize_format<T>::bytes(aser, item);
				}
			}
		};

		template <typename TKEY, typename TVAL>
		struct serialize_format<std::map<TKEY, TVAL>>
		{
			static bool push(serialize_push* aser, const std::map<TKEY, TVAL>& adata)
			{
				const int32_t lpos = aser->pos();
				if (adata.size() > (size_t)std::numeric_limits<int32_t>::max())
				{
					return false;
				}
				const int32_t lsize = (int32_t)adata.size();
				if (!serialize_format<int32_t>::push(aser, lsize))
				{
					return false;
				}
				for (auto& [_key, _value] : adata)
				{
					if (!serialize_format<TKEY>::push(aser, _key))
					{
						aser->pos() = lpos;
						return false;
					}
					if (!serialize_format<TVAL>::push(aser, _value))
					{
						aser->pos() = lpos;
						return false;
					}
				}
				return true;
			}

			static bool pop(serialize_pop* aser, std::map<TKEY, TVAL>& adata)
			{
				int32_t lsize = 0;
				if (!serialize_format<int32_t>::pop(aser, lsize))
				{
					return false;
				}
				if (lsize < 0)
				{
					return false;
				}
				std::map<TKEY, TVAL> ltemp;
				for (int32_t i = 0; i < lsize; ++i)
				{
					std::pair<TKEY, TVAL> lpair;
					if (!serialize_format<TKEY>::pop(aser, lpair.first))
					{
						return false;
					}
					if (!serialize_format<TVAL>::pop(aser, lpair.second))
					{
						return false;
					}
				 	const auto [_it, inserted] = ltemp.insert(std::move(lpair));
					if (!inserted)
					{
						return false;
					}
				}
				adata.swap(ltemp);
				return true;
			}

			static void bytes(serialize_byte* aser, const std::map<TKEY, TVAL>& adata)
			{
				serialize_format<int32_t>::bytes(aser, (int32_t)adata.size());
				for (auto& [_key, _value] : adata)
				{
					serialize_format<TKEY>::bytes(aser, _key);
					serialize_format<TVAL>::bytes(aser, _value);
				}
			}
		};

		struct nserialize
		{
			template <typename ...ARGS>
			static bool push(serialize_push* aser, const ARGS&... aargs)
			{
				if (aser == nullptr)
				{
					return false;
				}
				const int32_t lpos = aser->pos();
				if ((serialize_format<ARGS>::push(aser, aargs) && ...))
				{
					return true;
				}
				aser->pos() = lpos;
				return false;
			}

			template <typename ...ARGS>
			static bool pop(serialize_pop* aser, ARGS&... aargs)
			{
				if (aser == nullptr)
				{
					return false;
				}
				return (serialize_format<ARGS>::pop(aser, aargs) && ...);
			}

			template <typename ...ARGS>
			static void bytes(serialize_byte* aser, const ARGS&... aargs)
			{
				if (aser == nullptr)
				{
					return;
				}
				(serialize_format<ARGS>::bytes(aser, aargs), ...);
			}
		};
	}// namespace ser
}// namespace ngl

namespace ngl
{
	namespace ser
	{
		template <typename T>
		bool serialize_format<T>::push(serialize_push* aser, const T& adata)
		{
			if constexpr (std::is_enum<T>::value)
			{
				return serialize_format<int32_t>::push(aser, (int32_t)adata);
			}
			else if constexpr (tools::is_protobuf_message<T>::value)
			{
				const int32_t lpos = aser->pos();
				const size_t lbytes_long = adata.ByteSizeLong();
				if (lbytes_long > (size_t)std::numeric_limits<int32_t>::max())
				{
					return false;
				}
				const int32_t lbytes = (int32_t)lbytes_long;
				const int32_t lremain = aser->len() - aser->pos();
				if (lremain < (int32_t)sizeof(int32_t) || lbytes > (lremain - (int32_t)sizeof(int32_t)))
				{
					return false;
				}
				if (!serialize_format<int32_t>::push(aser, lbytes))
				{
					aser->pos() = lpos;
					return false;
				}
				if (!adata.SerializeToArray(&aser->buff()[aser->pos()], lbytes))
				{
					aser->pos() = lpos;
					return false;
				}
				aser->move_pos(lbytes);
				return true;
			}
			else
			{
				return adata.push_format(aser);
			}
		}

		template <typename T>
		bool serialize_format<T>::pop(serialize_pop* aser, T& adata)
		{
			if constexpr (std::is_enum<T>::value)
			{
				int32_t lvalues = 0;
				if (!serialize_format<int32_t>::pop(aser, lvalues))
				{
					return false;
				}
				adata = (T)lvalues;
				return true;
			}
			else if constexpr (tools::is_protobuf_message<T>::value)
			{
				int32_t lbytes = 0;
				if (!serialize_format<int32_t>::pop(aser, lbytes))
				{
					return false;
				}
				if (lbytes < 0 || lbytes > (aser->len() - aser->pos()))
				{
					return false;
				}
				T ltemp;
				if (!ltemp.ParseFromArray(&aser->buff()[aser->pos()], lbytes))
				{
					return false;
				}
				aser->move_pos(lbytes);
				adata = std::move(ltemp);
				return true;
			}
			else
			{
				return adata.pop_format(aser);
			}
		}

		template <typename T>
		void serialize_format<T>::bytes(serialize_byte* aser, const T& adata)
		{
			if constexpr (std::is_enum<T>::value)
			{
				serialize_format<int32_t>::bytes(aser, (int32_t)adata);
			}
			else if constexpr (tools::is_protobuf_message<T>::value)
			{
				const auto lbytes = adata.ByteSizeLong();
				const int32_t lvalue = lbytes > (size_t)std::numeric_limits<int32_t>::max()
					? std::numeric_limits<int32_t>::max()
					: (int32_t)lbytes;
				serialize_format<int32_t>::bytes(aser, lvalue);
				aser->move_pos(lvalue);
			}
			else
			{
				adata.bytes_format(aser);
			}
		}
	}//namespce ser
}//namespce ngl
