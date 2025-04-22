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

#include "net.pb.h"

#include "tools.h"
#include "operator_file.h"
#include "xmlprotocol.h"
#include "xml_protocol.h"


int main(int argc, char** argv) 
{
    xml_protocol::create_actor();

    google::protobuf::compiler::DiskSourceTree sourceTree;
    sourceTree.MapPath("", argv[1]);
    xml_protocol::nactor_auto(sourceTree, "db");
   
    xml_protocol::g_stream_sql << "/*Date:" << ngl::localtime::time2str("%Y-%m-%d %H:%M:%S")<< "*/" << std::endl;
    xml_protocol::g_stream_sql << std::endl;
    xml_protocol::g_stream_sql << " DROP Database IF EXISTS `lbtest`;" << std::endl;
    xml_protocol::g_stream_sql << " CREATE DATABASE lbtest default charset utf8 COLLATE utf8_general_ci;" << std::endl;
    xml_protocol::g_stream_sql << " use lbtest;" << std::endl;
    xml_protocol::g_stream_sql << std::endl;

    xml_protocol::db(sourceTree, "Template", "db");
  
    // 生成对应的sql文件
    ngl::writefile lsql("create_db.sql");
    lsql.write(xml_protocol::g_stream_sql.str());
    // 自动关联结构体为其提供协议号    
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