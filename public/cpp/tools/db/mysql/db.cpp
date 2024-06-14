#include "nprotocol.h"
#include "nlog.h"
#include "db.h"

#define _SQL_BUFF1_SIZE_ (102400)
#define _SQL_BUFF2_SIZE_ (102400)

namespace ngl
{
	db::db() :
		m_connectdb(false),
		m_mysql(nullptr),
		m_buff1(new char[_SQL_BUFF1_SIZE_]),
		m_buff2(new char[_SQL_BUFF2_SIZE_]),
		m_bufflen1(_SQL_BUFF1_SIZE_),
		m_bufflen2(_SQL_BUFF2_SIZE_)
	{}

	bool db::connectdb(const dbarg& arg)
	{
		if (m_connectdb)
		{
			return false;
		}
		m_arg = arg;
		log_error()->print("[mysql] {}@{}:{} [password={}][dbname={}]"
			, m_arg.m_account
			, m_arg.m_ip, m_arg.m_port
			, m_arg.m_passworld
			, m_arg.m_dbname
		);
		m_mysql = mysql_init((MYSQL*)nullptr);
		if (!m_mysql)
		{
			log_error()->print("[mysql] err mysql_init");
			return false;
		}

		if (!mysql_real_connect(
			m_mysql,
			m_arg.m_ip.c_str(),
			m_arg.m_account.c_str(),
			m_arg.m_passworld.c_str(),
			nullptr,
			m_arg.m_port,
			nullptr,
			0))
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

	void db::closedb()
	{
		if (m_mysql != nullptr)
		{
			mysql_close(m_mysql);
			m_mysql = nullptr;
		}
	}

	bool db::changedb(MYSQL* amysql, std::string& adbname)
	{
		if (m_arg.m_dbname != adbname)
		{
			m_arg.m_dbname = adbname;
		}
		return mysql_select_db(amysql, adbname.c_str()) == 0;
	}

	bool db::query(const char* asql, int alen)
	{
		int ret = mysql_real_query(m_mysql, asql, (unsigned long)(alen + 1));
		if (ret != 0)
		{
			log_error()->print("db::query[{}][{}]", mysql_error(m_mysql), (const char*)asql);
			return false;
		}
		log_error()->print("db::query[{}]", asql);
		return true;
	}

	void db::escape(const char* asql, std::string& aoutsql)
	{
		char lbuff[10240] = { 0 };
		mysql_real_escape_string(m_mysql, lbuff, asql, strlen(asql));
		aoutsql = lbuff;
	}

	bool db::select(const char* asql, int asqllen, callback aback)
	{
		int ret = mysql_real_query(m_mysql, asql, (unsigned long)(asqllen));
		if (ret == 0)
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
				uint32_t liTableRow = (uint32_t)mysql_num_rows(pRes);
				uint32_t liTableCol = (uint32_t)mysql_num_fields(pRes);
				MYSQL_ROW lMYSQL_ROW;
				unsigned long* lMYSQL_LENS;
				for (uint32_t i = 0; i < liTableRow; i++)
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
			log_error()->print("db::select[{}][{}]", mysql_error(m_mysql), (const char*)asql);
		}
		return false;
	}
}// namespace ngl