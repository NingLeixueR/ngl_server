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
		ojson& _fun_number(std::pair<const char*, T>& adata)
		{
			cJSON* ret = cJSON_GetObjectItem(m_json, adata.first);
			if (nullptr == ret || ret->type != cJSON_Number)
				return *this;
			adata.second = ret->valueint;
			return *this;
		}

		ojson& operator >> (std::pair<const char*, int8_t>& adata);
		ojson& operator >> (std::pair<const char*, int16_t>& adata);
		ojson& operator >> (std::pair<const char*, int32_t>& adata);
		ojson& operator >> (std::pair<const char*, int64_t>& adata);
		ojson& operator >> (std::pair<const char*, uint8_t>& adata);
		ojson& operator >> (std::pair<const char*, uint16_t>& adata);
		ojson& operator >> (std::pair<const char*, uint32_t>& adata);
		ojson& operator >> (std::pair<const char*, uint64_t>& adata);
		ojson& operator >> (std::pair<const char*, float>& adata);
		ojson& operator >> (std::pair<const char*, double>& adata);
		ojson& operator >> (std::pair<const char*, const char*>& adata);
		ojson& operator >> (std::pair<const char*, bool>& adata);
		ojson& operator >> (std::pair<const char*, json>& adata);
		ojson& operator >> (std::pair<const char*, cJSON*>& adata);

		//// --- 结构体
		template <typename T>
		ojson& operator >> (std::pair<const char*, T>& adata)
		{
			cJSON* ljson = nullptr;
			(*this) >> std::make_pair(adata.first, ljson);
			ojson ltemp;
			ltemp.set(ljson);
			ltemp >> adata.second;
			return (*this);
		}

		template <typename T>
		ojson& _fun_number(T& adata)
		{
			if (m_json->type == cJSON_Number)
			{
				adata = m_json->valueint;
			}
			return *this;
		}

		ojson& operator >> (int8_t& adata)
		{
			return _fun_number(adata);
		}
		ojson& operator >> (int16_t& adata)
		{
			return _fun_number(adata);
		}
		ojson& operator >> (int32_t& adata)
		{
			return _fun_number(adata);
		}
		ojson& operator >> (int64_t& adata)
		{
			return _fun_number(adata);
		}

		ojson& operator >> (std::string& adata)
		{
			if (m_json->type == cJSON_String)
			{
				adata = m_json->valuestring;
			}
			return *this;
		}

		ojson& operator >> (const char*& adata)
		{
			if (m_json->type == cJSON_String)
			{
				adata = m_json->valuestring;
			}
			return *this;
		}

		//// --- 数组类
		template <typename T>
		ojson& operator >> (std::pair<const char*, std::vector<T>>& adata)
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
			return *this;
		}

		template <typename T>
		ojson& operator >> (std::pair<const char*, std::list<T>>& adata)
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
			return *this;
		}

		template <typename T>
		ojson& operator >> (std::pair<const char*, std::set<T>>& adata)
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
			return *this;
		}

		template <typename TKEY, typename TVALUE>
		ojson& operator >> (std::pair<const char*, std::map<TKEY, TVALUE>>& adata)
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
			return *this;
		}
	};
}

