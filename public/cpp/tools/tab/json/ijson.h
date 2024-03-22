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
	class ijson
	{
	public:
		ijson();
		virtual ~ijson();
		virtual cJSON* get();

		void add(const char* akey, int8_t aval);
		void add(const char* akey, int16_t aval);
		void add(const char* akey, int32_t aval);
		void add(const char* akey, uint8_t aval);
		void add(const char* akey, uint16_t aval);
		void add(const char* akey, uint32_t aval);
		// Json标准是不支持int64这么大的数字的，
		// 它最大只支持到2^53-1，
		// 如果超过最大值可能会导致Json的解析出错误
		// 所以我们想要传输int64，就先转换成string
		void add(const char* akey, const int64_t aval);
		void add(const char* akey, const uint64_t aval);
		void add(const char* akey, const float aval);
		void add(const char* akey, const double aval);
		void add(const char* akey, const char* aval);
		void add(const char* akey, const bool aval);
		void add(const char* akey, cJSON* aval);
		void add(const char* akey, ijson& aval);
		void add(const char* akey, const std::string& aval);
	private:
		void addnumber(const char* akey, const std::vector<int32_t>& aval);
		void addnumber(const char* akey, const std::vector<uint32_t>& aval);
	public:
		void add(const char* akey, const std::vector<int8_t>& aval);
		void add(const char* akey, const std::vector<int16_t>& aval);
		void add(const char* akey, const std::vector<int32_t>& aval);
		void add(const char* akey, const std::vector<uint8_t>& aval);
		void add(const char* akey, const std::vector<uint16_t>& aval);
		void add(const char* akey, const std::vector<uint32_t>& aval);
		void add(const char* akey, const std::vector<float>& aval);
		void add(const char* akey, const std::vector<double>& aval);
		void add(const char* akey, const std::vector<int64_t>& aval);
		void add(const char* akey, const std::vector<uint64_t>& aval);

		template <typename KEY, typename VAL>
		void add(const char* akey, const std::map<KEY, VAL>& aval)
		{
			std::vector<KEY> lkey;
			std::vector<VAL> lval;
			for (auto itor = aval.begin(); itor != aval.end(); ++itor)
			{
				lkey.push_back(itor->first);
				lval.push_back(itor->second);
			}
			std::string lkeystr = akey;
			lkeystr += "_key";
			add(lkeystr.c_str(), lkey);
			std::string lvalstr = akey;
			lvalstr += "_val";
			add(lvalstr.c_str(), lval);
		}

		template <typename T>
		void add(const char* akey, const std::list<T>& aval)
		{
			std::vector<T> ltemp(aval.begin(), aval.end());
			add(akey, ltemp);
		}

		template <typename T>
		void add(const char* akey, const T& aval)
		{
			aval.add(akey, aval);
		}

		template <typename T>
		void add(const char* akey, const std::vector<T>& aval)
		{
			cJSON* larray = cJSON_CreateArray();
			for (const T& item : aval)
			{
				ngl::ijson ltemp;
				item.add(ltemp);
				cJSON_AddItemToArray(larray, ltemp.nofree());
			}
			add(akey, larray);
		}

		// 支持没有参数
		void add();

		template <typename T, typename ...ARG>
		void add(const char* akey, const T& avalue, const ARG&... arg)
		{
			add(akey, avalue);
			add(arg...);
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