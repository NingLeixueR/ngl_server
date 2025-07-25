#pragma once

#include <functional>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <set>

#include "cJSON.h"

namespace ngl
{
	class json_read;
	union nguid;

	template <typename T, bool IS_FORWARD>
	struct protobuf_data;

	class json_read
	{
		cJSON*	m_json;
		bool	m_free;
	public:
		json_read(const char* astr);

		json_read();

		virtual ~json_read();

		std::string get()const
		{
			char* json_str = cJSON_PrintUnformatted(m_json);
			std::string ltemp(json_str);
			free(json_str); 
			return ltemp;
		}

		bool check()
		{
			return m_json != nullptr;
		}

		cJSON* json()
		{
			return m_json;
		}

		bool read(const char* akey, std::string& adata) const;
		bool read(const char* akey, int8_t& adata) const;
		bool read(const char* akey, int16_t& adata) const;
		bool read(const char* akey, int32_t& adata) const;
		bool read(const char* akey, int64_t& adata) const;
		bool read(const char* akey, uint8_t& adata) const;
		bool read(const char* akey, uint16_t& adata) const;
		bool read(const char* akey, uint32_t& adata) const;
		bool read(const char* akey, uint64_t& adata) const;
		bool read(const char* akey, float& adata) const;
		bool read(const char* akey, double& adata) const;
		bool read(const char* akey, bool& adata) const;
		bool read(const char* akey, cJSON*& adata) const;
		bool read(const char* akey, json_read& adata) const;
		bool read(const char* akey, nguid& aval) const;
		
		template <typename T>
		bool read_number(
			const char* akey, const std::function<void(const cJSON* ajson)>& afun
		)const
		{
			cJSON* ltemp = nullptr;
			if (read(akey, ltemp) == false)
			{
				return false;
			}
			int lsize = cJSON_GetArraySize(ltemp);
			for (int i = 0; i < lsize; ++i)
			{
				cJSON* ret = cJSON_GetArrayItem(ltemp, i);
				afun(ret);
			}
			return true;
		}

		bool read(const char* akey, std::vector<int8_t>& adata) const;
		bool read(const char* akey, std::vector<int16_t>& adata) const;
		bool read(const char* akey, std::vector<int32_t>& adata) const;
		bool read(const char* akey, std::vector<int64_t>& adata) const;
		bool read(const char* akey, std::vector<uint8_t>& adata) const;
		bool read(const char* akey, std::vector<uint16_t>& adata) const;
		bool read(const char* akey, std::vector<uint32_t>& adata) const;
		bool read(const char* akey, std::vector<uint64_t>& adata) const;
		bool read(const char* akey, std::vector<float>& adata) const;
		bool read(const char* akey, std::vector<double>& adata) const;
		bool read(const char* akey, std::vector<bool>& adata) const;
		bool read(const char* akey, std::vector<std::string> & adata) const;

		bool read(const char* akey, std::list<int8_t>& adata) const;
		bool read(const char* akey, std::list<int16_t>& adata) const;
		bool read(const char* akey, std::list<int32_t>& adata) const;
		bool read(const char* akey, std::list<int64_t>& adata) const;
		bool read(const char* akey, std::list<uint8_t>& adata) const;
		bool read(const char* akey, std::list<uint16_t>& adata) const;
		bool read(const char* akey, std::list<uint32_t>& adata) const;
		bool read(const char* akey, std::list<uint64_t>& adata) const;
		bool read(const char* akey, std::list<float>& adata) const;
		bool read(const char* akey, std::list<double>& adata) const;
		bool read(const char* akey, std::list<bool>& adata) const;
		bool read(const char* akey, std::list<std::string>& adata) const;

		bool read(const char* akey, std::set<int8_t>& adata) const;
		bool read(const char* akey, std::set<int16_t>& adata) const;
		bool read(const char* akey, std::set<int32_t>& adata) const;
		bool read(const char* akey, std::set<int64_t>& adata) const;
		bool read(const char* akey, std::set<uint8_t>& adata) const;
		bool read(const char* akey, std::set<uint16_t>& adata) const;
		bool read(const char* akey, std::set<uint32_t>& adata) const;
		bool read(const char* akey, std::set<uint64_t>& adata) const;
		bool read(const char* akey, std::set<float>& adata) const;
		bool read(const char* akey, std::set<double>& adata) const;
		bool read(const char* akey, std::set<bool>& adata) const;
		bool read(const char* akey, std::set<std::string>& adata) const;

		template <typename T>
		bool read(const char* akey, T& adata) const
		{
			if constexpr (std::is_enum<T>::value)
			{
				int32_t ltemp = 0;
				if (read(akey, ltemp))
				{
					adata = (T)ltemp;
					return true;
				}
				return false;
			}
			else
			{
				return adata.read(*this, akey);
			}
		}

		template <typename T>
		bool read(const char* akey, std::vector<T>& adata) const
		{
			const cJSON* ret = cJSON_GetObjectItem(m_json, akey);
			if (nullptr == ret || ret->type != cJSON_Array)
			{
				return false;
			}
			int lsize = cJSON_GetArraySize(ret);
			for (int i = 0; i < lsize; ++i)
			{
				cJSON* tempret = cJSON_GetArrayItem(ret, i);
				T ltemp;
				json_read lretobj;
				lretobj.m_free = false;
				lretobj.m_json = tempret;
				ltemp.read(lretobj);
				adata.push_back(ltemp);
			}
			return true;
		}


		template <typename KEY, typename VAL>
		bool read_mapnumber(const char* akey, std::map<KEY, VAL>& aval) const;

		template <typename VAL>
		bool read(const char* akey, std::map<nguid, VAL>& aval) const;

		template <typename VAL>
		bool read(const char* akey, std::map<int16_t, VAL>& aval) const
		{
			return read_mapnumber(akey, aval);
		}

		template <typename VAL>
		bool read(const char* akey, std::map<uint16_t, VAL>& aval) const
		{
			return read_mapnumber(akey, aval);
		}

		template <typename VAL>
		bool read(const char* akey, std::map<int32_t, VAL>& aval) const
		{
			return read_mapnumber(akey, aval);
		}

		template <typename VAL>
		bool read(const char* akey, std::map<uint32_t, VAL>& aval) const
		{
			return read_mapnumber(akey, aval);
		}

		template <typename VAL>
		bool read(const char* akey, std::map<int64_t, VAL>& aval) const
		{
			return read_mapnumber(akey, aval);
		}

		template <typename VAL>
		bool read(const char* akey, std::map<uint64_t, VAL>& aval) const
		{
			return read_mapnumber(akey, aval);
		}

		template <typename VAL>
		bool read(
			const char* akey
			, std::map<std::string, VAL>& aval
		) const
		{
			json_read ltemp;
			if (read(akey, ltemp) == false)
			{
				return false;
			}

			for (cJSON* child = ltemp.m_json->child; child != nullptr; child = child->next)
			{
				if (!ltemp.read(child->string, aval[child->string]))
				{
					return false;
				}
			}
			return true;
		}

		template <typename T>
		bool read(const char* akey, std::list<T>& aval) const
		{
			std::vector<T> lvec;
			if (read(akey, lvec) == false)
			{
				return false;
			}
			aval.assign(lvec);
			return true;
		}

		template <typename T, typename ...ARG>
		bool read(const char* akey, T& avalue, ARG&... arg) const
		{
			if (read(akey, avalue) == false)
			{
				return false;
			}
			return read(arg...);
		}

		void read_key(std::vector<std::string>& avec) const
		{
			cJSON* item = nullptr;
			cJSON_ArrayForEach(item, m_json)
			{
				avec.push_back(item->string);
			}
		}

		void read_key(const std::function<void(const std::string&)>& afun) const
		{
			cJSON* item = nullptr;
			cJSON_ArrayForEach(item, m_json)
			{
				afun(item->string);
			}
		}

		void read_key(const std::function<void(cJSON*)>& afun) const
		{
			cJSON* item = nullptr;
			cJSON_ArrayForEach(item, m_json)
			{
				afun(item);
			}
		}
	};

}// namespace ngl