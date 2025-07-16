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
	class json_write;
	union nguid;

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

		void write(const char* akey, const std::list<int8_t>& aval);
		void write(const char* akey, const std::list<int16_t>& aval);
		void write(const char* akey, const std::list<int32_t>& aval);
		void write(const char* akey, const std::list<uint8_t>& aval);
		void write(const char* akey, const std::list<uint16_t>& aval);
		void write(const char* akey, const std::list<uint32_t>& aval);
		void write(const char* akey, const std::list<float>& aval);
		void write(const char* akey, const std::list<double>& aval);
		void write(const char* akey, const std::list<int64_t>& aval);
		void write(const char* akey, const std::list<uint64_t>& aval);
		void write(const char* akey, const std::list<std::string>& aval);

		void write(const char* akey, const std::set<int8_t>& aval);
		void write(const char* akey, const std::set<int16_t>& aval);
		void write(const char* akey, const std::set<int32_t>& aval);
		void write(const char* akey, const std::set<uint8_t>& aval);
		void write(const char* akey, const std::set<uint16_t>& aval);
		void write(const char* akey, const std::set<uint32_t>& aval);
		void write(const char* akey, const std::set<float>& aval);
		void write(const char* akey, const std::set<double>& aval);
		void write(const char* akey, const std::set<int64_t>& aval);
		void write(const char* akey, const std::set<uint64_t>& aval);
		void write(const char* akey, const std::set<std::string>& aval);

		template <typename VAL>
		void write(const char* akey, const std::map<std::string, VAL>& aval)
		{
			json_write ltemp;
			for (const auto& item : aval)
			{
				ltemp.write(item.first.c_str(), item.second);
			}
			write(akey, ltemp);
		}

		template <typename KEY, typename VAL>
		void write_mapnumber(const char* akey, const std::map<KEY, VAL>& aval);

		template <typename VAL>
		void write(const char* akey, const std::map<nguid, VAL>& aval);

		template <typename VAL>
		void write(const char* akey, const std::map<int16_t, VAL>& aval)
		{
			write_mapnumber(akey, aval);
		}

		template <typename VAL>
		void write(const char* akey, const std::map<uint16_t, VAL>& aval)
		{
			write_mapnumber(akey, aval);
		}

		template <typename VAL>
		void write(const char* akey, const std::map<int32_t, VAL>& aval)
		{
			write_mapnumber(akey, aval);
		}

		template <typename VAL>
		void write(const char* akey, const std::map<uint32_t, VAL>& aval)
		{
			write_mapnumber(akey, aval);
		}

		template <typename VAL>
		void write(const char* akey, const std::map<int64_t, VAL>& aval)
		{
			write_mapnumber(akey, aval);
		}

		template <typename VAL>
		void write(const char* akey, const std::map<uint64_t, VAL>& aval)
		{
			write_mapnumber(akey, aval);
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

		template <typename T>
		void write(const char* akey, const std::list<T>& aval)
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

		template <typename T>
		void write(const char* akey, const std::set<T>& aval)
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

		template <typename T>
		void write(const char* akey, const T& aval)
		{
			if constexpr (std::is_enum<T>::value)
			{
				write(akey, (int32_t)aval);
			}
			else
			{
				aval.write(*this, akey);
			}
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