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

	class ncjson;
	template <>
	struct json_format<ncjson>;

	template <>
	struct json_format<int64_t>
	{
		static bool pop(cJSON* ajson, const char* akey, int64_t& adata)
		{
			cJSON* ljson = ajson;
			if (ljson == nullptr)
			{
				return akey == nullptr;
			}
			if (akey != nullptr)
			{
				ljson = cJSON_GetObjectItem(ajson, akey);
				if (ljson == nullptr)
				{
					return true;
				}
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
			if (ljson == nullptr)
			{
				return akey == nullptr;
			}
			if (akey != nullptr)
			{
				ljson = cJSON_GetObjectItem(ajson, akey);
				if (ljson == nullptr)
				{
					return true;
				}
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
			if (ljson == nullptr)
			{
				return akey == nullptr;
			}
			if (akey != nullptr)
			{
				ljson = cJSON_GetObjectItem(ajson, akey);
				if (ljson == nullptr)
				{
					return true;
				}
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
			if (ljson == nullptr)
			{
				return akey == nullptr;
			}
			if (akey != nullptr)
			{
				ljson = cJSON_GetObjectItem(ajson, akey);
				if (ljson == nullptr)
				{
					return true;
				}
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
			if (ljson == nullptr)
			{
				return akey == nullptr;
			}
			if (akey != nullptr)
			{
				ljson = cJSON_GetObjectItem(ajson, akey);
				if (ljson == nullptr)
				{
					return true;
				}
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
			if (ljson == nullptr)
			{
				return akey == nullptr;
			}
			if (akey != nullptr)
			{
				ljson = cJSON_GetObjectItem(ajson, akey);
				if (ljson == nullptr)
				{
					return true;
				}
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
			return true;
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
			if (ljson == nullptr)
			{
				return akey == nullptr;
			}
			if (akey != nullptr)
			{
				ljson = cJSON_GetObjectItem(ajson, akey);
				if (ljson == nullptr)
				{
					return true;
				}
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
			return true;
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
			if (ljson == nullptr)
			{
				return akey == nullptr;
			}
			if (akey != nullptr)
			{
				ljson = cJSON_GetObjectItem(ajson, akey);
				if (ljson == nullptr)
				{
					return true;
				}
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
			return true;
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
			if (ljson == nullptr)
			{
				return akey == nullptr;
			}
			if (akey != nullptr)
			{
				ljson = cJSON_GetObjectItem(ajson, akey);
				if (ljson == nullptr)
				{
					return true;
				}
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
		template <typename T>
		static bool pop(cJSON* ajson, const char* akey, T& adata)
		{
			return json_format<T>::pop(ajson, akey, adata);
		}

		template <typename T, typename ...TARGS>
		static bool pop(cJSON* ajson, const char* akey, T& adata, TARGS&... aargs)
		{
			return pop(ajson, akey, adata) && pop(ajson, aargs...);
		}

		template <typename T>
		static void push(cJSON* ajson, const char* akey, const T& adata)
		{
			json_format<T>::push(ajson, akey, adata);
		}

		template <typename T, typename ...TARGS>
		static void push(cJSON* ajson, const char* akey, const T& adata, const TARGS&... aargs)
		{
			return push(ajson, akey, adata), push(ajson, aargs...);			
		}
	};
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

		bool is_invalid(const char* avalue)
		{
			if (avalue == nullptr)
			{
				return false;
			}
			if (std::string(avalue) == "")
			{
				return false;
			}
			return true;
		}
	public:
		ncjson(const char* ajson)
		{
			if (is_invalid(ajson))
			{
				m_json = cJSON_Parse(ajson);
			}
			else
			{
				m_json = cJSON_CreateObject();
			}			
		}

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
				return m_nonformatstr;
			}
			else
			{
				free_str();
				m_str = cJSON_Print(m_json);
				return m_str;
			}
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
			if constexpr (is_protobuf_message<T>::value)
			{
				cJSON* ljson = ajson;
				if (ljson == nullptr)
				{
					return akey == nullptr;
				}
				if (akey != nullptr)
				{
					ljson = cJSON_GetObjectItem(ljson, akey);
					if (ljson == nullptr)
					{
						return true;
					}
				}
				ncjson lncjson(ljson);
				lncjson.nofree();
				if (!tools::json2proto(lncjson.str(), adata))
				{
					return false;
				}
			}
			else
			{
				cJSON* ljson = ajson;
				if (ljson == nullptr)
				{
					return akey == nullptr;
				}
				if (akey != nullptr)
				{
					ljson = cJSON_GetObjectItem(ajson, akey);
					if (ljson == nullptr)
					{
						return true;
					}
				}
				return adata.json_pop(ljson);
			}
		}
		return true;
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
		push(ajson, nullptr, adata);
	}
}//namespace ngl

namespace ngl
{
	template <>
	struct json_format<ncjson>
	{
		static bool pop(cJSON* ajson, const char* akey, ncjson& adata)
		{
			return false;
		}

		static cJSON* create(const ncjson& adata)
		{
			return nullptr;
		}
		static void push(cJSON* ajson, const char* akey, const ncjson& adata)
		{
			cJSON_AddItemToObject(ajson, akey, ((ncjson*)(&adata))->nofree());
		}
		static void push(cJSON* ajson, const ncjson& adata)
		{
		}
	};
}
