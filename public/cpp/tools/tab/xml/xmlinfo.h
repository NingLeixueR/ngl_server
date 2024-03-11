#pragma once

#include <boost/lexical_cast.hpp>
#include <string>
#include <map>
#include <set>

#include "type.h"
#include "tools.h"

namespace ngl
{
	struct dbarg
	{
		uint32_t m_port;
		std::string m_ip;
		std::string m_account;
		std::string m_passworld;
		std::string m_dbname;
		dbarg() :
			m_port(0)
		{}
	};

	class xmlinfo
	{
	public:
		std::map<std::string, std::string> m_data;

		void log();

		template <typename TVALUE>
		bool find(const char* akey, TVALUE& adata)
		{
			std::string* lp = tools::findmap<std::string, std::string>(m_data, akey);
			if (lp == nullptr)
				return false;
			adata = boost::lexical_cast<TVALUE>(*lp);
			return true;
		}

		bool find(const char* akey, bool& adata);
		bool find(const char* akey, std::string& adata);

		bool ishavekey(const char* akey);
		bool threadnum(i32_threadsize& avalue);
		bool actorthreadnum(i32_threadsize& avalue);
		bool id(i32_id& avalue);
		bool port(i16_port& avalue);
		bool db(i32_serverid& avalue);
		bool actorserver(i32_serverid& avalue);
		bool reloadcsv(i32_serverid& avalue);
		bool name(std::string& avalue);
		bool ip(std::string& avalue);
		bool line(i32_line& avalue);
		bool area(i16_area& avalue);
		bool outernet(bool& avalue);
		bool varint(bool& avalue);
		bool isxor(bool& avalue);
		bool xor_str(std::string& avalue);
		bool isuselua();
	};

	struct dbserver_info
	{
		dbarg m_dbarg;
	};	
}// namespace ngl