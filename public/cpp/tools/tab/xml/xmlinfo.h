/*
* Copyright (c) [2020-2025] NingLeixueR
*
* Project: ngl_server
* License: MIT
*/
#pragma once

#include "tools/tab/xml/xml_serialize.h"
#include "tools/tools.h"
#include "tools/type.h"

#include <functional>
#include <map>
#include <set>
#include <string>
#include <string_view>

namespace ngl
{
	struct xarg_db
	{
		enum edb
		{
			edb_mysql = 0,
			edb_postgresql = 1,
		};

		edb			m_db{};
		std::string m_ip;
		uint32_t	m_port = 0;
		std::string m_account;
		std::string m_passworld;
		std::string m_dbname;

		DXMLSERIALIZE(xarg_db, true, m_db, m_ip, m_port, m_account, m_passworld, m_dbname)
	};

	struct mail_name
	{
		std::string m_mail;
		std::string m_name;

		DXMLSERIALIZE(mail_name, false, m_mail, m_name)
	};

	struct xarg_mail
	{
		std::string m_smtp;
		std::string m_email;
		std::string m_password;
		std::string m_name;
		std::vector<mail_name> m_recvs;

		DXMLSERIALIZE(xarg_mail, false, m_smtp, m_email, m_password, m_name, m_recvs)
	};

	struct xarg_telnet
	{
		std::string m_account;
		std::string m_password;

		DXMLSERIALIZE(xarg_telnet, false, m_account, m_password)
	};

	struct xarg_wss
	{
		std::string m_certificate_chain;
		std::string m_private_key;
		std::string m_ca_certificates;
		int32_t		m_verify_peer = 1;

		DXMLSERIALIZE(xarg_wss, false, m_certificate_chain, m_private_key, m_ca_certificates, m_verify_peer)
	};

	class xarg_info
	{
	public:
		using map_type = std::map<std::string, std::string, std::less<>>;

	private:
		map_type m_data;

		const std::string* find_raw(const char* akey) const;
	public:
		DXMLSERIALIZE(xarg_info, false, m_data)

		map_type& data()
		{
			return m_data;
		}

		const map_type& data() const
		{
			return m_data;
		}

		std::size_t size() const noexcept
		{
			return m_data.size();
		}

		template <typename TVALUE>
		bool find(const char* akey, TVALUE& adata) const
		{
			const std::string* lp = find_raw(akey);
			if (lp == nullptr)
			{
				return false;
			}
			try
			{
				TVALUE lvalue = tools::lexical_cast<TVALUE>(*lp);
				adata = std::move(lvalue);
			}
			catch (...)
			{
				return false;
			}
			return true;
		}

		bool find(const char* akey, bool& adata) const;

		bool find(const char* akey, std::string& adata) const;

		bool find_view(const char* akey, std::string_view& adata) const;

		template <typename TFUN>
		void foreach(TFUN&& afun) const
		{
			for (const auto& item : m_data)
			{
				afun(item);
			}
		}
	};

	struct xarg_redis
	{
		str_ip		m_ip;
		i16_port	m_port = 0;
		std::string m_passworld;

		DXMLSERIALIZE(redis, true, m_ip, m_port, m_passworld)
	};
}// namespace ngl
