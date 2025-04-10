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
        // 获取嵌套消息类型的名称
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

void foreachProtobufEnum(const google::protobuf::FileDescriptor* fileDescriptor, const std::string& axml)
{
    std::string ldata;
    {

        // 读取文件nactor_auto.h
        ngl::readfile lfile("../../public/cpp/actor/auto_edit/nactor_auto.h");
        lfile.read(ldata);
    }
    // 正则表达式：匹配 "using <name> = typedb<...>;"
    std::regex pattern(R"(using\s+([a-zA-Z0-9_]+)\s*=\s*typedb<([^>]+)>;)");

    // 用于存储匹配到的结果
    std::smatch matches;

    // 迭代文本并查找所有匹配项
    auto words_begin = std::sregex_iterator(ldata.begin(), ldata.end(), pattern);
    auto words_end = std::sregex_iterator();

    struct struttt
    {
        std::string name;
        std::string enumname;
        std::string proname;
        std::string actorname;
    };
    std::map<std::string,struttt> lmap;
    std::set<std::string> lset;
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) 
    {
        // 获取完整的匹配项（例如：using tdb_account = typedb<pbdb::ENUM_DB_ACCOUNT, pbdb::db_account, actor_login>;）
        std::string full_match = (*i).str(0);
        
        struttt ltemp;
        // 获取第一个括号里的部分（tdb_account, user_data, customer_data等）
        ltemp.name = (*i).str(1);

        // 获取第二部分，即 typedb<...> 内的内容
        std::string arguments = (*i).str(2);
        ngl::tools::splite(arguments.c_str(), ",", ltemp.enumname, ltemp.proname, ltemp.actorname);
        ngl::tools::splite(ltemp.enumname.c_str(), "::", ltemp.enumname, ltemp.enumname);
        ngl::tools::splite(ltemp.proname.c_str(), "::", ltemp.proname, ltemp.proname);
        lmap[ltemp.enumname] = ltemp;
        lset.insert(ltemp.actorname);
    }



    int messageCount = fileDescriptor->enum_type_count();
    for (int i = 0; i < messageCount; ++i)
    {
        const auto& enumDescriptor = fileDescriptor->enum_type(i);
        std::cout << "name: " << enumDescriptor->name() << std::endl;
        if (enumDescriptor->name() == "ENUM_DB")
        {
            int enum_value_count = enumDescriptor->value_count();
            std::cout << "Enum: " << enumDescriptor->name() << std::endl;

            // 遍历枚举值
            for (int j = 0; j < enum_value_count; ++j) 
            {
                const auto& enumValueDescriptor = enumDescriptor->value(j);
                std::cout << "Enum Name: " << enumValueDescriptor->name() << ", Enum Value: "
                    << enumValueDescriptor->number() << std::endl;
                if (lmap.contains(enumValueDescriptor->name()) == false)
                {
                    if (enumValueDescriptor->name() == "ENUM_DB_COUNT")
                    {
                        continue;
                    }
                    struttt ltemp;
                    
                    ltemp.enumname = enumValueDescriptor->name();
                    std::string ltempss1;
                    std::string ltempss2;
                    std::string ltempss3;
                    ngl::tools::splite(ltemp.enumname.c_str(), "_", ltempss1, ltempss2, ltempss3);
                    std::transform(ltempss3.begin(), ltempss3.end(), ltempss3.begin(), [](unsigned char c) { return std::tolower(c); });
                    std::transform(ltempss2.begin(), ltempss2.end(), ltempss2.begin(), [](unsigned char c) { return std::tolower(c); });

                    ltemp.actorname = "actor_xxxx";
                    ltemp.proname = std::format("{}_{}", ltempss2, ltempss3);
                    ltemp.name = std::format("t{}", ltemp.proname);
                    lmap[enumValueDescriptor->name()] = ltemp;
                    lset.insert("actor_xxxx");
                }
            }
        }
    }

    ngl::writefile lfile("../../public/cpp/actor/auto_edit/nactor_auto.h");

    std::stringstream m_stream;
    m_stream << R"(#pragma once

#include "ndb_modular.h"
#include "nsp_client.h"
#include "nsp_server.h"
#include "actor_db.h"
#include "db.pb.h"

namespace ngl
{
	template <pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB, typename TACTOR>
	class typedb
	{
		typedb() = delete;
		typedb(const typedb&) = delete;
		typedb& operator=(const typedb&) = delete;
	public:
		using db_actor   = ngl::actor_db<TDBTAB_TYPE, TDBTAB>;
		using db_modular = ndb_modular<TDBTAB_TYPE, TDBTAB, TACTOR>;
		// 订阅/发布[数据副本]
		using nsp_ser	 = nsp_server<TDBTAB_TYPE, TACTOR, TDBTAB>;
		template <typename TDerived>
		using nsp_cli	 = nsp_client<TDerived, TDBTAB>;
		// [aregister == true] 主要是注册协议,宏与类型的绑定
		// [aregister == false] 实例化db_actor,db server需要
		static void init(bool aregister);
	};

	template <pbdb::ENUM_DB ENUM, typename TDATA, typename TACTOR>
	void ndb_modular<ENUM, TDATA, TACTOR>::init_data()
	{
		initdata();
		typedb<ENUM, TDATA, TACTOR>::nsp_ser::loadfish_sync();
	}
)";

    m_stream << std::endl;
    m_stream << std::endl;
    for (const std::string& item : lset)
    {
        if (item == "actor_xxxx")
        {// 让编译出错 玩家确认修改
            m_stream << "\t#编译会出错，需要确认修改"<< std::endl;
        }
        m_stream << "\tclass " << item << ";" << std::endl;
    }
    m_stream << std::endl;
    m_stream << std::endl;
    int lcount = 1;
    do
    {
        for (const auto& item : lmap)
        {
            if (item.second.enumname == "ENUM_DB_COUNT")
            {
                continue;
            }
            

            std::string enumname = std::string("pbdb::") + item.second.enumname;
            ngl::tools::replace(" ", "", enumname, enumname);
            std::string proname = std::string("pbdb::") + item.second.proname;
            ngl::tools::replace(" ", "", proname, proname);
            std::string name = item.second.name;
            ngl::tools::replace(" ", "", name, name);
            std::string actorname = item.second.actorname;
            ngl::tools::replace(" ", "", actorname, actorname);
            if (lcount == 1 && proname.find("dbcross") != std::string::npos)
            {
                if (item.second.actorname == "actor_xxxx")
                {// 让编译出错 玩家确认修改
                    m_stream << "\t#编译会出错，需要确认修改" << std::endl;
                }
                m_stream << std::format("\tusing {} = typedb<{}, {}, {}>;", name, enumname, proname, actorname) << std::endl;
                continue;
            }
            if (lcount == 2 && proname.find("dbcross") == std::string::npos)
            {
                if (item.second.actorname == "actor_xxxx")
                {// 让编译出错 玩家确认修改
                    m_stream << "\t#编译会出错，需要确认修改" << std::endl;
                }
                m_stream << std::format("\tusing {} = typedb<{}, {}, {}>;", name, enumname, proname, actorname) << std::endl;
                continue;
            }
        }
        m_stream << std::endl;
    }while(++lcount <= 2);

    


    m_stream << R"(
	class tdb
	{
	public:
		static void tdb_init(bool ainstance);

		// # 定义在nactor_auto.cpp中,因为跨服是可选的
		static void tcrossdb_init(bool ainstance);
	};
}//namespace ngl)";
    lfile.write(m_stream.str());

    
    std::stringstream m_stream2;

    m_stream2 << " // 注意【makeproto 工具生成文件，不要手动修改】" << std::endl;
    m_stream2 << " // 创建时间【" << ngl::localtime::time2str() << "】" << std::endl;
    m_stream2 << R"(#include "auto_actor_enum.h"
#include "nprotocol_auto.h"
#include "nactor_auto.h"

namespace ngl
{
	void tdb::tdb_init(bool ainstance)
	{
)";

    for (const auto& item : lmap)
    {
        if (item.second.enumname == "ENUM_DB_COUNT")
        {
            continue;
        }

        std::string enumname = std::string("pbdb::") + item.second.enumname;
        ngl::tools::replace(" ", "", enumname, enumname);
        std::string proname = std::string("pbdb::") + item.second.proname;
        ngl::tools::replace(" ", "", proname, proname);
        std::string name = item.second.name;
        ngl::tools::replace(" ", "", name, name);
        std::string actorname = item.second.actorname;
        ngl::tools::replace(" ", "", actorname, actorname);
        if (proname.find("dbcross") == std::string::npos)
        {
            m_stream2 << std::format("\t\t{}::init(ainstance);", name) << std::endl;
        }
    }
    m_stream2 << R"(    }
}//namespace ngl)";
    ngl::writefile lfile2("../../public/cpp/actor/auto_edit/nactor_auto_tdb.cpp");
    lfile2.write(m_stream2.str());
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
    m_stream << " // 注意【makeproto 工具生成文件，不要手动修改】" << std::endl;
    m_stream << " // 创建时间【" << ngl::localtime::time2str() << "】" << std::endl;
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

std::string foreachProtobufMessages_json2pbbinary1(const google::protobuf::FileDescriptor* fileDescriptor, int32_t& aprotocol, const std::string& axml)
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
    std::stringstream m_stream;
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
            m_stream <<std::format("            mkdef_map({}::{});", lnamespace, messageDescriptor->name()) << std::endl;
        }
    }
    return m_stream.str();
}

void foreachProtobufMessages_json2pbbinary2(const std::string& astr)
{
    ngl::writefile lfile("./pbname_match.cpp");

    std::stringstream m_stream;
    m_stream << " // 注意【makeproto 工具生成文件，不要手动修改】" << std::endl;
    m_stream << " // 创建时间【" << ngl::localtime::time2str() << "】" << std::endl;
    m_stream << R"(#include "actor_base.h"
#include "tools.h"
#include "pack.h"

#include "example.pb.h"
#include "net.pb.h"

namespace ngl
{
	template <typename T>
	static bool jsontobinarypack(const char* aname, const char* ajson, std::shared_ptr<pack>& apack, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		T ltemp;
		if (tools::jsontopro(ajson, ltemp))
		{
			apack = actor_base::net_pack(ltemp, aactorid, arequestactorid);
			return true;
		}
        log_error()->print("jsontobinarypack fail pbname[{}] json[{}]", aname, ajson);
		return false;
	}

	template <typename T>
	class j2p
	{
	public:
		static void fun(const std::string& aname, const std::string& ajson, std::shared_ptr<ngl::pack>& apack, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			ngl::jsontobinarypack<T>(aname.c_str(), ajson.c_str(), apack, aactorid, arequestactorid);
		}
	};

	
	std::shared_ptr<pack> actor_base::jsonpack(const std::string& apbname, const std::string& ajson, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		using type_j2pfun = std::function<void(const std::string&, const std::string&, std::shared_ptr<ngl::pack>&, i64_actorid, i64_actorid)>;
		static std::map<std::string, type_j2pfun> lmap;
		if (lmap.empty())
		{
            #define mkdef_map(NAME) lmap[#NAME] = std::bind_front(&j2p<NAME>::fun))" << std::endl;


    m_stream << astr;

    m_stream << R"(         }
		auto lpfun = tools::findmap(lmap, apbname);
		if (lpfun != nullptr)
		{
			auto lpack = std::make_shared<ngl::pack>();
			(*lpfun)(apbname, ajson, lpack, aactorid, arequestactorid);
			return lpack;
		}
		return nullptr;
	}
}//namespace ngl)" << std::endl;
    lfile.write(m_stream.str());
}


std::pair<std::string, std::string> foreachProtobuf(google::protobuf::compiler::DiskSourceTree& sourceTree, int32_t& aprotocol, const char* aname)
{
    google::protobuf::compiler::Importer importer(&sourceTree, nullptr);
    const google::protobuf::FileDescriptor* fileDescriptor = importer.Import(std::string(aname) + ".proto");
    if (fileDescriptor == nullptr) {
        std::cerr << "Failed to import protobuf file descriptor" << std::endl;
        return std::pair<std::string, std::string>();
    }
    foreachProtobufMessages(fileDescriptor, aprotocol, aname);
    std::pair<std::string, std::string> lpair(foreachProtobufMessages2(fileDescriptor, aprotocol, aname), foreachProtobufMessages_json2pbbinary1(fileDescriptor, aprotocol, aname));
    return lpair;
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
                // 判断字段是否为map类型
                if (fieldDescriptor->is_map()) {
                    // MAX_LABEL === maps
                    std::pair<std::list<tmessage>, std::list<tmessage>>& lpair = lmap[(Label)(Label::MAX_LABEL+1)];
                    ls = &lpair.second;
                    // 输出map的key名称和value名称
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


void traverseProtobufEnum(google::protobuf::compiler::DiskSourceTree& sourceTree, const char* aname)
{
    google::protobuf::compiler::Importer importer(&sourceTree, nullptr);
    const google::protobuf::FileDescriptor* fileDescriptor = importer.Import(std::string(aname) + ".proto");
    if (fileDescriptor == nullptr) {
        std::cerr << "Failed to import protobuf file descriptor" << std::endl;
    }
    foreachProtobufEnum(fileDescriptor, aname);
}

int main(int argc, char** argv) 
{


    google::protobuf::compiler::DiskSourceTree sourceTree;
    sourceTree.MapPath("", argv[1]);
    traverseProtobufEnum(sourceTree, "db");
   
    g_stream_sql << "/*Date:" << ngl::localtime::time2str("%Y-%m-%d %H:%M:%S")<< "*/" << std::endl;
    g_stream_sql << std::endl;
    g_stream_sql << " DROP Database IF EXISTS `lbtest`;" << std::endl;
    g_stream_sql << " CREATE DATABASE lbtest default charset utf8 COLLATE utf8_general_ci;" << std::endl;
    g_stream_sql << " use lbtest;" << std::endl;
    g_stream_sql << std::endl;

    traverseProtobuf(sourceTree, "Template", "db");
  
    // 生成对应的sql文件
    ngl::writefile lsql("create_db.sql");
    lsql.write(g_stream_sql.str());
    // 自动关联结构体为其提供协议号    
    ngl::xmlprotocol::load();
    int32_t lnumber = 0;
    std::pair<std::string, std::string> lpair;
    std::pair<std::string, std::string> lpair2 = foreachProtobuf(sourceTree, lnumber, "net");
    lpair.first += lpair2.first;
    lpair.second += lpair2.second;
    lpair2 = foreachProtobuf(sourceTree, lnumber, "example");
    lpair.first += lpair2.first;
    lpair.second += lpair2.second;
    foreachProtobufMessages3(lpair.first);
    foreachProtobufMessages_json2pbbinary2(lpair.second);
    return 0;
}