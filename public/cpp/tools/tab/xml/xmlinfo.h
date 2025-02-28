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

	struct mailarg
	{
		std::string m_smtp;
		std::string m_email;
		std::string m_password;
		std::string m_name;
		std::vector<std::pair<std::string, std::string>> m_recvs;// key:mail value:name
		std::string m_title;
		std::string m_content;

		mailarg() = default;
	};

	class xmlinfo
	{
		std::map<std::string, std::string> m_data;
	public:
		void plog();

		std::map<std::string, std::string>& data()
		{
			return m_data;
		}

		//# 根据key查找value
		template <typename TVALUE>
		bool find(const char* akey, TVALUE& adata)
		{
			std::string* lp = tools::findmap<std::string, std::string>(m_data, akey);
			if (lp == nullptr)
			{
				return false;
			}
			adata = tools::lexical_cast<TVALUE>(*lp);
			return true;
		}

		//# 根据key查找value
		bool find(const char* akey, bool& adata);

		//# 根据key查找value
		bool find(const char* akey, std::string& adata);
	};

	struct dbserver_info
	{
		dbarg m_dbarg;
	};	

	struct mail_info
	{
		mailarg m_mailarg;
	};
	
}// namespace ngl