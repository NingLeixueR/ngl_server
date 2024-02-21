#pragma once

#include "basejson.h"

namespace ngl
{
	class ojson : public json
	{
		bool m_free;
	public:
		ojson(const char* astr);

		ojson();

		virtual ~ojson();

		template <typename T>
		bool _fun_number32(std::pair<const char*, T>& adata)
		{
			cJSON* ret = cJSON_GetObjectItem(m_json, adata.first);
			if (nullptr == ret || ret->type != cJSON_Number)
				return false;
			adata.second = (T)ret->valueint;
			return true;
		}

		template <typename T>
		bool _fun_number64(std::pair<const char*, T>& adata)
		{
			cJSON* ret = cJSON_GetObjectItem(m_json, adata.first);
			if (nullptr == ret || ret->type != cJSON_Number)
				return false;
			//memcpy(&adata.second, &ret->valuedouble, sizeof(T));
			adata.second = (T)ret->valuedouble;
			return true;
		}

		bool operator >> (std::pair<const char*, int8_t>& adata);
		bool operator >> (std::pair<const char*, int16_t>& adata);
		bool operator >> (std::pair<const char*, int32_t>& adata);
		bool operator >> (std::pair<const char*, int64_t>& adata);
		bool operator >> (std::pair<const char*, uint8_t>& adata);
		bool operator >> (std::pair<const char*, uint16_t>& adata);
		bool operator >> (std::pair<const char*, uint32_t>& adata);
		bool operator >> (std::pair<const char*, uint64_t>& adata);
		bool operator >> (std::pair<const char*, float>& adata);
		bool operator >> (std::pair<const char*, double>& adata);
		bool operator >> (std::pair<const char*, const char*>& adata);
		bool operator >> (std::pair<const char*, bool>& adata);
		bool operator >> (std::pair<const char*, json>& adata);
		bool operator >> (std::pair<const char*, cJSON*>& adata);

		//// --- 结构体
		template <typename T>
		bool operator >> (std::pair<const char*, T>& adata)
		{
			cJSON* ljson = nullptr;
			if ((*this) >> std::make_pair(adata.first, ljson) == false)
				return false;
			ojson ltemp;
			ltemp.set(ljson);
			ltemp >> adata.second;
			return true;
		}

		template <typename T>
		bool _fun_number(T& adata)
		{
			if (m_json->type == cJSON_Number)
			{
				adata = m_json->valueint;
				return true;
			}
			return false;
		}

		bool operator >> (int8_t& adata)
		{
			return _fun_number(adata);
		}
		bool operator >> (int16_t& adata)
		{
			return _fun_number(adata);
		}
		bool operator >> (int32_t& adata)
		{
			return _fun_number(adata);
		}
		bool operator >> (int64_t& adata)
		{
			return _fun_number(adata);
		}

		bool operator >> (std::string& adata)
		{
			if (m_json->type == cJSON_String)
			{
				adata = m_json->valuestring;
				return true;
			}
			return false;
		}

		bool operator >> (const char*& adata)
		{
			if (m_json->type == cJSON_String)
			{
				adata = m_json->valuestring;
				return true;
			}
			return false;
		}

		//// --- 数组类
		template <typename T>
		bool operator >> (std::pair<const char*, std::vector<T>>& adata)
		{
			std::pair<const char*, cJSON*> lpair(adata.first, nullptr);
			(*this) >> lpair;
			int lsize = cJSON_GetArraySize(lpair.second);
			ojson ltemp;
			for (int i = 0; i < lsize; i++)
			{
				ltemp.set(cJSON_GetArrayItem(lpair.second, i));
				T lT;
				ltemp >> lT;
				adata.second.push_back(lT);
			}
			return true;
		}

		template <typename T>
		bool operator >> (std::pair<const char*, std::list<T>>& adata)
		{
			std::pair<const char*, cJSON*> lpair(adata.first, nullptr);
			(*this) >> lpair;
			int lsize = cJSON_GetArraySize(lpair.second);
			ojson ltemp;
			for (int i = 0; i < lsize; i++)
			{
				ltemp.set(cJSON_GetArrayItem(lpair.second, i));
				T lT;
				ltemp >> lT;
				adata.second.push_back(lT);
			}
			return true;
		}

		template <typename T>
		bool operator >> (std::pair<const char*, std::set<T>>& adata)
		{
			std::pair<const char*, cJSON*> lpair(adata.first, nullptr);
			(*this) >> lpair;
			int lsize = cJSON_GetArraySize(lpair.second);
			ojson ltemp;
			for (int i = 0; i < lsize; i++)
			{
				ltemp.set(cJSON_GetArrayItem(lpair.second, i));
				T lT;
				ltemp >> lT;
				adata.second.insert(lT);
			}
			return true;
		}

		template <typename TKEY, typename TVALUE>
		bool operator >> (std::pair<const char*, std::map<TKEY, TVALUE>>& adata)
		{
			std::pair<const char*, std::list<TKEY>> lkeypair("key", std::list<TKEY>());
			std::pair<const char*, std::list<TVALUE>> lvalpair("value", std::list<TVALUE>());

			std::pair<const char*, cJSON*> lpair(adata.first, nullptr);
			(*this) >> lpair;
			ojson ljson;
			ljson.set(lpair.second);
			ljson >> lkeypair >> lvalpair;

			typename std::list<TKEY>::iterator itorkey = lkeypair.second.begin();
			typename std::list<TVALUE>::iterator itorval = lvalpair.second.begin();
			for (; itorkey != lkeypair.second.end() && itorval != lvalpair.second.end(); ++itorkey, ++itorval)
			{
				adata.second.insert(std::make_pair(*itorkey,*itorval));
			}
			return true;
		}
	};
}

