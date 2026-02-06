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

#include "npostgresql.h"
#include "nprotocol.h"
#include "nlog.h"

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

        PGconn* conn = PQconnectdb(lconninfo.c_str());

        if (PQstatus(conn) != CONNECTION_OK) 
        {
            log_error()->print("[postgresql] err {}", PQerrorMessage(conn));
            closedb();
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

    bool npostgresql::select(const char* asql, const npostgresql::callback& aback)
    {
        PGresult* res = PQexec(m_postgresql, asql);
        if (PQresultStatus(res) != PGRES_TUPLES_OK) 
        {
            log_error()->print("npostgresql::select[{}]", PQerrorMessage(m_postgresql));
            PQclear(res);
            return false;
        }
        return aback(res);
    }
}//namespace ngl