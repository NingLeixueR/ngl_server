#pragma once

#include "basejson.h"

namespace ngl
{
	class ijson : public json
	{
	public:
		ijson();
		ijson(bool _);
		virtual ~ijson();
		virtual cJSON* get();

		ijson& operator << (const std::pair<const char*, int8_t>& adata);
		ijson& operator << (const std::pair<const char*, int16_t>& adata);
		ijson& operator << (const std::pair<const char*, int32_t>& adata);
		ijson& operator << (const std::pair<const char*, int64_t>& adata);
		ijson& operator << (const std::pair<const char*, uint8_t>& adata);
		ijson& operator << (const std::pair<const char*, uint16_t>& adata);
		ijson& operator << (const std::pair<const char*, uint32_t>& adata);
		ijson& operator << (const std::pair<const char*, uint64_t>& adata);
		ijson& operator << (const std::pair<const char*, float>& adata);
		ijson& operator << (const std::pair<const char*, double>& adata);
		ijson& operator << (const std::pair<const char*, const char*>& adata);
		ijson& operator << (const std::pair<const char*, bool>& adata);
		ijson& operator << (const std::pair<const char*, json*>& adata);
		ijson& operator << (const std::pair<const char*, cJSON*>& adata);
		ijson& operator << (const std::pair<const char*, std::string>& adata);

		//// --- 结构体
		template <typename T>
		ijson& operator << (const std::pair<const char*, T>& adata)
		{
			ijson ltemp(false);
			ltemp << adata.second;
			return (*this) << std::make_pair(adata.first, ltemp.get());
		}

		template <typename T>
		void _fun_array(cJSON* tempArray, const T& adata)
		{
			ijson ltemp(false);
			ltemp << adata;////---基本类型不支持  只支持类外定义的结构体
			cJSON_AddItemToArray(tempArray, ltemp.get());
		}

		void _fun_array(cJSON* tempArray, const int8_t& adata)
		{
			cJSON_AddItemToArray(tempArray, cJSON_CreateNumber(adata));
		}
		void _fun_array(cJSON* tempArray, const int16_t& adata)
		{
			cJSON_AddItemToArray(tempArray, cJSON_CreateNumber(adata));
		}
		void _fun_array(cJSON* tempArray, const int32_t& adata)
		{
			cJSON_AddItemToArray(tempArray, cJSON_CreateNumber(adata));
		}
		void _fun_array(cJSON* tempArray, const int64_t& adata)
		{
			cJSON_AddItemToArray(tempArray, cJSON_CreateNumber(adata));
		}
		void _fun_array(cJSON* tempArray, const uint8_t& adata)
		{
			cJSON_AddItemToArray(tempArray, cJSON_CreateNumber(adata));
		}
		void _fun_array(cJSON* tempArray, const uint16_t& adata)
		{
			cJSON_AddItemToArray(tempArray, cJSON_CreateNumber(adata));
		}
		void _fun_array(cJSON* tempArray, const uint32_t& adata)
		{
			cJSON_AddItemToArray(tempArray, cJSON_CreateNumber(adata));
		}
		void _fun_array(cJSON* tempArray, const uint64_t& adata)
		{
			cJSON_AddItemToArray(tempArray, cJSON_CreateNumber(adata));
		}
		void _fun_array(cJSON* tempArray, const bool& adata)
		{
			cJSON_AddItemToArray(tempArray, adata? cJSON_CreateTrue(): cJSON_CreateFalse());
		}
		void _fun_array(cJSON* tempArray, const char*& adata)
		{
			cJSON_AddItemToArray(tempArray, cJSON_CreateString(adata));
		}

		void _fun_array(cJSON* tempArray, const std::string& adata)
		{
			cJSON_AddItemToArray(tempArray, cJSON_CreateString(adata.c_str()));
		}

		//// --- 数组类
		template <typename TITOR>
		ijson& operator << (std::pair<const char*, std::pair<TITOR, TITOR>>& adata)
		{
			cJSON* tempArray = cJSON_CreateArray();
			for (; adata.second.first != adata.second.second;++adata.second.first)
			{
				_fun_array(tempArray,*adata.second.first);
			}
			return (*this) << std::make_pair(adata.first, tempArray);
		}

		template <typename T>
		ijson& operator << (std::pair<const char*, std::vector<T>>& adata)
		{
			return (*this) << std::make_pair(adata.first, std::make_pair(adata.second.begin(), adata.second.end()));
		}

		template <typename T>
		ijson& operator << (std::pair<const char*, std::list<T>>& adata)
		{
			return (*this) << std::make_pair(adata.first, std::make_pair(adata.second.begin(), adata.second.end()));
		}

		template <typename T>
		ijson& operator << (std::pair<const char*, std::set<T>>& adata)
		{
			return (*this) << std::make_pair(adata.first, std::make_pair(adata.second.begin(), adata.second.end()));
		}

		template <typename TKEY,typename TVALUE>
		ijson& operator << (std::pair<const char*, std::map<TKEY, TVALUE>>& adata)
		{
			std::pair<const char*, std::list<TKEY>> lkeypair("key", std::list<TKEY>());
			std::pair<const char*, std::list<TVALUE>> lvalpair("value", std::list<TVALUE>());
			for (const std::pair<TKEY, TVALUE>& item : adata.second)
			{
				lkeypair.second.push_back(item.first);
				lvalpair.second.push_back(item.second);
			}
			ijson ltemp(false);
			ltemp << lkeypair << lvalpair;
			return (*this) << std::make_pair(adata.first, ltemp.get());
		}

		ijson& operator >> (std::string& adata);

		void set_nonformatstr(bool abool);

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
	private:
		const char* m_nonformatstr;
		const char* m_str;
		bool m_isnonformatstr;
		bool m_free;
	};


}

