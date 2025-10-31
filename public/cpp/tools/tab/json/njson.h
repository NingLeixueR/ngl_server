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

#include "cJSON.h"

#include <functional>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <set>

namespace ngl
{
	class njson_read
	{
		cJSON* m_json;
		bool  m_free;
	public:
		njson_read(const char* astr) :
			m_json(cJSON_Parse(astr))
			, m_free(true)
		{}

		njson_read() :
			m_json(nullptr)
			, m_free(false)
		{}

		bool check()
		{
			return m_json != nullptr;
		}

		bool set(cJSON* ajson)
		{
			if (m_json != nullptr && m_free != false)
			{
				return false;
			}
			m_json = ajson;
			return true;
		}

		cJSON*& json()
		{
			return m_json;
		}

		~njson_read()
		{
			if (m_json != nullptr && m_free)
			{
				cJSON_Delete(m_json);
			}
		}
	};

	class njson_write
	{
		cJSON* m_json;
		const char* m_nonformatstr;
		const char* m_str;
		bool m_isnonformatstr;
		bool m_free;
	public:
		njson_write() :
			m_json(cJSON_CreateObject())
			, m_nonformatstr(nullptr)
			, m_str(nullptr)
			, m_isnonformatstr(false)
			, m_free(true)
		{}

		~njson_write()
		{
			free_nonformatstr();
			free_str();
			if (m_json != nullptr && m_free)
			{
				cJSON_Delete(m_json);
				m_json = nullptr;
			}
		}

		cJSON*& json()
		{
			return m_json;
		}

		void free_str()
		{
			if (m_str != nullptr)
			{
				free((void*)m_str);
				m_str = nullptr;
			}
		}

		void free_nonformatstr()
		{
			if (m_nonformatstr != nullptr)
			{
				free((void*)m_nonformatstr);
				m_nonformatstr = nullptr;
			}
		}

		cJSON*& nofree()
		{
			m_free = false;
			return m_json;
		}

		void set_nonformatstr(bool abool)
		{
			m_isnonformatstr = abool;
		}

		const char* get()
		{
			if (m_isnonformatstr)
			{
				free_nonformatstr();
				m_nonformatstr = cJSON_PrintUnformatted(m_json);
				return m_nonformatstr;
			}
			else
			{
				free_str();
				m_str = cJSON_Print(m_json);
				return m_str;
			}
		}
	};

	template <typename T>
	struct json_format
	{
		static bool read(njson_read& ajson, const char* akey, T& adata);

		static bool read_array(njson_read& ajson, const char* akey, const std::function<void(T&)>& afun);

		static void write(njson_write& ajson, const char* akey, T& adata);
	};

	template <>
	struct json_format<cJSON*>
	{
		static bool read(njson_read& ajson, const char* akey, cJSON*& adata)
		{
			cJSON* ret = cJSON_GetObjectItem(ajson.json(), akey);
			if (nullptr == ret)
			{
				return false;
			}
			if (tools::check_cjson_or(ret->type, cJSON_Object, cJSON_Array))
			{
				adata = ret;
				return true;
			}
			return false;
		}

		static void write(njson_write& ajson, const char* akey, cJSON* adata)
		{
			cJSON_AddItemToObject(ajson.json(), akey, adata);
		}
	};

	struct tools_json
	{
		template <typename TNUMBER>
		static bool string_number(cJSON* ret, TNUMBER& adata)
		{
			if (tools::check_cjson_or(ret->type, cJSON_String))
			{
				try
				{
					adata = tools::lexical_cast<TNUMBER>(ret->valuestring);
					return true;
				}
				catch (...)
				{
					return false;
				}
			}
			return true;
		}

		template <typename TNUMBER>
		static bool read_basic_type(cJSON* ret, TNUMBER& adata)
		{
			adata = (TNUMBER)ret->valueint;
			return string_number(ret, adata);
		}

		static bool read_basic_type(cJSON* ret, float& adata)
		{
			adata = (float)ret->valuedouble;
			return string_number(ret, adata);
		}

		static bool read_basic_type(cJSON* ret, double& adata)
		{
			adata = (double)ret->valuedouble;
			return string_number(ret, adata);
		}

		static bool read_basic_type(cJSON* ret, bool& adata)
		{
			if (tools::check_cjson_or(ret->type, cJSON_True))
			{
				adata = true;
				return true;
			}
			if (tools::check_cjson_or(ret->type, cJSON_False))
			{
				adata = false;
				return true;
			}
			if (tools::check_cjson_or(ret->type, cJSON_Number))
			{
				adata = ret->valueint != 0;
				return true;
			}
			if (tools::check_cjson_or(ret->type, cJSON_String))
			{
				std::string lvaluestr(ret->valuestring);
				if (lvaluestr == "false" || lvaluestr == "FALSE" || lvaluestr == "0")
				{
					adata = false;
					return true;
				}
				if (lvaluestr == "true" || lvaluestr == "True" || lvaluestr == "1")
				{
					adata = true;
					return true;
				}
			}
			return false;
		}

		static bool read_basic_type(cJSON* ret, const char*& adata)
		{
			if (nullptr == ret)
			{
				return false;
			}
			if (tools::check_cjson_and(ret->type, cJSON_String))
			{
				return false;
			}
			adata = ret->valuestring;
			return true;
		}

		static bool read_basic_type(cJSON* ret, std::string& adata)
		{
			if (nullptr == ret)
			{
				return false;
			}
			if (tools::check_cjson_and(ret->type, cJSON_String))
			{
				return false;
			}
			adata = ret->valuestring;
			return true;
		}

		template <typename TNUMBER>
		static bool read_basic_type(njson_read& ajson, const char* akey, TNUMBER& adata)
		{
			cJSON* ret = cJSON_GetObjectItem(ajson.json(), akey);
			if (nullptr == ret)
			{
				return false;
			}
			if (tools::check_cjson_or(ret->type, cJSON_Number, cJSON_String, cJSON_True, cJSON_False))
			{
				return read_basic_type(ret, adata);
			}
			return false;
		}

		template <typename TNUMBER>
		static bool read_basic_array(njson_read& ajson, const char* akey, const std::function<void(TNUMBER&)>& afun)
		{
			cJSON* ltemp = nullptr;
			if (!json_format<cJSON*>::read(ajson, akey, ltemp))
			{
				return false;
			}
			int lsize = cJSON_GetArraySize(ltemp);
			for (int i = 0; i < lsize; ++i)
			{
				cJSON* ret = cJSON_GetArrayItem(ltemp, i);
				TNUMBER ltempnumber = TNUMBER();
				read_basic_type(ret, ltempnumber);
				afun(ltempnumber);
			}
			return true;
		}

		// 不可显示调用write_array<...>(...) 应该write_array(...)
		template <typename T>
		static void write_array(njson_write& ajson, const char* akey, const std::function<T*()>& afun)
		{
			if constexpr (std::is_enum<T>::value)
			{
				std::vector<int32_t> lvec;
				for (auto ltempitor = afun(); ltempitor != nullptr; ltempitor = afun())
				{
					lvec.push_back(*ltempitor);
				}
				write_array_item(ajson, akey, lvec);
			}
			else
			{
				cJSON* larray = cJSON_CreateArray();
				for (T* ltempitor = afun(); ltempitor != nullptr; ltempitor = afun())
				{
					njson_write ltemp;
					ltempitor->json_write(ltemp);
					cJSON_AddItemToArray(larray, ltemp.nofree());
				}
				json_format<cJSON*>::write(ajson, akey, larray);
			}
		}

		static void write_array_item(njson_write& ajson, const char* akey, std::vector<int32_t>& avec)
		{
			cJSON* larray = cJSON_CreateIntArray(avec.data(), (int32_t)avec.size());
			json_format<cJSON*>::write(ajson, akey, larray);
		}

		static void write_array_item(njson_write& ajson, const char* akey, std::vector<const char*>& avec)
		{
			cJSON* larray = cJSON_CreateStringArray(avec.data(), (int32_t)avec.size());
			json_format<cJSON*>::write(ajson, akey, larray);
		}

		static void write_array(njson_write& ajson, const char* akey, const std::function<int32_t* ()>& afun)
		{
			std::vector<int32_t> lvec;
			for (auto ltempitor = afun(); ltempitor != nullptr; ltempitor = afun())
			{
				lvec.push_back(*ltempitor);
			}
			write_array_item(ajson, akey, lvec);
		}

		static void write_array(njson_write& ajson, const char* akey, const std::function<std::string* ()>& afun)
		{
			std::vector<const char*> lvec;
			for (auto ltempitor = afun(); ltempitor != nullptr; ltempitor = afun())
			{
				lvec.push_back(ltempitor->c_str());
			}
			write_array_item(ajson, akey, lvec);
		}

		static void write_array(njson_write& ajson, const char* akey, const std::function<int8_t* ()>& afun)
		{
			std::vector<int32_t> lvec;
			for (auto ltempitor = afun(); ltempitor != nullptr; ltempitor = afun())
			{
				lvec.push_back(*ltempitor);
			}
			write_array_item(ajson, akey, lvec);
		}

		static void write_array(njson_write& ajson, const char* akey, const std::function<int16_t* ()>& afun)
		{
			std::vector<int32_t> lvec;
			for (auto ltempitor = afun(); ltempitor != nullptr; ltempitor = afun())
			{
				lvec.push_back(*ltempitor);
			}
			write_array_item(ajson, akey, lvec);
		}

		static void write_array(njson_write& ajson, const char* akey, const std::function<int64_t* ()>& afun)
		{
			std::vector<std::string> lvec1;
			std::vector<const char*> lvec2;
			for (auto ltempitor = afun(); ltempitor != nullptr; ltempitor = afun())
			{
				lvec1.push_back(tools::lexical_cast<std::string>(*ltempitor));
				lvec2.push_back(lvec1.rbegin()->c_str());
			}
			write_array_item(ajson, akey, lvec2);
		}

		static void write_array(njson_write& ajson, const char* akey, const std::function<uint8_t* ()>& afun)
		{
			std::vector<int32_t> lvec;
			for (auto ltempitor = afun(); ltempitor != nullptr; ltempitor = afun())
			{
				lvec.push_back(*ltempitor);
			}
			write_array_item(ajson, akey, lvec);
		}

		static void write_array(njson_write& ajson, const char* akey, const std::function<uint16_t* ()>& afun)
		{
			std::vector<int32_t> lvec;
			for (auto ltempitor = afun(); ltempitor != nullptr; ltempitor = afun())
			{
				lvec.push_back(*ltempitor);
			}
			write_array_item(ajson, akey, lvec);
		}

		static void write_array(njson_write& ajson, const char* akey, const std::function<uint32_t* ()>& afun)
		{
			std::vector<int32_t> lvec;
			for (auto ltempitor = afun(); ltempitor != nullptr; ltempitor = afun())
			{
				lvec.push_back(*ltempitor);
			}
			write_array_item(ajson, akey, lvec);
		}

		static void write_array(njson_write& ajson, const char* akey, const std::function<uint64_t* ()>& afun)
		{
			std::vector<std::string> lvec1;
			std::vector<const char*> lvec2;
			for (auto ltempitor = afun(); ltempitor != nullptr; ltempitor = afun())
			{
				lvec1.push_back(tools::lexical_cast<std::string>(*ltempitor));
				lvec2.push_back(lvec1.rbegin()->c_str());
			}
			write_array_item(ajson, akey, lvec2);
		}
	};

	template <>
	struct json_format<char*>
	{
		static bool read(njson_read& ajson, const char* akey, const char*& adata)
		{
			return tools_json::read_basic_type(ajson, akey, adata);
		}

		static bool read_array(njson_read& ajson, const char* akey, const std::function<void(const char*&)>& afun)
		{
			return tools_json::read_basic_array(ajson, akey, afun);
		}

		static void write(njson_write& ajson, const char* akey, const char* adata)
		{
			cJSON_AddItemToObject(ajson.json(), akey, cJSON_CreateString(adata));
		}
	};

	template <>
	struct json_format<std::string>
	{
		static bool read(njson_read& ajson, const char* akey, std::string& adata)
		{
			return tools_json::read_basic_type(ajson, akey, adata);
		}

		static bool read_array(njson_read& ajson, const char* akey, const std::function<void(std::string&)>& afun)
		{
			return tools_json::read_basic_array(ajson, akey, afun);
		}

		static void write(njson_write& ajson, const char* akey, std::string& adata)
		{
			json_format<char*>::write(ajson, akey, adata.c_str());
		}
	};

	template <>
	struct json_format<int8_t>
	{
		static bool read(njson_read& ajson, const char* akey, int8_t& adata)
		{
			return tools_json::read_basic_type(ajson, akey, adata);
		}

		static bool read_array(njson_read& ajson, const char* akey, const std::function<void(int8_t&)>& afun)
		{
			return tools_json::read_basic_array(ajson, akey, afun);
		}

		static void write(njson_write& ajson, const char* akey, const int8_t adata)
		{
			cJSON_AddNumberToObject(ajson.json(), akey, adata);
		}
	};

	template <>
	struct json_format<int16_t>
	{
		static bool read(njson_read& ajson, const char* akey, int16_t& adata)
		{
			return tools_json::read_basic_type(ajson, akey, adata);
		}

		static bool read_array(njson_read& ajson, const char* akey, const std::function<void(int16_t&)>& afun)
		{
			return tools_json::read_basic_array(ajson, akey, afun);
		}

		static void write(njson_write& ajson, const char* akey, const int16_t adata)
		{
			cJSON_AddNumberToObject(ajson.json(), akey, adata);
		}
	};

	template <>
	struct json_format<int32_t>
	{
		static bool read(njson_read& ajson, const char* akey, int32_t& adata)
		{
			return tools_json::read_basic_type(ajson, akey, adata);
		}

		static bool read_array(njson_read& ajson, const char* akey, const std::function<void(int32_t&)>& afun)
		{
			return tools_json::read_basic_array(ajson, akey, afun);
		}

		static void write(njson_write& ajson, const char* akey, const int32_t adata)
		{
			cJSON_AddNumberToObject(ajson.json(), akey, adata);
		}
	};

	template <>
	struct json_format<int64_t>
	{
		static bool read(njson_read& ajson, const char* akey, int64_t& adata)
		{
			return tools_json::read_basic_type(ajson, akey, adata);
		}

		static bool read_array(njson_read& ajson, const char* akey, const std::function<void(int64_t&)>& afun)
		{
			return tools_json::read_basic_array(ajson, akey, afun);
		}

		static void write(njson_write& ajson, const char* akey, const int64_t adata)
		{
			std::string lvalue = tools::lexical_cast<std::string>(adata);
			json_format<std::string>::write(ajson, akey, lvalue);
		}
	};

	template <>
	struct json_format<uint8_t>
	{
		static bool read(njson_read& ajson, const char* akey, uint8_t& adata)
		{
			return tools_json::read_basic_type(ajson, akey, adata);
		}

		static bool read_array(njson_read& ajson, const char* akey, const std::function<void(uint8_t&)>& afun)
		{
			return tools_json::read_basic_array(ajson, akey, afun);
		}

		static void write(njson_write& ajson, const char* akey, const uint8_t adata)
		{
			cJSON_AddNumberToObject(ajson.json(), akey, adata);
		}
	};

	template <>
	struct json_format<uint16_t>
	{
		static bool read(njson_read& ajson, const char* akey, uint16_t& adata)
		{
			return tools_json::read_basic_type(ajson, akey, adata);
		}

		static bool read_array(njson_read& ajson, const char* akey, const std::function<void(uint16_t&)>& afun)
		{
			return tools_json::read_basic_array(ajson, akey, afun);
		}

		static void write(njson_write& ajson, const char* akey, const uint16_t adata)
		{
			cJSON_AddNumberToObject(ajson.json(), akey, adata);
		}
	};

	template <>
	struct json_format<uint32_t>
	{
		static bool read(njson_read& ajson, const char* akey, uint32_t& adata)
		{
			return tools_json::read_basic_type(ajson, akey, adata);
		}

		static bool read_array(njson_read& ajson, const char* akey, const std::function<void(uint32_t&)>& afun)
		{
			return tools_json::read_basic_array(ajson, akey, afun);
		}

		static void write(njson_write& ajson, const char* akey, const uint32_t adata)
		{
			cJSON_AddNumberToObject(ajson.json(), akey, adata);
		}
	};

	template <>
	struct json_format<uint64_t>
	{
		static bool read(njson_read& ajson, const char* akey, uint64_t& adata)
		{
			return tools_json::read_basic_type(ajson, akey, adata);
		}

		static bool read_array(njson_read& ajson, const char* akey, const std::function<void(uint64_t&)>& afun)
		{
			return tools_json::read_basic_array(ajson, akey, afun);
		}

		static void write(njson_write& ajson, const char* akey, const uint64_t adata)
		{
			std::string lvalue = tools::lexical_cast<std::string>(adata);
			json_format<std::string>::write(ajson, akey, lvalue);
		}
	};

	template <>
	struct json_format<float>
	{
		static bool read(njson_read& ajson, const char* akey, float& adata)
		{
			return tools_json::read_basic_type(ajson, akey, adata);
		}

		static bool read_array(njson_read& ajson, const char* akey, const std::function<void(float&)>& afun)
		{
			return tools_json::read_basic_array(ajson, akey, afun);
		}

		static void write(njson_write& ajson, const char* akey, const float adata)
		{
			cJSON_AddNumberToObject(ajson.json(), akey, adata);
		}
	};

	template <>
	struct json_format<double>
	{
		static bool read(njson_read& ajson, const char* akey, double& adata)
		{
			return tools_json::read_basic_type(ajson, akey, adata);
		}

		static bool read_array(njson_read& ajson, const char* akey, const std::function<void(double&)>& afun)
		{
			return tools_json::read_basic_array(ajson, akey, afun);
		}

		static void write(njson_write& ajson, const char* akey, const double adata)
		{
			cJSON_AddNumberToObject(ajson.json(), akey, adata);
		}
	};

	template <>
	struct json_format<bool>
	{
		static bool read(njson_read& ajson, const char* akey, bool& adata)
		{
			return tools_json::read_basic_type(ajson, akey, adata);
		}

		static bool read_array(njson_read& ajson, const char* akey, const std::function<void(bool&)>& afun)
		{
			return tools_json::read_basic_array(ajson, akey, afun);
		}

		static void write(njson_write& ajson, const char* akey, const bool adata)
		{
			cJSON_AddItemToObject(ajson.json(), akey, adata ? cJSON_CreateTrue() : cJSON_CreateFalse());
		}
	};

	template <typename T>
	struct json_format<std::vector<T>>
	{
		static bool read(njson_read& ajson, const char* akey, std::vector<T>& adata)
		{
			return json_format<T>::read_array(ajson, akey, [&adata](T& aval)
				{
					adata.push_back(aval);
				});
		}

		static void write(njson_write& ajson, const char* akey, std::vector<T>& adata)
		{
			int32_t lindex = 0;
			std::function<T* ()> lfun = [&lindex, &adata]()->T*
				{
					if (lindex >= adata.size())
					{
						return nullptr;
					}
					return &adata[lindex++];
				};
			tools_json::write_array(ajson, akey, lfun);
		}
	};

	template <typename T>
	struct json_format<std::set<T>>
	{
		static bool read(njson_read& ajson, const char* akey, std::set<T>& adata)
		{
			return json_format<T>::read_array(ajson, akey, [&adata](T& aval)
				{
					adata.insert(aval);
				});
		}

		static bool read_array(njson_read& ajson, const char* akey, const std::function<void(std::vector<T>&)>& afun)
		{
			return false;
		}

		static void write(njson_write& ajson, const char* akey, std::set<T>& adata)
		{
			auto itor = adata.begin();
			tools_json::write_array(ajson, akey, [&itor, &adata]()->T*
				{
					if (itor == adata.end())
					{
						return nullptr;
					}
					T* ltemp = (T*)&(*itor);
					++itor;
					return ltemp;
				});
		}
	};

	template <typename TKEY, typename TVAL>
	struct json_format<std::map<TKEY, TVAL>>
	{
		static bool read(njson_read& ajson, const char* akey, std::map<TKEY, TVAL>& adata)
		{
			cJSON* lcjson = nullptr;
			if (!json_format<cJSON*>::read(ajson, akey, lcjson))
			{
				return false;
			}
			njson_read ljson;
			ljson.set(lcjson);
			for (cJSON* child = lcjson->child; child != nullptr; child = child->next)
			{
				TKEY lkey = tools::lexical_cast<TKEY>(child->string);
				if (!json_format<TVAL>::read(ljson, child->string, adata[lkey]))
				{
					return false;
				}
			}
			return true;
		}

		static bool read_array(njson_read& ajson, const char* akey, const std::function<void(std::map<TKEY, TVAL>&)>& afun)
		{
			return false;
		}

		static void write(njson_write& ajson, const char* akey, std::map<TKEY, TVAL>& adata)
		{
			njson_write ltemp;
			for (std::pair<const TKEY, TVAL>& item : adata)
			{
				json_format<TVAL>::write(ltemp, tools::lexical_cast<std::string>(item.first).c_str(), item.second);
			}
			json_format<cJSON*>::write(ajson, akey, ltemp.nofree());
		}
	};

	template <typename T>
	bool json_format<T>::read(njson_read& ajson, const char* akey, T& adata)
	{
		if constexpr (std::is_enum<T>::value)
		{
			int32_t lvalue = 0;
			if (!json_format<int32_t>::read(ajson, akey, lvalue))
			{
				return false;
			}
			adata = (T)lvalue;
			return true;
		}
		else
		{
			cJSON* ljson = nullptr;
			if (!json_format<cJSON*>::read(ajson, akey, ljson))
			{
				return false;
			}
			njson_read ltemp;
			if (!ltemp.set(ljson))
			{
				return false;
			}
			return adata.json_read(ltemp);
		}		
	}

	template <typename T>
	bool json_format<T>::read_array(njson_read& ajson, const char* akey, const std::function<void(T&)>& afun)
	{
		cJSON* ltemp = nullptr;
		if (!json_format<cJSON*>::read(ajson, akey, ltemp))
		{
			return false;
		}
		int lsize = cJSON_GetArraySize(ltemp);
		for (int i = 0; i < lsize; ++i)
		{
			cJSON* ret = cJSON_GetArrayItem(ltemp, i);
			njson_read ltemp;
			if (!ltemp.set(ret))
			{
				return false;
			}
			T ltempvalue;
			if (ltempvalue.json_read(ltemp))
			{
				afun(ltempvalue);
			}
		}
		return true;
	}

	template <typename T>
	void json_format<T>::write(njson_write& ajson, const char* akey, T& adata)
	{
		if constexpr (std::is_enum<T>::value)
		{
			json_format<int32_t>::write(ajson, akey, (int32_t)adata);
		}
		else if constexpr (is_protobuf_message<T>::value)
		{
			std::string ljsonstr;
			tools::proto2json(adata, ljsonstr);
			json_format<std::string>::write(ajson, akey, ljsonstr);
		}
		else
		{
			adata.json_write(ajson, akey);
		}
	}


	struct njson
	{
		static bool read(njson_read& ajson)
		{
			return true;
		}

		template <typename T>
		static bool read(njson_read& ajson, const char* akey, T& adata)
		{
			return json_format<T>::read(ajson, akey, adata);
		}

		template <typename T, typename ...TARGS>
		static bool read(njson_read& ajson, const char* akey, T& adata, TARGS&... aargs)
		{
			if (!read(ajson, akey, adata))
			{
				return false;
			}
			return read(ajson, aargs...);
		}

		static void write(njson_write& ajson)
		{
		}

		template <typename T>
		static void write(njson_write& ajson, const char* akey, T& adata)
		{
			json_format<T>::write(ajson, akey, adata);
		}

		static void write(njson_write& ajson, const char* akey, njson_write& adata)
		{
			json_format<cJSON*>::write(ajson, akey, adata.nofree());
		}

		template <typename T, typename ...TARGS>
		static void write(njson_write& ajson, const char* akey, T& adata, TARGS&... aargs)
		{
			write(ajson, akey, adata);
			write(ajson, aargs...);
		}
	};

}//namespace ngl

namespace ngl
{
	template <typename T>
	bool tools::json2custom(const std::string& json, T& adata)
	{
		njson_read ljread(json);
		return adata.json_read(ljread);
	}

	template <typename T>
	bool tools::custom2json(T& adata, std::string& json)
	{
		njson_write ljwrite;
		njson::write(ljwrite, tools::type_name<T>().c_str(), adata);

		json = ljwrite.get();

		return true;
	}
}//namespace ngl