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

#include "tools.h"
#include "operator_file.h"
#include "xmlprotocol.h"
//#include "manage_csv.h"
//#include "ttab_servers.h"
//#include "manage_curl.h"
//
//#include "localtime.h"

std::map<std::string, std::string> g_typearr = 
{ 
    {"bool", "bool"}, 
    {"int32", "int32_t"}, 
    {"int64", "int64_t"}, 
    {"string", "const char*"}, 
    {"float", "float"}, 
    {"double", "double"}
};

std::string type_name(const char* apackname, const google::protobuf::FieldDescriptor* fieldDescriptor)
{
    if (fieldDescriptor->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
    {
        const google::protobuf::Descriptor* nestedDescriptor = fieldDescriptor->message_type();
        fieldDescriptor->type();
        // ��ȡǶ����Ϣ���͵�����
        std::string nestedMessageTypeName = nestedDescriptor->full_name();
        std::cout << nestedDescriptor->DebugString() << std::endl;
        std::cout << "Nested message type name: " << nestedMessageTypeName << std::endl;
        std::string lparmname;
        ngl::tools::replace(std::string(apackname) + ".", "", nestedMessageTypeName, lparmname);
        return lparmname;
    }
    return "";
}

static std::stringstream g_stream_sql;
static std::stringstream g_stream_xml;

void foreachProtobufMessages(const google::protobuf::FileDescriptor* fileDescriptor, int32_t& aprotocol, const std::string& axml)
{
    ngl::writefile lfile("./config/" + axml + "_protocol.xml");

    std::string lnamespace;
    if (axml == "net")
    {
        lnamespace = "pbnet";
    }
    else if (axml == "example")
    {
        lnamespace = "pbexample";
    }
    else if (axml == "gm")
    {
        lnamespace = "GM";
    }

    std::stringstream m_stream;
    m_stream << "<?xml version = \"1.0\" encoding = \"utf-8\"?>" << std::endl;
    m_stream << "<con>" << std::endl;
    int messageCount = fileDescriptor->message_type_count();
    for (int i = 0; i < messageCount; ++i)
    {
        const google::protobuf::Descriptor* messageDescriptor = fileDescriptor->message_type(i);
        std::cout << "name: " << messageDescriptor->name() << std::endl;
        int32_t lprotocol = ngl::xmlprotocol::protocol(messageDescriptor->name());
        if (messageDescriptor->name().find("PROBUFF_NET_") != std::string::npos 
            || messageDescriptor->name().find("PROBUFF_GM_") != std::string::npos
            || messageDescriptor->name().find("PROBUFF_EXAMPLE_") != std::string::npos
            )
        {
            int32_t lnumber = (lprotocol == -1 ? ngl::xmlprotocol::free_protocol() : lprotocol);
            m_stream << "\t<config client = \"1\" name=\"class "
                << lnamespace << "::" << messageDescriptor->name()
                << "\" number=\"" << lnumber << "\"/>" << std::endl;
            m_stream << "\t<config client = \"1\" name=\""
                << lnamespace << "::" << messageDescriptor->name()
                << "\" number=\"" << lnumber << "\"/>" << std::endl;
        }
       
    }
    m_stream << "</con>" << std::endl;    
    lfile.write(m_stream.str());
}

std::string foreachProtobufMessages2(const google::protobuf::FileDescriptor* fileDescriptor, int32_t& aprotocol, const std::string& axml)
{
    std::string lnamespace;
    if (axml == "net")
    {
        lnamespace = "pbnet";
    }
    else if (axml == "db")
    {//pbdb
        lnamespace = "pbdb";
    }
    else if (axml == "example")
    {//pbexample
        lnamespace = "pbexample";
    }
    else if (axml == "gm")
    {
        lnamespace = "GM";
    }

    std::stringstream m_stream_pb;
    int messageCount = fileDescriptor->message_type_count();
    for (int i = 0; i < messageCount; ++i)
    {
        const google::protobuf::Descriptor* messageDescriptor = fileDescriptor->message_type(i);
        std::cout << "name: " << messageDescriptor->name() << std::endl;
        int32_t lprotocol = ngl::xmlprotocol::protocol(messageDescriptor->name());
        if (messageDescriptor->name().find("PROBUFF_NET_") != std::string::npos
            || messageDescriptor->name().find("PROBUFF_GM_") != std::string::npos
            || messageDescriptor->name().find("PROBUFF_EXAMPLE_") != std::string::npos
            )
        {
            m_stream_pb << std::format("            null<{}::{}>,", lnamespace, messageDescriptor->name()) << std::endl;
        }
    }
    return m_stream_pb.str();
}

void foreachProtobufMessages3(const std::string& astr)
{
    ngl::writefile lfile("./pb_protocol.cpp");

    std::stringstream m_stream;
    m_stream << " // ע�⡾makeproto ���������ļ�����Ҫ�ֶ��޸ġ�" << std::endl;
    m_stream << " // ����ʱ�䡾" << ngl::localtime::time2str() << "��" << std::endl;
    m_stream << "#include \"nprotocol.h\"" << std::endl;
    m_stream << "#include \"tprotocol.h\"" << std::endl;
    m_stream << "#include \"actor.h\"" << std::endl;
    m_stream << std::endl;
    m_stream << "#include \"example.pb.h\"" << std::endl;
    m_stream << "#include \"net.pb.h\"" << std::endl;
    m_stream << std::endl;
    m_stream << "#include <map>" << std::endl;
    m_stream << std::endl;
    m_stream << "namespace ngl" << std::endl;
    m_stream << "{" << std::endl;
    m_stream << "   template <typename PB>" << std::endl;
    m_stream << "   void help_role_tprotocol_forward_pb(const PB* apb)" << std::endl;
    m_stream << "   {" << std::endl;
    m_stream << "       int32_t lprotocolnum = tprotocol::protocol<PB>();" << std::endl;
    m_stream << "       tprotocol::tp_forward::template func<" << std::endl;
    m_stream << "           ngl::np_actor_forward<PB, EPROTOCOL_TYPE_PROTOCOLBUFF, true, ngl::forward>" << std::endl;
    m_stream << "           , ngl::np_actor_forward<PB, EPROTOCOL_TYPE_PROTOCOLBUFF, false, ngl::forward>" << std::endl;
    m_stream << "           , ngl::np_actor_forward<PB, EPROTOCOL_TYPE_PROTOCOLBUFF, true, PB>" << std::endl;
    m_stream << "           , ngl::np_actor_forward<PB, EPROTOCOL_TYPE_PROTOCOLBUFF, false, PB>" << std::endl;
    m_stream << "           , np_actormodule_forward<PB>" << std::endl;
    m_stream << "       >(EPROTOCOL_TYPE_PROTOCOLBUFF, lprotocolnum);" << std::endl;
    m_stream << "   }" << std::endl;
    m_stream << "   template <typename PB, typename ...ARG>" << std::endl;
    m_stream << "   void help_role_tprotocol_forward_pb(const PB* apb, const ARG*... arg)" << std::endl;
    m_stream << "   {" << std::endl;
    m_stream << "       help_role_tprotocol_forward_pb<PB>(apb);" << std::endl;
    m_stream << "       help_role_tprotocol_forward_pb<ARG...>(arg...);" << std::endl;
    m_stream << "   }" << std::endl;
    m_stream << "   void tprotocol_forward_pb()" << std::endl;
    m_stream << "   {" << std::endl;
    m_stream << "        help_role_tprotocol_forward_pb(" << std::endl;
    std::string ltemp = astr;
    if (!ltemp.empty())
    {
        if (ltemp[ltemp.size() - 2] == ',')
        {
            char* lbuff = new char[ltemp.size()];
            memset(lbuff, 0x0, ltemp.size());
            memcpy(lbuff, &ltemp[0], ltemp.size() - 2);
            ltemp = lbuff;
            delete[]lbuff;
        }
    }
    m_stream << ltemp << std::endl;
    m_stream << "        );" << std::endl;
    m_stream << "   }" << std::endl;
    m_stream << "}//namespace ngl" << std::endl;

    lfile.write(m_stream.str());
}

std::string foreachProtobuf(google::protobuf::compiler::DiskSourceTree& sourceTree, int32_t& aprotocol, const char* aname)
{
    google::protobuf::compiler::Importer importer(&sourceTree, nullptr);
    const google::protobuf::FileDescriptor* fileDescriptor = importer.Import(std::string(aname) + ".proto");
    if (fileDescriptor == nullptr) {
        std::cerr << "Failed to import protobuf file descriptor" << std::endl;
        return "";
    }
    foreachProtobufMessages(fileDescriptor, aprotocol, aname);
    return foreachProtobufMessages2(fileDescriptor, aprotocol, aname);
}

void traverseProtobufMessages(const char* apackname, const char* aname, const google::protobuf::FileDescriptor* fileDescriptor) {

   
    int messageCount = fileDescriptor->message_type_count();
    for (int i = 0; i < messageCount; ++i) 
    {
        const google::protobuf::Descriptor* messageDescriptor = fileDescriptor->message_type(i);

        std::cout << "name: " << messageDescriptor->name() << std::endl;
        std::string lmessage;
        ngl::tools::replace(std::string(apackname) + ".", "", messageDescriptor->name(), lmessage);

        std::stringstream lstream_parm1;
		std::stringstream lstream_parm2;
        std::stringstream lstream_parm3;
        std::stringstream lstream_parm4;

        using Label = google::protobuf::FieldDescriptor::Label;

        struct tmessage
        {
            std::string m_type;
            std::string m_type2;
            bool m_is_type2_base;
            std::string m_name;
        };

        std::map<Label, std::pair<std::list<tmessage>, std::list<tmessage>>> lmap;
        bool isprotocol = false;
        for (int j = 0; j < messageDescriptor->field_count(); ++j)
        {
            const google::protobuf::FieldDescriptor* fieldDescriptor = messageDescriptor->field(j);
            const google::protobuf::FieldDescriptor* fieldDescriptor_key = messageDescriptor->map_key();
            const google::protobuf::FieldDescriptor* fieldDescriptor_value = messageDescriptor->map_value();

            std::pair<std::list<tmessage>, std::list<tmessage>>& lpair = lmap[fieldDescriptor->label()];
            std::list<tmessage>* ls = nullptr;
            std::string lparmname;
            std::string lparmname2;
            bool lis_type2_base = false;
            std::cout << fieldDescriptor->type_name() << std::endl;
            if (g_typearr.find(fieldDescriptor->type_name()) != g_typearr.end())
            {
                ls = &lpair.first;
                lparmname = g_typearr[fieldDescriptor->type_name()];
            }
            else
            {
                std::cout << fieldDescriptor->camelcase_name() << std::endl;
                // �ж��ֶ��Ƿ�Ϊmap����
                if (fieldDescriptor->is_map()) {
                    // MAX_LABEL === maps
                    std::pair<std::list<tmessage>, std::list<tmessage>>& lpair = lmap[(Label)(Label::MAX_LABEL+1)];
                    ls = &lpair.second;
                    // ���map��key���ƺ�value����
                    std::cout << "Key Type: " << fieldDescriptor->message_type()->field(0)->type_name() << std::endl;

                    lparmname = fieldDescriptor->message_type()->field(0)->type_name();
                    lparmname = g_typearr[lparmname];
                    
                    if (fieldDescriptor->message_type()->field(1)->type_name() == std::string("message"))
                    {
                        std::cout << "Value Type: " << type_name(apackname, fieldDescriptor->message_type()->field(1)) << std::endl;
                        lparmname2 = type_name(apackname, fieldDescriptor->message_type()->field(1));
                        lis_type2_base = false;
                    }
                    else
                    {
                        std::cout << "Value Type: " << fieldDescriptor->message_type()->field(1)->type_name() << std::endl;
                        lparmname2 = fieldDescriptor->message_type()->field(1)->type_name();
                        lparmname2 = g_typearr[lparmname2];
                        lis_type2_base = true;
                    }
                    ngl::tools::replace("Template.", "", lparmname2, lparmname2);
                    ngl::tools::replace("Pb.", "", lparmname2, lparmname2);
                }
                else
                {
                    ls = &lpair.second;
                    lparmname = type_name(apackname, fieldDescriptor);
                    ngl::tools::replace("Template.", "", lparmname, lparmname);
                    ngl::tools::replace("Pb.", "", lparmname, lparmname);
                }
            }

            ls->push_back(tmessage
                {
                    .m_type = lparmname,
                    .m_type2 = lparmname2,
                    .m_is_type2_base = lis_type2_base,
                    .m_name = fieldDescriptor->name(),
                });
        }

        if (lmessage.find("db_") != std::string::npos)
      {
          g_stream_sql
              << "-- ----------------------------\r\n"
              << "-- Table structure for " << lmessage << "\r\n"
              << "-- ----------------------------\r\n"
              << "DROP TABLE IF EXISTS `" << lmessage << "`;\r\n"
              << "CREATE TABLE `" << lmessage << "` (\r\n"
              << "    `id` bigint(63) NOT NULL,\r\n"
              << "    `area` int(11) NOT NULL,\r\n"
              << "    `data` LongBlob,\r\n"
              << "    PRIMARY KEY (`id`)\r\n"
              << ")ENGINE = InnoDB DEFAULT CHARSET = utf8; \r\n";
              
      }
    }
}

void traverseProtobuf(google::protobuf::compiler::DiskSourceTree& sourceTree, const char* apackname, const char* aname)
{
    google::protobuf::compiler::Importer importer(&sourceTree, nullptr);
    const google::protobuf::FileDescriptor* fileDescriptor = importer.Import(std::string(aname) + ".proto");
    if (fileDescriptor == nullptr) {
        std::cerr << "Failed to import protobuf file descriptor" << std::endl;
        return;
    }

    traverseProtobufMessages(apackname, aname, fileDescriptor);
}

int main(int argc, char** argv) 
{
    //if (argc > 1)
    //{
    //    if (std::string(argv[1]) == "push_config")
    //    {
    //        ngl::allcsv::load();
    //        ngl::manage_csv<ngl::tab_servers>* mang = ngl::allcsv::get<ngl::manage_csv<ngl::tab_servers>>();
    //        mang->foreach([](ngl::tab_servers& tab)
    //            {
    //               auto lhttp = ngl::manage_curl::make_http();
    //                ngl::manage_curl::set_mode(lhttp, ngl::ENUM_MODE_HTTP);
    //                ngl::manage_curl::set_type(lhttp, ngl::ENUM_TYPE_GET);
    //                ngl::manage_curl::set_url(lhttp, "http://127.0.0.1:800/push_server_config.php");

    //                ngl::net_works const* lpstruct = ngl::ttab_servers::nworks(ngl::ENET_PROTOCOL::ENET_TCP, &tab);
    //                if (lpstruct == nullptr)
    //                    return;

    //                std::stringstream lstream;
    //                //xx=xx&xx=xx&xx=xx
    //                lstream
    //                    << "id=" << tab.m_id << "&"
    //                    << "area=" << tab.m_area << "&"
    //                    << "name=" << tab.m_name << "&"
    //                    << "ip=" << lpstruct->m_ip << "&"
    //                    << "nip=" << lpstruct->m_nip << "&"
    //                    << "port=" << lpstruct->m_port << "&"
    //                    << "type=" << tab.m_type;

    //                ngl::manage_curl::set_param(lhttp, lstream.str());
    //                ngl::manage_curl::getInstance().send(lhttp);
    //            });
    //        while (1)
    //        {
    //            ngl::sleep::seconds(1);
    //        }
    //    }
    //}

    google::protobuf::compiler::DiskSourceTree sourceTree;
    sourceTree.MapPath("", argv[1]);
   
    g_stream_sql << "/*Date:" << ngl::localtime::time2str("%Y-%m-%d %H:%M:%S")<< "*/" << std::endl;
    g_stream_sql << std::endl;
    g_stream_sql << " DROP Database IF EXISTS `lbtest`;" << std::endl;
    g_stream_sql << " CREATE DATABASE lbtest default charset utf8 COLLATE utf8_general_ci;" << std::endl;
    g_stream_sql << " use lbtest;" << std::endl;
    g_stream_sql << std::endl;

    traverseProtobuf(sourceTree, "Template", "db");
  
    // ���ɶ�Ӧ��sql�ļ�
    ngl::writefile lsql("create_db.sql");
    lsql.write(g_stream_sql.str());
    // �Զ������ṹ��Ϊ���ṩЭ���    
    ngl::xmlprotocol::load();
    int32_t lnumber = 0;
    std::string ltempsss = foreachProtobuf(sourceTree, lnumber, "net");
    ltempsss += foreachProtobuf(sourceTree, lnumber, "example");
    foreachProtobufMessages3(ltempsss);
    return 0;
}