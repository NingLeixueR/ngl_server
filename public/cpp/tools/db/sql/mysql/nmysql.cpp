/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements MySQL connection lifecycle and query execution.

#include "actor/protocol/nprotocol.h"
#include "tools/db/sql/mysql/nmysql.h"
#include "tools/log/nlog.h"

#include <limits>

namespace ngl
{
	namespace
	{
		bool mysql_length(std::size_t alen, unsigned long& aoutlen)
		{
			if (alen > static_cast<std::size_t>(std::numeric_limits<unsigned long>::max()))
			{
				log_error()->print("mysql sql too long [{}]", alen);
				return false;
			}
			aoutlen = static_cast<unsigned long>(alen);
			return true;
		}
	}

	bool nmysql::connectdb(const xarg_db& arg)
	{
		if (m_connectdb)
		{
			return false;
		}
		m_arg = arg;
		log_error()->print("[mysql] {}@{}:{} [dbname={}]", m_arg.m_account, m_arg.m_ip, m_arg.m_port, m_arg.m_dbname);
		m_mysql = mysql_init((MYSQL*)nullptr);
		if (!m_mysql)
		{
			log_error()->print("[mysql] err mysql_init");
			return false;
		}

		if (!mysql_real_connect(m_mysql, m_arg.m_ip.c_str(), m_arg.m_account.c_str(), m_arg.m_passworld.c_str(), nullptr, m_arg.m_port, nullptr, 0))
		{
			log_error()->print("[mysql] err db::query[{}]", mysql_error(m_mysql));
			closedb();
			return false;
		}

		char value = 1;
		mysql_options(m_mysql, MYSQL_OPT_RECONNECT, (char*)&value);

		if (mysql_set_character_set(m_mysql, "utf8"))
		{
			log_error()->print("[mysql] err utf8");
			closedb();
			return false;
		}

		if (!m_arg.m_dbname.empty() && !changedb(m_mysql, m_arg.m_dbname))
		{
			log_error()->print("[mysql] err dbname");
			closedb();
			return false;
		}

		m_connectdb = true;
		return true;
	}

	void nmysql::closedb()
	{
		if (m_mysql != nullptr)
		{
			mysql_close(m_mysql);
			m_mysql = nullptr;
		}
		m_connectdb = false;
	}

	bool nmysql::changedb(MYSQL* amysql, std::string& adbname)
	{
		if (m_arg.m_dbname != adbname)
		{
			m_arg.m_dbname = adbname;
		}
		return mysql_select_db(amysql, adbname.c_str()) == 0;
	}

	bool nmysql::query(const char* asql, std::size_t alen)
	{
		unsigned long lsqlen = 0;
		if (!mysql_length(alen, lsqlen))
		{
			return false;
		}
		const int ret = mysql_real_query(m_mysql, asql, lsqlen);
		if (ret != 0)
		{
			log_error()->print("nmysql::query[{}][{}]", mysql_error(m_mysql), asql);
			return false;
		}
		log_error()->print("nmysql::query[{}]", asql);
		return true;
	}

	void nmysql::escape(const char* asql, std::size_t asqllen, std::string& aoutsql)
	{
		unsigned long lsqlen = 0;
		if (!mysql_length(asqllen, lsqlen))
		{
			aoutsql.clear();
			return;
		}
		char lbuff[10240] = { 0 };
		mysql_real_escape_string(m_mysql, lbuff, asql, lsqlen);
		aoutsql = lbuff;
	}

	bool nmysql::select(const char* asql, std::size_t asqllen, const callback& aback)
	{
		unsigned long lsqlen = 0;
		if (!mysql_length(asqllen, lsqlen))
		{
			return false;
		}
		if (const int ret = mysql_real_query(m_mysql, asql, lsqlen); ret == 0)
		{
			MYSQL_RES* pRes = nullptr;
			do
			{
				if (mysql_field_count(m_mysql) > 0)
				{
					pRes = mysql_store_result(m_mysql);
				}
			} while (mysql_next_result(m_mysql) == 0);
			if (pRes)
			{
				auto liTableRow = mysql_num_rows(pRes);
				auto liTableCol = mysql_num_fields(pRes);
				MYSQL_ROW lMYSQL_ROW;
				unsigned long* lMYSQL_LENS;
				for (my_ulonglong i = 0; i < liTableRow; i++)
				{
					lMYSQL_ROW = mysql_fetch_row(pRes);
					lMYSQL_LENS = mysql_fetch_lengths(pRes);
					aback(lMYSQL_ROW, lMYSQL_LENS, liTableRow, liTableCol);
				}
				mysql_free_result(pRes);
				return true;
			}
		}
		else
		{
			log_error()->print("db::select fail [{}] [{}]", mysql_error(m_mysql), asql);
		}
		return false;
	}

	// Prepared-statement execution path used when callers bind parameters explicitly.
	bool nmysql::stmt_query(const char* asql, std::size_t alen, MYSQL_BIND* abind)
	{
		unsigned long lsqlen = 0;
		if (!mysql_length(alen, lsqlen))
		{
			return false;
		}
		MYSQL_STMT* lstmt = mysql_stmt_init(m_mysql);
		if (lstmt == nullptr)
		{
			return false;
		}
		int err = mysql_stmt_prepare(lstmt, asql, lsqlen);
		if (err != 0)
		{
			log_error()->print("mysql_stmt_prepare fail [{}]", err);
			mysql_stmt_close(lstmt);
			return false;
		}
		err = mysql_stmt_bind_param(lstmt, abind);
		if (err != 0)
		{
			log_error()->print("mysql_stmt_bind_param fail [{}]", err);
			mysql_stmt_close(lstmt);
			return false;
		}
		err = mysql_stmt_execute(lstmt);
		if (err != 0)
		{
			log_error()->print("mysql_stmt_execute fail [{}]", err);
			mysql_stmt_close(lstmt);
			return false;
		}
		mysql_stmt_close(lstmt);
		return true;
	}
}// namespace ngl
