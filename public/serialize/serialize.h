#pragma once

#include <memory>
#include <functional>
#include <cstdint>
#include <cstdio>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cstring>

#include "csvpair.h"
#include "logserialize.h"
#include "csv.h"
#include "bytes_order.h"
#include "varint.h"
#include "define.h"
#include "db.pb.h"
#include <google/protobuf/util/json_util.h>

namespace ngl
{
	template <typename TBASE>
	struct derived_class;

	struct forward;

	struct serialize_bytes;

	template <typename T, bool IS_FORWARD = false>
	struct protobuf_data
	{
		std::shared_ptr<T> m_data;
		bool m_isbinary;			// 默认序列化为二进制，只有db保存的时候序列化为xml
		
		protobuf_data()
			: m_data(nullptr)
			, m_isbinary(true)
		{}

		protobuf_data(std::shared_ptr<T>& adata) 
			: m_data(adata)
			, m_isbinary(true)
		{}

		void make()
		{
			m_data = std::make_shared<T>();
		}

		bool log(ngl::logserialize& atstr)const
		{
			return true;
		}
	};

	class serialize
	{
		char* m_buff;
		int32_t m_len;
		int32_t m_pos;
	public:
		serialize(char* abuff, int32_t alen);

		char* buff();
		int byte();
		int len();
		void add_bytes(int abytes);
		void dec_bytes(int abytes);
		bool basetype(void* adata, int32_t alen);

		template <typename T>
		bool basetype(const T& adata)
		{
			return basetype((void*)&adata, sizeof(T));
		}

		template <typename T, bool IS_FORWARD>
		bool push(const protobuf_data<T, IS_FORWARD>& adata)
		{
			assert(adata.m_data != nullptr);
			if (adata.m_isbinary)
			{
				int32_t lbytes = adata.m_data->ByteSize();
				if constexpr (IS_FORWARD == false)
				{
					if (push(lbytes) == false)
						return false;
				}
				if (adata.m_data->SerializeToArray(&buff()[byte()], lbytes) == false)
					return false;
				add_bytes(lbytes);
				return true;
			}
			else
			{
				google::protobuf::util::JsonPrintOptions options;
				options.add_whitespace = false;
				options.always_print_primitive_fields = false;
				options.preserve_proto_field_names = true;
				std::string json;
				bool ret = google::protobuf::json::MessageToJsonString(*adata.m_data, &json, options).ok();
				if (ret)
				{
					int32_t len = json.size() + 1;
					memcpy(&buff()[byte()], json.c_str(), len);
					add_bytes(len);
				}
				return ret;
			}
		}

		template <typename KEY, typename VALUE>
		bool push(const protobuf_data<std::map<KEY, VALUE>>& adata)
		{
			assert(adata.m_data != nullptr);
			if (adata.m_isbinary)
			{
				int16_t lsize = adata.m_data->size();
				if (push(lsize) == false)
					return false;
				for (auto itor = adata.m_data->begin(); itor != adata.m_data->end(); ++itor)
				{
					if (push(itor->first) == false)
						return false;
					int32_t lbytes = itor->second.ByteSize();
					if (push(lbytes) == false)
						return false;
					if (itor->second.SerializeToArray(&buff()[byte()], lbytes) == false)
						return false;
					add_bytes(lbytes);
				}
			}			
			return true;
		}

		template <typename T>
		bool push(const protobuf_data<std::vector<T>>& adata)
		{
			assert(adata.m_data != nullptr);
			if (adata.m_isbinary)
			{
				int16_t lsize = adata.m_data->size();
				if (push(lsize) == false)
					return false;
				for (auto itor = adata.m_data->begin(); itor != adata.m_data->end(); ++itor)
				{
					int32_t lbytes = itor->ByteSize();
					if (push(lbytes) == false)
						return false;
					if (itor->SerializeToArray(&buff()[byte()], lbytes) == false)
						return false;
					add_bytes(lbytes);
				}
			}			
			return true;
		}

		template <typename T>
		bool push(const protobuf_data<std::list<T>>& adata)
		{
			assert(adata.m_data != nullptr);
			if (adata.m_isbinary)
			{
				int16_t lsize = adata.m_data->size();
				if (push(lsize) == false)
					return false;
				for (auto itor = adata.m_data->begin(); itor != adata.m_data->end(); ++itor)
				{
					int32_t lbytes = itor->ByteSize();
					if (push(lbytes) == false)
						return false;
					if (itor->SerializeToArray(&buff()[byte()], lbytes) == false)
						return false;
					add_bytes(lbytes);
				}
			}
			return true;
		}

		// 普通指针不支持序列化派生类
		template <typename T>
		bool push(T* adata)
		{
			if (adata == nullptr)
			{
				return push(false);
			}
			if (push(true) == false)
				return false;
			return push(*adata);
		}

		template <typename T>
		bool push(const T* adata)
		{
			return push(*adata);
		}

		template <typename T>
		bool push(const std::shared_ptr<T>& adata)
		{
			return push(*adata);
		}

		template <typename T>
		bool push(const T& adata);

		bool push(const forward& adata);
		bool push(forward& adata);
		// 基本类型
		bool push(const int8_t adata);
		bool push(const uint8_t adata);
		bool push(const int16_t adata);
		bool push(const uint16_t adata);
		bool push(const int32_t adata);
		bool push(const uint32_t adata);
		bool push(const int64_t adata);
		bool push(const uint64_t adata);
		bool push(const float adata);
		bool push(const double adata);
		bool push(const bool adata);
		bool push(const std::vector<bool>& avec);
		bool push(const std::vector<int8_t>& avec);
		bool push(const std::vector<uint8_t>& avec);

		template <typename TSTL>
		bool push_vector_number_compile(const TSTL& avec)
		{
			if (push_stlsize(avec) == false)
				return false;
			if (avec.empty())
				return true;
			for (auto& item : avec)
			{
				if (push(item) == false)
					return false;
			}
			return true;
		}

		bool push(const std::vector<int32_t>& avec);
		bool push(const std::vector<uint32_t>& avec);
		bool push(const std::vector<int64_t>& avec);
		bool push(const std::vector<uint64_t>& avec);
		// 单双浮点型,不需要考虑字节序
		bool push(const std::vector<float>& avec);
		bool push(const std::vector<double>& avec);

		template <typename T>
		bool push_stlsize(const T& astl)
		{
			int16_t lsize = astl.size();
			return push(lsize);
		}

		template <typename T>
		bool push(const std::vector<T>& avec)
		{
			if (!push_stlsize(avec))
				return false;
			for (int i = 0; i < avec.size(); ++i) 
			{
				if (!push(avec[i])) 
					return false;
			}
			return true;
		}

		bool push(std::pair<uint32_t, const char*>& adata);

		template <typename T>
		bool push_vector_number(const std::vector<T>& avec)
		{
			if (push_stlsize(avec) == false)
				return false;
			if (avec.empty())
				return true;
			return basetype((void*)&avec[0], avec.size()*sizeof(T));
		}

		template <typename T>
		bool push(const std::list<T>& alist)
		{
			return push_vector_number_compile(alist);
		}

		template <typename T>
		bool push(const std::set<T>& aset)
		{
			return push_vector_number_compile(aset);
		}

		template <typename T>
		bool push(const std::unordered_set<T>& aset)
		{
			return push_vector_number_compile(aset);
		}

		template <typename TKEY, typename TVALUE>
		bool push(const std::map<TKEY, TVALUE>& amap)
		{
			if (!push_stlsize(amap))
				return false;
			for (const std::pair<const TKEY, TVALUE>& item : amap) 
			{
				if (!push(item.first))
					return false;
				if (!push(item.second))
					return false;
			}
			return true;
		}

		template <typename TKEY, typename TVALUE>
		bool push(const std::unordered_map<TKEY, TVALUE>& amap)
		{
			if (!push_stlsize(amap))
				return false;
			for (std::pair<const TKEY, TVALUE>& item : amap) {
				if (!push(item.first))
					return false;
				if (!push(item.second))
					return false;
			}
			return true;
		}

		bool push(const std::string& astr);
		// 支持没有参数
		bool push();

		template <typename T, typename ...ARG>
		bool push(const T& avalue, const ARG&... arg)
		{
			return push(avalue) && push(arg...);
		}
	};

	class unserialize
	{
		const char* m_buff;
		int32_t m_len;
		int32_t m_pos;

	public:
		unserialize(const char* abuff, int32_t alen);
		const char* buff();
		int byte();
		int len();
		void add_bytes(int abytes);
		void dec_bytes(int abytes);

		template <typename T>
		bool basetype(T& adata)
		{
			if (m_pos + sizeof(T) > m_len)
				return false;
			memcpy(&adata, &m_buff[m_pos], sizeof(T));
			m_pos += sizeof(T);
			return true;
		}

		bool basetype(void* adata, int32_t alen);

		template <typename T, bool IS_FORWARD>
		bool pop(protobuf_data<T, IS_FORWARD>& adata)
		{
			if (adata.m_data == nullptr)
				adata.make();
			if (adata.m_isbinary)
			{
				int32_t lbytes = len() - byte();
				T& ldata = *adata.m_data.get();
				if constexpr (IS_FORWARD == false)
				{
					if (pop(lbytes) == false)
						return false;
				}
				if (ldata.ParseFromArray(&buff()[byte()], lbytes) == false)
					return false;
				add_bytes(lbytes);
			}
			else
			{
				int32_t lbytes = len() - byte();
				T& ldata = *adata.m_data.get();
				google::protobuf::util::JsonParseOptions parseOptions;
				std::string jsonString(&buff()[byte()]);
				google::protobuf::util::JsonStringToMessage(jsonString, &(*adata.m_data), parseOptions);
				add_bytes(lbytes);
			}
			
			return true;
		}

		template <typename KEY, typename VALUE>
		bool pop(protobuf_data<std::map<KEY, VALUE>>& adata)
		{
			if (adata.m_data == nullptr)
				adata.make();
			if (adata.m_isbinary)
			{
				std::map<KEY, VALUE>& lstl = *adata.m_data;
				int16_t lsize = 0;
				if (pop(lsize) == false)
					return false;
				for (int i = 0; i < lsize; ++i)
				{
					KEY lkey;
					if (pop(lkey) == false)
						return false;
					int32_t lbytes = 0;
					if (pop(lbytes) == false)
						return false;
					VALUE& lvalues = lstl[lkey];
					if (lvalues.ParseFromArray(&buff()[byte()], lbytes) == false)
						return false;
					add_bytes(lbytes);
				}
			}			
			return true;
		}

		template <typename T>
		bool pop(protobuf_data<std::vector<T>>& adata)
		{
			if (adata.m_data == nullptr)
				adata.make();
			if (adata.m_isbinary)
			{
				std::vector<T>& lstl = *adata.m_data;
				int16_t lsize = 0;
				if (pop(lsize) == false)
					return false;
				for (int i = 0; i < lsize; ++i)
				{
					int32_t lbytes = 0;
					if (pop(lbytes) == false)
						return false;
					T ltemp;
					if (ltemp.ParseFromArray(&buff()[byte()], lbytes) == false)
						return false;
					add_bytes(lbytes);
					lstl.push_back(ltemp);
				}
			}			
			return true;
		}

		template <typename T>
		bool pop(const protobuf_data<std::list<T>>& adata)
		{
			if (adata.m_data == nullptr)
				adata.make();
			if (adata.m_isbinary)
			{
				std::list<T>& lstl = adata.m_data;
				int16_t lsize = 0;
				if (pop(lsize) == false)
					return false;
				for (int i = 0; i < lsize; ++i)
				{
					int32_t lbytes = 0;
					if (pop(lbytes) == false)
						return false;
					T ltemp;
					if (ltemp.ParseFromArray(&buff()[byte()], lbytes) == false)
						return false;
					add_bytes(lbytes);
					lstl.push_back(ltemp);
				}
			}			
			return true;
		}

		template <typename T>
		bool pop(std::shared_ptr<T>& adata)
		{
			adata = std::shared_ptr<T>(new T());
			return pop(*adata);
		}

		template <typename T>
		bool pop(T& adata);

		template <typename T>
		bool pop(T*& adata)
		{
			bool lbool = false;
			if (pop(lbool) == false)
				return false;
			adata = new T();
			return pop(*adata);
		}

		template <typename TBASE>
		bool pop(derived_class<TBASE>& adata)
		{
			if (!pop(adata.m_enum))
				return false;
			if (!adata.set_tbase())
				return false;
			if (!adata.m_base->pop(*this))
				return false;
			return true;
		}

		bool pop(int8_t& adata);
		bool pop(uint8_t& adata);
		bool pop(int16_t& adata);
		bool pop(uint16_t& adata);
		bool pop(int32_t& adata);
		bool pop(uint32_t& adata);
		bool pop(int64_t& adata);
		bool pop(uint64_t& adata);
		bool pop(float& adata);
		bool pop(double& adata);
		bool pop(bool& adata);
		bool pop(std::vector<bool>& adata);
		bool pop(std::vector<int8_t>& adata);
		bool pop(std::vector<uint8_t>& adata);
		bool pop(std::vector<int32_t>& adata);
		bool pop(std::vector<uint32_t>& adata);
		bool pop(std::vector<int64_t>& adata);
		bool pop(std::vector<uint64_t>& adata);
		bool pop(std::vector<float>& adata);
		bool pop(std::vector<double>& adata);
		bool pop(std::pair<uint32_t, const char*>& adata);
		bool pop(forward& adata);
		bool pop(std::string& adata);

		template <typename T>
		bool pop(std::vector<T>& adata)
		{
			int16_t lsize = 0;
			if (!pop(lsize))
				return false;
			if (lsize < 0)
				return false;
			adata.resize(lsize);
			for (int i = 0; i < lsize; ++i) {
				if (!pop(adata[i]))
					return false;
			}
			return true;
		}

		template <typename T>
		bool pop_vector_number(std::vector<T>& adata)
		{
			int16_t lsize = 0;
			if (!pop(lsize))
				return false;
			if (lsize < 0)
				return false;
			adata.resize(lsize);
			return basetype(&adata[0], sizeof(T) * lsize);
		}

		template <typename T>
		bool pop(std::list<T>& adata)
		{
			int16_t lsize = 0;
			if (!pop(lsize))
				return false;
			if (lsize < 0)
				return false;
			for (int i = 0; i < lsize; ++i)
			{
				T ltemp;
				if (!pop(ltemp))
					return false;
				adata.push_back(ltemp);
			}
			return true;
		}

		template <typename T>
		bool pop(std::set<T>& adata)
		{
			int16_t lsize = 0;
			if (!pop(lsize))
				return false;
			if (lsize < 0)
				return false;
			for (int i = 0; i < lsize; ++i)
			{
				T ltemp;
				if (!pop(ltemp))
					return false;
				adata.insert(ltemp);
			}
			return true;
		}

		template <typename T>
		bool pop(std::unordered_set<T>& adata)
		{
			int16_t lsize = 0;
			if (!pop(lsize))
				return false;
			if (lsize < 0)
				return false;
			for (int i = 0; i < lsize; ++i)
			{
				T ltemp;
				if (!pop(ltemp))
					return false;
				adata.insert(ltemp);
			}
			return true;
		}

		template <typename TKEY, typename TVALUE>
		bool pop(std::map<TKEY, TVALUE>& adata)
		{
			int16_t lsize = 0;
			if (!pop(lsize))
				return false;
			if (lsize < 0)
				return false;
			for (int i = 0; i < lsize; ++i)
			{
				TKEY lkey;
				if (!pop(lkey))
					return false;
				TVALUE& lvalue = adata[lkey];
				if (!pop(lvalue))
					return false;
				//adata.insert(std::make_pair(lkey, lvalue));
			}
			return true;
		}

		template <typename TKEY, typename TVALUE>
		bool pop(std::unordered_map<TKEY, TVALUE>& adata)
		{
			int16_t lsize = 0;
			if (!pop(lsize))
				return false;
			if (lsize < 0)
				return false;
			for (int i = 0; i < lsize; ++i)
			{
				TKEY lkey;
				if (!pop(lkey))
					return false;
				TVALUE& lvalue = adata[lkey];
				if (!pop(lvalue))
					return false;
				//adata.insert(std::make_pair(lkey, lvalue));
			}
			return true;
		}

		// 支持没有参数
		bool pop();

		template <typename T, typename ...ARG>
		bool pop(T& avalue, ARG&... arg)
		{
			return pop(avalue) && pop(arg...);
		}
	};

	class serialize_bytes
	{
		int m_size;
	public:
		serialize_bytes();
		// 支持没有参数
		int bytes();
		int add_bytes(int abytes);

		template <typename T, bool IS_FORWARD>
		int bytes(const protobuf_data<T, IS_FORWARD>& adata)
		{
			assert(adata.m_data != nullptr);
			if (adata.m_isbinary)
			{
				int32_t lbytes = adata.m_data->ByteSize();
				if constexpr (IS_FORWARD == false)
				{
					if (bytes(lbytes) == false)
						return false;
				}
				add_bytes(lbytes);
			}
			return bytes();			
		}

		template <typename KEY, typename VALUE>
		int bytes(const protobuf_data<std::map<KEY, VALUE>>& adata)
		{
			assert(adata.m_data != nullptr);
			bytes(int16_t(adata.m_data->size()));
			for (std::pair<const KEY, VALUE>& pair : *adata.m_data)
			{
				bytes(pair.first);
				int32_t lbytes = pair.second.ByteSize();
				if (bytes(lbytes) == false)
					return false;
				add_bytes(lbytes);
			}
			return bytes();
		}

		template <typename T>
		int bytes(const protobuf_data<std::vector<T>>& adata)
		{
			assert(adata.m_data != nullptr);
			bytes(int16_t(adata.m_data->size()));
			for (auto& item : *adata.m_data)
			{
				int32_t lbytes = item.ByteSize();
				if (bytes(lbytes) == false)
					return false;
				add_bytes(lbytes);
			}
			return bytes();
		}

		template <typename T>
		int bytes(const protobuf_data<std::list<T>>& adata)
		{
			assert(adata.m_data != nullptr);
			bytes(int16_t(adata.m_data.size()));
			for (auto& item : adata.m_data)
			{
				int32_t lbytes = item.ByteSize();
				if (bytes(lbytes) == false)
					return false;
				add_bytes(lbytes);
			}
			return bytes();
		}

		template <typename T>
		int bytes(const T* adata)
		{
			return bytes(*adata);
		}

		template <typename T>
		int bytes(T* adata)
		{
			return bytes(*adata);
		}

		template <typename T>
		int bytes(const T& adata);

		template <typename T>
		int bytes(const std::shared_ptr<T>& adata)
		{
			(*adata).bytes(*this);
			return m_size;
		}

		int bytes(const forward& adata);
		int bytes(forward& adata);
		int bytes(const int8_t adata);
		int bytes(const uint8_t adata);
		int bytes(const int16_t adata);
		int bytes(const uint16_t adata);
		int bytes(const int32_t adata);
		int bytes(const uint32_t adata);
		int bytes(const int64_t adata);
		int bytes(const uint64_t adata);
		int bytes(const float adata);
		int bytes(const double adata);
		int bytes(const bool adata);
		int bytes(const std::vector<bool>& avec);
		int bytes(const std::vector<int8_t>& avec);
		int bytes(const std::vector<uint8_t>& avec);
		int bytes(const std::vector<int16_t>& avec);
		int bytes(const std::vector<uint16_t>& avec);
		int bytes(const std::vector<int32_t>& avec);
		int bytes(const std::vector<uint32_t>& avec);
		int bytes(const std::vector<int64_t>& avec);
		int bytes(const std::vector<uint64_t>& avec);
		int bytes(const std::vector<float>& avec);
		int bytes(const std::vector<double>& avec);
		int bytes(const std::list<int8_t>& avec);
		int bytes(const std::list<uint8_t>& avec);
		int bytes(const std::list<int16_t>& avec);
		int bytes(const std::list<uint16_t>& avec);
		int bytes(const std::list<int32_t>& avec);
		int bytes(const std::list<uint32_t>& avec);
		int bytes(const std::list<int64_t>& avec);
		int bytes(const std::list<uint64_t>& avec);
		int bytes(const std::list<float>& avec);
		int bytes(const std::list<double>& avec);
		int bytes(const std::set<int8_t>& avec);
		int bytes(const std::set<uint8_t>& avec);
		int bytes(const std::set<int16_t>& avec);
		int bytes(const std::set<uint16_t>& avec);
		int bytes(const std::set<int32_t>& avec);
		int bytes(const std::set<uint32_t>& avec);
		int bytes(const std::set<int64_t>& avec);
		int bytes(const std::set<uint64_t>& avec);
		int bytes(const std::set<float>& avec);
		int bytes(const std::set<double>& avec);
		int bytes(const std::unordered_set<int8_t>& avec);
		int bytes(const std::unordered_set<uint8_t>& avec);
		int bytes(const std::unordered_set<int16_t>& avec);
		int bytes(const std::unordered_set<uint16_t>& avec);
		int bytes(const std::unordered_set<int32_t>& avec);
		int bytes(const std::unordered_set<uint32_t>& avec);
		int bytes(const std::unordered_set<int64_t>& avec);
		int bytes(const std::unordered_set<uint64_t>& avec);
		int bytes(const std::unordered_set<float>& avec);
		int bytes(const std::unordered_set<double>& avec);
		int bytes(const std::string& astr);

		template <typename T>
		int bytes(const std::vector<T>& avec)
		{
			bytes(int16_t(avec.size()));
			for (int i = 0; i < avec.size(); ++i)
				bytes(avec[i]);
			return m_size;
		}

		template <typename T>
		int bytes(const std::list<T>& alist)
		{
			bytes(int16_t(alist.size()));
			for (const T& item : alist)
				bytes(item);
			return m_size;
		}

		template <typename T>
		int bytes(const std::set<T>& aset)
		{
			bytes(int16_t(aset.size()));
			for (const T& item : aset)
				bytes(item);
			return m_size;
		}

		template <typename T>
		int bytes(const std::unordered_set<T>& aset)
		{
			bytes(int16_t(aset.size()));
			for (const T& item : aset)
				bytes(item);
			return m_size;
		}

		template <typename TKEY, typename TVALUE>
		int bytes(const std::map<TKEY, TVALUE>& amap)
		{
			bytes(int16_t(amap.size()));
			for (const std::pair<const TKEY, TVALUE>& item : amap)
			{
				bytes(item.first);
				bytes(item.second);
			}
			return m_size;
		}

		template <typename TKEY, typename TVALUE>
		int bytes(const std::unordered_map<TKEY, TVALUE>& amap)
		{
			bytes(int16_t(amap.size()));
			for (const std::pair<const TKEY, TVALUE>& item : amap)
			{
				bytes(item.first);
				bytes(item.second);
			}
			return m_size;
		}

		template <typename T, typename ...ARG>
		int bytes(const T& avalue, const ARG&... arg)
		{
			bytes(avalue) && bytes(arg...);
			return m_size;
		}
	};

	//### 用于序列化枚举类型
	template <typename T>
	class enum_operator
	{
		template <typename E, bool IS_ENUM>
		class enum_operator_push
		{
		public:
			static bool operator_push(serialize& ser, const E& adata)
			{
				return adata.push(ser);
			}
		};

		template <typename E>
		class enum_operator_push<E, true>
		{
		public:
			static bool operator_push(serialize& ser, const E& adata)
			{
				return ser.push((int)adata);
			}
		};

		template <typename E, bool IS_ENUM>
		class enum_operator_pop
		{
		public:
			static bool operator_pop(ngl::unserialize& ser, T& adata)
			{
				return adata.pop(ser);
			}
		};

		template <typename E>
		class enum_operator_pop<E, true>
		{
		public:
			static bool operator_pop(ngl::unserialize& ser, E& adata)
			{
				int ltemp = 0;
				if (ser.pop(ltemp))
				{
					adata = (E)ltemp;
					return true;
				}
				return false;
			}
		};

		template <typename E, bool IS_ENUM>
		class enum_operator_bytes
		{
		public:
			static bool operator_bytes(ngl::serialize_bytes& ser, const E& adata)
			{
				return adata.bytes(ser);
			}
		};

		template <typename E>
		class enum_operator_bytes<E, true>
		{
		public:
			static bool operator_bytes(ngl::serialize_bytes& ser, const E& adata)
			{
				return ser.bytes((int)adata);
			}
		};

		template <typename E, bool IS_ENUM>
		class enum_operator_log
		{
		public:
			static bool operator_log(logserialize& astr, const E& adata)
			{
				return adata.log(astr);
			}
		};

		template <typename E>
		class enum_operator_log<E, true>
		{
		public:
			static bool operator_log(logserialize& astr, const E& adata)
			{
				return astr((int32_t)adata);
			}
		};

	public:
		static bool push(ngl::serialize& ser, const T& adata)
		{
			return enum_operator_push<T, std::is_enum<T>::value>::operator_push(ser, adata);
		}

		static bool pop(ngl::unserialize& ser, T& adata)
		{
			return enum_operator_pop<T, std::is_enum<T>::value>::operator_pop(ser, adata);
		}

		static int bytes(ngl::serialize_bytes& abytes, const T& adata)
		{
			return enum_operator_bytes<T, std::is_enum<T>::value>::operator_bytes(abytes, adata);
		}

		static bool log(logserialize& astr, const T& adata)
		{
			return enum_operator_log<T, std::is_enum<T>::value || std::is_integral<T>::value>::operator_log(astr, adata);
		}

		template <typename T1, typename ...ARG>
		static bool push(ngl::serialize& ser, const T1& avalue, const ARG& ... arg)
		{
			return push(ser, avalue) && push(ser, arg...);
		}

		template <typename T1, typename ...ARG>
		static bool pop(ngl::serialize& ser, T1& avalue, ARG& ... arg)
		{
			return pop(ser, avalue) && pop(ser, arg...);
		}

		template <typename T1, typename ...ARG>
		static int bytes(ngl::serialize_bytes& abytes, const T1& adata, ARG& ... arg)
		{
			return bytes(abytes, adata) && bytes(abytes, arg...);
		}
	};

	template <typename T>
	bool serialize::push(const T& adata)
	{
		return enum_operator<T>::push(*this, adata);
	}

	template <typename T>
	bool logserialize::operator()(const T& avalue)
	{
		return enum_operator<T>::log(*this, avalue);
	}

	struct forward
	{
		uint32_t m_len = 0;
		const char* m_buff = nullptr;
		bool pop(unserialize& ser);
		bool push(serialize& ser)const;
		int bytes(serialize_bytes& abytes)const;
		bool log(logserialize& atstr)const { return true; }
		const char* name() { return "forward"; }
	};

	template <typename T>
	bool unserialize::pop(T& adata)
	{
		return ngl::enum_operator<T>::pop(*this, adata);
	}

	template <typename T>
	int serialize_bytes::bytes(const T& adata)
	{
		m_size += enum_operator<T>::bytes(*this, adata);
		return m_size;
	}	
}