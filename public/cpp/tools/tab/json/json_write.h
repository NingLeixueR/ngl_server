#pragma once

#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <set>

#include "cJSON.h"

namespace ngl
{
	class json_write
	{
	public:
		json_write();
		virtual ~json_write();
		virtual cJSON* get();

		void write(const char* akey, int8_t aval);
		void write(const char* akey, int16_t aval);
		void write(const char* akey, int32_t aval);
		void write(const char* akey, uint8_t aval);
		void write(const char* akey, uint16_t aval);
		void write(const char* akey, uint32_t aval);
		// Json标准是不支持int64这么大的数字的，
		// 它最大只支持到2^53-1，
		// 如果超过最大值可能会导致Json的解析出错误
		// 所以我们想要传输int64，就先转换成string
		void write(const char* akey, const int64_t aval);
		void write(const char* akey, const uint64_t aval);
		void write(const char* akey, const float aval);
		void write(const char* akey, const double aval);
		void write(const char* akey, const char* aval);
		void write(const char* akey, const bool aval);
		void write(const char* akey, cJSON* aval);
		void write(const char* akey, json_write& aval);
		void write(const char* akey, const std::string& aval);
	private:
		void writenumber(const char* akey, const std::vector<int32_t>& aval);
		void writenumber(const char* akey, const std::vector<uint32_t>& aval);
	public:
		void write(const char* akey, const std::vector<int8_t>& aval);
		void write(const char* akey, const std::vector<int16_t>& aval);
		void write(const char* akey, const std::vector<int32_t>& aval);
		void write(const char* akey, const std::vector<uint8_t>& aval);
		void write(const char* akey, const std::vector<uint16_t>& aval);
		void write(const char* akey, const std::vector<uint32_t>& aval);
		void write(const char* akey, const std::vector<float>& aval);
		void write(const char* akey, const std::vector<double>& aval);
		void write(const char* akey, const std::vector<int64_t>& aval);
		void write(const char* akey, const std::vector<uint64_t>& aval);
		void write(const char* akey, const std::vector<std::string>& aval);

		template <typename KEY, typename VAL>
		void write(const char* akey, const std::map<KEY, VAL>& aval)
		{
			std::vector<std::pair<KEY, VAL>> lvec;
			for (auto itor = aval.begin(); itor != aval.end(); ++itor)
			{
				lvec.push_back(std::make_pair(itor->first, itor->second));
			}
			write(akey, lvec);
		}

		template <typename T>
		void write(const char* akey, const std::list<T>& aval)
		{
			std::vector<T> ltemp(aval.begin(), aval.end());
			write(akey, ltemp);
		}

		template <typename T>
		void write(const char* akey, const T& aval)
		{
			aval.write(*this, akey);
		}

		template <typename T>
		void write(const char* akey, const std::vector<T>& aval)
		{
			cJSON* larray = cJSON_CreateArray();
			for (const T& item : aval)
			{
				ngl::json_write ltemp;
				item.write(ltemp);
				cJSON_AddItemToArray(larray, ltemp.nofree());
			}
			write(akey, larray);
		}

		template <typename TKEY, typename TVAL>
		void write(const char* akey, const std::vector<std::pair<TKEY, TVAL>>& aval)
		{
			cJSON* larray = cJSON_CreateArray();
			for (const std::pair<TKEY, TVAL>& apair : aval)
			{
				ngl::json_write ltemp;
				ltemp.write("k", apair.first);
				ltemp.write("v", apair.second);
				cJSON_AddItemToArray(larray, ltemp.nofree());
			}
			write(akey, larray);
		}


		// 支持没有参数
		void write();

		template <typename T, typename ...ARG>
		void write(const char* akey, const T& avalue, const ARG&... arg)
		{
			write(akey, avalue);
			write(arg...);
		}

		void get(std::string& adata);

		void set_nonformatstr(bool abool);

		void free_str();

		void free_nonformatstr();

		cJSON* nofree();
	private:
		cJSON*			m_json;
		const char*		m_nonformatstr;
		const char*		m_str;
		bool			m_isnonformatstr;
		bool			m_free;
	};
}// namespace ngl