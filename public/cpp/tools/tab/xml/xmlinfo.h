/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "xml_serialize.h"
#include "tools.h"
#include "type.h"

#include <string>
#include <map>
#include <set>

namespace ngl
{
	struct xarg_db
	{
		std::string m_ip;
		uint32_t	m_port = 0;
		std::string m_account;
		std::string m_passworld;
		std::string m_dbname;

		dxmlserialize(xarg_db, true, m_ip, m_port, m_account, m_passworld, m_dbname)
	};

	struct mail_name
	{
		std::string m_mail;
		std::string m_name;

		dxmlserialize(mail_name, false, m_mail, m_name)
	};

	struct xarg_mail
	{
		std::string m_smtp;
		std::string m_email;
		std::string m_password;
		std::string m_name;
		std::vector<mail_name> m_recvs;

		dxmlserialize(xarg_mail, false, m_smtp, m_email, m_password, m_name, m_recvs)
	};

	struct xarg_telnet
	{
		std::string m_account;
		std::string m_password;

		dxmlserialize(xarg_telnet, false, m_account, m_password)
	};

	class xarg_info
	{
		std::map<std::string, std::string> m_data;
	public:
		dxmlserialize(xarg_info, false, m_data)

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
}// namespace ngl