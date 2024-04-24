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
	class ojson
	{
		cJSON*	m_json;
		bool	m_free;
	public:
		ojson(const char* astr);

		ojson();

		virtual ~ojson();

		bool check()
		{
			return m_json != nullptr;
		}

		bool read(const char* akey, std::string& adata);
		bool read(const char* akey, int8_t& adata);
		bool read(const char* akey, int16_t& adata);
		bool read(const char* akey, int32_t& adata);
		bool read(const char* akey, int64_t& adata);
		bool read(const char* akey, uint8_t& adata);
		bool read(const char* akey, uint16_t& adata);
		bool read(const char* akey, uint32_t& adata);
		bool read(const char* akey, uint64_t& adata);
		bool read(const char* akey, float& adata);
		bool read(const char* akey, double& adata);
		bool read(const char* akey, bool& adata);
		bool read(const char* akey, cJSON*& adata);
		bool read(const char* akey, ojson& adata);

		template <typename T>
		bool read_number(
			const char* akey
			, std::vector<T>& aval
			, const std::function<bool(cJSON* ajson, T& aval)>& afun
		)
		{
			cJSON* ltemp = nullptr;
			if (read(akey, ltemp) == false)
				return false;
			int lsize = cJSON_GetArraySize(ltemp);
			for (int i = 0; i < lsize; ++i)
			{
				cJSON* ret = cJSON_GetArrayItem(ltemp, i);
				T lval;
				if (afun(ret, lval) == false)
					continue;
				aval.push_back(lval);
			}
			return true;
		}

		bool read(const char* akey, std::vector<int8_t>& adata);
		bool read(const char* akey, std::vector<int16_t>& adata);
		bool read(const char* akey, std::vector<int32_t>& adata);
		bool read(const char* akey, std::vector<int64_t>& adata);
		bool read(const char* akey, std::vector<uint8_t>& adata);
		bool read(const char* akey, std::vector<uint16_t>& adata);
		bool read(const char* akey, std::vector<uint32_t>& adata);
		bool read(const char* akey, std::vector<uint64_t>& adata);
		bool read(const char* akey, std::vector<float>& adata);
		bool read(const char* akey, std::vector<double>& adata);
		bool read(const char* akey, std::vector<bool>& adata);
		bool read(const char* akey, std::vector<std::string> & adata);

		template <typename T>
		bool read(const char* akey, T& adata)
		{
			return adata.read(*this, akey);
		}

		template <typename T>
		bool read(const char* akey, std::vector<T>& adata)
		{
			cJSON* ret = cJSON_GetObjectItem(m_json, akey);
			if (nullptr == ret || ret->type != cJSON_Array)
				return false;
			int lsize = cJSON_GetArraySize(ret);
			for (int i = 0; i < lsize; ++i)
			{
				cJSON* tempret = cJSON_GetArrayItem(ret, i);
				T ltemp;
				ojson lretobj;
				lretobj.m_free = false;
				lretobj.m_json = tempret;
				ltemp.read(lretobj);
				adata.push_back(ltemp);
			}
			return true;
		}

		template <typename KEY, typename VAL>
		bool read(const char* akey, std::map<KEY, VAL>& aval)
		{
			std::vector<KEY> lkey;
			std::vector<VAL> lval;
			
			std::string lkeystr = akey;
			lkeystr += "_key";
			if (read(lkeystr.c_str(), lkey) == false)
				return false;
			std::string lvalstr = akey;
			lvalstr += "_val";
			if (read(lvalstr.c_str(), lval) == false)
				return false;
			for (int i = 0; i < lkey.size() && i < lval.size(); ++i)
			{
				aval.insert({ lkey[i], lval[i] });
			}
			return true;
		}

		template <typename T>
		bool read(const char* akey, std::list<T>& aval)
		{
			std::vector<T> lvec;
			if (read(akey, lvec) == false)
				return false;
			aval.assign(lvec);
			return true;
		}

		template <typename T, typename ...ARG>
		bool read(const char* akey, T& avalue, ARG&... arg)
		{
			if (read(akey, avalue) == false)
				return false;
			return read(arg...);
		}
	};
}// namespace ngl

