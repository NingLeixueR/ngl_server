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
// File overview: Implements logic for postgresql.


#include "tools/db/sql/postgresql/npostgresql.h"
#include "actor/protocol/nprotocol.h"
#include "tools/log/nlog.h"

#include <iostream>

namespace ngl
{
	bool npostgresql::connectdb(const xarg_db& arg)
	{
        if (m_connectdb)
        {
            return false;
        }

        std::string lconninfo = std::format(
            "host={} port={} dbname={} user={} password={}"
            , arg.m_ip, arg.m_port, arg.m_dbname, arg.m_account, arg.m_passworld
        );
        //const char* conninfo = "host=localhost port=5432 dbname=mydb user=postgres password=123456";

        m_postgresql = PQconnectdb(lconninfo.c_str());

        if (PQstatus(m_postgresql) != CONNECTION_OK)
        {
            log_error()->print("[postgresql] err {}", PQerrorMessage(m_postgresql));
            closedb();
            return false;
        }

        log_error()->print("postgresql://{}:{}/{} success", arg.m_ip, arg.m_port, arg.m_dbname);
        return true;
	}

    void npostgresql::closedb()
    {
        PQfinish(m_postgresql);
    }

    bool npostgresql::query(const char* asql)
    {
        PGresult* res = PQexec(m_postgresql, asql);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) 
        {
            log_error()->print("npostgresql::query[{}]", PQerrorMessage(m_postgresql));
            PQclear(res);
            return false;
        }
        PQclear(res);
        return true;
    }

    bool npostgresql::select(const char* asql, int resultFormat, const npostgresql::callback& aback)
    {
        PGresult* result = PQexecParams(m_postgresql, asql, 0, nullptr, nullptr, nullptr, nullptr, resultFormat);
        if (PQresultStatus(result) != PGRES_TUPLES_OK)
        {
            log_error()->print("npostgresql::select[{}]", PQerrorMessage(m_postgresql));
            PQclear(result);
            return false;
        }
        return aback(result);
    }
}//namespace ngl