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
#include "tools.h"

#include <functional>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <set>

namespace ngl
{
	template <typename T>
	struct json_format
	{
		static bool pop(cJSON* ajson, const char* akey, T& adata);

		static void push(cJSON* ajson, const char* akey, const T& adata);
		static void push(cJSON* ajson, const T& adata);
	};

	template <>
	struct json_format<int64_t>
	{
		static bool pop(cJSON* ajson, const char* akey, int64_t& adata)
		{
			cJSON* ljson = ajson;
			if (akey != nullptr)
			{
				ljson = cJSON_GetObjectItem(ajson, akey);
			}
			if (tools::bit(ljson->type, cJSON_Number))
			{
				adata = (int64_t)ljson->valueint;
				return true;
			}
			if (tools::bit(ljson->type, cJSON_String))
			{
				adata = tools::lexical_cast<int64_t>(ljson->valuestring);
				return true;
			}
			if (tools::bit(ljson->type, cJSON_True))
			{
				adata = 1;
				return true;
			}
			if (tools::bit(ljson->type, cJSON_False))
			{
				adata = 0;
				return true;
			}
			return false;
		}

		static cJSON* create(const int64_t adata)
		{
			return cJSON_CreateString(tools::lexical_cast<std::string>(adata).c_str());
		}
		static void push(cJSON* ajson, const char* akey, const int64_t adata)
		{
			cJSON_AddItemToObject(ajson, akey, create(adata));
		}
		static void push(cJSON* ajson, const int64_t adata)
		{
			cJSON_AddItemToArray(ajson, create(adata));
		}
	};

	template <>
	struct json_format<uint64_t>
	{
		static bool pop(cJSON* ajson, const char* akey, uint64_t& adata)
		{
			int64_t lvalue = 0;
			if (!json_format<int64_t>::pop(ajson, akey, lvalue))
			{
				return false;
			}
			adata = (uint64_t)lvalue;
			return true;
		}

		static cJSON* create(const uint64_t adata)
		{
			return cJSON_CreateString(tools::lexical_cast<std::string>(adata).c_str());
		}
		static void push(cJSON* ajson, const char* akey, const uint64_t adata)
		{
			cJSON_AddItemToObject(ajson, akey, create(adata));
		}
		static void push(cJSON* ajson, const uint64_t adata)
		{
			cJSON_AddItemToArray(ajson, create(adata));
		}
	};

	template <>
	struct json_format<int32_t>
	{
		static bool pop(cJSON* ajson, const char* akey, int32_t adata)
		{
			int64_t lvalue = 0;
			if (!json_format<int64_t>::pop(ajson, akey, lvalue))
			{
				return false;
			}
			adata = (int32_t)lvalue;
			return true;
		}

		static cJSON* create(const int32_t adata)
		{
			return cJSON_CreateNumber(adata);
		}
		static void push(cJSON* ajson, const char* akey, const int32_t adata)
		{
			cJSON_AddNumberToObject(ajson, akey, adata);
		}
		static void push(cJSON* ajson, const int32_t adata)
		{
			cJSON_AddItemToArray(ajson, create(adata));
		}
	};

	template <>
	struct json_format<uint32_t>
	{
		static bool pop(cJSON* ajson, const char* akey, uint32_t& adata)
		{
			int64_t lvalue = 0;
			if (!json_format<int64_t>::pop(ajson, akey, lvalue))
			{
				return false;
			}
			adata = (uint32_t)lvalue;
			return true;
		}

		static void push(cJSON* ajson, const char* akey, const uint32_t adata)
		{
			json_format<int32_t>::push(ajson, akey, (int32_t)adata);
		}
		static void push(cJSON* ajson, const int32_t adata)
		{
			json_format<int32_t>::push(ajson, (int32_t)adata);
		}
	};

	template <>
	struct json_format<int16_t>
	{
		static bool pop(cJSON* ajson, const char* akey, int16_t& adata)
		{
			int64_t lvalue = 0;
			if (!json_format<int64_t>::pop(ajson, akey, lvalue))
			{
				return false;
			}
			adata = (int16_t)lvalue;
			return true;
		}

		static void push(cJSON* ajson, const char* akey, const int16_t adata)
		{
			json_format<int32_t>::push(ajson, akey, (int32_t)adata);
		}
		static void push(cJSON* ajson, const int16_t adata)
		{
			json_format<int32_t>::push(ajson, (int32_t)adata);
		}
	};

	template <>
	struct json_format<uint16_t>
	{
		static bool pop(cJSON* ajson, const char* akey, uint16_t& adata)
		{
			int64_t lvalue = 0;
			if (!json_format<int64_t>::pop(ajson, akey, lvalue))
			{
				return false;
			}
			adata = (uint16_t)lvalue;
			return true;
		}

		static void push(cJSON* ajson, const char* akey, const uint16_t adata)
		{
			json_format<int32_t>::push(ajson, akey, (int32_t)adata);
		}
		static void push(cJSON* ajson, const uint16_t adata)
		{
			json_format<int32_t>::push(ajson, (int32_t)adata);
		}
	};

	template <>
	struct json_format<int8_t>
	{
		static bool pop(cJSON* ajson, const char* akey, int8_t& adata)
		{
			int64_t lvalue = 0;
			if (!json_format<int64_t>::pop(ajson, akey, lvalue))
			{
				return false;
			}
			adata = (int8_t)lvalue;
			return true;
		}

		static void push(cJSON* ajson, const char* akey, const int8_t adata)
		{
			json_format<int32_t>::push(ajson, akey, (int32_t)adata);
		}
		static void push(cJSON* ajson, const int8_t adata)
		{
			json_format<int32_t>::push(ajson, (int32_t)adata);
		}
	};

	template <>
	struct json_format<uint8_t>
	{
		static bool pop(cJSON* ajson, const char* akey, uint8_t& adata)
		{
			int64_t lvalue = 0;
			if (!json_format<int64_t>::pop(ajson, akey, lvalue))
			{
				return false;
			}
			adata = (uint8_t)lvalue;
			return true;
		}

		static void push(cJSON* ajson, const char* akey, const uint8_t adata)
		{
			json_format<int32_t>::push(ajson, akey, (int32_t)adata);
		}
		static void push(cJSON* ajson, const uint8_t adata)
		{
			json_format<int32_t>::push(ajson, (int32_t)adata);
		}
	};
	
	template <>
	struct json_format<const char*>
	{
		static bool pop(cJSON* ajson, const char* akey, const char*& adata)
		{
			cJSON* ljson = ajson;
			if (akey != nullptr)
			{
				ljson = cJSON_GetObjectItem(ajson, akey);
			}
			if (!tools::bit(ljson->type, cJSON_String))
			{
				return false;
			}
			adata = ljson->valuestring;
			return true;
		}

		static void push(cJSON* ajson, const char* akey, const char* adata)
		{
			cJSON_AddItemToObject(ajson, akey, cJSON_CreateString(adata));
		}
		static void push(cJSON* ajson, const char* adata)
		{
			cJSON_AddItemToArray(ajson, cJSON_CreateString(adata));
		}
	};

	template <>
	struct json_format<std::string>
	{
		static bool pop(cJSON* ajson, const char* akey, std::string& adata)
		{
			cJSON* ljson = ajson;
			if (akey != nullptr)
			{
				ljson = cJSON_GetObjectItem(ajson, akey);
			}
			if (!tools::bit(ljson->type, cJSON_String))
			{
				return false;
			}
			adata = ljson->valuestring;
			return true;
		}

		static void push(cJSON* ajson, const char* akey, const std::string& adata)
		{
			cJSON_AddItemToObject(ajson, akey, cJSON_CreateString(adata.c_str()));
		}
		static void push(cJSON* ajson, const std::string& adata)
		{
			cJSON_AddItemToArray(ajson, cJSON_CreateString(adata.c_str()));
		}
	};

	template <>
	struct json_format<double>
	{
		static bool pop(cJSON* ajson, const char* akey, double& adata)
		{
			cJSON* ljson = ajson;
			if (akey != nullptr)
			{
				ljson = cJSON_GetObjectItem(ajson, akey);
			}
			if (tools::bit(ljson->type, cJSON_Number))
			{
				adata = (float)ljson->valuedouble;
				return true;
			}
			if (tools::bit(ljson->type, cJSON_String))
			{
				adata = tools::lexical_cast<double>(ljson->valuestring);
				return true;
			}
			if (tools::bit(ljson->type, cJSON_True))
			{
				adata = 1.0f;
				return true;
			}
			if (tools::bit(ljson->type, cJSON_True))
			{
				adata = 1.0f;
				return true;
			}
			return false;
		}

		static void push(cJSON* ajson, const char* akey, const double adata)
		{
			cJSON_AddNumberToObject(ajson, akey, adata);
		}
		static void push(cJSON* ajson, const double adata)
		{
			cJSON_AddItemToArray(ajson, cJSON_CreateNumber(adata));
		}
	};

	template <>
	struct json_format<float>
	{
		static bool pop(cJSON* ajson, const char* akey, float& adata)
		{
			double lvalue = 0.0f;
			if (!json_format<double>::pop(ajson, akey, lvalue))
			{
				return false;
			}
			adata = (float)lvalue; 
			return true;
		}

		static void push(cJSON* ajson, const char* akey, const float adata)
		{
			cJSON_AddNumberToObject(ajson, akey, adata);
		}
		static void push(cJSON* ajson, const float adata)
		{
			cJSON_AddItemToArray(ajson, cJSON_CreateNumber(adata));
		}
	};
	
	template <>
	struct json_format<bool>
	{
		static bool pop(cJSON* ajson, const char* akey, bool& adata)
		{
			cJSON* ljson = ajson;
			if (akey != nullptr)
			{
				ljson = cJSON_GetObjectItem(ajson, akey);
			}
			if (tools::bit(ljson->type, cJSON_String))
			{
				std::string lvalue = ljson->valuestring;
				tools::transform_toupper(lvalue);
				if (lvalue == "TRUE")
				{
					adata = true;
					return true;
				}
				if (lvalue == "FALSE")
				{
					adata = false;
					return true;
				}
				return false;
			}
			if (tools::bit(ljson->type, cJSON_True))
			{
				adata = true;
				return true;
			}
			if (tools::bit(ljson->type, cJSON_True))
			{
				adata = false;
				return true;
			}
			return false;
		}

		static void push(cJSON* ajson, const char* akey, const bool adata)
		{
			cJSON_AddItemToObject(ajson, akey, adata ? cJSON_CreateTrue() : cJSON_CreateFalse());
		}
		static void push(cJSON* ajson, const bool adata)
		{
			cJSON_AddItemToArray(ajson, adata ? cJSON_CreateTrue() : cJSON_CreateFalse());
		}
	};

	template <typename T>
	struct json_format<std::vector<T>>
	{
		static bool pop(cJSON* ajson, const char* akey, std::vector<T>& adata)
		{
			cJSON* ljson = ajson;
			if (akey != nullptr)
			{
				ljson = cJSON_GetObjectItem(ajson, akey);
			}
			if (!tools::bit(ljson->type, cJSON_Object) && !tools::bit(ljson->type, cJSON_Array))
			{
				return false;
			}
			int lsize = cJSON_GetArraySize(ljson);
			for (int i = 0; i < lsize; ++i)
			{
				cJSON* item = cJSON_GetArrayItem(ljson, i);
				if (item == nullptr)
				{
					return false;
				}
				T ltemp = T();
				if (!json_format<T>::pop(item, nullptr, ltemp))
				{
					return false;
				}
				adata.push_back(ltemp);
			}
		}

		static void push(cJSON* ajson, const std::vector<T>& adata)
		{
			for (const T& item : adata)
			{
				if constexpr (std::is_class<T>::value)
				{
					cJSON* ljson = cJSON_CreateObject();
					json_format<T>::push(ljson, item);
					cJSON_AddItemToArray(ajson, ljson);
				}
				else
				{
					json_format<T>::push(ajson, item);
				}
			}
		}

		static void push(cJSON* ajson, const char* akey, const std::vector<T>& adata)
		{
			cJSON* ljson = cJSON_CreateArray();
			push(ljson, adata);
			cJSON_AddItemToObject(ajson, akey, ljson);
		}
	};

	template <typename T>
	struct json_format<std::list<T>>
	{
		static bool pop(cJSON* ajson, const char* akey, std::list<T>& adata)
		{
			cJSON* ljson = ajson;
			if (akey != nullptr)
			{
				ljson = cJSON_GetObjectItem(ajson, akey);
			}
			if (!tools::bit(ljson->type, cJSON_Object) && !tools::bit(ljson->type, cJSON_Array))
			{
				return false;
			}
			int lsize = cJSON_GetArraySize(ljson);
			for (int i = 0; i < lsize; ++i)
			{
				cJSON* item = cJSON_GetArrayItem(ljson, i);
				if (item == nullptr)
				{
					return false;
				}
				T ltemp = T();
				if (!json_format<T>::pop(item, nullptr, ltemp))
				{
					return false;
				}
				adata.push_back(ltemp);
			}
		}

		static void push(cJSON* ajson, const std::list<T>& adata)
		{
			for (const T& item : adata)
			{
				if constexpr (std::is_class<T>::value)
				{
					cJSON* ljson = cJSON_CreateObject();
					json_format<T>::push(ljson, item);
					cJSON_AddItemToArray(ajson, ljson);
				}
				else
				{
					json_format<T>::push(ajson, item);
				}
			}
		}

		static void push(cJSON* ajson, const char* akey, const std::list<T>& adata)
		{
			cJSON* ljson = cJSON_CreateArray();
			push(ljson, adata);
			cJSON_AddItemToObject(ajson, akey, ljson);
		}
	};

	template <typename T>
	struct json_format<std::set<T>>
	{
		static bool pop(cJSON* ajson, const char* akey, std::set<T>& adata)
		{
			cJSON* ljson = ajson;
			if (akey != nullptr)
			{
				ljson = cJSON_GetObjectItem(ajson, akey);
			}
			if (!tools::bit(ljson->type, cJSON_Object) && !tools::bit(ljson->type, cJSON_Array))
			{
				return false;
			}
			int lsize = cJSON_GetArraySize(ljson);
			for (int i = 0; i < lsize; ++i)
			{
				cJSON* item = cJSON_GetArrayItem(ljson, i);
				if (item == nullptr)
				{
					return false;
				}
				T ltemp = T();
				if (!json_format<T>::pop(item, nullptr, ltemp))
				{
					return false;
				}
				adata.insert(ltemp);
			}
		}

		static void push(cJSON* ajson, const std::set<T>& adata)
		{
			for (const T& item : adata)
			{
				if constexpr (std::is_class<T>::value)
				{
					cJSON* ljson = cJSON_CreateObject();
					json_format<T>::push(ljson, item);
					cJSON_AddItemToArray(ajson, ljson);
				}
				else
				{
					json_format<T>::push(ajson, item);
				}
			}
		}

		static void push(cJSON* ajson, const char* akey, const std::set<T>& adata)
		{
			cJSON* ljson = cJSON_CreateArray();
			push(ljson, adata);
			cJSON_AddItemToObject(ajson, akey, ljson);
		}
	};

	template <typename TKEY, typename TVALUE>
	struct json_format<std::map<TKEY, TVALUE>>
	{
		static bool pop(cJSON* ajson, const char* akey, std::map<TKEY, TVALUE>& adata)
		{
			cJSON* ljson = ajson;
			if (akey != nullptr)
			{
				ljson = cJSON_GetObjectItem(ajson, akey);
			}
			if (!tools::bit(ljson->type, cJSON_Object) && !tools::bit(ljson->type, cJSON_Array))
			{
				return false;
			}
			int lsize = cJSON_GetArraySize(ljson);
			for (int i = 0; i < lsize; ++i)
			{
				cJSON* item = cJSON_GetArrayItem(ljson, i);
				if (item == nullptr)
				{
					return false;
				}
				TKEY lkey = TKEY();
				TVALUE lvalue = TVALUE();
				if (!json_format<TKEY>::pop(item, "key", lkey))
				{
					return false;
				}
				if (!json_format<TVALUE>::pop(item, "value", lvalue))
				{
					return false;
				}
				adata.insert(std::make_pair(lkey, lvalue));
			}
			return true;
		}

		static void push(cJSON* ajson, const std::map<TKEY, TVALUE>& adata)
		{
			for (const auto& item : adata)
			{
				cJSON* ljson = cJSON_CreateObject();
				json_format<TKEY>::push(ljson, "key", item.first);
				json_format<TVALUE>::push(ljson, "value", item.second);
				cJSON_AddItemToArray(ajson, ljson);
			}
		}

		static void push(cJSON* ajson, const char* akey, const std::map<TKEY, TVALUE>& adata)
		{
			cJSON* ljson = cJSON_CreateArray();
			push(ljson, adata);
			cJSON_AddItemToObject(ajson, akey, ljson);
		}
	};

	class ncjson;

	struct njson
	{
		static bool pop(cJSON* ajson)
		{
			return true;
		}

		template <typename T>
		static bool pop(cJSON* ajson, const char* akey, T& adata)
		{
			return json_format<T>::pop(ajson, akey, adata);
		}

		template <typename T, typename ...TARGS>
		static bool pop(cJSON* ajson, const char* akey, T& adata, TARGS&... aargs)
		{
			if (!pop(ajson, akey, adata))
			{
				return false;
			}
			return pop(ajson, aargs...);
		}

		static void push(cJSON* ajson)
		{
		}

		template <typename T>
		static void push(cJSON* ajson, const char* akey, const T& adata)
		{
			json_format<T>::push(ajson, akey, adata);
		}

		static void push(cJSON* ajson, const char* akey, const ncjson& adata);

		template <typename T, typename ...TARGS>
		static void push(cJSON* ajson, const char* akey, const T& adata, const TARGS&... aargs)
		{
			push(ajson, akey, adata);
			push(ajson, aargs...);
		}
	};



	////////class jthirdparty
	////////{
	////////public:
	////////	cJSON* get(cJSON* ajson, const char* akey)
	////////	{
	////////		return cJSON_GetObjectItem(ajson, akey);
	////////	}
	////////	
	////////	template <typename T>
	////////	bool get(cJSON* ajson, const char* akey, T& adata)
	////////	{
	////////		cJSON* ljson = get(ajson, akey);
	////////		if (ljson == nullptr)
	////////		{
	////////			return false;
	////////		}
	////////		if (tools::bit_or(ljson->type, cJSON_Number)
	////////		{
	////////			adata = (T)ajson->valueint;
	////////			return true;
	////////		}
	////////		if (tools::bit_or(ljson->type, cJSON_String))
	////////		{
	////////			tools::lex
	////////		}
	////////		if (tools::bit_or(ljson->type, cJSON_Number, cJSON_String, cJSON_True, cJSON_False))
	////////		{
	////////			return read_basic_type(ret, adata);
	////////		}
	////////	}
	////////};
	////////class ncjson
	////////{
	////////	cJSON* m_json;
	////////	bool  m_free;
	////////public:
	////////	ncjson(const char* astr) :
	////////		m_json(cJSON_Parse(astr))
	////////		, m_free(true)
	////////	{}
	////////	ncjson() :
	////////		m_json(nullptr)
	////////		, m_free(false)
	////////	{}
	////////	bool check()
	////////	{
	////////		return m_json != nullptr;
	////////	}
	////////	bool set(cJSON* ajson)
	////////	{
	////////		if (m_json != nullptr && m_free != false)
	////////		{
	////////			return false;
	////////		}
	////////		m_json = ajson;
	////////		return true;
	////////	}
	////////	cJSON*& json()
	////////	{
	////////		return m_json;
	////////	}
	////////	~ncjson()
	////////	{
	////////		if (m_json != nullptr && m_free)
	////////		{
	////////			cJSON_Delete(m_json);
	////////		}
	////////	}
	////////};
	////////class njwrite
	////////{
	////////	cJSON* m_json;
	////////	const char* m_nonformatstr;
	////////	const char* m_str;
	////////	bool m_isnonformatstr;
	////////	bool m_free;
	////////public:
	////////	njwrite() :
	////////		m_json(cJSON_CreateObject())
	////////		, m_nonformatstr(nullptr)
	////////		, m_str(nullptr)
	////////		, m_isnonformatstr(false)
	////////		, m_free(true)
	////////	{}
	////////	~njwrite()
	////////	{
	////////		free_nonformatstr();
	////////		free_str();
	////////		if (m_json != nullptr && m_free)
	////////		{
	////////			cJSON_Delete(m_json);
	////////			m_json = nullptr;
	////////		}
	////////	}
	////////	cJSON*& json()
	////////	{
	////////		return m_json;
	////////	}
	////////	void free_str()
	////////	{
	////////		if (m_str != nullptr)
	////////		{
	////////			free((void*)m_str);
	////////			m_str = nullptr;
	////////		}
	////////	}
	////////	void free_nonformatstr()
	////////	{
	////////		if (m_nonformatstr != nullptr)
	////////		{
	////////			free((void*)m_nonformatstr);
	////////			m_nonformatstr = nullptr;
	////////		}
	////////	}
	////////	cJSON*& nofree()
	////////	{
	////////		m_free = false;
	////////		return m_json;
	////////	}
	////////	void set_nonformatstr(bool abool)
	////////	{
	////////		m_isnonformatstr = abool;
	////////	}
	////////	const char* get()
	////////	{
	////////		if (m_isnonformatstr)
	////////		{
	////////			free_nonformatstr();
	////////			m_nonformatstr = cJSON_PrintUnformatted(m_json);
	////////			return m_nonformatstr;
	////////		}
	////////		else
	////////		{
	////////			free_str();
	////////			m_str = cJSON_Print(m_json);
	////////			return m_str;
	////////		}
	////////	}
	////////};
	////////template <typename T>
	////////struct json_format
	////////{
	////////	static bool read(ncjson& ajson, const char* akey, T& adata);
	////////	static bool read_array(ncjson& ajson, const char* akey, const std::function<void(T&)>& afun);
	////////	static void write(njwrite& ajson, const char* akey, T& adata);
	////////};
	////////template <>
	////////struct json_format<cJSON*>
	////////{
	////////	static bool read(ncjson& ajson, const char* akey, cJSON*& adata)
	////////	{
	////////		cJSON* ret = cJSON_GetObjectItem(ajson.json(), akey);
	////////		if (nullptr == ret)
	////////		{
	////////			return false;
	////////		}
	////////		if (tools::bit_or(ret->type, cJSON_Object, cJSON_Array))
	////////		{
	////////			adata = ret;
	////////			return true;
	////////		}
	////////		return false;
	////////	}
	////////	static void write(njwrite& ajson, const char* akey, cJSON* adata)
	////////	{
	////////		cJSON_AddItemToObject(ajson.json(), akey, adata);
	////////	}
	////////};
	////////struct tools_json
	////////{
	////////	template <typename TNUMBER>
	////////	static bool string_number(cJSON* ret, TNUMBER& adata)
	////////	{
	////////		if (tools::bit_or(ret->type, cJSON_String))
	////////		{
	////////			try
	////////			{
	////////				adata = tools::lexical_cast<TNUMBER>(ret->valuestring);
	////////				return true;
	////////			}
	////////			catch (...)
	////////			{
	////////				return false;
	////////			}
	////////		}
	////////		return true;
	////////	}
	////////	template <typename TNUMBER>
	////////	static bool read_basic_type(cJSON* ret, TNUMBER& adata)
	////////	{
	////////		adata = (TNUMBER)ret->valueint;
	////////		return string_number(ret, adata);
	////////	}
	////////	static bool read_basic_type(cJSON* ret, float& adata)
	////////	{
	////////		adata = (float)ret->valuedouble;
	////////		return string_number(ret, adata);
	////////	}
	////////	static bool read_basic_type(cJSON* ret, double& adata)
	////////	{
	////////		adata = (double)ret->valuedouble;
	////////		return string_number(ret, adata);
	////////	}
	////////	static bool read_basic_type(cJSON* ret, bool& adata)
	////////	{
	////////		if (tools::bit_or(ret->type, cJSON_True))
	////////		{
	////////			adata = true;
	////////			return true;
	////////		}
	////////		if (tools::bit_or(ret->type, cJSON_False))
	////////		{
	////////			adata = false;
	////////			return true;
	////////		}
	////////		if (tools::bit_or(ret->type, cJSON_Number))
	////////		{
	////////			adata = ret->valueint != 0;
	////////			return true;
	////////		}
	////////		if (tools::bit_or(ret->type, cJSON_String))
	////////		{
	////////			std::string lvaluestr(ret->valuestring);
	////////			if (lvaluestr == "false" || lvaluestr == "FALSE" || lvaluestr == "0")
	////////			{
	////////				adata = false;
	////////				return true;
	////////			}
	////////			if (lvaluestr == "true" || lvaluestr == "True" || lvaluestr == "1")
	////////			{
	////////				adata = true;
	////////				return true;
	////////			}
	////////		}
	////////		return false;
	////////	}
	////////	static bool read_basic_type(cJSON* ret, const char*& adata)
	////////	{
	////////		if (nullptr == ret)
	////////		{
	////////			return false;
	////////		}
	////////		if (tools::bit_and(ret->type, cJSON_String))
	////////		{
	////////			return false;
	////////		}
	////////		adata = ret->valuestring;
	////////		return true;
	////////	}
	////////	static bool read_basic_type(cJSON* ret, std::string& adata)
	////////	{
	////////		if (nullptr == ret)
	////////		{
	////////			return false;
	////////		}
	////////		if (tools::bit_and(ret->type, cJSON_String))
	////////		{
	////////			return false;
	////////		}
	////////		adata = ret->valuestring;
	////////		return true;
	////////	}
	////////	template <typename TNUMBER>
	////////	static bool read_basic_type(ncjson& ajson, const char* akey, TNUMBER& adata)
	////////	{
	////////		cJSON* ret = cJSON_GetObjectItem(ajson.json(), akey);
	////////		if (nullptr == ret)
	////////		{
	////////			return false;
	////////		}
	////////		if (tools::bit_or(ret->type, cJSON_Number, cJSON_String, cJSON_True, cJSON_False))
	////////		{
	////////			return read_basic_type(ret, adata);
	////////		}
	////////		return false;
	////////	}
	////////	template <typename TNUMBER>
	////////	static bool read_basic_array(ncjson& ajson, const char* akey, const std::function<void(TNUMBER&)>& afun)
	////////	{
	////////		cJSON* ltemp = nullptr;
	////////		if (!json_format<cJSON*>::read(ajson, akey, ltemp))
	////////		{
	////////			return false;
	////////		}
	////////		int lsize = cJSON_GetArraySize(ltemp);
	////////		for (int i = 0; i < lsize; ++i)
	////////		{
	////////			cJSON* ret = cJSON_GetArrayItem(ltemp, i);
	////////			TNUMBER ltempnumber = TNUMBER();
	////////			read_basic_type(ret, ltempnumber);
	////////			afun(ltempnumber);
	////////		}
	////////		return true;
	////////	}
	////////	// 不可显示调用write_array<...>(...) 应该write_array(...)
	////////	template <typename T>
	////////	static void write_array(njwrite& ajson, const char* akey, const std::function<T*()>& afun)
	////////	{
	////////		if constexpr (std::is_enum<T>::value)
	////////		{
	////////			std::vector<int32_t> lvec;
	////////			for (auto ltempitor = afun(); ltempitor != nullptr; ltempitor = afun())
	////////			{
	////////				lvec.push_back(*ltempitor);
	////////			}
	////////			write_array_item(ajson, akey, lvec);
	////////		}
	////////		else
	////////		{
	////////			cJSON* larray = cJSON_CreateArray();
	////////			for (T* ltempitor = afun(); ltempitor != nullptr; ltempitor = afun())
	////////			{
	////////				njwrite ltemp;
	////////				ltempitor->json_write(ltemp);
	////////				cJSON_AddItemToArray(larray, ltemp.nofree());
	////////			}
	////////			json_format<cJSON*>::write(ajson, akey, larray);
	////////		}
	////////	}
	////////	static void write_array_item(njwrite& ajson, const char* akey, std::vector<int32_t>& avec)
	////////	{
	////////		cJSON* larray = cJSON_CreateIntArray(avec.data(), (int32_t)avec.size());
	////////		json_format<cJSON*>::write(ajson, akey, larray);
	////////	}
	////////	static void write_array_item(njwrite& ajson, const char* akey, std::vector<const char*>& avec)
	////////	{
	////////		cJSON* larray = cJSON_CreateStringArray(avec.data(), (int32_t)avec.size());
	////////		json_format<cJSON*>::write(ajson, akey, larray);
	////////	}
	////////	static void write_array(njwrite& ajson, const char* akey, const std::function<int32_t* ()>& afun)
	////////	{
	////////		std::vector<int32_t> lvec;
	////////		for (auto ltempitor = afun(); ltempitor != nullptr; ltempitor = afun())
	////////		{
	////////			lvec.push_back(*ltempitor);
	////////		}
	////////		write_array_item(ajson, akey, lvec);
	////////	}
	////////	static void write_array(njwrite& ajson, const char* akey, const std::function<std::string* ()>& afun)
	////////	{
	////////		std::vector<const char*> lvec;
	////////		for (auto ltempitor = afun(); ltempitor != nullptr; ltempitor = afun())
	////////		{
	////////			lvec.push_back(ltempitor->c_str());
	////////		}
	////////		write_array_item(ajson, akey, lvec);
	////////	}
	////////	static void write_array(njwrite& ajson, const char* akey, const std::function<int8_t* ()>& afun)
	////////	{
	////////		std::vector<int32_t> lvec;
	////////		for (auto ltempitor = afun(); ltempitor != nullptr; ltempitor = afun())
	////////		{
	////////			lvec.push_back(*ltempitor);
	////////		}
	////////		write_array_item(ajson, akey, lvec);
	////////	}
	////////	static void write_array(njwrite& ajson, const char* akey, const std::function<int16_t* ()>& afun)
	////////	{
	////////		std::vector<int32_t> lvec;
	////////		for (auto ltempitor = afun(); ltempitor != nullptr; ltempitor = afun())
	////////		{
	////////			lvec.push_back(*ltempitor);
	////////		}
	////////		write_array_item(ajson, akey, lvec);
	////////	}
	////////	static void write_array(njwrite& ajson, const char* akey, const std::function<int64_t* ()>& afun)
	////////	{
	////////		std::vector<std::string> lvec1;
	////////		std::vector<const char*> lvec2;
	////////		for (auto ltempitor = afun(); ltempitor != nullptr; ltempitor = afun())
	////////		{
	////////			lvec1.push_back(tools::lexical_cast<std::string>(*ltempitor));
	////////			lvec2.push_back(lvec1.rbegin()->c_str());
	////////		}
	////////		write_array_item(ajson, akey, lvec2);
	////////	}
	////////	static void write_array(njwrite& ajson, const char* akey, const std::function<uint8_t* ()>& afun)
	////////	{
	////////		std::vector<int32_t> lvec;
	////////		for (auto ltempitor = afun(); ltempitor != nullptr; ltempitor = afun())
	////////		{
	////////			lvec.push_back(*ltempitor);
	////////		}
	////////		write_array_item(ajson, akey, lvec);
	////////	}
	////////	static void write_array(njwrite& ajson, const char* akey, const std::function<uint16_t* ()>& afun)
	////////	{
	////////		std::vector<int32_t> lvec;
	////////		for (auto ltempitor = afun(); ltempitor != nullptr; ltempitor = afun())
	////////		{
	////////			lvec.push_back(*ltempitor);
	////////		}
	////////		write_array_item(ajson, akey, lvec);
	////////	}
	////////	static void write_array(njwrite& ajson, const char* akey, const std::function<uint32_t* ()>& afun)
	////////	{
	////////		std::vector<int32_t> lvec;
	////////		for (auto ltempitor = afun(); ltempitor != nullptr; ltempitor = afun())
	////////		{
	////////			lvec.push_back(*ltempitor);
	////////		}
	////////		write_array_item(ajson, akey, lvec);
	////////	}
	////////	static void write_array(njwrite& ajson, const char* akey, const std::function<uint64_t* ()>& afun)
	////////	{
	////////		std::vector<std::string> lvec1;
	////////		std::vector<const char*> lvec2;
	////////		for (auto ltempitor = afun(); ltempitor != nullptr; ltempitor = afun())
	////////		{
	////////			lvec1.push_back(tools::lexical_cast<std::string>(*ltempitor));
	////////			lvec2.push_back(lvec1.rbegin()->c_str());
	////////		}
	////////		write_array_item(ajson, akey, lvec2);
	////////	}
	////////};
	////////template <>
	////////struct json_format<char*>
	////////{
	////////	static bool read(ncjson& ajson, const char* akey, const char*& adata)
	////////	{
	////////		return tools_json::read_basic_type(ajson, akey, adata);
	////////	}
	////////	static bool read_array(ncjson& ajson, const char* akey, const std::function<void(const char*&)>& afun)
	////////	{
	////////		return tools_json::read_basic_array(ajson, akey, afun);
	////////	}
	////////	static void write(njwrite& ajson, const char* akey, const char* adata)
	////////	{
	////////		cJSON_AddItemToObject(ajson.json(), akey, cJSON_CreateString(adata));
	////////	}
	////////};
	////////template <>
	////////struct json_format<std::string>
	////////{
	////////	static bool read(ncjson& ajson, const char* akey, std::string& adata)
	////////	{
	////////		return tools_json::read_basic_type(ajson, akey, adata);
	////////	}
	////////	static bool read_array(ncjson& ajson, const char* akey, const std::function<void(std::string&)>& afun)
	////////	{
	////////		return tools_json::read_basic_array(ajson, akey, afun);
	////////	}
	////////	static void write(njwrite& ajson, const char* akey, std::string& adata)
	////////	{
	////////		json_format<char*>::write(ajson, akey, adata.c_str());
	////////	}
	////////};
	////////template <>
	////////struct json_format<int8_t>
	////////{
	////////	static bool read(ncjson& ajson, const char* akey, int8_t& adata)
	////////	{
	////////		return tools_json::read_basic_type(ajson, akey, adata);
	////////	}
	////////	static bool read_array(ncjson& ajson, const char* akey, const std::function<void(int8_t&)>& afun)
	////////	{
	////////		return tools_json::read_basic_array(ajson, akey, afun);
	////////	}
	////////	static void write(njwrite& ajson, const char* akey, const int8_t adata)
	////////	{
	////////		cJSON_AddNumberToObject(ajson.json(), akey, adata);
	////////	}
	////////};
	////////template <>
	////////struct json_format<int16_t>
	////////{
	////////	static bool read(ncjson& ajson, const char* akey, int16_t& adata)
	////////	{
	////////		return tools_json::read_basic_type(ajson, akey, adata);
	////////	}
	////////	static bool read_array(ncjson& ajson, const char* akey, const std::function<void(int16_t&)>& afun)
	////////	{
	////////		return tools_json::read_basic_array(ajson, akey, afun);
	////////	}
	////////	static void write(njwrite& ajson, const char* akey, const int16_t adata)
	////////	{
	////////		cJSON_AddNumberToObject(ajson.json(), akey, adata);
	////////	}
	////////};
	////////template <>
	////////struct json_format<int32_t>
	////////{
	////////	static bool read(ncjson& ajson, const char* akey, int32_t& adata)
	////////	{
	////////		return tools_json::read_basic_type(ajson, akey, adata);
	////////	}
	////////	static bool read_array(ncjson& ajson, const char* akey, const std::function<void(int32_t&)>& afun)
	////////	{
	////////		return tools_json::read_basic_array(ajson, akey, afun);
	////////	}
	////////	static void write(njwrite& ajson, const char* akey, const int32_t adata)
	////////	{
	////////		cJSON_AddNumberToObject(ajson.json(), akey, adata);
	////////	}
	////////};
	////////template <>
	////////struct json_format<int64_t>
	////////{
	////////	static bool read(ncjson& ajson, const char* akey, int64_t& adata)
	////////	{
	////////		return tools_json::read_basic_type(ajson, akey, adata);
	////////	}
	////////	static bool read_array(ncjson& ajson, const char* akey, const std::function<void(int64_t&)>& afun)
	////////	{
	////////		return tools_json::read_basic_array(ajson, akey, afun);
	////////	}
	////////	static void write(njwrite& ajson, const char* akey, const int64_t adata)
	////////	{
	////////		std::string lvalue = tools::lexical_cast<std::string>(adata);
	////////		json_format<std::string>::write(ajson, akey, lvalue);
	////////	}
	////////};
	////////template <>
	////////struct json_format<uint8_t>
	////////{
	////////	static bool read(ncjson& ajson, const char* akey, uint8_t& adata)
	////////	{
	////////		return tools_json::read_basic_type(ajson, akey, adata);
	////////	}
	////////	static bool read_array(ncjson& ajson, const char* akey, const std::function<void(uint8_t&)>& afun)
	////////	{
	////////		return tools_json::read_basic_array(ajson, akey, afun);
	////////	}
	////////	static void write(njwrite& ajson, const char* akey, const uint8_t adata)
	////////	{
	////////		cJSON_AddNumberToObject(ajson.json(), akey, adata);
	////////	}
	////////};
	////////template <>
	////////struct json_format<uint16_t>
	////////{
	////////	static bool read(ncjson& ajson, const char* akey, uint16_t& adata)
	////////	{
	////////		return tools_json::read_basic_type(ajson, akey, adata);
	////////	}
	////////	static bool read_array(ncjson& ajson, const char* akey, const std::function<void(uint16_t&)>& afun)
	////////	{
	////////		return tools_json::read_basic_array(ajson, akey, afun);
	////////	}
	////////	static void write(njwrite& ajson, const char* akey, const uint16_t adata)
	////////	{
	////////		cJSON_AddNumberToObject(ajson.json(), akey, adata);
	////////	}
	////////};
	////////template <>
	////////struct json_format<uint32_t>
	////////{
	////////	static bool read(ncjson& ajson, const char* akey, uint32_t& adata)
	////////	{
	////////		return tools_json::read_basic_type(ajson, akey, adata);
	////////	}
	////////	static bool read_array(ncjson& ajson, const char* akey, const std::function<void(uint32_t&)>& afun)
	////////	{
	////////		return tools_json::read_basic_array(ajson, akey, afun);
	////////	}
	////////	static void write(njwrite& ajson, const char* akey, const uint32_t adata)
	////////	{
	////////		cJSON_AddNumberToObject(ajson.json(), akey, adata);
	////////	}
	////////};
	////////template <>
	////////struct json_format<uint64_t>
	////////{
	////////	static bool read(ncjson& ajson, const char* akey, uint64_t& adata)
	////////	{
	////////		return tools_json::read_basic_type(ajson, akey, adata);
	////////	}
	////////	static bool read_array(ncjson& ajson, const char* akey, const std::function<void(uint64_t&)>& afun)
	////////	{
	////////		return tools_json::read_basic_array(ajson, akey, afun);
	////////	}
	////////	static void write(njwrite& ajson, const char* akey, const uint64_t adata)
	////////	{
	////////		std::string lvalue = tools::lexical_cast<std::string>(adata);
	////////		json_format<std::string>::write(ajson, akey, lvalue);
	////////	}
	////////};
	////////template <>
	////////struct json_format<float>
	////////{
	////////	static bool read(ncjson& ajson, const char* akey, float& adata)
	////////	{
	////////		return tools_json::read_basic_type(ajson, akey, adata);
	////////	}
	////////	static bool read_array(ncjson& ajson, const char* akey, const std::function<void(float&)>& afun)
	////////	{
	////////		return tools_json::read_basic_array(ajson, akey, afun);
	////////	}
	////////	static void write(njwrite& ajson, const char* akey, const float adata)
	////////	{
	////////		cJSON_AddNumberToObject(ajson.json(), akey, adata);
	////////	}
	////////};
	////////template <>
	////////struct json_format<double>
	////////{
	////////	static bool read(ncjson& ajson, const char* akey, double& adata)
	////////	{
	////////		return tools_json::read_basic_type(ajson, akey, adata);
	////////	}
	////////	static bool read_array(ncjson& ajson, const char* akey, const std::function<void(double&)>& afun)
	////////	{
	////////		return tools_json::read_basic_array(ajson, akey, afun);
	////////	}
	////////	static void write(njwrite& ajson, const char* akey, const double adata)
	////////	{
	////////		cJSON_AddNumberToObject(ajson.json(), akey, adata);
	////////	}
	////////};
	////////template <>
	////////struct json_format<bool>
	////////{
	////////	static bool read(ncjson& ajson, const char* akey, bool& adata)
	////////	{
	////////		return tools_json::read_basic_type(ajson, akey, adata);
	////////	}
	////////	static bool read_array(ncjson& ajson, const char* akey, const std::function<void(bool&)>& afun)
	////////	{
	////////		return tools_json::read_basic_array(ajson, akey, afun);
	////////	}
	////////	static void write(njwrite& ajson, const char* akey, const bool adata)
	////////	{
	////////		cJSON_AddItemToObject(ajson.json(), akey, adata ? cJSON_CreateTrue() : cJSON_CreateFalse());
	////////	}
	////////};
	////////template <typename T>
	////////struct json_format<std::vector<T>>
	////////{
	////////	static bool read(ncjson& ajson, const char* akey, std::vector<T>& adata)
	////////	{
	////////		return json_format<T>::read_array(ajson, akey, [&adata](T& aval)
	////////			{
	////////				adata.push_back(aval);
	////////			});
	////////	}
	////////	static void write(njwrite& ajson, const char* akey, std::vector<T>& adata)
	////////	{
	////////		int32_t lindex = 0;
	////////		std::function<T* ()> lfun = [&lindex, &adata]()->T*
	////////			{
	////////				if (lindex >= adata.size())
	////////				{
	////////					return nullptr;
	////////				}
	////////				return &adata[lindex++];
	////////			};
	////////		tools_json::write_array(ajson, akey, lfun);
	////////	}
	////////};
	////////template <typename T>
	////////struct json_format<std::set<T>>
	////////{
	////////	static bool read(ncjson& ajson, const char* akey, std::set<T>& adata)
	////////	{
	////////		return json_format<T>::read_array(ajson, akey, [&adata](T& aval)
	////////			{
	////////				adata.insert(aval);
	////////			});
	////////	}
	////////	static bool read_array(ncjson& ajson, const char* akey, const std::function<void(std::vector<T>&)>& afun)
	////////	{
	////////		return false;
	////////	}
	////////	static void write(njwrite& ajson, const char* akey, std::set<T>& adata)
	////////	{
	////////		auto itor = adata.begin();
	////////		tools_json::write_array(ajson, akey, [&itor, &adata]()->T*
	////////			{
	////////				if (itor == adata.end())
	////////				{
	////////					return nullptr;
	////////				}
	////////				T* ltemp = (T*)&(*itor);
	////////				++itor;
	////////				return ltemp;
	////////			});
	////////	}
	////////};
	////////template <typename TKEY, typename TVAL>
	////////struct json_format<std::map<TKEY, TVAL>>
	////////{
	////////	static bool read(ncjson& ajson, const char* akey, std::map<TKEY, TVAL>& adata)
	////////	{
	////////		cJSON* lcjson = nullptr;
	////////		if (!json_format<cJSON*>::read(ajson, akey, lcjson))
	////////		{
	////////			return false;
	////////		}
	////////		ncjson ljson;
	////////		ljson.set(lcjson);
	////////		for (cJSON* child = lcjson->child; child != nullptr; child = child->next)
	////////		{
	////////			TKEY lkey = tools::lexical_cast<TKEY>(child->string);
	////////			if (!json_format<TVAL>::read(ljson, child->string, adata[lkey]))
	////////			{
	////////				return false;
	////////			}
	////////		}
	////////		return true;
	////////	}
	////////	static bool read_array(ncjson& ajson, const char* akey, const std::function<void(std::map<TKEY, TVAL>&)>& afun)
	////////	{
	////////		return false;
	////////	}
	////////	static void write(njwrite& ajson, const char* akey, std::map<TKEY, TVAL>& adata)
	////////	{
	////////		njwrite ltemp;
	////////		for (std::pair<const TKEY, TVAL>& item : adata)
	////////		{
	////////			json_format<TVAL>::write(ltemp, tools::lexical_cast<std::string>(item.first).c_str(), item.second);
	////////		}
	////////		json_format<cJSON*>::write(ajson, akey, ltemp.nofree());
	////////	}
	////////};
	////////template <typename T>
	////////bool json_format<T>::read(ncjson& ajson, const char* akey, T& adata)
	////////{
	////////	if constexpr (std::is_enum<T>::value)
	////////	{
	////////		int32_t lvalue = 0;
	////////		if (!json_format<int32_t>::read(ajson, akey, lvalue))
	////////		{
	////////			return false;
	////////		}
	////////		adata = (T)lvalue;
	////////		return true;
	////////	}
	////////	else
	////////	{
	////////		cJSON* ljson = nullptr;
	////////		if (!json_format<cJSON*>::read(ajson, akey, ljson))
	////////		{
	////////			return false;
	////////		}
	////////		ncjson ltemp;
	////////		if (!ltemp.set(ljson))
	////////		{
	////////			return false;
	////////		}
	////////		return adata.json_read(ltemp);
	////////	}		
	////////}
	////////template <typename T>
	////////bool json_format<T>::read_array(ncjson& ajson, const char* akey, const std::function<void(T&)>& afun)
	////////{
	////////	cJSON* ltemp = nullptr;
	////////	if (!json_format<cJSON*>::read(ajson, akey, ltemp))
	////////	{
	////////		return false;
	////////	}
	////////	int lsize = cJSON_GetArraySize(ltemp);
	////////	for (int i = 0; i < lsize; ++i)
	////////	{
	////////		cJSON* ret = cJSON_GetArrayItem(ltemp, i);
	////////		ncjson ltemp;
	////////		if (!ltemp.set(ret))
	////////		{
	////////			return false;
	////////		}
	////////		T ltempvalue;
	////////		if (ltempvalue.json_read(ltemp))
	////////		{
	////////			afun(ltempvalue);
	////////		}
	////////	}
	////////	return true;
	////////}
	////////template <typename T>
	////////void json_format<T>::write(njwrite& ajson, const char* akey, T& adata)
	////////{
	////////	if constexpr (std::is_enum<T>::value)
	////////	{
	////////		json_format<int32_t>::write(ajson, akey, (int32_t)adata);
	////////	}
	////////	else if constexpr (is_protobuf_message<T>::value)
	////////	{
	////////		std::string ljsonstr;
	////////		tools::proto2json(adata, ljsonstr);
	////////		json_format<std::string>::write(ajson, akey, ljsonstr);
	////////	}
	////////	else
	////////	{
	////////		adata.json_write(ajson, akey);
	////////	}
	////////}
	///////*struct njson
	//////{
	//////	static bool read(ncjson& ajson)
	//////	{
	//////		return true;
	//////	}
	//////	template <typename T>
	//////	static bool read(ncjson& ajson, const char* akey, T& adata)
	//////	{
	//////		return json_format<T>::read(ajson, akey, adata);
	//////	}
	//////	template <typename T, typename ...TARGS>
	//////	static bool read(ncjson& ajson, const char* akey, T& adata, TARGS&... aargs)
	//////	{
	//////		if (!read(ajson, akey, adata))
	//////		{
	//////			return false;
	//////		}
	//////		return read(ajson, aargs...);
	//////	}
	//////	static void write(njwrite& ajson)
	//////	{
	//////	}
	//////	template <typename T>
	//////	static void write(njwrite& ajson, const char* akey, T& adata)
	//////	{
	//////		json_format<T>::write(ajson, akey, adata);
	//////	}
	//////	static void write(njwrite& ajson, const char* akey, njwrite& adata)
	//////	{
	//////		json_format<cJSON*>::write(ajson, akey, adata.nofree());
	//////	}
	//////	template <typename T, typename ...TARGS>
	//////	static void write(njwrite& ajson, const char* akey, T& adata, TARGS&... aargs)
	//////	{
	//////		write(ajson, akey, adata);
	//////		write(ajson, aargs...);
	//////	}
	//////};*/
}//namespace ngl

namespace ngl
{
	class ncjson
	{
		cJSON* m_json = nullptr;
		const char* m_str = nullptr;
		const char* m_nonformatstr = nullptr;
		bool m_isfree = true;
		bool m_isnonformatstr = true;

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
	public:
		ncjson(const std::string& ajson) :
			m_json(cJSON_Parse(ajson.c_str()))
		{}

		ncjson() :
			m_json(cJSON_CreateObject())
		{}

		ncjson(cJSON* ajson) :
			m_json(ajson)
		{}

		bool check()
		{
			return m_json != nullptr;
		}

		cJSON* json()
		{
			return m_json;
		}

		cJSON* nofree()
		{
			m_isfree = false;
			return m_json;
		}

		void set_nonformatstr(bool anonformatstr)
		{
			m_isnonformatstr = anonformatstr;
		}

		const char* str()
		{
			if (m_isnonformatstr)
			{
				free_nonformatstr();
				m_nonformatstr = cJSON_PrintUnformatted(m_json);
			}
			else
			{
				free_str();
				m_str = cJSON_Print(m_json);
			}
			return m_str;
		}

		~ncjson()
		{
			if (m_json != nullptr && m_isfree)
			{
				cJSON_Delete(m_json);
			}
			free_nonformatstr();
			free_str();
		}
	};

	template <typename T>
	bool tools::json2custom(const std::string& ajson, T& adata)
	{
		ncjson ltemp(ajson);
		return json_format<T>::pop(ltemp.json(), tools::type_name<T>().c_str(), adata);
	}

	template <typename T>
	bool tools::custom2json(const T& adata, std::string& json)
	{
		ncjson ltemp;
		json_format<T>::push(ltemp.json(), tools::type_name<T>().c_str(), adata);
		json = ltemp.str();
		return true;
	}
}//namespace ngl

namespace ngl
{
	template <typename T>
	bool json_format<T>::pop(cJSON* ajson, const char* akey, T& adata)
	{
		if constexpr (std::is_enum<T>::value)
		{
			int64_t lvalue = 0;
			if (!json_format<int64_t>::pop(ajson, akey, lvalue))
			{
				return false;
			}
			adata = (T)lvalue;
		}
		else
		{
			cJSON* ljson = ajson;
			if (akey != nullptr)
			{
				ljson = cJSON_GetObjectItem(ajson, akey);
			}
			return adata.json_pop(ljson);
		}
	}

	template <typename T>
	void json_format<T>::push(cJSON* ajson, const char* akey, const T& adata)
	{
		if constexpr (std::is_enum<T>::value)
		{
			cJSON_AddNumberToObject(ajson, akey, (int32_t)adata);
		}
		else
		{
			if constexpr (is_protobuf_message<T>::value)
			{
				std::string ljson;
				if (akey != nullptr && tools::proto2json(adata, ljson))
				{
					ncjson lnctemp(ljson.c_str());
					njson::push(ajson, akey, lnctemp);
				}
			}
			else
			{
				adata.json_push(ajson, akey);
			}
		}
	}

	template <typename T>
	void json_format<T>::push(cJSON* ajson, const T& adata)
	{
		if constexpr (!std::is_enum<T>::value)
		{
			adata.json_push(ajson, nullptr);
		}
	}
}//namespace ngl
