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

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "tools/tools.h"

#include <functional>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <set>

namespace ngl
{
	class ncjson
	{
		rapidjson::Document m_doc;
		rapidjson::Document::AllocatorType* m_allocator;
		std::string m_nonformat_str;
		std::string m_str;
		bool m_parsecheck = true;

		bool parse(const char* ajsonstr)
		{
			if (ajsonstr == nullptr)
			{
				return false;
			}
			if (m_doc.Parse(ajsonstr).HasParseError())
			{
				return false;
			}
			return true;
		}
	public:
		ncjson(const char* ajsonstr)
		{
			m_allocator = &m_doc.GetAllocator();
			if (!parse(ajsonstr))
			{
				m_parsecheck = false;
				std::cout << "json parse error [" << ajsonstr << "]" << std::endl;
			}
		}

		ncjson()
		{
			m_allocator = &m_doc.GetAllocator();
		}

		void set_object()
		{
			m_doc.SetObject();
		}

		bool parsecheck()
		{
			return m_parsecheck;
		}

		rapidjson::Document& doc()
		{
			return m_doc;
		}

		rapidjson::Document::AllocatorType& allocator()
		{
			return *m_allocator;
		}

		const char* nonformat_str()
		{
			rapidjson::StringBuffer buffer;
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			m_doc.Accept(writer);
			m_nonformat_str = buffer.GetString();
			return m_nonformat_str.c_str();
		}

		const char* str()
		{
			rapidjson::StringBuffer buffer;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
			m_doc.Accept(writer);
			m_str = buffer.GetString();
			return m_str.c_str();
		}

		static std::string vstr(const rapidjson::Value* ajson, bool pretty = false)
		{
			if (!ajson) 
			{
				return "";
			}
			rapidjson::StringBuffer buffer;
			if (pretty) 
			{
				rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
				ajson->Accept(writer);
			}
			else 
			{
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				ajson->Accept(writer);
			}
			return buffer.GetString();
		}
	};

	template <typename T>
	struct json_format
	{
		static bool pop(rapidjson::Value* ajson, const char* akey, T& adata);
		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, const T& adata);
		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const T& adata);
	};

	template <typename T>
	struct json_format<T*>
	{
		static bool pop(rapidjson::Value* ajson, const char* akey, T*& adata)
		{
			if (adata == nullptr)
			{
				return false;
			}
			return json_format<T>::pop(ajson, akey, *adata);
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, const T* adata)
		{
			if (adata == nullptr)
			{
				return false;
			}
			return json_format<T>::push(ajson, aallocator, akey, *adata);
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const T* adata)
		{
			if (adata == nullptr)
			{
				return false;
			}
			return json_format<T>::pushback(ajson, aallocator, *adata);
		}
	};

	template <typename T>
	struct json_format<std::shared_ptr<T>>
	{
		static bool pop(rapidjson::Value* ajson, const char* akey, std::shared_ptr<T>& adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			auto itor = ajson->FindMember(akey);
			if (itor == ajson->MemberEnd())
			{
				return false;
			}
			return pop(&itor->value, adata);
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, const std::shared_ptr<T>& adata)
		{
			if (adata == nullptr)
			{
				return false;
			}
			return json_format<T>::push(ajson, aallocator, akey, *adata);
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const std::shared_ptr<T>& adata)
		{
			if (adata == nullptr)
			{
				return false;
			}
			return json_format<T>::pushback(ajson, aallocator, *adata);
		}
	};

	class tool_rapidjson_value
	{
	public:
		static rapidjson::Value* find(rapidjson::Value* ajson, const char* akey)
		{
			if (akey != nullptr)
			{
				auto itor = ajson->FindMember(akey);
				if (itor == ajson->MemberEnd())
				{
					return nullptr;
				}
				return &itor->value;
			}
			return ajson;
		}
	};

	

	template <>
	struct json_format<int64_t>
	{
		static bool pop(rapidjson::Value* ajson, const char* akey, int64_t& adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			rapidjson::Value* ljson = tool_rapidjson_value::find(ajson, akey);
			if (ljson == nullptr)
			{
				return false;
			}
			
			if (!ljson->IsInt64())
			{
				return false;
			}
			adata = ljson->GetInt64();
			return true;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, int64_t adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			rapidjson::Value val;
			val.SetInt64(adata);
			ajson->AddMember(rapidjson::StringRef(akey), val, *aallocator);
			return true;
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, int64_t adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			ajson->PushBack(adata, *aallocator);
			return true;
		}
	};

	template <>
	struct json_format<uint64_t>
	{
		static bool pop(rapidjson::Value* ajson, const char* akey, uint64_t& adata)
		{
			int64_t lvalue = 0;
			if (!json_format<int64_t>::pop(ajson, akey, lvalue))
			{
				return false;
			}
			adata = (uint64_t)lvalue;
			return true;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, uint64_t adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			rapidjson::Value val;
			val.SetUint64(adata);
			ajson->AddMember(rapidjson::StringRef(akey), val, *aallocator);
			return true;
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, uint64_t adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			ajson->PushBack(adata, *aallocator);
			return true;
		}
	};

	template <>
	struct json_format<int32_t>
	{
		static bool pop(rapidjson::Value* ajson, const char* akey, int32_t& adata)
		{
			int64_t lvalue = 0;
			if (!json_format<int64_t>::pop(ajson, akey, lvalue))
			{
				return false;
			}
			adata = (int32_t)lvalue;
			return true;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, int32_t adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			int64_t lvalue = 0;
			if (!json_format<int64_t>::push(ajson, aallocator, akey, lvalue))
			{
				return false;
			}
			adata = (int32_t)lvalue;
			return true;
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, int32_t adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			ajson->PushBack(adata, *aallocator);
			return true;
		}
	};

	template <>
	struct json_format<uint32_t>
	{
		static bool pop(rapidjson::Value* ajson, const char* akey, uint32_t& adata)
		{
			int64_t lvalue = 0;
			if (!json_format<int64_t>::pop(ajson, akey, lvalue))
			{
				return false;
			}
			adata = (uint32_t)lvalue;
			return true;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, uint32_t adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			uint64_t lvalue = 0;
			if (!json_format<uint64_t>::push(ajson, aallocator, akey, lvalue))
			{
				return false;
			}
			adata = (uint32_t)lvalue;
			return true;
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, uint32_t adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			ajson->PushBack(adata, *aallocator);
			return true;
		}
	};

	template <>
	struct json_format<int16_t>
	{
		static bool pop(rapidjson::Value* ajson, const char* akey, int16_t& adata)
		{
			int64_t lvalue = 0;
			if (!json_format<int64_t>::pop(ajson, akey, lvalue))
			{
				return false;
			}
			adata = (int16_t)lvalue;
			return true;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, int16_t adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			int64_t lvalue = 0;
			if (!json_format<int64_t>::push(ajson, aallocator, akey, lvalue))
			{
				return false;
			}
			adata = (int16_t)lvalue;
			return true;
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, int16_t adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			ajson->PushBack(adata, *aallocator);
			return true;
		}
	};

	template <>
	struct json_format<uint16_t>
	{
		static bool pop(rapidjson::Value* ajson, const char* akey, uint16_t& adata)
		{
			int64_t lvalue = 0;
			if (!json_format<int64_t>::pop(ajson, akey, lvalue))
			{
				return false;
			}
			adata = (uint16_t)lvalue;
			return true;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, uint16_t adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			uint64_t lvalue = 0;
			if (!json_format<uint64_t>::push(ajson, aallocator, akey, lvalue))
			{
				return false;
			}
			adata = (uint16_t)lvalue;
			return true;
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, uint16_t adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			ajson->PushBack(adata, *aallocator);
			return true;
		}
	};

	template <>
	struct json_format<int8_t>
	{
		static bool pop(rapidjson::Value* ajson, const char* akey, int8_t& adata)
		{
			int64_t lvalue = 0;
			if (!json_format<int64_t>::pop(ajson, akey, lvalue))
			{
				return false;
			}
			adata = (int8_t)lvalue;
			return true;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, int8_t adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			int64_t lvalue = 0;
			if (!json_format<int64_t>::push(ajson, aallocator, akey, lvalue))
			{
				return false;
			}
			adata = (int8_t)lvalue;
			return true;
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, int8_t adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			ajson->PushBack(adata, *aallocator);
			return true;
		}
	};

	template <>
	struct json_format<uint8_t>
	{
		static bool pop(rapidjson::Value* ajson, const char* akey, uint8_t& adata)
		{
			int64_t lvalue = 0;
			if (!json_format<int64_t>::pop(ajson, akey, lvalue))
			{
				return false;
			}
			adata = (uint8_t)lvalue;
			return true;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, uint8_t adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			uint64_t lvalue = 0;
			if (!json_format<uint64_t>::push(ajson, aallocator, akey, lvalue))
			{
				return false;
			}
			adata = (uint8_t)lvalue;
			return true;
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, uint8_t adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			ajson->PushBack(adata, *aallocator);
			return true;
		}
	};
	
	template <>
	struct json_format<const char*>
	{
		static bool pop(rapidjson::Value* ajson, const char* akey, const char*& adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			rapidjson::Value* ljson = tool_rapidjson_value::find(ajson, akey);
			if (ljson == nullptr)
			{
				return false;
			}			
			if (!ljson->IsString())
			{
				return false;
			}
			adata = ljson->GetString();
			return true;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, const char* adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			rapidjson::Value val;
			val.SetString(adata, std::string(adata).size());
			ajson->AddMember(rapidjson::StringRef(akey), val, *aallocator);
			return true;
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			ajson->PushBack(rapidjson::StringRef(adata), *aallocator);
			return true;
		}
	};

	template <>
	struct json_format<std::string>
	{
		static bool pop(rapidjson::Value* ajson, const char* akey, std::string& adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			rapidjson::Value* ljson = tool_rapidjson_value::find(ajson, akey);
			if (ljson == nullptr)
			{
				return false;
			}
			if (!ljson->IsString())
			{
				return false;
			}
			adata = ljson->GetString();
			return true;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, const std::string& adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			rapidjson::Value val;
			val.SetString(adata.c_str(), adata.size());
			ajson->AddMember(rapidjson::StringRef(akey), val, *aallocator);
			return true;
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const std::string& adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			ajson->PushBack(rapidjson::StringRef(adata.c_str()), *aallocator);
			return true;
		}
	};

	template <>
	struct json_format<double>
	{
		static bool pop(rapidjson::Value* ajson, const char* akey, double& adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			rapidjson::Value* ljson = tool_rapidjson_value::find(ajson, akey);
			if (ljson == nullptr)
			{
				return false;
			}
			if (ljson->IsDouble())
			{
				return false;
			}
			adata = ljson->GetDouble();
			return true;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, double adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			rapidjson::Value val;
			val.SetDouble(adata);
			ajson->AddMember(rapidjson::StringRef(akey), val, *aallocator);
			return true;
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, double adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			ajson->PushBack(adata, *aallocator);
			return true;
		}
	};

	template <>
	struct json_format<float>
	{
		static bool pop(rapidjson::Value* ajson, const char* akey, float& adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			rapidjson::Value* ljson = tool_rapidjson_value::find(ajson, akey);
			if (ljson == nullptr)
			{
				return false;
			}
			if (!ljson->IsFloat())
			{
				return false;
			}
			adata = ljson->GetFloat();
			return true;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, float adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			rapidjson::Value val;
			val.SetFloat(adata);
			ajson->AddMember(rapidjson::StringRef(akey), val, *aallocator);
			return true;
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, float adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			ajson->PushBack(adata, *aallocator);
			return true;
		}
	};
	
	template <>
	struct json_format<bool>
	{
		static bool pop(rapidjson::Value* ajson, const char* akey, bool& adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			rapidjson::Value* ljson = tool_rapidjson_value::find(ajson, akey);
			if (ljson == nullptr)
			{
				return false;
			}
			if (ljson->IsBool())
			{
				return false;
			}
			adata = ljson->GetBool();
			return true;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, bool adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			rapidjson::Value val;
			val.SetBool(adata);
			ajson->AddMember(rapidjson::StringRef(akey), val, *aallocator);
			return true;
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, bool adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			ajson->PushBack(adata, *aallocator);
			return true;
		}
	};

	template <typename T>
	struct json_format<std::vector<T>>
	{
		static bool pop(rapidjson::Value* ajson, const char* akey, std::vector<T>& adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			rapidjson::Value* ljson = tool_rapidjson_value::find(ajson, akey);
			if (ljson == nullptr)
			{
				return false;
			}
			if (!ljson->IsArray())
			{
				return false;
			}
			auto ltemparray = ljson->GetArray();
			int32_t lcount = ltemparray.Size();
			adata.resize(lcount);
			int32_t lindex = 0;
			for (auto& item : ltemparray)
			{
				if (lindex >= lcount)
				{
					adata.clear();
					return false;
				}
				if (!json_format<T>::pop(&item, nullptr, adata[lindex]))
				{
					return false;
				}
				++lindex;
			}
			return true;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, const std::vector<T>& adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			rapidjson::Value val;
			val.SetArray();

			for (const auto& item : adata)
			{
				if (!json_format<T>::pushback(&val, aallocator, item))
				{
					return false;
				}
			}
			ajson->AddMember(rapidjson::StringRef(akey), val, *aallocator);

			return true;
		}
	};

	template <typename T>
	struct json_format<std::list<T>>
	{
		static bool pop(rapidjson::Value* ajson, const char* akey, std::list<T>& adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			rapidjson::Value* ljson = tool_rapidjson_value::find(ajson, akey);
			if (ljson == nullptr)
			{
				return false;
			}
			if (!ljson->IsArray())
			{
				return false;
			}
			auto ltemparray = ljson->GetArray();
			for (auto& item : ltemparray)
			{
				T ltemp;
				if (!json_format<T>::pop(&item, nullptr, ltemp))
				{
					return false;
				}
				adata.push_back(ltemp);
			}
			return true;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, const std::list<T>& adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			rapidjson::Value val;
			val.SetArray();

			for (const auto& item : adata)
			{
				if (!json_format<T>::pushback(&val, aallocator, item))
				{
					return false;
				}
			}
			ajson->AddMember(rapidjson::StringRef(akey), val, *aallocator);

			return true;
		}
	};

	template <typename T>
	struct json_format<std::set<T>>
	{
		static bool pop(rapidjson::Value* ajson, const char* akey, std::set<T>& adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			rapidjson::Value* ljson = tool_rapidjson_value::find(ajson, akey);
			if (ljson == nullptr)
			{
				return false;
			}
			if (!ljson->IsArray())
			{
				return false;
			}
			auto ltemparray = ljson->GetArray();
			for (auto& item : ltemparray)
			{
				T ltemp;
				if (!json_format<T>::pop(&item, nullptr, ltemp))
				{
					return false;
				}
				adata.insert(ltemp);
			}
			return true;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, const std::set<T>& adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			rapidjson::Value val;
			val.SetArray();

			for (const auto& item : adata)
			{
				if (!json_format<T>::pushback(&val, aallocator, item))
				{
					return false;
				}
			}
			ajson->AddMember(rapidjson::StringRef(akey), val, *aallocator);

			return true;
		}
	};

	template <typename TKEY, typename TVALUE>
	struct json_format<std::map<TKEY, TVALUE>>
	{
		static bool pop(rapidjson::Value* ajson, const char* akey, std::map<TKEY, TVALUE>& adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			rapidjson::Value* ljson = tool_rapidjson_value::find(ajson, akey);
			if (ljson == nullptr)
			{
				return false;
			}
			if (!ljson->IsArray())
			{
				return false;
			}
			auto ltemparray = ljson->GetArray();
			for (auto& item : ltemparray)
			{
				TKEY lkey;
				TVALUE lvalue;
				if (!json_format<TKEY>::pop(&item, "key", lkey))
				{
					return false;
				}
				if (!json_format<TVALUE>::pop(&item, "value", lvalue))
				{
					return false;
				}
				adata.insert(std::make_pair(lkey, lvalue));
			}
			return true;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, const std::map<TKEY, TVALUE>& adata)
		{
			if (ajson == nullptr)
			{
				return false;
			}
			rapidjson::Value val;
			val.SetArray();

			for (const auto& [_key, _value] : adata)
			{
				rapidjson::Value item;
				item.SetObject();
				if (!json_format<TKEY>::push(&item, aallocator, "key", _key))
				{
					return false;
				}
				if (!json_format<TVALUE>::push(&item, aallocator, "value", _value))
				{
					return false;
				}
				val.PushBack(item, *aallocator);
			}
			ajson->AddMember(rapidjson::StringRef(akey), val, *aallocator);

			return true;
		}
	};

	class njson
	{
		template <std::size_t... INDEX, typename ...TARGS>
		static bool pop(rapidjson::Value& ajson, std::index_sequence<INDEX...>, const std::array<const char*, sizeof...(TARGS)>& akeys, TARGS&... aargs)
		{
			return (json_format<TARGS>::pop(&ajson, akeys[INDEX], aargs) && ...);
		}

		template <std::size_t... INDEX, typename ...TARGS>
		static bool push(rapidjson::Value& ajson, rapidjson::Document::AllocatorType* aallocator, std::index_sequence<INDEX...>, const std::array<const char*, sizeof...(TARGS)>& akeys, const TARGS&... aargs)
		{
			return (json_format<TARGS>::push(&ajson, aallocator, akeys[INDEX], aargs)&& ...);
		}
	public:
		template <typename ...TARGS>
		static bool pop(ncjson& ajson, const std::array<const char*, sizeof...(TARGS)>& akeys, TARGS&... aargs)
		{
			rapidjson::Value& lval = static_cast<rapidjson::Value&>(ajson.doc());
			return pop(lval, std::make_index_sequence<sizeof...(TARGS)>{}, akeys, aargs...);
		}

		template <typename ...TARGS>
		static bool pop(rapidjson::Value& ajson, const std::array<const char*, sizeof...(TARGS)>& akeys, TARGS&... aargs)
		{
			return pop(ajson, std::make_index_sequence<sizeof...(TARGS)>{}, akeys, aargs...);
		}

		template <typename ...TARGS>
		static bool push(ncjson& ajson, const std::array<const char*, sizeof...(TARGS)>& akeys, const TARGS&... aargs)
		{
			ajson.set_object(); 
			rapidjson::Value& lval = static_cast<rapidjson::Value&>(ajson.doc());
			rapidjson::Document::AllocatorType* lallocator = &ajson.doc().GetAllocator();
			return push(lval, lallocator, std::make_index_sequence<sizeof...(TARGS)>{}, akeys, aargs...);
		}

		template <typename ...TARGS>
		static bool push(rapidjson::Value& ajson, rapidjson::Document::AllocatorType* aallocator, const std::array<const char*, sizeof...(TARGS)>& akeys, const TARGS&... aargs)
		{
			return push(ajson, aallocator, std::make_index_sequence<sizeof...(TARGS)>{}, akeys, aargs...);
		}
	};
}//namespace ngl

namespace ngl
{
	template <typename T>
	bool tools::json2custom(const std::string& ajson, T& adata)
	{
		ncjson ltemp;
		ltemp.parse(ajson.c_str());
		return njson::pop(ltemp, { tools::type_name<T>().c_str() }, adata);
	}

	template <typename T>
	bool tools::custom2json(const T& adata, std::string& json)
	{
		ncjson ltemp;
		njson::push(ltemp, { tools::type_name<T>().c_str() }, adata);
		json = ltemp.str();
		return true;
	}
}//namespace ngl

namespace ngl
{
	template <typename T>
	bool json_format<T>::pop(rapidjson::Value* ajson, const char* akey, T& adata)
	{
		if (ajson == nullptr)
		{
			return false;
		}
		rapidjson::Value* ljson = tool_rapidjson_value::find(ajson, akey);
		if (ljson == nullptr)
		{
			return false;
		}
		if constexpr (std::is_enum<T>::value)
		{
			int64_t lvalue = 0;
			if (!json_format<int64_t>::pop(ljson, nullptr, lvalue))
			{
				return false;
			}
			adata = (T)lvalue;
		}
		else
		{
			if constexpr (is_protobuf_message<T>::value)
			{
				std::string lstr = ncjson::vstr(ljson);
				if (lstr.empty())
				{
					return true;
				}
				if (!tools::json2proto(lstr, adata))
				{
					return false;
				}
			}
			else
			{
				return adata.json_pop(*ljson);
			}
		}
		return true;
	}

	template <typename T>
	bool json_format<T>::push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, const T& adata)
	{
		if (ajson == nullptr)
		{
			return false;
		}
		if constexpr (std::is_enum<T>::value)
		{
			if (!json_format<int64_t>::push(ajson, aallocator, akey, (int64_t)adata))
			{
				return false;
			}
		}
		else
		{
			if constexpr (is_protobuf_message<T>::value)
			{
				std::string lstr;
				if (!tools::proto2json(adata, lstr))
				{
					return false;
				}
				ncjson ljsontemp(lstr.c_str());
				if (!ljsontemp.parsecheck())
				{
					return false;
				}
				rapidjson::Value copiedValue;
				copiedValue.CopyFrom(ljsontemp.doc(), *aallocator);
				ajson->AddMember(rapidjson::StringRef(akey), copiedValue, *aallocator);
			}
			else
			{
				rapidjson::Value lval;
				lval.SetObject();
				if (!adata.json_push(lval, aallocator))
				{
					return false;
				}
				ajson->AddMember(rapidjson::StringRef(akey), lval, *aallocator);
			}
		}
		return true;
	}

	template <typename T>
	bool json_format<T>::pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const T& adata)
	{
		if constexpr (std::is_enum<T>::value)
		{
			if (!json_format<int64_t>::pushback(ajson, aallocator, (int64_t)adata))
			{
				return false;
			}
		}
		else
		{
			if constexpr (is_protobuf_message<T>::value)
			{
				std::string lstr;
				if (!tools::proto2json(adata, lstr))
				{
					return false;
				}
				ncjson ljsontemp(lstr.c_str());
				if (!ljsontemp.parsecheck())
				{
					return false;
				}
				rapidjson::Value copiedValue;
				copiedValue.CopyFrom(ljsontemp.doc(), *aallocator);
				ajson->PushBack(copiedValue, *aallocator);
			}
			else
			{
				rapidjson::Value ljson;
				ljson.SetObject();
				if (!adata.json_push(ljson, aallocator))
				{
					return false;
				}
				ajson->PushBack(ljson, *aallocator);
			}
		}
		return true;

	}
}//namespace ngl