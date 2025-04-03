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
	class json_read
	{
		cJSON*	m_json;
		bool	m_free;
	public:
		json_read(const char* astr);

		json_read();

		virtual ~json_read();

		bool check()
		{
			return m_json != nullptr;
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

		template <typename T>
		bool read_number(const char* akey, std::vector<T>& aval, const std::function<bool(const cJSON* ajson, T& aval)>& afun)const
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
				T lval;
				if (afun(ret, lval) == false)
				{
					continue;
				}
				aval.push_back(lval);
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

		template <typename T>
		bool read(const char* akey, T& adata) const
		{
			return adata.read(*this, akey);
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

		template <typename VAL>
		bool read(const char* akey, std::vector<std::pair<std::string, VAL>>& adata) const
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
				std::string lkey = tempret->child->string;
				VAL lval;
				json_read lretobj;
				lretobj.m_free = false;
				lretobj.m_json = tempret;
				if (lretobj.read(lkey.c_str(), lval) == false)
				{
					continue;
				}
				adata.push_back(std::make_pair(lkey, lval));
			}
			return true;
		}

		template <typename KEY, typename VAL>
		bool read(const char* akey, std::map<KEY, VAL>& aval) const
		{
			std::vector<std::pair<std::string, VAL>> lvec;
			if (read(akey, lvec) == false)
			{
				return false;
			}
			for (int i = 0; i < lvec.size(); ++i)
			{
				aval.insert(std::make_pair(tools::lexical_cast<KEY>(lvec[i].first), lvec[i].second));
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
			cJSON* item = NULL;
			cJSON_ArrayForEach(item, m_json)
			{
				avec.push_back(item->string);
			}
		}

		void read_key(const std::function<void(const std::string&)>& afun) const
		{
			cJSON* item = NULL;
			cJSON_ArrayForEach(item, m_json)
			{
				afun(item->string);
			}
		}

		void read_key(const std::function<void(cJSON*)>& afun) const
		{
			cJSON* item = NULL;
			cJSON_ArrayForEach(item, m_json)
			{
				afun(item);
			}
		}
	};
}// namespace ngl