#pragma once

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

#include <google/protobuf/util/json_util.h>

#include "bytesorder.h"
#include "varint.h"
#include "define.h"
#include "db.pb.h"
#include "tools.h"
#include "nlog.h"
#include "csv.h"

namespace ngl
{
	struct forward;

	class serialize_bytes;

	template <typename T, bool IS_FORWARD = false>
	struct protobuf_data
	{
		std::shared_ptr<T>	m_data;
		bool				m_isbinary;			// 默认序列化为二进制，只有db保存的时候序列化为json
		
		protobuf_data()
			: m_data(nullptr)
			, m_isbinary(true)
		{
		}

		protobuf_data(std::shared_ptr<T>& adata) 
			: m_data(adata)
			, m_isbinary(true)
		{
		}

		~protobuf_data()
		{
		}

		void make()
		{
			m_data = std::make_shared<T>();
		}
	};

	class serialize
	{
		char*	m_buff;
		int32_t m_len;
		int32_t m_pos;
	public:
		inline serialize(char* abuff, int32_t alen)
			:m_buff(abuff), m_len(alen), m_pos(0)
		{}

		inline char* buff()
		{
			return m_buff;
		}

		inline int byte()
		{
			return m_pos;
		}

		inline int len()
		{
			return m_len;
		}

		inline void add_bytes(int abytes)
		{
			m_pos += abytes;
		}

		inline void dec_bytes(int abytes)
		{
			m_pos -= abytes;
		}

		inline bool basetype(void* adata, int32_t alen)
		{
			if (m_pos + alen > m_len)
				return false;
			memcpy(&m_buff[m_pos], adata, alen);
			m_pos += alen;
			return true;
		}

		template <typename T>
		inline bool basetype(const T& adata)
		{
			return basetype((void*)&adata, sizeof(T));
		}

		template <typename T>
		static bool proto_json(const T& adata, std::string& ajson)
		{
			google::protobuf::util::JsonPrintOptions options;
			options.add_whitespace = false;
			options.always_print_primitive_fields = false;
			options.preserve_proto_field_names = false;
			return google::protobuf::util::MessageToJsonString(adata, &ajson, options).ok();
		}

		template <typename T, bool IS_FORWARD>
		inline bool push(const protobuf_data<T, IS_FORWARD>& adata)
		{
			if (adata.m_data == nullptr)
				return false;
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
				std::string json;
				if (proto_json(*adata.m_data, json))
				{
					int32_t len = json.size() + 1;
					memcpy(&buff()[byte()], json.c_str(), len);
					add_bytes(len);
					return true;
				}
				return false;
			}
		}

		template <typename KEY, typename VALUE>
		inline bool push(const protobuf_data<std::map<KEY, VALUE>>& adata)
		{
			if (adata.m_data == nullptr)
			{
				log_error()->print("serialize::push<{},{}>(protobuf_data:std::map)", dtype_name(KEY), dtype_name(VALUE));
				return false;
			}
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
		inline bool push(const protobuf_data<std::vector<T>>& adata)
		{
			if (adata.m_data == nullptr)
			{
				log_error()->print("serialize::push<{}>(protobuf_data:std::vector)", dtype_name(T));
				return false;
			}
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
		inline bool push(const protobuf_data<std::list<T>>& adata)
		{
			if (adata.m_data == nullptr)
			{
				log_error()->print("serialize::push<{}>(protobuf_data:std::list)", dtype_name(T));
				return false;
			}
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
		inline bool push(T* adata)
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
		inline bool push(const T* adata)
		{
			return push(*adata);
		}

		template <typename T>
		inline bool push(const std::shared_ptr<T>& adata)
		{
			return push(*adata);
		}

		template <typename T>
		inline bool push(const T& adata);

		inline bool push(const forward& adata);

		inline bool push(forward& adata);

#pragma region basetype // 基础类型
		// # 基本类型
		inline bool push(const int8_t adata)
		{
			return basetype(adata);
		}

		inline bool push(const uint8_t adata)
		{
			return basetype(adata);
		}

		inline bool push(const int16_t adata)
		{
			int16_t lvalues = adata;
			lvalues = bytesorder::transformlittle(lvalues);
			return basetype(lvalues);
		}

		inline bool push(const uint16_t adata)
		{
			return push((const int16_t)adata);
		}

		inline bool push(const int32_t adata)
		{
			int32_t lvalues = adata;
			lvalues = bytesorder::transformlittle(lvalues);
			varint::parm<int32_t> lparm
			{
				.m_value = lvalues,
				.m_buf = &m_buff[m_pos],
				.m_len = m_len - m_pos,
				.m_bytes = &m_pos,
			};
			return varint::encode(lparm);
		}

		inline bool push(const uint32_t adata)
		{
			return push((const int32_t)adata);
		}

		inline bool push(const int64_t adata)
		{
			int64_t lvalues = adata;
			lvalues = bytesorder::transformlittle(lvalues);
			varint::parm<int64_t> lparm
			{
				.m_value = lvalues,
				.m_buf = &m_buff[m_pos],
				.m_len = m_len - m_pos,
				.m_bytes = &m_pos,
			};
			return varint::encode(lparm);
		}

		inline bool push(const uint64_t adata)
		{
			return push((const int64_t)adata);
		}

		inline bool push(const float adata)
		{
			return basetype(adata);
		}

		inline bool push(const double adata)
		{
			return basetype(adata);
		}

		inline bool push(const bool adata)
		{
			return basetype(adata);
		}

#pragma endregion 

		template <typename T>
		inline bool push_vector_number(const std::vector<T>& avec)
		{
			if (push_stlsize(avec) == false)
				return false;
			if (avec.empty())
				return true;
			return basetype((void*)&avec[0], avec.size() * sizeof(T));
		}

		inline bool push(const std::vector<bool>& avec)
		{
			if (push_stlsize(avec) == false)
				return false;
			for (bool lbool : avec)
			{
				if (push(lbool) == false)
					return false;
			}
			return true;
		}

		inline bool push(const std::vector<int8_t>& avec)
		{
			return push_vector_number(avec);
		}

		inline bool push(const std::vector<uint8_t>& avec)
		{
			return push_vector_number(avec);
		}

		// # 单双浮点型,不需要考虑字节序
		inline bool push(const std::vector<float>& avec)
		{
			return push_vector_number(avec);
		}

		inline bool push(const std::vector<double>& avec)
		{
			return push_vector_number(avec);
		}

		template <typename TSTL>
		inline bool push_vector_number_compile(const TSTL& avec)
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

		inline bool push(const std::vector<int32_t>& avec)
		{
			return push_vector_number_compile(avec);
		}

		inline bool push(const std::vector<uint32_t>& avec)
		{
			return push_vector_number_compile(avec);
		}

		inline bool push(const std::vector<int64_t>& avec)
		{
			return push_vector_number_compile(avec);
		}

		inline bool push(const std::vector<uint64_t>& avec)
		{
			return push_vector_number_compile(avec);
		}

		template <typename T>
		inline bool push_stlsize(const T& astl)
		{
			int16_t lsize = astl.size();
			return push(lsize);
		}

		template <typename T>
		inline bool push(const std::vector<T>& avec)
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

		inline bool push(std::pair<uint32_t, const char*>& adata)
		{
			if (!push(adata.first))
				return false;
			return basetype((void*)adata.second, adata.first);
		}

		template <typename T>
		inline bool push(const std::list<T>& alist)
		{
			return push_vector_number_compile(alist);
		}

		template <typename T>
		inline bool push(const std::set<T>& aset)
		{
			return push_vector_number_compile(aset);
		}

		template <typename T>
		inline bool push(const std::unordered_set<T>& aset)
		{
			return push_vector_number_compile(aset);
		}

		template <typename TKEY, typename TVALUE>
		inline bool push(const std::map<TKEY, TVALUE>& amap)
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
		inline bool push(const std::unordered_map<TKEY, TVALUE>& amap)
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


		inline bool push(const std::string& astr)
		{
			if (!push_stlsize(astr))
				return false;
			return basetype((void*)astr.c_str(), sizeof(char) * astr.size());
		}

		// # 支持没有参数
		inline bool push()
		{
			return true;
		}

		template <typename T, typename ...ARG>
		inline bool push(const T& avalue, const ARG&... arg)
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
		inline unserialize(const char* abuff, int32_t alen) :
			m_buff(abuff),
			m_len(alen),
			m_pos(0)
		{}

		inline const char* buff()
		{
			return m_buff;
		}

		inline int byte()
		{
			return m_pos;
		}

		inline int len()
		{
			return m_len;
		}

		inline void add_bytes(int abytes)
		{
			m_pos += abytes;
		}

		inline void dec_bytes(int abytes)
		{
			m_pos -= abytes;
		}

		template <typename T>
		inline bool basetype(T& adata)
		{
			if (m_pos + sizeof(T) > m_len)
				return false;
			memcpy(&adata, &m_buff[m_pos], sizeof(T));
			m_pos += sizeof(T);
			return true;
		}

		inline bool basetype(void* adata, int32_t alen)
		{
			if (m_pos + alen > m_len)
				return false;
			memcpy(adata, &m_buff[m_pos], alen);
			m_pos += alen;
			return true;
		}

		template <typename T, bool IS_FORWARD>
		inline bool pop(protobuf_data<T, IS_FORWARD>& adata)
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
				google::protobuf::util::Status lstat = google::protobuf::util::JsonStringToMessage(jsonString, &(*adata.m_data), parseOptions);
				add_bytes(lbytes);
			}
			
			return true;
		}

		template <typename KEY, typename VALUE>
		inline bool pop(protobuf_data<std::map<KEY, VALUE>>& adata)
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
		inline bool pop(protobuf_data<std::vector<T>>& adata)
		{
			adata.make();
			if (adata.m_isbinary)
			{
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
					std::string json;
					if (tools::protostr(ltemp, json) == false)
					{
						//log_error()->print("pop [{}] error", dtype_name(T));
						return false;
					}
					add_bytes(lbytes);
					(*adata.m_data).push_back(ltemp);
				}
			}
			return true;
		}

		template <typename T>
		inline bool pop(const protobuf_data<std::list<T>>& adata)
		{
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
		inline bool pop(std::shared_ptr<T>& adata)
		{
			adata = std::shared_ptr<T>(new T());
			return pop(*adata);
		}

		template <typename T>
		bool pop(T& adata);

		template <typename T>
		inline bool pop(T*& adata)
		{
			bool lbool = false;
			if (pop(lbool) == false)
				return false;
			adata = new T();
			return pop(*adata);
		}

#pragma region basetype // 基础类型

		inline bool pop(int8_t& adata)
		{
			return basetype(adata);
		}

		inline bool pop(uint8_t& adata)
		{
			return basetype(adata);
		}

		inline bool pop(int16_t& adata)
		{
			if (basetype(adata) == false)
				return false;
			adata = bytesorder::transformlittle(adata);
			return true;
		}

		inline bool pop(uint16_t& adata)
		{
			int16_t ltemp = adata;
			if (pop(ltemp) == false)
				return false;
			adata = ltemp;
			return true;
		}

		inline bool pop(int32_t& adata)
		{
			varint::parm<int32_t> lparm
			{
				.m_value = adata,
				.m_buf = (char*)&m_buff[m_pos],
				.m_len = m_len - m_pos,
				.m_bytes = &m_pos,
			};
			if (varint::decode(lparm) == false)
				return false;
			adata = lparm.m_value;
			adata = bytesorder::transformlittle(adata);
			return true;
		}

		inline bool pop(uint32_t& adata)
		{
			int32_t ldata = 0;
			if (pop(ldata) == false)
				return false;
			adata = ldata;
			return true;
		}

		inline bool pop(int64_t& adata)
		{
			varint::parm<int64_t> lparm
			{
				.m_value = adata,
				.m_buf = (char*)&m_buff[m_pos],
				.m_len = m_len - m_pos,
				.m_bytes = &m_pos,
			};
			if (varint::decode(lparm) == false)
				return false;
			adata = lparm.m_value;
			adata = bytesorder::transformlittle(adata);
			return true;
		}

		inline bool pop(uint64_t& adata)
		{
			int64_t ldata = 0;
			if (pop(ldata) == false)
				return false;
			adata = ldata;
			return true;
		}

		inline bool pop(float& adata)
		{
			return basetype(adata);
		}

		inline bool pop(double& adata)
		{
			return basetype(adata);
		}

		inline bool pop(bool& adata)
		{
			if (basetype(adata))
				return true;
			return false;
		}
#pragma endregion 

#pragma region vector
		inline bool pop(std::vector<bool>& adata)
		{
			int16_t lsize = 0;
			if (!pop(lsize))
				return false;
			if (lsize < 0)
				return false;
			adata.resize(lsize);
			for (int i = 0; i < lsize; ++i)
			{
				bool lbool = false;
				if (pop(lbool) == false)
					return false;
				adata[i] = lbool;
			}
			return true;
		}

		template <typename T>
		inline bool pop_vector_number(std::vector<T>& adata)
		{
			int16_t lsize = 0;
			if (!pop(lsize))
				return false;
			if (lsize < 0)
				return false;
			adata.resize(lsize);
			return basetype(&adata[0], sizeof(T) * lsize);
		}

		inline bool pop(std::vector<int8_t>& adata)
		{
			return pop_vector_number(adata);
		}

		inline bool pop(std::vector<uint8_t>& adata)
		{
			return pop_vector_number(adata);
		}

		inline bool pop(std::vector<int32_t>& adata)
		{
			int16_t lsize = 0;
			if (!pop(lsize))
				return false;
			if (lsize < 0)
				return false;
			adata.resize(lsize);
			for (auto& item : adata)
			{
				if (pop(item) == false)
					return false;
			}
			return true;
		}

		inline bool pop(std::vector<uint32_t>& adata)
		{
			int16_t lsize = 0;
			if (!pop(lsize))
				return false;
			if (lsize < 0)
				return false;
			adata.resize(lsize);
			for (auto& item : adata)
			{
				if (pop(item) == false)
					return false;
			}
			return true;
		}

		inline bool pop(std::vector<int64_t>& adata)
		{
			int16_t lsize = 0;
			if (!pop(lsize))
				return false;
			if (lsize < 0)
				return false;
			adata.resize(lsize);
			for (auto& item : adata)
			{
				if (pop(item) == false)
					return false;
			}
			return true;
		}

		inline bool pop(std::vector<uint64_t>& adata)
		{
			int16_t lsize = 0;
			if (!pop(lsize))
				return false;
			if (lsize < 0)
				return false;
			adata.resize(lsize);
			for (auto& item : adata)
			{
				if (pop(item) == false)
					return false;
			}
			return true;
		}

		inline bool pop(std::vector<float>& adata)
		{
			return pop_vector_number(adata);
		}

		inline bool pop(std::vector<double>& adata)
		{
			return pop_vector_number(adata);
		}
#pragma endregion 

		inline bool pop(std::pair<uint32_t, const char*>& adata)
		{
			if (!pop(adata.first))
				return false;
			if (m_pos + adata.first > m_len)
				return false;
			adata.second = &m_buff[m_pos];
			m_pos += adata.first;
			return true;
		}

		bool pop(forward& adata);

		inline bool pop(std::string& adata)
		{
			uint16_t lsize = 0;
			if (!pop(lsize))
				return false;
			if (lsize > 0)
			{
				adata.resize(lsize);
				if (!basetype((void*)&adata[0], lsize))
					return false;
			}
			return true;
		}

		template <typename T>
		inline bool pop(std::vector<T>& adata)
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
		inline bool pop(std::list<T>& adata)
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
		inline bool pop(std::set<T>& adata)
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
		inline bool pop(std::unordered_set<T>& adata)
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
		inline bool pop(std::map<TKEY, TVALUE>& adata)
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
			}
			return true;
		}

		template <typename TKEY, typename TVALUE>
		inline bool pop(std::unordered_map<TKEY, TVALUE>& adata)
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
			}
			return true;
		}

		// # 支持没有参数
		inline bool pop()
		{
			return true;
		}


		template <typename T, typename ...ARG>
		inline bool pop(T& avalue, ARG&... arg)
		{
			return pop(avalue) && pop(arg...);
		}
	};

	class serialize_bytes
	{
		int m_size;
	public:
		inline serialize_bytes() :
			m_size(0)
		{}

		// # 支持没有参数
		inline int bytes()
		{
			return m_size;
		}

		inline int add_bytes(int abytes)
		{
			return m_size += abytes;
		}

		template <typename T, bool IS_FORWARD>
		inline int bytes(const protobuf_data<T, IS_FORWARD>& adata)
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
		inline int bytes(const protobuf_data<std::map<KEY, VALUE>>& adata)
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
		inline int bytes(const protobuf_data<std::vector<T>>& adata)
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
		inline int bytes(const protobuf_data<std::list<T>>& adata)
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
		inline int bytes(const T* adata)
		{
			return bytes(*adata);
		}

		template <typename T>
		inline int bytes(T* adata)
		{
			return bytes(*adata);
		}

		template <typename T>
		int bytes(const T& adata);

		template <typename T>
		inline int bytes(const std::shared_ptr<T>& adata)
		{
			(*adata).bytes(*this);
			return m_size;
		}

		int bytes(const forward& adata);
		int bytes(forward& adata);

#pragma region basetype // 基础类型
		inline int bytes(const int8_t adata)
		{
			return m_size += sizeof(int8_t);
		}

		inline int bytes(const uint8_t adata)
		{
			return m_size += sizeof(uint8_t);
		}

		inline int bytes(const int16_t adata)
		{
			return m_size += sizeof(int16_t);
		}

		inline int bytes(const uint16_t adata)
		{
			return m_size += sizeof(uint16_t);
		}

		inline int bytes(const int32_t adata)
		{
			int32_t lvalues = adata;
			lvalues = bytesorder::transformlittle(lvalues);
			varint::parm_length<int32_t> laprm{ .m_value = lvalues };
			return m_size += varint::length(laprm);
		}

		inline int bytes(const uint32_t adata)
		{
			return bytes((const int32_t)adata);
		}

		inline int bytes(const int64_t adata)
		{
			int64_t lvalues = adata;
			lvalues = bytesorder::transformlittle(lvalues);
			varint::parm_length<int64_t> laprm{ .m_value = lvalues };
			return m_size += varint::length(laprm);
		}

		inline int bytes(const uint64_t adata)
		{
			return bytes((const int64_t)adata);
		}

		inline int bytes(const float adata)
		{
			return m_size += sizeof(adata);
		}

		inline int bytes(const double adata)
		{
			return m_size += sizeof(adata);
		}

		inline int bytes(const bool adata)
		{
			return m_size += sizeof(adata);
		}
#pragma endregion 

#pragma region vector
		inline int bytes(const std::vector<bool>& avec)
		{
			bytes(uint16_t(avec.size()));
			return m_size += avec.size() * sizeof(bool);
		}

		inline int bytes(const std::vector<int8_t>& avec)
		{
			bytes(int16_t(avec.size()));
			return m_size += sizeof(int8_t) * avec.size();
		}

		inline int bytes(const std::vector<uint8_t>& avec)
		{
			bytes(int16_t(avec.size()));
			return m_size += sizeof(uint8_t) * avec.size();
		}

		inline int bytes(const std::vector<int16_t>& avec)
		{
			bytes(int16_t(avec.size()));
			return m_size += sizeof(int16_t) * avec.size();
		}

		inline int bytes(const std::vector<uint16_t>& avec)
		{
			bytes(int16_t(avec.size()));
			return m_size += sizeof(uint16_t) * avec.size();
		}

		inline int bytes(const std::vector<int32_t>& avec)
		{
			bytes(int16_t(avec.size()));
			for (int i = 0; i < avec.size(); ++i)
			{
				bytes(avec[i]);
			}
			return m_size;
		}

		inline int bytes(const std::vector<uint32_t>& avec)
		{
			bytes(int16_t(avec.size()));
			for (int i = 0; i < avec.size(); ++i)
			{
				bytes(avec[i]);
			}
			return m_size;
		}

		inline int bytes(const std::vector<int64_t>& avec)
		{
			bytes(int16_t(avec.size()));
			for (int i = 0; i < avec.size(); ++i)
			{
				bytes(avec[i]);
			}
			return m_size;
		}

		inline int bytes(const std::vector<uint64_t>& avec)
		{
			bytes(int16_t(avec.size()));
			for (int i = 0; i < avec.size(); ++i)
			{
				bytes(avec[i]);
			}
			return m_size;
		}

		inline int bytes(const std::vector<float>& avec)
		{
			bytes(int16_t(avec.size()));
			return m_size += sizeof(float) * avec.size();
		}

		inline int bytes(const std::vector<double>& avec)
		{
			bytes(int16_t(avec.size()));
			return m_size += sizeof(double) * avec.size();
		}
#pragma endregion 

#pragma region list
		inline int bytes(const std::list<int8_t>& avec)
		{
			bytes(int16_t(avec.size()));
			return m_size += sizeof(int8_t) * avec.size();
		}

		inline int bytes(const std::list<uint8_t>& avec)
		{
			bytes(int16_t(avec.size()));
			return m_size += sizeof(uint8_t) * avec.size();
		}

		inline int bytes(const std::list<int16_t>& avec)
		{
			bytes(int16_t(avec.size()));
			return m_size += sizeof(int16_t) * avec.size();
		}

		inline int bytes(const std::list<uint16_t>& avec)
		{
			bytes(int16_t(avec.size()));
			return m_size += sizeof(uint16_t) * avec.size();
		}

		inline int bytes(const std::list<int32_t>& avec)
		{
			bytes(int16_t(avec.size()));
			for (int32_t item : avec)
				bytes(item);
			return m_size;
		}

		inline int bytes(const std::list<uint32_t>& avec)
		{
			bytes(int16_t(avec.size()));
			for (uint32_t item : avec)
				bytes(item);
			return m_size;
		}

		inline int bytes(const std::list<int64_t>& avec)
		{
			bytes(int16_t(avec.size()));
			for (int64_t item : avec)
				bytes(item);
			return m_size;
		}

		inline int bytes(const std::list<uint64_t>& avec)
		{
			bytes(int16_t(avec.size()));
			for (uint64_t item : avec)
				bytes(item);
			return m_size;
		}

		inline int bytes(const std::list<float>& avec)
		{
			bytes(int16_t(avec.size()));
			return m_size += sizeof(float) * avec.size();
		}

		inline int bytes(const std::list<double>& avec)
		{
			bytes(int16_t(avec.size()));
			return m_size += sizeof(double) * avec.size();
		}

#pragma endregion 

#pragma region set
		inline int bytes(const std::set<int8_t>& avec)
		{
			bytes(int16_t(avec.size()));
			return m_size += sizeof(int8_t) * avec.size();
		}

		inline int bytes(const std::set<uint8_t>& avec)
		{
			bytes(int16_t(avec.size()));
			return m_size += sizeof(uint8_t) * avec.size();
		}

		inline int bytes(const std::set<int16_t>& avec)
		{
			bytes(int16_t(avec.size()));
			return m_size += sizeof(int16_t) * avec.size();
		}

		inline int bytes(const std::set<uint16_t>& avec)
		{
			bytes(int16_t(avec.size()));
			return m_size += sizeof(uint16_t) * avec.size();
		}

		inline int bytes(const std::set<int32_t>& avec)
		{
			bytes(int16_t(avec.size()));
			for (int32_t item : avec)
				bytes(item);
			return m_size;
		}

		inline int bytes(const std::set<uint32_t>& avec)
		{
			bytes(int16_t(avec.size()));
			for (uint32_t item : avec)
				bytes(item);
			return m_size;
		}

		inline int bytes(const std::set<int64_t>& avec)
		{
			bytes(int16_t(avec.size()));
			for (int64_t item : avec)
				bytes(item);
			return m_size;
		}

		inline int bytes(const std::set<uint64_t>& avec)
		{
			bytes(int16_t(avec.size()));
			for (uint64_t item : avec)
				bytes(item);
			return m_size;
		}

		inline int bytes(const std::set<float>& avec)
		{
			bytes(int16_t(avec.size()));
			return m_size += sizeof(float) * avec.size();
		}

		inline int bytes(const std::set<double>& avec)
		{
			bytes(int16_t(avec.size()));
			return m_size += sizeof(double) * avec.size();
		}
#pragma endregion 

#pragma region unordered_set
		inline int bytes(const std::unordered_set<int8_t>& avec)
		{
			bytes(int16_t(avec.size()));
			return m_size += sizeof(int8_t) * avec.size();
		}

		inline int bytes(const std::unordered_set<uint8_t>& avec)
		{
			bytes(int16_t(avec.size()));
			return m_size += sizeof(uint8_t) * avec.size();
		}

		inline int bytes(const std::unordered_set<int16_t>& avec)
		{
			bytes(int16_t(avec.size()));
			return m_size += sizeof(int16_t) * avec.size();
		}

		inline int bytes(const std::unordered_set<uint16_t>& avec)
		{
			bytes(int16_t(avec.size()));
			return m_size += sizeof(uint16_t) * avec.size();
		}

		inline int bytes(const std::unordered_set<int32_t>& avec)
		{
			bytes(int16_t(avec.size()));
			for (int32_t item : avec)
				bytes(item);
			return m_size;
		}

		inline int bytes(const std::unordered_set<uint32_t>& avec)
		{
			bytes(int16_t(avec.size()));
			for (uint32_t item : avec)
				bytes(item);
			return m_size;
		}

		inline int bytes(const std::unordered_set<int64_t>& avec)
		{
			bytes(int16_t(avec.size()));
			for (int64_t item : avec)
				bytes(item);
			return m_size;
		}

		inline int bytes(const std::unordered_set<uint64_t>& avec)
		{
			bytes(int16_t(avec.size()));
			for (uint64_t item : avec)
				bytes(item);
			return m_size;
		}

		inline int bytes(const std::unordered_set<float>& avec)
		{
			bytes(int16_t(avec.size()));
			return m_size += sizeof(float) * avec.size();
		}

		inline int bytes(const std::unordered_set<double>& avec)
		{
			bytes(int16_t(avec.size()));
			return m_size += sizeof(double) * avec.size();
		}
#pragma endregion 

		inline int bytes(const std::string& astr)
		{
			bytes(int16_t(astr.size()));
			return m_size += astr.size() * sizeof(char);
		}

		template <typename T>
		inline int bytes(const std::vector<T>& avec)
		{
			bytes(int16_t(avec.size()));
			for (int i = 0; i < avec.size(); ++i)
				bytes(avec[i]);
			return m_size;
		}

		template <typename T>
		inline int bytes(const std::list<T>& alist)
		{
			bytes(int16_t(alist.size()));
			for (const T& item : alist)
				bytes(item);
			return m_size;
		}

		template <typename T>
		inline int bytes(const std::set<T>& aset)
		{
			bytes(int16_t(aset.size()));
			for (const T& item : aset)
				bytes(item);
			return m_size;
		}

		template <typename T>
		inline int bytes(const std::unordered_set<T>& aset)
		{
			bytes(int16_t(aset.size()));
			for (const T& item : aset)
				bytes(item);
			return m_size;
		}

		template <typename TKEY, typename TVALUE>
		inline int bytes(const std::map<TKEY, TVALUE>& amap)
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
		inline int bytes(const std::unordered_map<TKEY, TVALUE>& amap)
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
		inline int bytes(const T& avalue, const ARG&... arg)
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

	struct forward
	{
		uint32_t m_len = 0;
		const char* m_buff = nullptr;
		bool pop(unserialize& ser);
		bool push(serialize& ser)const;
		int bytes(serialize_bytes& abytes)const;
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
}// namespace ngl