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
		uint32_t	m_port = 0;
		std::string m_ip;
		std::string m_account;
		std::string m_passworld;
		std::string m_dbname;
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
	};

	struct telnetarg
	{
		std::string m_account;
		std::string m_passworld;
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

		//# 遍历所有key/value
		void foreach(const std::function<void(const std::pair<const std::string, std::string>&)>& afun);
	};

	struct dbserver_info
	{
		dbarg m_dbarg;
	};	

	struct mail_info
	{
		mailarg m_mailarg;
	};

	struct telnet_info
	{
		telnetarg m_telnetarg;
	};
}// namespace ngl