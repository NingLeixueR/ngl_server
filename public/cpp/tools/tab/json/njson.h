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
// File overview: JSON configuration file reader and value accessor.

#pragma once

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "tools/tools.h"

#include <functional>
#include <limits>
#include <memory>
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
		explicit ncjson(const char* ajsonstr)
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
			// Cache the compact representation so callers can safely use the
			// returned pointer until the next serialization.
			rapidjson::StringBuffer buffer;
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			m_doc.Accept(writer);
			m_nonformat_str = buffer.GetString();
			return m_nonformat_str.c_str();
		}

		const char* str()
		{
			// Pretty output is mainly used for diagnostics and exported config.
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
			rapidjson::Value* ljson = ajson;
			if (akey != nullptr)
			{
				if (!ajson->IsObject())
				{
					return false;
				}
				auto itor = ajson->FindMember(akey);
				if (itor == ajson->MemberEnd())
				{
					return false;
				}
				ljson = &itor->value;
			}
			if (ljson == nullptr)
			{
				return false;
			}
			std::shared_ptr<T> ltemp = std::make_shared<T>();
			if (!json_format<T>::pop(ljson, nullptr, *ltemp))
			{
				return false;
			}
			adata = std::move(ltemp);
			return true;
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
		// Finds a child member when a key is provided, otherwise returns the
		// input node itself.
		static rapidjson::Value* find(rapidjson::Value* ajson, const char* akey)
		{
			if (ajson == nullptr)
			{
				return nullptr;
			}
			if (akey != nullptr)
			{
				if (!ajson->IsObject())
				{
					return nullptr;
				}
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

	namespace njson_detail
	{
		inline bool add_member(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, rapidjson::Value& avalue)
		{
			if (ajson == nullptr || aallocator == nullptr || akey == nullptr || !ajson->IsObject())
			{
				return false;
			}
			rapidjson::Value lkey;
			lkey.SetString(akey, *aallocator);
			ajson->AddMember(lkey, avalue, *aallocator);
			return true;
		}

		inline bool push_back(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, rapidjson::Value& avalue)
		{
			if (ajson == nullptr || aallocator == nullptr || !ajson->IsArray())
			{
				return false;
			}
			ajson->PushBack(avalue, *aallocator);
			return true;
		}

		template <typename T>
		bool assign_signed_integral(int64_t avalue, T& adata)
		{
			// Range checks prevent silent narrowing when reading JSON numbers.
			if (avalue < static_cast<int64_t>(std::numeric_limits<T>::min()) || avalue > static_cast<int64_t>(std::numeric_limits<T>::max()))
			{
				return false;
			}
			adata = static_cast<T>(avalue);
			return true;
		}

		template <typename T>
		bool assign_unsigned_integral(uint64_t avalue, T& adata)
		{
			if (avalue > static_cast<uint64_t>(std::numeric_limits<T>::max()))
			{
				return false;
			}
			adata = static_cast<T>(avalue);
			return true;
		}
	}

	

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

			if (ljson->IsInt64())
			{
				adata = ljson->GetInt64();
				return true;
			}

			if (ljson->IsInt())
			{
				adata = ljson->GetInt();
				return true;
			}

			if (ljson->IsString())
			{
				std::string lvalue = ljson->GetString();
				try
				{
					adata = tools::lexical_cast<int64_t>(lvalue);
				}
				catch (...)
				{
					return false;
				}
				return true;
			}
			
			return false;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, int64_t adata)
		{
			rapidjson::Value val;
			val.SetInt64(adata);
			return njson_detail::add_member(ajson, aallocator, akey, val);
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, int64_t adata)
		{
			rapidjson::Value val;
			val.SetInt64(adata);
			return njson_detail::push_back(ajson, aallocator, val);
		}
	};

	template <>
	struct json_format<uint64_t>
	{
		static bool pop(rapidjson::Value* ajson, const char* akey, uint64_t& adata)
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

			if (ljson->IsUint64())
			{
				adata = ljson->GetUint64();
				return true;
			}

			if (ljson->IsUint())
			{
				adata = ljson->GetUint();
				return true;
			}

			return false;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, uint64_t adata)
		{
			rapidjson::Value val;
			val.SetUint64(adata);
			return njson_detail::add_member(ajson, aallocator, akey, val);
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, uint64_t adata)
		{
			rapidjson::Value val;
			val.SetUint64(adata);
			return njson_detail::push_back(ajson, aallocator, val);
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
			return njson_detail::assign_signed_integral(lvalue, adata);
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, int32_t adata)
		{
			return json_format<int64_t>::push(ajson, aallocator, akey, static_cast<int64_t>(adata));
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, int32_t adata)
		{
			rapidjson::Value val;
			val.SetInt(adata);
			return njson_detail::push_back(ajson, aallocator, val);
		}
	};

	template <>
	struct json_format<uint32_t>
	{
		static bool pop(rapidjson::Value* ajson, const char* akey, uint32_t& adata)
		{
			uint64_t lvalue = 0;
			if (!json_format<uint64_t>::pop(ajson, akey, lvalue))
			{
				return false;
			}
			return njson_detail::assign_unsigned_integral(lvalue, adata);
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, uint32_t adata)
		{
			return json_format<uint64_t>::push(ajson, aallocator, akey, static_cast<uint64_t>(adata));
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, uint32_t adata)
		{
			rapidjson::Value val;
			val.SetUint(adata);
			return njson_detail::push_back(ajson, aallocator, val);
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
			return njson_detail::assign_signed_integral(lvalue, adata);
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, int16_t adata)
		{
			return json_format<int64_t>::push(ajson, aallocator, akey, static_cast<int64_t>(adata));
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, int16_t adata)
		{
			rapidjson::Value val;
			val.SetInt(adata);
			return njson_detail::push_back(ajson, aallocator, val);
		}
	};

	template <>
	struct json_format<uint16_t>
	{
		static bool pop(rapidjson::Value* ajson, const char* akey, uint16_t& adata)
		{
			uint64_t lvalue = 0;
			if (!json_format<uint64_t>::pop(ajson, akey, lvalue))
			{
				return false;
			}
			return njson_detail::assign_unsigned_integral(lvalue, adata);
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, uint16_t adata)
		{
			return json_format<uint64_t>::push(ajson, aallocator, akey, static_cast<uint64_t>(adata));
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, uint16_t adata)
		{
			rapidjson::Value val;
			val.SetUint(adata);
			return njson_detail::push_back(ajson, aallocator, val);
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
			return njson_detail::assign_signed_integral(lvalue, adata);
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, int8_t adata)
		{
			return json_format<int64_t>::push(ajson, aallocator, akey, static_cast<int64_t>(adata));
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, int8_t adata)
		{
			rapidjson::Value val;
			val.SetInt(adata);
			return njson_detail::push_back(ajson, aallocator, val);
		}
	};

	template <>
	struct json_format<uint8_t>
	{
		static bool pop(rapidjson::Value* ajson, const char* akey, uint8_t& adata)
		{
			uint64_t lvalue = 0;
			if (!json_format<uint64_t>::pop(ajson, akey, lvalue))
			{
				return false;
			}
			return njson_detail::assign_unsigned_integral(lvalue, adata);
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, uint8_t adata)
		{
			return json_format<uint64_t>::push(ajson, aallocator, akey, static_cast<uint64_t>(adata));
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, uint8_t adata)
		{
			rapidjson::Value val;
			val.SetUint(adata);
			return njson_detail::push_back(ajson, aallocator, val);
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
			if (adata == nullptr)
			{
				return false;
			}
			rapidjson::Value val;
			val.SetString(adata, *aallocator);
			return njson_detail::add_member(ajson, aallocator, akey, val);
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* adata)
		{
			if (adata == nullptr)
			{
				return false;
			}
			rapidjson::Value val;
			val.SetString(adata, *aallocator);
			return njson_detail::push_back(ajson, aallocator, val);
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
			rapidjson::Value val;
			val.SetString(adata.c_str(), static_cast<rapidjson::SizeType>(adata.size()), *aallocator);
			return njson_detail::add_member(ajson, aallocator, akey, val);
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const std::string& adata)
		{
			rapidjson::Value val;
			val.SetString(adata.c_str(), static_cast<rapidjson::SizeType>(adata.size()), *aallocator);
			return njson_detail::push_back(ajson, aallocator, val);
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
			if (!ljson->IsNumber())
			{
				return false;
			}
			adata = ljson->GetDouble();
			return true;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, double adata)
		{
			rapidjson::Value val;
			val.SetDouble(adata);
			return njson_detail::add_member(ajson, aallocator, akey, val);
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, double adata)
		{
			rapidjson::Value val;
			val.SetDouble(adata);
			return njson_detail::push_back(ajson, aallocator, val);
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
			if (!ljson->IsNumber())
			{
				return false;
			}
			adata = ljson->GetFloat();
			return true;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, float adata)
		{
			rapidjson::Value val;
			val.SetFloat(adata);
			return njson_detail::add_member(ajson, aallocator, akey, val);
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, float adata)
		{
			rapidjson::Value val;
			val.SetFloat(adata);
			return njson_detail::push_back(ajson, aallocator, val);
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
			if (!ljson->IsBool())
			{
				return false;
			}
			adata = ljson->GetBool();
			return true;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, bool adata)
		{
			rapidjson::Value val;
			val.SetBool(adata);
			return njson_detail::add_member(ajson, aallocator, akey, val);
		}

		static bool pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, bool adata)
		{
			rapidjson::Value val;
			val.SetBool(adata);
			return njson_detail::push_back(ajson, aallocator, val);
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
			std::vector<T> lparsed;
			lparsed.reserve(ltemparray.Size());
			for (auto& item : ltemparray)
			{
				T ltemp;
				if (!json_format<T>::pop(&item, nullptr, ltemp))
				{
					return false;
				}
				lparsed.push_back(std::move(ltemp));
			}
			adata = std::move(lparsed);
			return true;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, const std::vector<T>& adata)
		{
			if (ajson == nullptr || aallocator == nullptr || akey == nullptr || !ajson->IsObject())
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
			return njson_detail::add_member(ajson, aallocator, akey, val);
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
			std::list<T> lparsed;
			auto ltemparray = ljson->GetArray();
			for (auto& item : ltemparray)
			{
				T ltemp;
				if (!json_format<T>::pop(&item, nullptr, ltemp))
				{
					return false;
				}
				lparsed.push_back(std::move(ltemp));
			}
			adata = std::move(lparsed);
			return true;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, const std::list<T>& adata)
		{
			if (ajson == nullptr || aallocator == nullptr || akey == nullptr || !ajson->IsObject())
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
			return njson_detail::add_member(ajson, aallocator, akey, val);
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
			std::set<T> lparsed;
			for (auto& item : ltemparray)
			{
				T ltemp;
				if (!json_format<T>::pop(&item, nullptr, ltemp))
				{
					return false;
				}
				if (!lparsed.insert(std::move(ltemp)).second)
				{
					return false;
				}
			}
			adata = std::move(lparsed);
			return true;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, const std::set<T>& adata)
		{
			if (ajson == nullptr || aallocator == nullptr || akey == nullptr || !ajson->IsObject())
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
			return njson_detail::add_member(ajson, aallocator, akey, val);
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
			std::map<TKEY, TVALUE> lparsed;
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
				if (!lparsed.emplace(std::move(lkey), std::move(lvalue)).second)
				{
					return false;
				}
			}
			adata = std::move(lparsed);
			return true;
		}

		static bool push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, const std::map<TKEY, TVALUE>& adata)
		{
			if (ajson == nullptr || aallocator == nullptr || akey == nullptr || !ajson->IsObject())
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
			return njson_detail::add_member(ajson, aallocator, akey, val);
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
		// Variadic front-end that maps parallel field-name arrays to values.
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
		ncjson ltemp(ajson.c_str());
		if (!ltemp.parsecheck())
		{
			return false;
		}
		return njson::pop(ltemp, { tools::type_name<T>().c_str() }, adata);
	}

	template <typename T>
	bool tools::custom2json(const T& adata, std::string& json)
	{
		ncjson ltemp;
		if (!njson::push(ltemp, { tools::type_name<T>().c_str() }, adata))
		{
			return false;
		}
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
			adata = static_cast<T>(lvalue);
			return true;
		}
		else if constexpr (tools::is_protobuf_message<T>::value)
		{
			std::string lstr = ncjson::vstr(ljson);
			if (lstr.empty())
			{
				return true;
			}
			T ltemp;
			if (!tools::json2proto(lstr, ltemp))
			{
				return false;
			}
			adata = std::move(ltemp);
			return true;
		}
		else
		{
			return adata.json_pop(*ljson);
		}
	}

	template <typename T>
	bool json_format<T>::push(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const char* akey, const T& adata)
	{
		if (ajson == nullptr || aallocator == nullptr)
		{
			return false;
		}
		if constexpr (std::is_enum<T>::value)
		{
			if (!json_format<int64_t>::push(ajson, aallocator, akey, static_cast<int64_t>(adata)))
			{
				return false;
			}
			return true;
		}
		else if constexpr (tools::is_protobuf_message<T>::value)
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
			return njson_detail::add_member(ajson, aallocator, akey, copiedValue);
		}
		else
		{
			rapidjson::Value lval;
			lval.SetObject();
			if (!adata.json_push(lval, aallocator))
			{
				return false;
			}
			return njson_detail::add_member(ajson, aallocator, akey, lval);
		}
	}

	template <typename T>
	bool json_format<T>::pushback(rapidjson::Value* ajson, rapidjson::Document::AllocatorType* aallocator, const T& adata)
	{
		if (ajson == nullptr || aallocator == nullptr)
		{
			return false;
		}
		if constexpr (std::is_enum<T>::value)
		{
			if (!json_format<int64_t>::pushback(ajson, aallocator, static_cast<int64_t>(adata)))
			{
				return false;
			}
			return true;
		}
		else if constexpr (tools::is_protobuf_message<T>::value)
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
			return njson_detail::push_back(ajson, aallocator, copiedValue);
		}
		else
		{
			rapidjson::Value ljson;
			ljson.SetObject();
			if (!adata.json_push(ljson, aallocator))
			{
				return false;
			}
			return njson_detail::push_back(ajson, aallocator, ljson);
		}
	}
}//namespace ngl
