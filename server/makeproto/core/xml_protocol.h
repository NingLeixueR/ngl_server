#pragma once

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

class xml_protocol
{
    static std::map<std::string, std::string> g_typearr;

    static std::string type_name(const char* apackname, const google::protobuf::FieldDescriptor* fieldDescriptor)
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

public:
    static std::stringstream g_stream_sql;
    static std::stringstream g_stream_xml;

    static void xml_fun(const google::protobuf::FileDescriptor* fileDescriptor, int32_t& aprotocol, const std::string& axml)
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
                m_stream_pb << std::format("            null<{}::{}>,", lnamespace, messageDescriptor->name()) << std::endl;
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
                m_stream << std::format("            mkdef_map({}::{});", lnamespace, messageDescriptor->name()) << std::endl;
            }
        }
        return m_stream.str();
    }

    static void json2_fun(const std::string& astr)
    {
        ngl::writefile lfile("./pbname_match.cpp");

        std::stringstream m_stream;
        m_stream << " // ע�⡾makeproto ���������ļ�����Ҫ�ֶ��޸ġ�" << std::endl;
        m_stream << " // ����ʱ�䡾" << ngl::localtime::time2str() << "��" << std::endl;
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

    static void nactor_auto_fun(const google::protobuf::FileDescriptor* fileDescriptor, const std::string& axml)
    {
        std::string ldata;
        {

            // ��ȡ�ļ�nactor_auto.h
            ngl::readfile lfile("../../public/cpp/actor/auto_edit/nactor_auto.h");
            lfile.read(ldata);
        }
        // ������ʽ��ƥ�� "using <name> = typedb<...>;"
        std::regex pattern(R"(using\s+([a-zA-Z0-9_]+)\s*=\s*typedb<([^>]+)>;)");

        // ���ڴ洢ƥ�䵽�Ľ��
        std::smatch matches;

        // �����ı�����������ƥ����
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
            // ��ȡ������ƥ������磺using tdb_account = typedb<pbdb::ENUM_DB_ACCOUNT, pbdb::db_account, actor_login>;��
            std::string full_match = (*i).str(0);

            struttt ltemp;
            // ��ȡ��һ��������Ĳ��֣�tdb_account, user_data, customer_data�ȣ�
            ltemp.name = (*i).str(1);

            // ��ȡ�ڶ����֣��� typedb<...> �ڵ�����
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

                // ����ö��ֵ
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
		// ����/����[���ݸ���]
		using nsp_ser	 = nsp_server<TDBTAB_TYPE, TACTOR, TDBTAB>;
		template <typename TDerived>
		using nsp_cli	 = nsp_client<TDerived, TDBTAB>;
		// [aregister == true] ��Ҫ��ע��Э��,�������͵İ�
		// [aregister == false] ʵ����db_actor,db server��Ҫ
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
            {// �ñ������ ���ȷ���޸�
                m_stream << "\t#����������Ҫȷ���޸�" << std::endl;
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
                    {// �ñ������ ���ȷ���޸�
                        m_stream << "\t#����������Ҫȷ���޸�" << std::endl;
                    }
                    m_stream << std::format("\tusing {} = typedb<{}, {}, {}>;", name, enumname, proname, actorname) << std::endl;
                    continue;
                }
                if (lcount == 2 && proname.find("dbcross") == std::string::npos)
                {
                    if (item.second.actorname == "actor_xxxx")
                    {// �ñ������ ���ȷ���޸�
                        m_stream << "\t#����������Ҫȷ���޸�" << std::endl;
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

		// # ������nactor_auto.cpp��,��Ϊ����ǿ�ѡ��
		static void tcrossdb_init(bool ainstance);
	};
}//namespace ngl)";
        lfile.write(m_stream.str());

        std::stringstream m_stream2;
        m_stream2 << " // ע�⡾makeproto ���������ļ�����Ҫ�ֶ��޸ġ�" << std::endl;
        m_stream2 << " // ����ʱ�䡾" << ngl::localtime::time2str() << "��" << std::endl;
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
                    // �ж��ֶ��Ƿ�Ϊmap����
                    if (fieldDescriptor->is_map()) {
                        // MAX_LABEL === maps
                        std::pair<std::list<tmessage>, std::list<tmessage>>& lpair = lmap[(Label)(Label::MAX_LABEL + 1)];
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

    // ����enum����actorģ��
    static void enum_actor(std::string aenum)
    {
        // ֻ�жϷǵ��������Ķ����յ���
        bool issign = aenum != "ACTOR_ROLE" && aenum != "ACTOR_ROBOT";
        std::string ltolower = aenum;
        ngl::tools::transform_tolower(ltolower);

        std::string lpath = std::format("../../public/cpp/actor/actor_logic/{}", ltolower);
        if (ngl::tools::directories_exists(lpath) == false)
        {
            if (ngl::tools::create_directories(lpath) == false)
            {
                return;
            }
        }
        // �ж��ļ��Ƿ����
        lpath += std::format("/{}", ltolower);
        std::string lpathH = std::format("{}_1.h", lpath);
        std::string lpathCPP = std::format("{}_1.cpp", lpath);
        if (ngl::tools::file_exists(lpathH) || ngl::tools::file_exists(lpathCPP))
        {
            return;
        }

        ngl::writefile lfileH(lpathH);

        std::string lnrh= std::format(R"(#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "ndb_modular.h"
#include "nsp_server.h"
#include "nsp_client.h"
#include "db_manage.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "ranklist.h"
#include "db_data.h"
#include "db_pool.h"
#include "ntimer.h"
#include "notice.h"
#include "net.pb.h"
#include "net.h"
#include "db.h"

namespace ngl
{
    class {0} : public actor
	{
		{0}(const {0}&) = delete;
		{0}& operator=(const {0}&) = delete;

		{0}();
	public:
		friend class actor_instance<{0}>;
		static {0}& getInstance()
		{
			return actor_instance<{0}>::instance();
		}

		virtual ~{0}() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

        bool timer_handle(const message<np_timerparm>& adata);
	};
}//namespace ngl)", ltolower);
        lfileH.write(lnrh);



       
        ngl::writefile lfileCPP(lpathCPP);

        std::string lnrcpp = R"(#include "{0}.h"

namespace ngl
{
	{0}::{0}() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = {1},
					.m_area = tab_self_area,
					/*.m_manage_dbclient = true,*/
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	ENUM_ACTOR {0}::actor_type()
	{
		return {1};
	}

	i64_actorid {0}::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void {0}::init()
	{
		// ��DB�ṹ:DB.set(this);
        //
        /* ����timer_handle��ʱ��
        np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_chat::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
        */
	}

	void {0}::loaddb_finish(bool adbishave)
	{
	}

	void {0}::nregister()
	{
        // ���Զ���np_��Ϣ
		//register_handle_custom<{0}>::func<
		//	np_xxxx
		//>(true);

        // ��pb��Ϣ
		register_handle_proto<{0}>::func<
		>(true);
	}

    bool {0}::timer_handle(const message<np_timerparm>& adata)
    {
        return true;
    }
}// namespace ngl)";
        lfileCPP.write(lnrcpp);
    }

    static void create_actor()
    {
        //"enum ENUM_ACTOR{}"
        std::set<std::string> lactorenumset;
        {
            std::string ldata;
            // ��ȡ�ļ�nactortype.h
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
                    // ȥ���������е�\t �ո� , ��ע��
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
        return;
        // ��ȡ.pro�ļ�
        //message PROBUFF_NET_MSG_RESPONSE//G2C [actor_robot]
        struct vepro
        {
            std::string m_name;
            std::string m_tag;
            std::vector<std::string> m_vecactorname;
            std::string m_pbname;
        };
        std::map<std::string, std::map<std::string, std::map<std::string, std::string>>> lmessagemap;
        std::map<std::string, vepro> lmessageinfo;

        auto lfun = [&lmessagemap,&lmessageinfo](const char* fname, const char* pbname)
            {
                std::string ldata;
                // ��ȡ�ļ�nactortype.h
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
                    vepro ltempvepro;
                    ltempvepro.m_pbname = pbname;
                    for (size_t i = lpos + sizeof("message"); i < lnr.size(); ++i)
                    {
                        if (lnr[i] == ' ' || lnr[i] == '/')
                        {
                            break;
                        }
                        ltempvepro.m_name += lnr[i];
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
                        for (std::string& itemactor : ltempvepro.m_vecactorname)
                        {
                            lmessagemap[itemactor][ltempvepro.m_tag][ltempvepro.m_name] = llastnr.find("//") == std::string::npos ? "" : llastnr;
                            lmessageinfo[ltempvepro.m_name] = ltempvepro;
                        }
                    }
                }
            };
       
        lfun("net", "pbnet");
        lfun("example", "pbexample");
        // ����message���actor�ļ����Ƿ�ʵ���˶�Ӧ��Э��û�������

        struct hactorfile
        {
            std::string m_actorname;
            std::string m_begnr;
            std::string m_endnr;
            std::string m_cppbegnr;
            std::string m_cppendnr;
            std::set<std::string> m_handle;
        };
        std::map<std::string, hactorfile> lhactorfilemap;
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
                        std::cout << "δ�ҵ�actor�ļ�:" << lactorhfile << std::endl;
                        continue;
                    }
                    std::string lnr;
                    std::string llastnr;
                    bool lboolkk = false;
                    hactorfile& lhactorfile = lhactorfilemap[lactortolower];
                    lhactorfile.m_actorname = lactortolower;
                    std::string* lpnr = &lhactorfile.m_begnr;
                    for (; lfile.readline(lnr); llastnr = lnr)
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
                            if (llastnr.find("//") != std::string::npos)
                            {

                            }
                            else
                            {
                                *lpnr += std::format("{}\n", llastnr);
                            }
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
                                lhactorfile.m_handle.insert(lstrhandl);
                            }
                        }
                        else
                        {
                            //*lpnr += std::format("{}\n", lnr);
                            *lpnr += std::format("{}\n", llastnr);
                        }
                    }
                    *lpnr += std::format("{}\n", llastnr);                    
                }
               
                {
                    std::string ldirectories = std::format("../../public/cpp/actor/actor_logic/{0}/message", lactortolower);
                    if (ngl::tools::directories_exists(ldirectories) == false)
                    {
                        ngl::tools::create_directories(ldirectories);
                    }
                    std::string lactorhfile = std::format("../../public/cpp/actor/actor_logic/{0}/message/{0}_handle.cpp", lactortolower);

                    if (ngl::tools::file_exists(lactorhfile) == false)
                    {
                        ngl::writefile lwfile(lactorhfile);
                        lwfile.write(R"(#include "{}.h"

namespace ngl
{
}//namespace ngl)");
                    }

                    ngl::readfile lfile(lactorhfile);
                    if (lfile.is_open() == false)
                    {
                        std::cout << "δ�ҵ�actor�ļ�:" << lactorhfile << std::endl;
                        continue;
                    }

                    hactorfile& lhactorfile = lhactorfilemap[lactortolower];
                    std::string* lpnr = &lhactorfile.m_cppbegnr;
                    std::string lnr;
                    while (lfile.readline(lnr))
                    {
                        size_t lpos = lnr.find("}//namespace ngl");
                        if (lpos != std::string::npos)
                        {
                            lpnr = &lhactorfile.m_cppendnr;
                        }

                        *lpnr += std::format("{}\n", lnr);
                    }
                }
            }
        }

        // д���ļ�
        for (const auto& item1 : lmessagemap)
        {
            std::string actorname = item1.first;
            auto itor = lhactorfilemap.find(actorname);
            if (itor == lhactorfilemap.end())
            {
                continue;
            }
            hactorfile& lhactorfile = itor->second;
            for (const auto& item2 : item1.second)
            {
                std::string ltype = item2.first;
                std::stringstream lstream;
                for (const auto& message : item2.second)
                {
                    std::string lmessage = message.first;
                    vepro& ltttt = lmessageinfo[message.first];

                    if (ltype == "C2G2")
                    {
                        if (ltttt.m_pbname.empty())
                        {
                            lmessage = std::format("mforward<{}>", message.first);
                        }
                        else
                        {
                            lmessage = std::format("mforward<{}::{}>", ltttt.m_pbname, message.first);
                        }
                    }
                    else
                    {
                        if (ltttt.m_pbname.empty())
                        {
                            lmessage = std::format("{}", message.first);
                        }
                        else
                        {
                            lmessage = std::format("{}::{}", ltttt.m_pbname, message.first);
                        }
                    }
                    //if(lhactorfile.m_handle.find(lmessage) == lhactorfile.m_handle.end())
                    {
                        std::cout << actorname << "#" << message.first << std::endl;
                        if (message.second.empty() == false)
                        {
                            lstream << std::format(R"(		{})", message.second) << std::endl;
                        }
                        lstream << std::format(R"(		bool handle(const message<{}>& adata);)", lmessage) << std::endl;
                    }                  
                }

                // д��.h�ļ�
                if (lstream.str().empty() == false)
                {

                    std::string lactorhfile = std::format("../../public/cpp/actor/actor_logic/{0}/{0}.h", actorname);
                    ngl::writefile lhfile(lactorhfile);
                    lhfile.write(lhactorfile.m_begnr);
                    lhfile.write(lstream.str());
                    lhfile.write(lhactorfile.m_endnr);
                }
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
std::stringstream xml_protocol::g_stream_xml;