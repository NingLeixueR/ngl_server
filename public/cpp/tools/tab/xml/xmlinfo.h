#pragma once

#include <string>
#include <map>
#include <set>

#include "tools.h"
#include "type.h"

namespace ngl
{
	struct dbarg
	{
		uint32_t	m_port;
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

		void plog();

		//# 根据key查找
		template <typename TVALUE>
		bool find(const char* akey, TVALUE& adata)
		{
			std::string* lp = tools::findmap<std::string, std::string>(m_data, akey);
			if (lp == nullptr)
				return false;
			adata = tools::lexical_cast<TVALUE>(*lp);
			return true;
		}
		bool find(const char* akey, bool& adata);
		bool find(const char* akey, std::string& adata);
		//# 是否存在key
		bool ishavekey(const char* akey);
		//# key:threadnum 线程数量 
		bool threadnum(i32_threadsize& avalue);
		//# key:actorthreadnum actor线程数量 
		bool actorthreadnum(i32_threadsize& avalue);
		//# key:id id
		bool id(i32_id& avalue);
		//# key:port 端口
		bool port(i16_port& avalue);
		//# key:db db服务器id
		bool db(i32_serverid& avalue);
		//# key:actorserver actorserver服务器id
		bool actorserver(i32_serverid& avalue);
		//# key:reloadcsv reloadcsv服务器id
		bool reloadcsv(i32_serverid& avalue);
		//# key:name 名称
		bool name(std::string& avalue);
		//# key:ip ip
		bool ip(std::string& avalue);
		//# key:line 线路
		bool line(i32_line& avalue);
		//# key:area 区服
		bool area(i16_area& avalue);
		//# key:varint 是否开启varint
		bool varint(bool& avalue);
		//# key:isxor 是否简单加密
		bool isxor(bool& avalue);
		//# key:xor_str 简单加密xor的字符串
		bool xor_str(std::string& avalue);
	};

	struct dbserver_info
	{
		dbarg m_dbarg;
	};	
}// namespace ngl