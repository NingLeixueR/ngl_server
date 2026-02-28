#pragma once

#include "tools/tab/xml/xml_serialize.h"
#include "tools/operator_file.h"
#include "tools/tab/xml/xml_serialize.h"
#include "tools/tab/xml/xmlprotocol.h"
#include "tools/localtime.h"
#include "actor/pb/net.pb.h"
#include "tools/tools.h"
#include "tools/tab/xml/xml.h"

#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/descriptor.h>

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

class xml_protocol
{
    static std::map<std::string, std::string> g_typearr;

    static std::string type_name(const char* apackname, const google::protobuf::FieldDescriptor* fieldDescriptor)
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

public:
    static std::stringstream g_stream_sql;
    static std::stringstream g_stream_pgsql;
    static std::stringstream g_stream_xml;

    static std::string get_cpp_type_name(const google::protobuf::FieldDescriptor* field) {
        switch (field->cpp_type()) {
        case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
            return "int32_t";
        case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
            return "int64_t";
        case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
            return "uint32_t";
        case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
            return "uint64_t";
        case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
            return "float";
        case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
            return "double";
        case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
            return "bool";
        case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
            return "std::string";
        case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
            return field->message_type()->full_name(); // 嵌套消息类型名
        case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
            return field->enum_type()->full_name(); // 枚举类型名
        default:
            return "unknown";
        }
    }

    static std::map<std::string, std::string> m_nscriptmap;
    static std::set<std::string> m_nscriptset;
    // 递归打印字段类型信息
    std::string FieldType(const google::protobuf::FieldDescriptor* field) 
    {
        return google::protobuf::FieldDescriptor::TypeName(field->type());
    }

    static void nscript_func(const google::protobuf::Descriptor* messageDescriptor, std::string& lnamespace, const std::string& astruct)
    {
        std::stringstream m_stream;
        std::string lmesname;
        
        lmesname = messageDescriptor->full_name();
        ngl::tools::replace("\\.", "::", lmesname, lmesname);

        std::string lfstr1 = "M";
        std::string lfstr2 = "Entry";
        bool lbool = lmesname.find(lfstr1) != std::string::npos;
        lbool = lbool && lmesname.find(lfstr2) != std::string::npos;
        if (m_nscriptset.contains(lmesname))
        {
            return;
        }
        if (lbool)
        {
            return;
        }
        m_nscriptmap[lmesname] = "";
        m_nscriptset.insert(lmesname);

        m_stream << "   template <>" << std::endl;
        m_stream << "   struct serialize_lua<" << lmesname << ">" << std::endl;
        m_stream << "   {" << std::endl;
        m_stream << "       static void stack_push(lua_State* L, const " << lmesname << "& adata, const char* aname = nullptr)" << std::endl;
        m_stream << "       {" << std::endl;
        if (messageDescriptor->field_count() > 0)
        {
            m_stream << "            ngl::nlua_table::table_push(L, aname, {";
            std::cout << "name: " << messageDescriptor->name() << std::endl;
            for (int i = 0; i < messageDescriptor->field_count(); ++i)
            {
                const google::protobuf::FieldDescriptor* field = messageDescriptor->field(i);
                if (i != 0)
                {
                    m_stream << ",";
                }               
                m_stream << std::format("\"{}\"", field->name());
            }
            m_stream << "} , ";
            for (int i = 0; i < messageDescriptor->field_count(); ++i)
            {
                const google::protobuf::FieldDescriptor* field = messageDescriptor->field(i);
                if (i != 0)
                {
                    m_stream << ",";
                }
                m_stream << "adata." << field->name() << "()";
            }
            m_stream << std::endl;
            m_stream << "               );" << std::endl;
        }

        m_stream << "       }" << std::endl;

        m_stream << "       static bool stack_pop(lua_State* L, " << lmesname << "& adata, bool apop = true, const char* aname = nullptr)" << std::endl;
        m_stream << "       {" << std::endl;
        if (messageDescriptor->field_count() > 0)
        {
            m_stream << R"(         if (lua_isnil(L, -1))
            {
            return true;
            })";
            m_stream << std::endl;
            for (int i = 0; i < messageDescriptor->field_count(); ++i)
            {
                const google::protobuf::FieldDescriptor* field = messageDescriptor->field(i);
                if (field->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE ||
                    field->type() == google::protobuf::FieldDescriptor::TYPE_GROUP ||
                    field->is_repeated()
                    )
                {

                }
                else
                {
                    std::string ltypename = get_cpp_type_name(field);
                    ngl::tools::replace("\\.", "::", ltypename, ltypename);
                    m_stream << "           " << ltypename << " l" << field->name() << ";" << std::endl;
                }
            }
            m_stream << "           if(!ngl::nlua_table::table_pop(L, aname, { ";
            for (int i = 0; i < messageDescriptor->field_count(); ++i)
            {
                const google::protobuf::FieldDescriptor* field = messageDescriptor->field(i);
                if (i != 0)
                {
                    m_stream << ", ";
                }
                m_stream << std::format("\"{}\"", field->name());
            }
            m_stream << "}, ";
            for (int i = 0; i < messageDescriptor->field_count(); ++i)
            {
                const google::protobuf::FieldDescriptor* field = messageDescriptor->field(i);
                if (i != 0)
                {
                    m_stream << ", ";
                }
                if (field->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE ||
                    field->type() == google::protobuf::FieldDescriptor::TYPE_GROUP ||
                    field->is_repeated()
                    )
                {
                    m_stream << " *adata.mutable_" << field->name() << "()";
                }
                else
                {
                    m_stream << "l" << field->name();
                }
            }
            m_stream << "))" << std::endl;
            m_stream << "           {" << std::endl;
            m_stream << "               return false;" << std::endl;
            m_stream << "           }" << std::endl;
            for (int i = 0; i < messageDescriptor->field_count(); ++i)
            {
                const google::protobuf::FieldDescriptor* field = messageDescriptor->field(i);
                if (field->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE ||
                    field->type() == google::protobuf::FieldDescriptor::TYPE_GROUP 
                    )
                {
                    if (field->is_map()) 
                    {
                        const google::protobuf::Descriptor* map_desc = field->message_type();
                        const google::protobuf::FieldDescriptor* value_field = map_desc->field(1);
                        if (value_field && value_field->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
                        {
                            nscript_func(value_field->message_type(), lnamespace, messageDescriptor->name());
                        }
                       
                    }
                    else
                    {
                        nscript_func(field->message_type(), lnamespace, messageDescriptor->name());
                    }
                }
                else if(field->is_repeated())
                {

                }
                else if (field->type() == google::protobuf::FieldDescriptor::TYPE_ENUM)
                {
                    m_stream << "           adata.set_" << field->name() << "(l" << field->name() << ");" << std::endl;
                }
                else
                {
                    m_stream << "           adata.set_" << field->name() << "(l" << field->name() << ");" << std::endl;
                }
            }
        }

        m_stream << "           if (apop)" << std::endl;
        m_stream << "           {" << std::endl;
        m_stream << "               lua_pop(L, 1);" << std::endl;
        m_stream << "           }" << std::endl;

        m_stream << "           return true;" << std::endl;
        m_stream << "       }" << std::endl;

        m_stream << "       static void table_push(lua_State * L, const char* aname, const " << lmesname << "& adata)" << std::endl;
        m_stream << "       {" << std::endl;
        m_stream << "           stack_push(L, adata, aname);" << std::endl;
        m_stream << "       }" << std::endl;
        m_stream << "       static bool table_pop(lua_State * L, const char* aname, " << lmesname << "& adata)" << std::endl;
        m_stream << "       {" << std::endl;
        m_stream << "           return stack_pop(L, adata, false, aname);" << std::endl;
        m_stream << "       }" << std::endl;


        m_stream << "   };" << std::endl;

        m_nscriptmap[lmesname] = m_stream.str();

    }

    static void nscript_auto(const google::protobuf::FileDescriptor* fileDescriptor, const std::string& axml)
    {
        m_nscriptmap.clear();
        std::string lnamespace;
        if (axml == "net")
        {
            lnamespace = "pbnet";
        }
        else if (axml == "example")
        {
            lnamespace = "pbexample";
        }
        else if (axml == "db")
        {
            lnamespace = "pbdb";
        }

        ngl::writefile lfile(std::format("./nscript_{}.h", lnamespace).c_str());
        std::stringstream m_stream;
        m_stream << " // 注意【makeproto 工具生成文件，不要手动修改】" << std::endl;
        m_stream << " // 创建时间【" << ngl::localtime::time2str() << "】" << std::endl;
        m_stream << std::endl;
        m_stream << "#pragma once" << std::endl;
        m_stream << "#include \"tools/serialize/ndefine.h\"" << std::endl;
        m_stream << "#include \"actor/pb/example.pb.h\"" << std::endl;
        m_stream << "#include \"actor/pb/net.pb.h\"" << std::endl;
        m_stream << "#include \"actor/pb/db.pb.h\"" << std::endl;
        m_stream << std::endl;
        m_stream << "#include <array>" << std::endl;
        m_stream << std::endl;

        m_stream << "namespace ngl" << std::endl;
        m_stream << "{" << std::endl;
        int messageCount = fileDescriptor->message_type_count();
        for (int i = 0; i < messageCount; ++i)
        {
            const google::protobuf::Descriptor* messageDescriptor = fileDescriptor->message_type(i);
            nscript_func(messageDescriptor, lnamespace, "");
        }
        for (const auto& item : m_nscriptmap)
        {
            m_stream << item.second;
        }

        m_stream << "}//namespace ngl" << std::endl;
        lfile.write(m_stream.str());
    }

    static void nscript_foreachProtobuf(google::protobuf::compiler::DiskSourceTree& sourceTree, const char* aname)
    {
        google::protobuf::compiler::Importer importer(&sourceTree, nullptr);
        const google::protobuf::FileDescriptor* fileDescriptor = importer.Import(std::string(aname) + ".proto");
        if (fileDescriptor == nullptr) {
            std::cerr << "Failed to import protobuf file descriptor" << std::endl;
            return;
        }
        nscript_auto(fileDescriptor, aname);
        return;
    }

    static void xml_fun(const google::protobuf::FileDescriptor* fileDescriptor, int32_t& aprotocol, const std::string& axml)
    {
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

        ngl::xarg_protocols lxarg;

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
                lxarg.m_config.push_back(
                    ngl::xarg_protocols::info
                    {
                        .m_client = 1,
                        .m_name = std::format("{}::{}", lnamespace, messageDescriptor->name()),
                        .m_number = (lprotocol == -1 ? ngl::xmlprotocol::free_protocol() : lprotocol)
                    }
                );
            }

        }
        lxarg.xml_push(std::string("./config/" + axml + "_protocol.xml").c_str());
    }
    
    static std::string register_fun(const google::protobuf::FileDescriptor* fileDescriptor, int32_t& aprotocol, const std::string& axml)
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
                m_stream_pb << std::format("            {}::{},", lnamespace, messageDescriptor->name()) << std::endl;
            }
        }
        return m_stream_pb.str();
    }

    static std::string json1_fun(const google::protobuf::FileDescriptor* fileDescriptor, int32_t& aprotocol, const std::string& axml)
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
        static bool lfirst = true;
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
                m_stream << "               ";
                if (!lfirst)
                {
                    m_stream << ", ";
                }
                lfirst = false;
                m_stream << std::format("{}::{}", lnamespace, messageDescriptor->name()) << std::endl;
            }
        }
        return m_stream.str();
    }

    static void json2_fun(const std::string& astr)
    {
        ngl::writefile lfile("./pbname_match.cpp");

        std::stringstream m_stream;
        m_stream << " // 注意【makeproto 工具生成文件，不要手动修改】" << std::endl;
        m_stream << " // 创建时间【" << ngl::localtime::time2str() << "】" << std::endl;
        m_stream << R"(
#include "actor/actor_base/actor_base.h"
#include "tools/serialize/pack.h"
#include "actor/pb/example.pb.h"
#include "actor/pb/net.pb.h"
#include "tools/tools.h"

namespace ngl
{
	template <typename T>
	static bool jsontobinarypack(const char* aname, const char* ajson, std::shared_ptr<pack>& apack, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		T ltemp;
		if (tools::json2proto(ajson, ltemp))
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
            auto fun = [&]<typename ...TARG>()
            {
                ((lmap[tools::type_name<TARG>()] = std::bind_front(&j2p<TARG>::fun)), ...);
            };
            fun.operator()<)" << std::endl;


        m_stream << astr;
        m_stream << "            >();" << std::endl;

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

    static std::pair<std::string, std::string> foreachProtobuf(google::protobuf::compiler::DiskSourceTree& sourceTree, int32_t& aprotocol, const char* aname)
    {
        google::protobuf::compiler::Importer importer(&sourceTree, nullptr);
        const google::protobuf::FileDescriptor* fileDescriptor = importer.Import(std::string(aname) + ".proto");
        if (fileDescriptor == nullptr) {
            std::cerr << "Failed to import protobuf file descriptor" << std::endl;
            return std::pair<std::string, std::string>();
        }
        xml_fun(fileDescriptor, aprotocol, aname);
        std::pair<std::string, std::string> lpair(register_fun(fileDescriptor, aprotocol, aname), json1_fun(fileDescriptor, aprotocol, aname));
        return lpair;
    }

    static void pb_protocol(const std::string& astr)
    {
        ngl::writefile lfile("./pb_protocol.cpp");

        std::stringstream m_stream;
        m_stream << " // 注意【makeproto 工具生成文件，不要手动修改】" << std::endl;
        m_stream << " // 创建时间【" << ngl::localtime::time2str() << "】" << std::endl;
        m_stream << "#include \"actor/protocol/nprotocol.h\"" << std::endl;
        m_stream << "#include \"actor/protocol/tprotocol.h\"" << std::endl;
        m_stream << "#include \"actor/actor_base/actor.h\"" << std::endl;
        m_stream << "#include \"actor/pb/example.pb.h\"" << std::endl;
        m_stream << "#include \"actor/pb/net.pb.h\"" << std::endl;
        m_stream << std::endl;
        m_stream << std::endl;
        m_stream << "#include <map>" << std::endl;
        m_stream << std::endl;
        m_stream << "namespace ngl" << std::endl;
        m_stream << "{" << std::endl;
        m_stream << "   template <typename PB>" << std::endl;
        m_stream << "   void hrole_forward_pb()" << std::endl;
        m_stream << "   {" << std::endl;
        m_stream << "       int32_t lprotocolnum = tprotocol::protocol<PB>();" << std::endl;
        m_stream << "       tprotocol::tp_customs<" << std::endl;
        m_stream << "           ngl::np_actor_forward<PB, forward_g2c<PB>>" << std::endl;
        m_stream << "           , ngl::np_actor_forward<PB, forward_c2g<PB>>" << std::endl;
        m_stream << "           , ngl::np_actor_forward<PB, forward_g2c<forward>>" << std::endl;
        m_stream << "           , ngl::np_actor_forward<PB, forward_c2g<forward>>" << std::endl;
        m_stream << "           , np_actormodule_forward<PB>" << std::endl;
        m_stream << "       >(lprotocolnum, 0);" << std::endl;
        m_stream << "   }" << std::endl;
        m_stream << "   template <typename ...ARG>" << std::endl;
        m_stream << "   void help_role_forward_pb()" << std::endl;
        m_stream << "   {" << std::endl;
        m_stream << "      (hrole_forward_pb<ARG>(), ...);" << std::endl;
        m_stream << "   }" << std::endl;
        m_stream << "   void tprotocol_forward_pb()" << std::endl;
        m_stream << "   {" << std::endl;
        m_stream << "        help_role_forward_pb<" << std::endl;
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
        m_stream << "        >();" << std::endl;
        m_stream << "   }" << std::endl;
        m_stream << "}//namespace ngl" << std::endl;

        lfile.write(m_stream.str());
    }

    static void nactor_auto_fun(const google::protobuf::FileDescriptor* fileDescriptor, const std::string& axml)
    {
        std::string ldata;
        {

            // 读取文件nactor_auto.h
            ngl::readfile lfile("../../public/cpp/actor/auto/nactor_auto.h");
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
        std::map<std::string, struttt> lmap;
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
                        if (enumValueDescriptor->name() == "ENUM_DB_COUNT" || enumValueDescriptor->name() == "ENUM_DB_FAIL" || enumValueDescriptor->name() == "ENUM_DB_ALL")
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

        ngl::writefile lfile("../../public/cpp/actor/auto/nactor_auto.h");

        std::stringstream m_stream;
        m_stream << R"(#pragma once

#include "actor/actor_base/nsp/nsp_server_function.h"
#include "actor/actor_base/nsp/nsp_write_function.h"
#include "actor/actor_base/nsp/nsp_read_function.h"
#include "actor/actor_base/ndb_modular.h"
#include "actor/actor_base/actor_db.h"
#include "actor/pb/db.pb.h"

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
		using nsp_ser		= nsp_server<TDBTAB_TYPE, TACTOR, TDBTAB>;
		template <typename TDerived>
		using nsp_cread		= nsp_read<TDerived, TACTOR, TDBTAB>;
		template <typename TDerived>
		using nsp_cwrite	= nsp_write<TDerived, TACTOR, TDBTAB>;

		// [aregister == true] 主要是注册协议,宏与类型的绑定
		// [aregister == false] 实例化db_actor,db server需要
		static void init(bool aregister);
	};

	template <pbdb::ENUM_DB ENUM, typename TDATA, typename TACTOR>
	void ndb_modular<ENUM, TDATA, TACTOR>::init_data()
	{
		initdata();
	}
)";

        m_stream << std::endl;
        m_stream << std::endl;
        for (const std::string& item : lset)
        {
            if (item == "actor_xxxx")
            {// 让编译出错 玩家确认修改
                m_stream << "\t#编译会出错，需要确认修改" << std::endl;
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
                if (item.second.enumname == "ENUM_DB_COUNT" || item.second.enumname == "ENUM_DB_FAIL")
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
        } while (++lcount <= 2);

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
        std::stringstream m_stream3;
        m_stream2 << " // 注意【makeproto 工具生成文件，不要手动修改】" << std::endl;
        m_stream2 << " // 创建时间【" << ngl::localtime::time2str() << "】" << std::endl;
        m_stream2 << R"(#include "actor/auto/auto_actor_enum.h"
#include "actor/auto/nprotocol_auto.h"
#include "actor/auto/nactor_auto.h"

namespace ngl
{
	void tdb::tdb_init(bool ainstance)
	{
)";
        for (const auto& item : lmap)
        {
            if (item.second.enumname == "ENUM_DB_COUNT" || item.second.enumname == "ENUM_DB_FAIL")
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
        ngl::writefile lfile2("../../public/cpp/actor/auto/nactor_auto_tdb.cpp");
        lfile2.write(m_stream2.str());


        m_stream3 << R"(namespace ngl
{
	struct reg_channel_db
	{
		template <typename T>
		static void func()
		{
			tprotocol::tp_customs<
				np_channel_data<T>
				, np_channel_register<T>
				, np_channel_register_reply<T>
				, np_channel_exit<T>
				, np_channel_dataid_sync<T>
			>(-1, 1);

			tprotocol::tp_customs<
				np_channel_check<T>
			>(-1, 0);
		}

		template <typename ...TAGS>
		static void funcs()
		{
			(func<TAGS>(), ...);
		}
	};

	void register_channel_db()
	{
)" << std::endl;

        int lindex333 = 110000000;
        m_stream3 << std::format("\t\ttprotocol::set_customs_index({});", lindex333) << std::endl;
        m_stream3 << "\t\treg_channel_db::funcs<" << std::endl;
        bool lbools = false;
        for (const auto& item : lmap)
        {
            if (item.second.enumname == "ENUM_DB_COUNT" || item.second.enumname == "ENUM_DB_FAIL")
            {
                continue;
            }

            std::string enumname = std::string("pbdb::") + &(item.second.enumname.c_str()[sizeof("enum_")-1]);
            ngl::tools::replace(" ", "", enumname, enumname);
            ngl::tools::transform_tolower(enumname);
           
            //m_stream3 << std::format("\t\ttprotocol::set_customs_index({});", lindex333) << std::endl;
            //m_stream3 << std::format("\t\treg_channel_db::func<", enumname) << std::endl;
            m_stream3 << "\t\t\t";
            if (lbools)
            {
                m_stream3 << ",";
            }
            m_stream3 << enumname << std::endl;
            lbools = true;

            lindex333 += 10;
        }
        m_stream3 << "\t\t>();" << std::endl;
        m_stream3 << "\t}" << std::endl;
        m_stream3 << "}//namespace ngl" << std::endl;

        ngl::writefile lfile3("../../public/cpp/actor/auto/register_channel_db.h");
        lfile3.write(m_stream3.str());
    }

    static void nactor_auto(google::protobuf::compiler::DiskSourceTree& sourceTree, const char* aname)
    {
        google::protobuf::compiler::Importer importer(&sourceTree, nullptr);
        const google::protobuf::FileDescriptor* fileDescriptor = importer.Import(std::string(aname) + ".proto");
        if (fileDescriptor == nullptr) {
            std::cerr << "Failed to import protobuf file descriptor" << std::endl;
        }
        nactor_auto_fun(fileDescriptor, aname);
    }

    static void db_fun(const char* apackname, const char* aname, const google::protobuf::FileDescriptor* fileDescriptor)
    {
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
                        std::pair<std::list<tmessage>, std::list<tmessage>>& lpair = lmap[(Label)(Label::MAX_LABEL + 1)];
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

                g_stream_pgsql
                    << "-- ----------------------------\r\n"
                    << "-- Table structure for " << lmessage << "\r\n"
                    << "-- ----------------------------\r\n"
                    << "DROP TABLE IF EXISTS " << lmessage << ";\r\n"
                    << "CREATE TABLE " << lmessage << " (\r\n"
                    << "    id NOT NULL PRIMARY KEY,\r\n"
                    << "    area integer NOT NULL,\r\n"
                    << "    data bytea,\r\n"
                    << "); \r\n";

            }
        }
    }

    static void db(google::protobuf::compiler::DiskSourceTree& sourceTree, const char* apackname, const char* aname)
    {
        google::protobuf::compiler::Importer importer(&sourceTree, nullptr);
        const google::protobuf::FileDescriptor* fileDescriptor = importer.Import(std::string(aname) + ".proto");
        if (fileDescriptor == nullptr) {
            std::cerr << "Failed to import protobuf file descriptor" << std::endl;
            return;
        }
        db_fun(apackname, aname, fileDescriptor);
    }

    // 根据enum生成actor模板
    static void enum_actor(std::string aenum)
    {
        // 只判断非单利其他的都按照单例
        bool issign = aenum != "ACTOR_ROLE" && aenum != "ACTOR_ROBOT";
        std::string ltolower = aenum;
        ngl::tools::transform_tolower(ltolower);

        std::string lpath = std::format("../../public/cpp/actor/actor_logic/{}", ltolower);
        if (ngl::tools::directories_exists(lpath) == false)
        {
            if (ngl::tools::create_dir(lpath) == false)
            {
                return;
            }
        }
        // 判断文件是否存在
        lpath += std::format("/{}", ltolower);
        std::string lpathH = std::format("{}.h", lpath);
        std::string lpathCPP = std::format("{}.cpp", lpath);
        if (ngl::tools::file_exists(lpathH) || ngl::tools::file_exists(lpathCPP))
        {
            return;
        }

        ngl::writefile lfileH(lpathH);

        std::string lnrh= std::format(R"(#pragma once

#include "actor/actor_logic/actor_ranklist/ranklist.h"
#include "actor/actor_logic/actor_notice/notice.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
#include "tools/db/sql/mysql/nmysql_pool.h"
#include "actor/actor_base/actor_manage.h"
#include "actor/actor_base/actor_create.h"
#include "actor/actor_base/ndb_modular.h"
#include "actor/actor_base/ndbclient.h"
#include "actor/protocol/nprotocol.h"
#include "actor/actor_base/ntimer.h"
#include "tools/db/sql/db_data.h"
#include "actor/pb/net.pb.h"
#include "net/tcp/ntcp.h"
#include "db.h"

namespace ngl
{{
    class {0} : public actor
	{{
		{0}(const {0}&) = delete;
		{0}& operator=(const {0}&) = delete;

		{0}();
	public:
		friend class actor_instance<{0}>;
		static {0}& instance()
		{{
			return actor_instance<{0}>::instance();
		}}

		virtual ~{0}() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

        bool timer_handle(const message<np_timerparm>& adata);
	}};
}}//namespace ngl)", ltolower);
        lfileH.write(lnrh);



       
        ngl::writefile lfileCPP(lpathCPP);

        std::string lnrcpp = std::format(R"(#include "{0}.h"

namespace ngl
{{
	{0}::{0}() :
		actor(
			actorparm
			{{
				.m_parm
				{{
					.m_type = {1},
					.m_area = tab_self_area,
					/*.m_manage_dbclient = true,*/
				}},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			}})
	{{
	}}

	ENUM_ACTOR {0}::actor_type()
	{{
		return {1};
	}}

	i64_actorid {0}::actorid()
	{{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}}

	void {0}::init()
	{{
		// 绑定DB结构:DB.set(this);
        
        // 设置timer_handle定时器
        /*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{{
			log_error()->print("{0}::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}}
		set_timer(tparm);
        */
	}}

	void {0}::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{{
        if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{{
            log_error()->print("actor_testlua2::loaddb_finish [{{}}:{{}}]", (int32_t)atype, (int32_t)astat);
			return;
		}}
	}}

	void {0}::nregister()
	{{
        // 定时器
		actor::register_timer<{0}, false>(&{0}::timer_handle);

        // 绑定自定义np_消息
        register_handle<{0}
		>(e_ready_all);
        
        // 绑定脚本消息
        register_script_handle<{0}
		>(false);
	}}

    bool {0}::timer_handle(const message<np_timerparm>& adata)
    {{
        return true;
    }}
}}// namespace ngl)", ltolower, aenum);
        lfileCPP.write(lnrcpp);
    }

    static void create_actor()
    {
        //"enum ENUM_ACTOR{}"
        std::set<std::string> lactorenumset;
        {
            std::string ldata;
            // 读取文件nactortype.h
            ngl::readfile lfile("../../public/cpp/actor/actor_base/nactortype.h");
            std::string lnr;
            bool lbool = false;
            while (lfile.readline(lnr))
            {
                if (lbool == false)
                {
                    if (lnr.find("enum ENUM_ACTOR") != std::string::npos)
                    {
                        lbool = true;
                    }
                }
                else
                {
                    // 去掉行数据中的\t 空格 , 与注释
                    ngl::tools::replace(" ", "", lnr, lnr);
                    ngl::tools::replace("\t", "", lnr, lnr);
                    ngl::tools::replace(",", "", lnr, lnr);
                    ngl::tools::replace("\\{", "", lnr, lnr);
                    size_t lpos = lnr.find("//");
                    if (lpos != std::string::npos)
                    {
                        lnr.resize(lpos);
                    }
                    lpos = lnr.find("=");
                    if (lpos != std::string::npos)
                    {
                        lnr.resize(lpos);
                    }
                    if (lnr.empty() == false)
                    {
                        if (lnr == "};")
                        {
                            break;
                        }
                        std::cout << lnr << std::endl;
                        if (lnr == "ACTOR_NONE" 
                            || lnr == "ACTOR_SIGNLE_START"
                            || lnr == "ACTOR_SERVER"
                            || lnr == "ACTOR_CLIENT"
                            || lnr == "ACTOR_CREATE"
                            || lnr == "ACTOR_EVENTS"
                            || lnr == "ACTOR_EVENTS_MAX_COUNT"
                            || lnr == "ACTOR_DB"
                            || lnr == "ACTOR_SIGNLE_FINISH"
                            || lnr == "ACTOR_COUNT"
                            || lnr == "ACTOR_GATEWAY"
                            || lnr == "ACTOR_GATEWAY_C2G"
                            || lnr == "ACTOR_GATEWAY_G2C"
                            )
                        {
                            continue;
                        }
                        lactorenumset.insert(lnr);
                    }
                }
            }
        }

        for (const std::string& item : lactorenumset)
        {
            enum_actor(item);
        }
        struct hactorfile
        {
            std::string m_actorname;
            std::string m_begnr;
            std::string m_endnr;
            std::string m_cppbegnr;
            std::string m_cppendnr;
            struct vepro
            {
                std::string m_message;
                std::string m_tag;
                std::vector<std::string> m_vecactorname;
                std::string m_pbname;
                std::string m_content;
                std::string m_zhushi;
            };
            std::map<std::string, vepro> m_handle;
        };
        std::map<std::string, hactorfile> lhactorfilemap;

        auto lfun = [&lhactorfilemap](const char* fname, const char* pbname)
            {
                std::string ldata;
                // 读取文件nactortype.h
                std::string lfilename = std::format("../proto/{}.proto", fname);
                ngl::readfile lfile(lfilename);
                std::string lnr;
                std::string llastnr;
                bool lbool = false;
                for (;lfile.readline(lnr); llastnr = lnr)
                {
                    size_t lpos = lnr.find("message");
                    if (lpos == std::string::npos)
                    {
                        continue;
                    }
                    hactorfile::vepro ltempvepro;
                    ltempvepro.m_pbname = pbname;
                    for (size_t i = lpos + sizeof("message"); i < lnr.size(); ++i)
                    {
                        if (lnr[i] == ' ' || lnr[i] == '/')
                        {
                            break;
                        }
                        ltempvepro.m_message += lnr[i];
                    }

                    lpos = lnr.find("//");
                    if (lpos == std::string::npos)
                    {
                        continue;
                    }
                    for (size_t i = lpos + sizeof("//") - 1; i < lnr.size(); ++i)
                    {
                        if (lnr[i] == ' ' || lnr[i] == '[')
                        {
                            break;
                        }
                        ltempvepro.m_tag += lnr[i];
                    }

                    lpos = lnr.find("[");
                    if (lpos == std::string::npos)
                    {
                        continue;
                    }
                    std::string lactornamestr;
                    for (size_t i = lpos + sizeof("[") - 1; i < lnr.size(); ++i)
                    {
                        if (lnr[i] == ' ' || lnr[i] == ']')
                        {
                            break;
                        }
                        lactornamestr += lnr[i];
                    }
                    ngl::tools::splite(lactornamestr.c_str(), ",", ltempvepro.m_vecactorname);
                    if (ltempvepro.m_vecactorname.empty() == false)
                    {
                        std::cout << lnr << std::endl;
                        bool isfirst = true;
                        for (std::string& itemactor : ltempvepro.m_vecactorname)
                        {
                            if (itemactor == "actor_gateway")
                            {
                                continue;
                            }
                            lhactorfilemap[itemactor].m_actorname = itemactor;
                           
                            if (isfirst)
                            {
                                isfirst = false;
                                if (ltempvepro.m_tag == "C2G2")
                                {
                                    ltempvepro.m_message = std::format("mforward<{}::{}>", ltempvepro.m_pbname, ltempvepro.m_message);
                                }
                                else
                                {
                                    ltempvepro.m_message = std::format("{}::{}", ltempvepro.m_pbname, ltempvepro.m_message);
                                }
                            }
                            ltempvepro.m_zhushi = llastnr.find("//") == std::string::npos ? "" : llastnr;
                            lhactorfilemap[itemactor].m_handle[ltempvepro.m_message] = ltempvepro;
                        }
                    }
                }
            };
       
        lfun("net", "pbnet");
        lfun("example", "pbexample");
        // 根据message检查actor文件中是否实现了对应的协议没有则插入

        {
            for (const std::string& item : lactorenumset)
            {
                std::string lactortolower = item;
                ngl::tools::transform_tolower(lactortolower);

                {
                    std::string lactorhfile = std::format("../../public/cpp/actor/actor_logic/{0}/{0}.h", lactortolower);
                    ngl::readfile lfile(lactorhfile);
                    if (lfile.is_open() == false)
                    {
                        std::cout << "未找到actor文件:" << lactorhfile << std::endl;
                        continue;
                    }
                    std::string lnr;
                    std::string llastnr;
                    bool lboolkk = false;
                    hactorfile& lhactorfile = lhactorfilemap[lactortolower];
                    lhactorfile.m_actorname = lactortolower;
                    std::string* lpnr = &lhactorfile.m_begnr;
                    for (; lfile.readline(lnr); )
                    {
                        if (lboolkk == false)
                        {
                            std::string lfindname = std::format("class {}", lactortolower);
                            size_t lpos = lnr.find(lfindname);
                            if (lpos != std::string::npos)
                            {
                                lboolkk = true;
                            }
                        }
                        if (lboolkk)
                        {
                            size_t lpos = lnr.find("};");
                            if (lpos != std::string::npos)
                            {
                                lpnr = &lhactorfile.m_endnr;
                            }
                        }

                        size_t lpos = lnr.find("handle(const message<");
                        size_t lpos2 = lnr.find("timer_handle(const message<");
                        if (lpos != std::string::npos && lpos2 == std::string::npos)
                        {
                            llastnr = "";
                           // *lpnr += std::format("{}\n", lnr);
                            std::string lstrhandl;
                            int lcout = 0;
                            for (size_t i = lpos + sizeof("handle(const message<") - 1; i < lnr.size(); ++i)
                            {
                                if (lcout == 0 && lnr[i] == '>')
                                {
                                    break;
                                }
                                if (lnr[i] == '<')
                                {
                                    ++lcout;
                                }
                                else if (lnr[i] == '>')
                                {
                                    --lcout;
                                }
                                lstrhandl += lnr[i];
                            }
                            if (lstrhandl.empty() == false)
                            {
                                lhactorfile.m_handle[lstrhandl];
                            }
                        }
                        else
                        {
                            //*lpnr += std::format("{}\n", lnr);
                            if (lnr.find("//") == std::string::npos)
                            {
                                if (llastnr.empty() == false)
                                {
                                    *lpnr += std::format("{}\n", llastnr);
                                    llastnr = "";
                                }
                                *lpnr += std::format("{}\n", lnr);
                            }
                            else if(lnr.find("}//namespace ngl") != std::string::npos)
                            {
                                if (llastnr.empty() == false)
                                {
                                    *lpnr += std::format("{}\n", llastnr);
                                    llastnr = "";
                                }
                                *lpnr += std::format("{}\n", lnr);
                            }
                            else
                            {
                                std::string lnrkkk = lnr;
                                ngl::tools::replace(" ", "", lnrkkk, lnrkkk);
                                ngl::tools::replace("\t", "", lnrkkk, lnrkkk);
                                lnrkkk.resize(2);
                                if (lnrkkk == "//")
                                {
                                    llastnr = lnr;
                                }
                                else
                                {
                                    *lpnr += std::format("{}\n", lnr);
                                }
                            }
                        }
                    }                 
                }
               
                {
                    std::string ldirectories = std::format("../../public/cpp/actor/actor_logic/{0}/message", lactortolower);
                    if (ngl::tools::directories_exists(ldirectories) == false)
                    {
                        ngl::tools::create_dir(ldirectories);
                    }
                    std::string lactorhfile = std::format("../../public/cpp/actor/actor_logic/{0}/message/{0}_handle.cpp", lactortolower);

                    if (ngl::tools::file_exists(lactorhfile) == false)
                    {
                        ngl::writefile lwfile(lactorhfile);

                        std::stringstream sstream;
                        sstream << "#include \"" << lactortolower << ".h\"" << std::endl;
                        sstream << std::endl;
                        sstream << "namespace ngl" << std::endl;
                        sstream << "{" << std::endl;
                        sstream << "}//namespace ngl" << std::endl;
                        lwfile.write(sstream.str());
                    }

                    ngl::readfile lfile(lactorhfile);
                    if (lfile.is_open() == false)
                    {
                        std::cout << "未找到actor文件:" << lactorhfile << std::endl;
                        continue;
                    }

                    hactorfile& lhactorfile = lhactorfilemap[lactortolower];
                    std::string lnr;
                    bool ismessage = false;
                    std::string lmessage;
                    while (lfile.readline(lnr))
                    {
                        if (ismessage == false)
                        {
                            size_t lpos = lnr.find("handle(const message<");
                            if (lpos != std::string::npos)
                            {
                                // 获取message
                                int lcout = 0;
                                for (size_t i = lpos + sizeof("handle(const message<") - 1; i < lnr.size(); ++i)
                                {
                                    if (lnr[i] == '>' && lcout == 0)
                                    {
                                        break;
                                    }
                                    if (lnr[i] == '<')
                                    {
                                        ++lcout;
                                    }
                                    else if (lnr[i] == '>')
                                    {
                                        --lcout;
                                    }
                                    lmessage += lnr[i];
                                }
                                ismessage = true;
                            }
                            else
                            {

                                size_t lpos = lnr.find("}//namespace ngl");
                                if (lpos != std::string::npos)
                                {
                                    lhactorfile.m_cppendnr += "}//namespace ngl";
                                    continue;
                                }
                                else
                                {
                                    if (lnr != "")
                                    {
                                        lhactorfile.m_cppbegnr += std::format("{}\n", lnr);
                                    }
                                }
                            }
                        }
                        else
                        {
                            if (lnr != "\t{")
                            {
                                lhactorfile.m_handle[lmessage].m_content += std::format("{}\n",lnr);
                            }
                            if (lnr == "\t}")
                            {
                                ismessage = false;
                                lmessage = "";
                            }
                        }
                    }
                }
            }
        }

        std::cout << std::endl;
        // 写入文件
        
        for (const auto& item1 : lhactorfilemap)
        {
            std::string actorname = item1.second.m_actorname;
            auto& lhactorfile = item1.second;
            std::stringstream lstream;
            std::stringstream lstreamcpp;
            for (const auto& itme2 : item1.second.m_handle)
            {
                auto& lmessage = itme2.first;
                if (!itme2.second.m_zhushi.empty())
                {
                    lstream << std::format(R"(		{})", itme2.second.m_zhushi) << std::endl;
                }
                lstream << std::format(R"(		bool handle(const message<{}>& adata);)", lmessage) << std::endl;

                lstreamcpp << std::format("\tbool {}::handle(const message<{}>& adata)", actorname, lmessage) << std::endl;
                lstreamcpp << "\t{" << std::endl;
                if (itme2.second.m_content.empty())
                {
                    lstreamcpp << "\t\treturn true;" << std::endl;
                    lstreamcpp << "\t}" << std::endl;
                }
                else
                {
                    lstreamcpp << itme2.second.m_content;
                }
            }

            // 读取文件
            {
                std::string lcontent;

                std::string lactorfile = std::format("../../public/cpp/actor/actor_logic/{0}/{0}.cpp", actorname);
                std::set<std::string> lset;
                {
                    ngl::readfile lreadfile(lactorfile);
                    std::string lnr;
                    std::string lfindstr1 = std::format("register_handle<{}", actorname);
                    std::string lfindstr2 = std::format("register_script_handle<{}", actorname);
                    std::string lfindstr11 = std::format("register_handle< {}", actorname);
                    std::string lfindstr21 = std::format("register_script_handle< {}", actorname);
                    bool lboool1 = false;
                    bool lboool2 = false;
                    while (lreadfile.readline(lnr))
                    {
                        {
                            size_t lpos1 = lnr.find(lfindstr1);
                            size_t lpos11 = lnr.find(lfindstr11);
                            if (lpos1 != std::string::npos || lpos1 != std::string::npos)
                            {
                                lboool1 = true;
                                continue;
                            }
                            if (lboool1 && lnr.find(';') != std::string::npos)
                            {
                                lboool1 = false;
                                continue;
                            }
                            if (lboool1)
                            {
                                ngl::tools::replace(" ", "", lnr, lnr);
                                ngl::tools::replace("\t", "", lnr, lnr);
                                ngl::tools::replace(",", "", lnr, lnr);
                                lset.insert(lnr);
                            }
                        }
                        {
                            size_t lpos1 = lnr.find(lfindstr2);
                            size_t lpos11 = lnr.find(lfindstr21);
                            if (lpos1 != std::string::npos || lpos11 != std::string::npos)
                            {
                                lboool2 = true;
                                continue;
                            }
                            if (lboool2 && lnr.find(';') != std::string::npos)
                            {
                                lboool2 = false;
                                continue;
                            }
                            if (lboool2)
                            {
                                ngl::tools::replace(" ", "", lnr, lnr);
                                ngl::tools::replace("\t", "", lnr, lnr);
                                ngl::tools::replace(",", "", lnr, lnr);
                                lset.insert(lnr);
                            }
                        }

                    }
                }
                {
                    ngl::readfile lreadfile(lactorfile);
                    lreadfile.readcurrent(lcontent);
                }

                std::set<std::string> lset2;
                for (const auto& itme2 : item1.second.m_handle)
                {
                    if (lset.find(itme2.first) == lset.end())
                    {
                        lset2.insert(itme2.first);
                    }
                }

                if (!lcontent.empty() && !lset2.empty() && actorname != "actor_robot" && actorname != "actor_role")
                {
                    {
                        std::string lfindstr = std::format("actor::register_timer<{0}>(&{0}::timer_handle);", actorname);
                        size_t lpos = lcontent.find(lfindstr);
                        std::string lbeg(lcontent.begin(), lcontent.begin() + lpos);
                        for (; lcontent[lpos] != '\n';++lpos)
                        {
                            lbeg += lcontent[lpos];
                        }
                        std::string lend(lcontent.begin() + lpos + 1, lcontent.end());
                        lcontent = lbeg;
                        lcontent += "\n";
                        lcontent += "\n\t*************************************\n";
                        for (const auto& lstr : lset2)
                        {
                            lcontent += lstr;
                            lcontent += ',';
                        }
                        lcontent += "\n\t*************************************\n";
                        lcontent += lend;
                    }
                    ngl::writefile lcppfile(lactorfile);
                    lcppfile.write(lcontent);
                }

            }

            if (actorname == "actor_robot")
            {//g2c
                std::string lcontent;
                {
                    ngl::readfile lreadfile("../../public/cpp/actor/actor_logic/nprotocol_g2c.cpp");
                    lreadfile.readcurrent(lcontent);
                }

                ngl::tools::replace("\r\n", "\n", lcontent, lcontent);
                ngl::tools::replace("\r", "\n", lcontent, lcontent);

                size_t lpos = lcontent.find("register_g2c<");
                std::string lbeg(lcontent.begin(), lcontent.begin() + lpos);
                for (; lcontent[lpos] != '\n'; ++lpos)
                {
                    lbeg += lcontent[lpos];
                }
                lbeg += "\n";
                std::string lend = R"(
		>();
	}
}//namespace ngl)";
                lcontent = lbeg;

                int i = 0;
                for (const auto& itme2 : item1.second.m_handle)
                {
                    if (itme2.first.find("np_") == std::string::npos)
                    {
                        if (i + 1 == item1.second.m_handle.size())
                        {
                            lcontent += std::format("\t\t\t{}", itme2.first);
                        }
                        else
                        {
                            lcontent += std::format("\t\t\t{},\n", itme2.first);
                        }
                    }
                    ++i;
                }
                lcontent += lend;
                ngl::writefile lhfile("../../public/cpp/actor/actor_logic/nprotocol_g2c.cpp");
                lhfile.write(lcontent);
            }

            if (actorname == "actor_role")
            {//c2g
                std::string lcontent;
                {
                    ngl::readfile lreadfile("../../public/cpp/actor/actor_logic/nprotocol_c2g.cpp");
                    lreadfile.readcurrent(lcontent);
                }
                ngl::tools::replace("\r\n", "\n", lcontent, lcontent);
                ngl::tools::replace("\r", "\n", lcontent, lcontent);

                size_t lpos = lcontent.find("register_c2g<");
                std::string lbeg(lcontent.begin(), lcontent.begin() + lpos);
                for (; lcontent[lpos] != '\n'; ++lpos)
                {
                    lbeg += lcontent[lpos];
                }
                lbeg += "\n";
                std::string lend = R"(
	}
}// namespace ngl)";
                // 整理
                std::set<std::string> lpt;
                std::map<std::string, std::set<std::string>> l2ci;
                for (const auto& itme2 : item1.second.m_handle)
                {
                   
                    // 转发 mforward<pbnet::XXX>
                    auto& lmessage = itme2.first;
                    if (lmessage.find("np_") != std::string::npos)
                    {
                        continue;
                    }
                    lpt.insert(lmessage);
                }
                for (const auto& item1 : lhactorfilemap)
                {
                    for (const auto& item2 : item1.second.m_handle)
                    {
                        if (item2.first.find("np_") != std::string::npos)
                        {
                            continue;
                        }
                        size_t lpos = item2.first.find("mforward<");
                        std::string ltemp;
                        if (lpos != std::string::npos)
                        {
                            for (int i = lpos + sizeof("mforward<")-1; item2.first[i] != '>'; ++i)
                            {
                                ltemp += item2.first[i];
                            }
                            l2ci[item1.first].insert(ltemp);
                        }
                    }                    
                }

                lcontent = lbeg;
                // role需要处理的
                bool lfirsttem = true;
                for (const auto& item1 : lpt)
                {
                    if (lfirsttem)
                    {
                        lfirsttem = false;
                        lcontent += std::format("\t\t\t{}\n", item1);
                    }
                    else
                    {
                        lcontent += std::format("\t\t\t, {}\n", item1);
                    }
                }
                lcontent += std::format("\t\t>();\n");
                // 二次转发
                for (const auto& item1 : l2ci)
                {
                    lcontent += "\n";
					std::string ltemp = item1.first;
					ngl::tools::transform_toupper(ltemp);
                    lcontent += std::format("\t\t// {} 模块二次转发\n", item1.first);
                    lcontent += std::format("\t\tregister_c2g_2< {}\n", ltemp);
                    bool lfirst = true;
                    for (const auto& item2 : item1.second)
                    {
                        if (lfirst)
                        {
                            lfirst = false;
                            lcontent += std::format("\t\t\t, {}\n", item2);
                        }
                    }
                    lcontent += std::format("\t\t>();\n");
                }

                lcontent += lend;
                ngl::writefile lhfile("../../public/cpp/actor/actor_logic/nprotocol_c2g.cpp");
                lhfile.write(lcontent);

            }
            // 写入.h文件
            if (lstream.str().empty() == false)
            {
                   std::string lactorhfile = std::format("../../public/cpp/actor/actor_logic/{0}/{0}.h", actorname);
                   ngl::writefile lhfile(lactorhfile);
                   lhfile.write(lhactorfile.m_begnr);
                   lhfile.write(lstream.str());
                   lhfile.write(lhactorfile.m_endnr);
            }
            if (lstreamcpp.str().empty() == false)
            {
                std::string lactorcppfile = std::format("../../public/cpp/actor/actor_logic/{0}/message/{0}_handle.cpp", actorname);
                ngl::writefile lcppfile(lactorcppfile);
                lcppfile.write(lhactorfile.m_cppbegnr);
                lcppfile.write(lstreamcpp.str());
                lcppfile.write(lhactorfile.m_cppendnr);
            }
        }
    }
};

std::map<std::string, std::string> xml_protocol::g_typearr =
{
    {"bool", "bool"},
    {"int32", "int32_t"},
    {"int64", "int64_t"},
    {"string", "const char*"},
    {"float", "float"},
    {"double", "double"}
};

std::stringstream xml_protocol::g_stream_sql;
std::stringstream xml_protocol::g_stream_pgsql;
std::stringstream xml_protocol::g_stream_xml;
std::map<std::string, std::string> xml_protocol::m_nscriptmap;
std::set<std::string> xml_protocol::m_nscriptset;