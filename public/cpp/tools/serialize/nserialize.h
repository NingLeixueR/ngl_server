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

#include "net.pb.h"
#include "tools.h"

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
#include <vector>
#include <string>
#include <list>
#include <set>
#include <map>

namespace ngl
{
	namespace ser
	{
		// # TBUFF char* 对应push
		// # TBUFF const char* 对应pop
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

		class serialize_push : public serialize<char*>
		{
		public:
			serialize_push(char* abuff, int32_t alen) :
				serialize<char*>(abuff, alen)
			{}

			virtual bool basetype(void* adata, int32_t abytes)
			{
				if (pos() + abytes > len())
				{
					return false;
				}
				memcpy(&buff()[pos()], adata, abytes);
				move_pos(abytes);
				return true;
			}
		};

		class serialize_pop : public serialize<const char*>
		{
		public:
			serialize_pop(const char* abuff, int32_t alen) :
				serialize<const char*>(abuff, alen)
			{}

			virtual bool basetype(void* adata, int32_t abytes)
			{
				if (pos() + abytes > len())
				{
					return false;
				}
				memcpy(adata, &buff()[pos()], abytes);
				move_pos(abytes);
				return true;
			}
		};

		template <typename T>
		struct serialize_format
		{
			static bool push(serialize_push* aserialize, const T& adata);
			static bool pop(serialize_pop* aserialize, T& adata);
			static void bytes(serialize_byte* aserialize, const T& adata);
		};

		template <typename T>
		struct serialize_format<T*>
		{
			static bool push(serialize_push* aserialize, const T* adata)
			{
				if (adata == nullptr)
				{
					return false;
				}
				return serialize_format<T>::push(aserialize, *adata);
			}

			static bool pop(serialize_pop* aserialize, T* adata)
			{
				if (adata == nullptr)
				{
					return false;
				}
				return serialize_format<T>::pop(aserialize, *adata);
			}

			static void bytes(serialize_byte* aserialize, const T* adata)
			{
				if (adata == nullptr)
				{
					return;
				}
				serialize_format<T>::bytes(aserialize, *adata);
			}
		};

		template <>
		struct serialize_format<int8_t>
		{
			static bool push(serialize_push* aserialize, const int8_t adata)
			{
				return aserialize->basetype((void*)&adata, sizeof(adata));
			}

			static bool pop(serialize_pop* aserialize, int8_t& adata)
			{
				return aserialize->basetype((void*)&adata, sizeof(adata));
			}

			static void bytes(serialize_byte* aserialize, const int8_t adata)
			{
				aserialize->move_pos(sizeof(adata));
			}
		};

		template <>
		struct serialize_format<int16_t>
		{
			static bool push(serialize_push* aserialize, const int16_t adata)
			{
				tools::parm<int16_t> lparm(adata);
				lparm.m_value = tools::transformlittle(lparm);
				return aserialize->basetype((void*)&adata, sizeof(adata));
			}

			static bool pop(serialize_pop* aserialize, int16_t& adata)
			{
				if (!aserialize->basetype((void*)&adata, sizeof(adata)))
				{
					return false;
				}
				tools::parm<int16_t> lparm(adata);
				adata = tools::transformlittle(lparm);
				return true;
			}

			static void bytes(serialize_byte* aserialize, const int16_t adata)
			{
				aserialize->move_pos(sizeof(adata));
			}
		};

		template <>
		struct serialize_format<int32_t>
		{
			static bool push(serialize_push* aserialize, const int32_t adata)
			{
				tools::parm<int32_t> lparm(adata);
				lparm.m_value = tools::transformlittle(lparm);
				return aserialize->basetype((void*)&adata, sizeof(adata));
			}

			static bool pop(serialize_pop* aserialize, int32_t& adata)
			{
				if (!aserialize->basetype((void*)&adata, sizeof(adata)))
				{
					return false;
				}
				tools::parm<int32_t> lparm(adata);
				adata = tools::transformlittle(lparm);
				return true;
			}

			static void bytes(serialize_byte* aserialize, const int32_t adata)
			{
				aserialize->move_pos(sizeof(adata));
			}
		};

		template <>
		struct serialize_format<int64_t>
		{
			static bool push(serialize_push* aserialize, const int64_t adata)
			{
				tools::parm<int64_t> lparm(adata);
				lparm.m_value = tools::transformlittle(lparm);
				return aserialize->basetype((void*)&adata, sizeof(adata));
			}

			static bool pop(serialize_pop* aserialize, int64_t& adata)
			{
				if (!aserialize->basetype((void*)&adata, sizeof(adata)))
				{
					return false;
				}
				tools::parm<int64_t> lparm(adata);
				adata = tools::transformlittle(lparm);
				return true;
			}

			static void bytes(serialize_byte* aserialize, const int64_t adata)
			{
				aserialize->move_pos(sizeof(adata));
			}
		};

		template <>
		struct serialize_format<uint8_t>
		{
			static bool push(serialize_push* aserialize, const uint8_t adata)
			{
				return serialize_format<int8_t>::push(aserialize, (int8_t)adata);
			}

			static bool pop(serialize_pop* aserialize, uint8_t& adata)
			{
				int8_t lvalue = 0;
				if (serialize_format<int8_t>::pop(aserialize, lvalue))
				{
					adata = (uint8_t)lvalue;
					return true;
				}
				return false;
			}

			static void bytes(serialize_byte* aserialize, const uint8_t adata)
			{
				serialize_format<int8_t>::bytes(aserialize, adata);
			}
		};

		template <>
		struct serialize_format<uint16_t>
		{
			static bool push(serialize_push* aserialize, const uint16_t adata)
			{
				return serialize_format<int16_t>::push(aserialize, (int16_t)adata);
			}

			static bool pop(serialize_pop* aserialize, uint16_t& adata)
			{
				int16_t lvalue = 0;
				if (serialize_format<int16_t>::pop(aserialize, lvalue))
				{
					adata = (uint16_t)lvalue;
					return true;
				}
				return false;
			}

			static void bytes(serialize_byte* aserialize, const uint16_t adata)
			{
				serialize_format<int16_t>::bytes(aserialize, adata);
			}
		};

		template <>
		struct serialize_format<uint32_t>
		{
			static bool push(serialize_push* aserialize, const uint32_t adata)
			{
				return serialize_format<uint32_t>::push(aserialize, adata);
			}

			static bool pop(serialize_pop* aserialize, uint32_t& adata)
			{
				int32_t lvalue = 0;
				if (serialize_format<int32_t>::pop(aserialize, lvalue))
				{
					adata = lvalue;
					return true;
				}
				return false;
			}

			static void bytes(serialize_byte* aserialize, const uint32_t adata)
			{
				serialize_format<int32_t>::bytes(aserialize, (int32_t)adata);
			}
		};

		template <>
		struct serialize_format<uint64_t>
		{
			static bool push(serialize_push* aserialize, const uint64_t adata)
			{
				return serialize_format<int64_t>::push(aserialize, adata);
			}

			static bool pop(serialize_pop* aserialize, uint64_t& adata)
			{
				int64_t lvalue = 0;
				if (serialize_format<int64_t>::pop(aserialize, lvalue))
				{
					adata = (uint64_t)lvalue;
					return true;
				}
				return false;
			}

			static void bytes(serialize_byte* aserialize, const uint64_t adata)
			{
				serialize_format<int64_t>::bytes(aserialize, (int64_t)adata);
			}
		};

		template <>
		struct serialize_format<bool>
		{
			static bool push(serialize_push* aserialize, const bool adata)
			{
				int8_t lvalue = adata ? 1 : 0;
				return serialize_format<int8_t>::push(aserialize, lvalue);
			}

			static bool pop(serialize_pop* aserialize, bool& adata)
			{
				int8_t lvalue = 0;
				if (serialize_format<int8_t>::pop(aserialize, lvalue))
				{
					adata = lvalue == 0 ? false : true;
					return true;
				}
				return false;
			}

			static void bytes(serialize_byte* aserialize, const bool adata)
			{
				serialize_format<int8_t>::bytes(aserialize, adata ? 1 : 0);
			}
		};

		template <>
		struct serialize_format<float>
		{
			static bool push(serialize_push* aserialize, const float adata)
			{
				return aserialize->basetype((void*)&adata, sizeof(adata));
			}

			static bool pop(serialize_pop* aserialize, float& adata)
			{
				return aserialize->basetype((void*)&adata, sizeof(adata));
			}

			static void bytes(serialize_byte* aserialize, const float adata)
			{
				aserialize->move_pos(sizeof(adata));
			}
		};

		template <>
		struct serialize_format<double>
		{
			static bool push(serialize_push* aserialize, const double adata)
			{
				return aserialize->basetype((void*)&adata, sizeof(adata));
			}

			static bool pop(serialize_pop* aserialize, double& adata)
			{
				return aserialize->basetype((void*)&adata, sizeof(adata));
			}

			static void bytes(serialize_byte* aserialize, const double adata)
			{
				aserialize->move_pos(sizeof(adata));
			}
		};

		template <>
		struct serialize_format<std::string>
		{
			enum
			{
				eserialize_format_bytes = 102400,
			};
			static bool push(serialize_push* aserialize, const std::string& adata)
			{
				int32_t lsize = (int32_t)adata.size();
				if (!serialize_format<int32_t>::push(aserialize, lsize))
				{
					return false;
				}
				return aserialize->basetype((void*)adata.c_str(), lsize);
			}

			static bool pop(serialize_pop* aserialize, std::string& adata)
			{
				int32_t lsize = 0;
				if (!serialize_format<int32_t>::pop(aserialize, lsize))
				{
					return false;
				}
				if (lsize > eserialize_format_bytes)
				{
					return false;
				}
				adata.resize(lsize);
				return aserialize->basetype((void*)adata.data(), lsize);
			}

			static void bytes(serialize_byte* aserialize, const std::string& adata)
			{
				int32_t lsize = (int32_t)adata.size();
				serialize_format<int32_t>::bytes(aserialize, lsize);
				aserialize->move_pos(lsize);
			}
		};

		template <typename T>
		struct serialize_format<std::vector<T>>
		{
			static bool push(serialize_push* aserialize, const std::vector<T>& adata)
			{
				int32_t lsize = (int32_t)adata.size();
				if (!aserialize->basetype((void*)&lsize, sizeof(lsize)))
				{
					return false;
				}
				for (auto itor = adata.begin(); itor != adata.end(); ++itor)
				{
					if (!serialize_format<T>::push(aserialize, *itor))
					{
						return false;
					}
				}
				return true;
			}

			static bool pop(serialize_pop* aserialize, std::vector<T>& adata)
			{
				int32_t lsize = 0;
				if (!aserialize->basetype((void*)&lsize, sizeof(lsize)))
				{
					return false;
				}
				if (lsize > 0)
				{
					adata.resize(lsize);
				}
				for (int32_t i = 0; i < lsize; ++i)
				{
					if (!serialize_format<T>::pop(aserialize, adata[i]))
					{
						return false;
					}
				}
				return true;
			}

			static void bytes(serialize_byte* aserialize, const std::vector<T>& adata)
			{
				int32_t lsize = (int32_t)adata.size();
				serialize_format<int32_t>::bytes(aserialize, lsize);
				for (auto itor = adata.begin(); itor != adata.end(); ++itor)
				{
					serialize_format<T>::bytes(aserialize, *itor);
				}
			}
		};

		template <typename T>
		struct serialize_format<std::set<T>>
		{
			static bool push(serialize_push* aserialize, const std::set<T>& adata)
			{
				int32_t lsize = (int32_t)adata.size();
				if (!aserialize->basetype((void*)&lsize, sizeof(lsize)))
				{
					return false;
				}
				for (auto itor = adata.begin(); itor != adata.end(); ++itor)
				{
					if (!serialize_format<T>::push(aserialize, *itor))
					{
						return false;
					}
				}
				return true;
			}

			static bool pop(serialize_pop* aserialize, std::set<T>& adata)
			{
				int32_t lsize = 0;
				if (!aserialize->basetype((void*)&lsize, sizeof(lsize)))
				{
					return false;
				}
				for (int32_t i = 0; i < lsize; ++i)
				{
					T ltemp;
					if (!serialize_format<T>::pop(aserialize, ltemp))
					{
						return false;
					}
					adata.insert(ltemp);
				}
				return true;
			}

			static void bytes(serialize_byte* aserialize, const std::set<T>& adata)
			{
				int32_t lsize = (int32_t)adata.size();
				serialize_format<int32_t>::bytes(aserialize, lsize);
				for (auto itor = adata.begin(); itor != adata.end(); ++itor)
				{
					serialize_format<T>::bytes(aserialize, *itor);
				}
			}
		};

		template <typename T>
		struct serialize_format<std::list<T>>
		{
			static bool push(serialize_push* aserialize, const std::list<T>& adata)
			{
				int32_t lsize = adata.size();
				if (!aserialize->basetype((void*)&lsize, sizeof(lsize)))
				{
					return false;
				}
				for (auto itor = adata.begin(); itor != adata.end(); ++itor)
				{
					if (!serialize_format<T>::push(aserialize, *itor))
					{
						return false;
					}
				}
				return true;
			}

			static bool pop(serialize_pop* aserialize, std::list<T>& adata)
			{
				int32_t lsize = 0;
				if (!aserialize->basetype((void*)&lsize, sizeof(lsize)))
				{
					return false;
				}
				for (int32_t i = 0; i < lsize; ++i)
				{
					T ltemp;
					if (!serialize_format<T>::pop(aserialize, ltemp))
					{
						return false;
					}
					adata.push_back(ltemp);
				}
				return true;
			}

			static void bytes(serialize_byte* aserialize, const std::list<T>& adata)
			{
				int32_t lsize = adata.size();
				serialize_format<int32_t>::bytes(aserialize, lsize);
				for (auto itor = adata.begin(); itor != adata.end(); ++itor)
				{
					serialize_format<T>::bytes(aserialize, *itor);
				}
			}
		};

		template <typename TKEY, typename TVAL>
		struct serialize_format<std::map<TKEY, TVAL>>
		{
			static bool push(serialize_push* aserialize, const std::map<TKEY, TVAL>& adata)
			{
				int32_t lsize = (int32_t)adata.size();
				if (!aserialize->basetype((void*)&lsize, sizeof(lsize)))
				{
					return false;
				}
				for (auto itor = adata.begin(); itor != adata.end(); ++itor)
				{
					if (!serialize_format<TKEY>::push(aserialize, itor->first))
					{
						return false;
					}
					if (!serialize_format<TVAL>::push(aserialize, itor->second))
					{
						return false;
					}
				}
				return true;
			}

			static bool pop(serialize_pop* aserialize, std::map<TKEY, TVAL>& adata)
			{
				int32_t lsize = 0;
				if (!aserialize->basetype((void*)&lsize, sizeof(lsize)))
				{
					return false;
				}
				for (int32_t i = 0; i < lsize; ++i)
				{
					TKEY ltempkey;
					if (!serialize_format<TKEY>::pop(aserialize, ltempkey))
					{
						return false;
					}
					TVAL ltempvalue;
					if (!serialize_format<TVAL>::pop(aserialize, ltempvalue))
					{
						return false;
					}
					adata.insert(std::make_pair(ltempkey, ltempvalue));
				}
				return true;
			}

			static void bytes(serialize_byte* aserialize, const std::map<TKEY, TVAL>& adata)
			{
				int32_t lsize = (int32_t)adata.size();
				serialize_format<int32_t>::bytes(aserialize, lsize);
				for (auto itor = adata.begin(); itor != adata.end(); ++itor)
				{
					serialize_format<TKEY>::bytes(aserialize, itor->first);
					serialize_format<TVAL>::bytes(aserialize, itor->second);
				}
			}
		};

		struct nserialize
		{
			template <typename ...ARGS>
			static bool push(serialize_push* aserialize, const ARGS&... aargs)
			{
				return true && (serialize_format<ARGS>::push(aserialize, aargs) && ...);
			}

			template <typename ...ARGS>
			static bool pop(serialize_pop* aserialize, ARGS&... aargs)
			{
				return true && (serialize_format<ARGS>::pop(aserialize, aargs) && ...);
			}

			template <typename ...ARGS>
			static void bytes(serialize_byte* aserialize, const ARGS&... aargs)
			{
				(serialize_format<ARGS>::bytes(aserialize, aargs), ...);
			}
		};
	}// namespace ser
}// namespace ngl


namespace ngl
{
	namespace ser
	{
		template <typename T>
		bool serialize_format<T>::push(serialize_push* aserialize, const T& adata)
		{
			if constexpr (std::is_enum<T>::value)
			{
				return serialize_format<int32_t>::push(aserialize, (int32_t)adata);
			}
			else if constexpr (is_protobuf_message<T>::value)
			{
				int32_t lbytes = adata.ByteSize();
				if (!serialize_format<int32_t>::push(aserialize, lbytes))
				{
					return false;
				}
				if (lbytes > (aserialize->len() - aserialize->pos()))
				{
					return false;
				}
				if (!adata.SerializeToArray(&aserialize->buff()[aserialize->pos()], lbytes))
				{
					return false;
				}
				aserialize->move_pos(lbytes);
				return true;
			}
			else
			{
				return adata.push_format(aserialize);
			}
		}

		template <typename T>
		bool serialize_format<T>::pop(serialize_pop* aserialize, T& adata)
		{
			if constexpr (std::is_enum<T>::value)
			{
				int32_t lvalues = 0;
				if (!serialize_format<int32_t>::pop(aserialize, lvalues))
				{
					return false;
				}
				adata = (T)lvalues;
				return true;
			}
			else if constexpr (is_protobuf_message<T>::value)
			{
				int32_t lbytes = 0;
				if (!serialize_format<int32_t>::pop(aserialize, lbytes))
				{
					return false;
				}
				if (lbytes > (aserialize->len() - aserialize->pos()))
				{
					return false;
				}
				if (!adata.ParseFromArray(&aserialize->buff()[aserialize->pos()], lbytes))
				{
					return false;
				}
				aserialize->move_pos(lbytes);
				return true;
			}
			else
			{
				return adata.pop_format(aserialize);
			}
		}

		template <typename T>
		void serialize_format<T>::bytes(serialize_byte* aserialize, const T& adata)
		{
			if constexpr (std::is_enum<T>::value)
			{
				int32_t lvalue = (int32_t)adata;
				serialize_format<int32_t>::bytes(aserialize, lvalue);
			}
			else if constexpr (is_protobuf_message<T>::value)
			{
				int32_t lvalue = (int32_t)adata.ByteSize();
				serialize_format<int32_t>::bytes(aserialize, lvalue);
				aserialize->move_pos(lvalue);
			}
			else
			{
				adata.bytes_format(aserialize);
			}
		}
	}//namespce ser
}//namespce ngl