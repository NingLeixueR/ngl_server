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
#include "pb_fieldnumber_copy.h"

#include "net.pb.h"

#include "tools.h"
#include "operator_file.h"
#include "xmlprotocol.h"
#include "xml_protocol.h"




int main(int argc, char** argv) 
{
    //message brief_activityvalues
    //{
    //    map<int32,int32> mactivity_rolelv = 1;		// 活动期间玩家等级变化值 key:活动id value:等级增加值
    //    map<int32,int32> mactivity_rolegold = 2;	// 活动期间玩家获取金币量 key:活动id value:等级增加值
    //}

    //    message brief_base
    //{
    //    optional string mname = 1;					// 名字
    //    optional int32 mlv = 2;					// 等级
    //    optional int32 mmoneygold = 3;					// 金币
    //    optional int32 mmoneysilver = 4;					// 银币
    //    optional int32 mvip = 5;					// vip等级
    //    optional int32 mnotalkutc = 6;					// 被禁言的期限
    //    optional int32 mcreateutc = 7;					// 创角时间
    //}

    //    // ENUM_DB_BRIEF,			// 玩家简要数据
    //    message db_brief
    //{
    //    optional int64 mid = 1;					// role id
    //    brief_base m_base = 2;					// 基本数据
    //    optional brief_activityvalues mactivityvalues = 9;	//活动值
    //}
    pbdb::db_brief lbrief1;
    lbrief1.set_mid(1);
    lbrief1.mutable_m_base()->set_mname("brief1");
    lbrief1.mutable_m_base()->set_mlv(1);
    lbrief1.mutable_m_base()->set_mmoneygold(2);
    lbrief1.mutable_m_base()->set_mmoneysilver(3);
    lbrief1.mutable_m_base()->set_mvip(4);
    lbrief1.mutable_m_base()->set_mnotalkutc(5);
    lbrief1.mutable_m_base()->set_mcreateutc(6);

    (*lbrief1.mutable_mactivityvalues()->mutable_mactivity_rolegold())[1] = 1;
    (*lbrief1.mutable_mactivityvalues()->mutable_mactivity_rolegold())[2] = 2;
    (*lbrief1.mutable_mactivityvalues()->mutable_mactivity_rolegold())[3] = 3;

    (*lbrief1.mutable_mactivityvalues()->mutable_mactivity_rolelv())[1] = 1;
    (*lbrief1.mutable_mactivityvalues()->mutable_mactivity_rolelv())[2] = 2;
    (*lbrief1.mutable_mactivityvalues()->mutable_mactivity_rolelv())[3] = 3;

    pbdb::db_brief lbrief2;
    lbrief2.set_mid(1);
    lbrief2.mutable_m_base()->set_mname("brief2");
    lbrief2.mutable_m_base()->set_mlv(10);
    lbrief2.mutable_m_base()->set_mmoneygold(20);
    lbrief2.mutable_m_base()->set_mmoneysilver(30);
    lbrief2.mutable_m_base()->set_mvip(40);
    lbrief2.mutable_m_base()->set_mnotalkutc(50);
    lbrief2.mutable_m_base()->set_mcreateutc(60);

    (*lbrief2.mutable_mactivityvalues()->mutable_mactivity_rolegold())[10] = 1;
    (*lbrief2.mutable_mactivityvalues()->mutable_mactivity_rolegold())[20] = 2;
    (*lbrief2.mutable_mactivityvalues()->mutable_mactivity_rolegold())[30] = 3;

    (*lbrief2.mutable_mactivityvalues()->mutable_mactivity_rolelv())[10] = 1;
    (*lbrief2.mutable_mactivityvalues()->mutable_mactivity_rolelv())[20] = 2;
    (*lbrief2.mutable_mactivityvalues()->mutable_mactivity_rolelv())[30] = 3;

    pb_field::copy(lbrief1, &lbrief2, { 2 });

    std::string ljson;
    ngl::tools::proto2json(lbrief2, ljson);

    pb_field::copy(lbrief1, &lbrief2, { 9 });
    std::string ljson2;
    ngl::tools::proto2json(lbrief2, ljson2);

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