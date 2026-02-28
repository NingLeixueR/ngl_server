#include <functional>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <fstream> 
#include <memory>
#include <string>
#include <vector>
#include <regex>
#include <set>
#include <map>

#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/descriptor.h>

#include "actor/pb/net.pb.h"

#include "tools/operator_file.h"
#include "xml_protocol.h"
#include "tools/tools.h"


int main(int argc, char** argv) 
{
    xml_protocol::create_actor();

    google::protobuf::compiler::DiskSourceTree sourceTree;
    sourceTree.MapPath("", argv[1]);

    xml_protocol::nscript_foreachProtobuf(sourceTree, "db");
    xml_protocol::nscript_foreachProtobuf(sourceTree, "net");
    xml_protocol::nscript_foreachProtobuf(sourceTree, "example");

    xml_protocol::nactor_auto(sourceTree, "db");

    xml_protocol::g_stream_sql << "/*Date:" << ngl::localtime::time2str("%Y-%m-%d %H:%M:%S")<< "*/" << std::endl;
    xml_protocol::g_stream_sql << std::endl;
    xml_protocol::g_stream_sql << " DROP Database IF EXISTS `lbtest`;" << std::endl;
    xml_protocol::g_stream_sql << " CREATE DATABASE lbtest default charset utf8 COLLATE utf8_general_ci;" << std::endl;
    xml_protocol::g_stream_sql << " use lbtest;" << std::endl;
    xml_protocol::g_stream_sql << std::endl;

    xml_protocol::g_stream_pgsql << "/*Date:" << ngl::localtime::time2str("%Y-%m-%d %H:%M:%S") << "*/" << std::endl;
    xml_protocol::g_stream_pgsql << std::endl;
    xml_protocol::g_stream_pgsql << " DROP DATABASE IF EXISTS lbtest;" << std::endl;
    xml_protocol::g_stream_pgsql << " CREATE DATABASE lbtest WITH ENCODING='UTF8' LC_COLLATE='zh_CN.UTF-8' LC_CTYPE='zh_CN.UTF-8' OWNER=lbtest_user;" << std::endl;
    xml_protocol::g_stream_pgsql << std::endl;

    xml_protocol::db(sourceTree, "Template", "db");

    {
        {
            ngl::writefile lsql("../configure/config/create_db.sql");
            lsql.write(xml_protocol::g_stream_sql.str());
        }
        {
            ngl::writefile lsql("../configure/config/create_pgdb.sql");
            lsql.write(xml_protocol::g_stream_pgsql.str());
        }
    }    


    ngl::xmlprotocol::load();
    int32_t lnumber = 0;
    std::pair<std::string, std::string> lpair;
    std::pair<std::string, std::string> lpair2 = xml_protocol::foreachProtobuf(sourceTree, lnumber, "net");
    lpair.first += lpair2.first;
    lpair.second += lpair2.second;
    lpair2 = xml_protocol::foreachProtobuf(sourceTree, lnumber, "example");
    lpair.first += lpair2.first;
    lpair.second += lpair2.second;
    xml_protocol::pb_protocol(lpair.first);
    xml_protocol::json2_fun(lpair.second);

    return 0;
}