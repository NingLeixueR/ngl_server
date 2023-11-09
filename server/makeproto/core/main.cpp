#include <boost/filesystem.hpp>
#include <iostream>
#include <functional>
#include "operator_file.h"
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>  
#include <string>  
#include <iostream> 
#include <regex>
#include <set>
#include <map>
#include <boost/lexical_cast.hpp> 
#include <filesystem>

#include "regular.h"

#include <memory>
#include <functional>


#include <iostream>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/dynamic_message.h>

#include "conversion.h"
#include "xmlprotocol.h"

std::map<std::string, std::string> g_typearr = { {"bool", "bool"}, {"int32", "int32_t"}, {"int64", "int64_t"}, {"string", "const char*"}, {"float", "float"}, {"double", "double"}};


void db(std::stringstream& astream
    , const std::string& aname0
    , const std::string& aparm1
    , const std::string& aparm2
    , const std::string& aparm3
    , const std::string& aparm4
    )
{
    std::string lparm = R"(
    struct <0>
	{
	private:
		bool m_ismalloc;
	public:
        using Type = <1>::<0>;

		ngl::lua_mapfunction<<0>> m_fun;
		<1>::<0>* m_data;
        ngl::data_modified_base* m_modified;
<2>
        <0>():
			m_ismalloc(true),
            m_data(new <1>::<0>()),
            m_modified(nullptr)
        {}

        <0>(<1>::<0>* adata, ngl::data_modified_base* amodified):
            m_ismalloc(false)
			, m_data(adata)
            , m_modified(amodified)
            <4>
        {}


        ~<0>()
		{
			if (m_ismalloc)
				delete m_data;
		}

		static const char* name()
		{
			return "<0>";
		}

		inline ngl::lua_mapfunction<<0>>& function()
		{
			return m_fun;
		}

		inline <1>::<0>& get(bool abool = false)
		{
           if(abool == false && m_modified != nullptr)
			    m_modified->modified();
			return *m_data;
		}

		inline const <1>::<0>& getconst()const
		{
			return *m_data;
		}

		inline void tolua(lua_State* L, const char* atab, const char* aname)
		{
			ngl::lua_help lhelp(L);
		    lhelp.new_table("<0>");

			int lindex = 0;
			
<3>

			if (atab != nullptr)
			{
				lhelp.push_tab(atab, aname, "db_account");
			}
		}

	};)";

    ngl::regular::replace("<0>", aname0, lparm, lparm);
    ngl::regular::replace("<1>", aparm1, lparm, lparm);
    ngl::regular::replace("<2>", aparm2, lparm, lparm);
    ngl::regular::replace("<3>", aparm3, lparm, lparm);
    ngl::regular::replace("<4>", aparm4, lparm, lparm);

    astream << lparm << std::endl;
    std::cout << lparm << std::endl;
}


void data(std::stringstream& astream
    , const std::string& aname0
    , const std::string& aparm1
    , const std::string& aparm2
    , const std::string& aparm3
    , const std::string& aparm4
)
{
    std::string lparm = R"(
    struct <0>
	{
    private:
		bool m_ismalloc;
	public:
        using Type = <1>::<0>;

		ngl::lua_mapfunction<<0>> m_fun;
		<1>::<0>* m_data;
		ngl::data_modified_base* m_modified;

<2>
        <0>():
			m_ismalloc(true),
            m_data(new <1>::<0>()),
            m_modified(nullptr)
        {}

        <0>(<1>::<0>* adata, ngl::data_modified_base* amodified):
			m_ismalloc(false)
            , m_data(adata)
            , m_modified(amodified)
            <4>
        {}

        ~<0>()
		{
			if (m_ismalloc)
				delete m_data;
		}

		static const char* name()
		{
			return "<0>";
		}

		inline ngl::lua_mapfunction<<0>>& function()
		{
			return m_fun;
		}

		inline <1>::<0>& get(bool abool = false)
		{
           if(abool == false && m_modified != nullptr)
			    m_modified->modified();
			return *m_data;
		}

		inline const <1>::<0>& getconst()const
		{
			return *m_data;
		}

		inline void tolua(lua_State* L, const char* atab, const char* aname)
		{
			ngl::lua_help lhelp(L);
			lhelp.new_table("<0>");

			int lindex = 0;
			
<3>

			if (atab != nullptr)
			{
				lhelp.push_tab(atab, aname, "rolebase");
			}
		}
	};)";

    ngl::regular::replace("<0>", aname0, lparm, lparm);
    ngl::regular::replace("<1>", aparm1, lparm, lparm);
    ngl::regular::replace("<2>", aparm2, lparm, lparm);
    ngl::regular::replace("<3>", aparm3, lparm, lparm);
    ngl::regular::replace("<4>", aparm4, lparm, lparm);

    astream << lparm << std::endl;
    std::cout << lparm << std::endl;
}


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
        ngl::regular::replace(std::string(apackname) + ".", "", nestedMessageTypeName, lparmname);
        return lparmname;
    }
    return "";
}


std::string doc_html_statr(const std::string& aname)
{
    //<h1><name></h1>
    std::string lparm = R"(
        <html>
            <head>
                <meta charset="UTF-8">
                <meta http-equiv="X-UA-Compatible" content="IE=edge">
                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                <title>Document</title>
                <style>
                    a:link {
						text-decoration: none;
					}
					a:hover {
						text-decoration: none;
					}
                    a:visited{
                        text-decoration: none;
                    }
                    a:active{
                        text-decoration: none;
                    }
                    /* .tab-content can be styled as you like */
                    main {
                        max-width: 400px;
                        margin: 0 auto;
                    }

                    p {
                        font-family: monospace;
                        font-size: 13px;
                    }

                    summary {
                        font-size: 1rem;
                        font-weight: 600;
                        background-color: #f3f3f3;
                        color: #000;
                        padding: 1rem;
                        margin-bottom: 1rem;
                        outline: none;
                        border-radius: 0.25rem;
                        cursor: pointer;
                        position: relative;
                    }

                    details[open] summary~* {
                        animation: sweep .5s ease-in-out;
                    }

                    @keyframes sweep {
                        0% {
                            opacity: 0;
                            margin-top: -10px
                        }

                        100% {
                            opacity: 1;
                            margin-top: 0px
                        }
                    }

                    details>summary::after {
                        position: absolute;
                        content: "+";
                        right: 20px;
                    }

                    details[open]>summary::after {
                        position: absolute;
                        content: "-";
                        right: 20px;
                    }

                    details>summary::-webkit-details-marker {
                        display: none;
                    }
                </style>
            </head>
            <body>
    <main>
        <h1><name></h1>
            
            
)";

    ngl::regular::replace("<name>", aname, lparm, lparm);

    return lparm;
}

std::string doc_html_finish()
{
    std::string lparm = R"(
            </main>
        </body>
    </html>
)";



    return lparm;
}

std::string doc_html_required_base(const std::string& atype, const std::string& atypename)
{
   std::string lparm = R"(
        <details close>
          <summary><font color="purple">required</font> <font color="green"><type></font> <typename></summary>
            <div class="tab-content">
				<h4><font color="blue">设置<typename></font></h3>
				<h4><space>void set_<typename>(<type>)</h4>
				<h4><font color="blue">获取<typename></font></h3>
				<h4><space><type> <typename>()</h4>
                <h4><font color="blue">是否存在<typename></font></h3>
				<h4><space>bool has_<typename>() const</h4>
                <h4><font color="blue">清除<typename></font></h3>
				<h4><space>bool clear_<typename>()</h4>
            </div>
        </details>
        )";
   ngl::regular::replace("<type>", atype, lparm, lparm);
   ngl::regular::replace("<typename>", atypename, lparm, lparm);
   ngl::regular::replace("<space>", "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", lparm, lparm);
   return lparm;
}


std::string doc_html_required_obj(const std::string& atype, const std::string& atypename)
{
    std::string lparm = R"(
        <details close>
            <summary><font color="purple">required</font> <font color="green"><a href = "<type>.html"><type></a></font> <typename></summary>
            <div class="tab-content">
				<h4><font color="blue">是否存在<typename></font></h3>
				<h4><space>bool has_<typename>()</h4>
				<h4><font color="blue">获取<typename></font></h3>
				<h4><space><a href = "<type>.html">lua_table(<type>)</a> <typename>()</h4>
                <h4><font color="blue">清空<typename></font></h3>
                <h4><space>void clear_<typename>()</h4>                
            </div>
        </details>
)";
    ngl::regular::replace("<type>", atype, lparm, lparm);
    ngl::regular::replace("<typename>", atypename, lparm, lparm);
    ngl::regular::replace("<space>", "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", lparm, lparm);
    return lparm;
}


std::string doc_html_repeated_base(const std::string& atype, const std::string& atypename)
{
    std::string lparm = R"(
        <details close>
            <summary><font color="purple">repeated</font> <font color="green"><type></font> <typename></summary>
            <div class="tab-content">
				<h4><font color="blue">添加<typename></font></h3>
				<h4><space>void add_<typename>(<type>)</h4>
				<h4><font color="blue">获取<typename></font></h3>
				<h4><space>lua_table(<type>) <typename>()</h4>
                <h4><font color="blue">获取指定index的元素</font></h3>
                <h4><space><type> <typename>_index(int index)</h4>     
                <h4><font color="blue">根据指定index设置元素</font></h3>
                <h4><space>void set_<typename>(int index, <type> value)</h4> 
                <h4><font color="blue">清空所有元素</font></h3>
                <h4><space>void clear_<typename>()</h4>     
                <h4><font color="blue">获取元素数量</font></h3>
                <h4><space>int <typename>_size()</h4>   
            </div>
        </details>
)";
    ngl::regular::replace("<type>", atype, lparm, lparm);
    ngl::regular::replace("<typename>", atypename, lparm, lparm);
    ngl::regular::replace("<space>", "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", lparm, lparm);
    return lparm;
}


std::string doc_html_repeated_obj(const std::string& atype, const std::string& atypename)
{
    std::string lparm = R"(
        <details close>
            <summary><font color="purple">repeated</font> <font color="green"><type></font> <typename></summary>
            <div class="tab-content">
				<h4><font color="blue">添加<typename></font></h3>
				<h4><space>void add_<typename>(<type>)</h4>
				<h4><font color="blue">获取指定index的<typename></font></h3>
				<h4><space><a href = "<type>.html">lua_table(<type>)</a> <typename>(int index)</h4>
                <h4><font color="blue">清空所有元素</font></h3>
                <h4><space>void clear_<typename>()</h4>     
                <h4><font color="blue">获取元素数量</font></h3>
                <h4><space>int <typename>_size()</h4>   
            </div>
        </details>
)";
    ngl::regular::replace("<type>", atype, lparm, lparm);
    ngl::regular::replace("<typename>", atypename, lparm, lparm);
    ngl::regular::replace("<space>", "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", lparm, lparm);
    return lparm;
}

static std::stringstream g_stream;
static std::stringstream g_stream_sql;
static std::stringstream g_stream_xml;


void foreachProtobufMessages(const google::protobuf::FileDescriptor* fileDescriptor, int32_t& aprotocol, const std::string& axml)
{
    
    ngl::writefile lfile("./config/" + axml + "_protocol.xml");

    std::string lnamespace;
    if (axml == "net")
    {
        lnamespace = "pbnet";
    }else if (axml == "gm")
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
        if (messageDescriptor->name().find("PROBUFF_NET_") != std::string::npos || messageDescriptor->name().find("PROBUFF_GM_") != std::string::npos)
        {
            int32_t lnumber = (lprotocol == -1 ? ngl::xmlprotocol::free_protocol() : lprotocol);
            m_stream << "\t<config client = \"1\" name=\"class "
                << lnamespace << "::" << messageDescriptor->name()
                << "\" number=\"" << lnumber << "\"/>" << std::endl;
            //actor_forward<TYPE, EPROTOCOL_TYPE_PROTOCOLBUFF, true, ngl::forward>
            m_stream << "\t<config client = \"0\" name=\""
                << "struct ngl::actor_forward<class " << lnamespace << "::" << messageDescriptor->name() << ",1,1,struct ngl::forward>"
                << "\" number=\"" << lnumber << "\"/>" << std::endl;
            m_stream << "\t<config client = \"0\" name=\""
                << "struct ngl::actor_forward<class " << lnamespace << "::" << messageDescriptor->name() << ",1,0,struct ngl::forward>"
                << "\" number=\"" << lnumber << "\"/>" << std::endl;

            m_stream << "\t<config client = \"0\" name=\""
                << "struct ngl::actor_forward<class " << lnamespace << "::" << messageDescriptor->name() << ",1,1,class " << lnamespace << "::" << messageDescriptor->name() << ">"
                << "\" number=\"" << lnumber << "\"/>" << std::endl;
            m_stream << "\t<config client = \"0\" name=\""
                << "struct ngl::actor_forward<class " << lnamespace << "::" << messageDescriptor->name() << ",1,0,class " << lnamespace << "::" << messageDescriptor->name() << ">"
                << "\" number=\"" << lnumber << "\"/>" << std::endl;

            m_stream << "\t<config client = \"0\" name=\""
                << "struct ngl::actor_module_forward<class " << lnamespace << "::" << messageDescriptor->name() << ">"
                << "\" number=\"" << lnumber << "\"/>" << std::endl;
        }
       
    }
    m_stream << "</con>" << std::endl;

    lfile.write(m_stream.str());
}
void foreachProtobuf(google::protobuf::compiler::DiskSourceTree& sourceTree, int32_t& aprotocol, const char* aname)
{
    google::protobuf::compiler::Importer importer(&sourceTree, nullptr);
    const google::protobuf::FileDescriptor* fileDescriptor = importer.Import(std::string(aname) + ".proto");
    if (fileDescriptor == nullptr) {
        std::cerr << "Failed to import protobuf file descriptor" << std::endl;
        return;
    }

    foreachProtobufMessages(fileDescriptor, aprotocol, aname);
}

void traverseProtobufMessages(const char* apackname, const char* aname, const google::protobuf::FileDescriptor* fileDescriptor) {

    std::string lfilename("lua_");
    lfilename += aname;
    ngl::writefile lfile(lfilename + ".h");
    ngl::writefile lfilecpp(lfilename + ".cpp");

    std::stringstream m_stream;
    m_stream << "#pragma once" << std::endl;
    m_stream << "#include \"" << "luatools.h\"" << std::endl;
    m_stream << "#include \"" << "actor_db_client.h\"" << std::endl;
    m_stream << "#include \"" << aname << ".pb.h\"" << std::endl;


    const int dependencyCount = fileDescriptor->dependency_count();
    for (int i = 0; i < dependencyCount; ++i) {
        const google::protobuf::FileDescriptor* dependencyDescriptor = fileDescriptor->dependency(i);
        const std::string dependencyPath = dependencyDescriptor->name();
        std::cout << "Imported file path: " << dependencyPath << std::endl;
        std::string lprotobufname;
        ngl::regular::replace(".proto", "", dependencyPath, lprotobufname);
        m_stream << "#include \"lua_"<< lprotobufname <<".h\"" << std::endl;
    }
    m_stream << "namespace ngl{\n";

    int messageCount = fileDescriptor->message_type_count();
    for (int i = 0; i < messageCount; ++i) {
        const google::protobuf::Descriptor* messageDescriptor = fileDescriptor->message_type(i);

        std::cout << "name: " << messageDescriptor->name() << std::endl;
        std::string lmessage;
        ngl::regular::replace(std::string(apackname) + ".", "", messageDescriptor->name(), lmessage);

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
                    ngl::regular::replace("Template.", "", lparmname2, lparmname2);
                    ngl::regular::replace("Pb.", "", lparmname2, lparmname2);
                }
                else
                {
                    ls = &lpair.second;
                    lparmname = type_name(apackname, fieldDescriptor);
                    ngl::regular::replace("Template.", "", lparmname, lparmname);
                    ngl::regular::replace("Pb.", "", lparmname, lparmname);
                }
               
            }

            ls->push_back(tmessage
                {
                    .m_type = lparmname,
                    .m_type2 = lparmname2,
                    .m_is_type2_base = lis_type2_base,
                    .m_name = fieldDescriptor->name(),
                });

            isprotocol = isprotocol || fieldDescriptor->name() == "m_protocoltype";
            if (isprotocol)
            {
                g_stream << "\t\t\tactor_lua::register_operator<ngl::" << messageDescriptor->name() << ">();" << std::endl;
            }

        }

        std::stringstream m_stream_doc;
        m_stream_doc << doc_html_statr(messageDescriptor->name());



        
        //lstream_parm1
        for (auto& item : lmap[Label::LABEL_REQUIRED].second)
        {
            lstream_parm1 << "\t\tngl::" << item.m_type << " " << item.m_name << ";" << std::endl;
        }
        for (auto& item : lmap[Label::LABEL_REPEATED].second)
        {
            lstream_parm1 << "\t\tstd::vector<ngl::" << item.m_type << "> " << item.m_name << ";" << std::endl;
        }

        for (auto& item : lmap[(Label)(Label::MAX_LABEL + 1)].second)
        {
            lstream_parm1 << "\t\tstd::map<" << item.m_type << ",";
            if (item.m_is_type2_base)
            {
                lstream_parm1 << item.m_type2 << "> " << item.m_name << ";" << std::endl;
            }
            else
            {
                lstream_parm1 << "ngl::" << item.m_type2 << "> " << item.m_name << ";" << std::endl;
            }
        }


        if (lmap[Label::LABEL_REQUIRED].first.empty() == false)
        {
            lstream_parm2 << "\t\t\tlindex = lhelp.required_base(this, lindex" << std::endl;
            for (auto& item : lmap[Label::LABEL_REQUIRED].first)
            {
                if(item.m_type == "const char*")
                    lstream_parm2 << "\t\t\t\t, tolua_required_base_str("<< item.m_type <<", "<< item.m_name <<")" << std::endl;
                else
                    lstream_parm2 << "\t\t\t\t, tolua_required_base(" << item.m_type << ", " << item.m_name << ")" << std::endl;

                m_stream_doc << doc_html_required_base(item.m_type, item.m_name);
            }
            lstream_parm2 << "\t\t\t);" << std::endl;
        }

        for (auto& item : lmap[Label::LABEL_REQUIRED].second)
        {
            lstream_parm4 << ", " << item.m_name << "(get(true).mutable_" << item.m_name << "(), amodified)" << std::endl;

            lstream_parm2 << "\t\t\t" << item.m_name << ".tolua(L, \"" << lmessage << "\", \"" << item.m_name << "\");" << std::endl;
            //lstream_parm2 << "\t\t\t" << item.m_name << ".m_data = get().mutable_" << item.m_name << "();" << std::endl;

            std::string ltemptype;
            ngl::regular::replace("Template.", "", item.m_type, ltemptype);
            lstream_parm2 << "\t\t\t" << "lindex = lhelp.required_obj(this, lindex, tolua_required_obj(ngl::" << ltemptype << ", " << item.m_name << "));" << std::endl;
              
            m_stream_doc << doc_html_required_obj(ltemptype, item.m_name);
        }

        if (lmap[Label::LABEL_REPEATED].first.empty() == false)
        {
            lstream_parm2 << "\t\t\tlindex = lhelp.repeated_base(this, lindex" << std::endl;
            for (auto& item : lmap[Label::LABEL_REPEATED].first)
            {
                if (item.m_type == "const char*")
                    lstream_parm2 << "\t\t\t\t, tolua_repeated_base_str(" << item.m_type << ", " << item.m_name << ")" << std::endl;
                else
                    lstream_parm2 << "\t\t\t\t, tolua_repeated_base(" << item.m_type << ", " << item.m_name << ")" << std::endl;

                m_stream_doc << doc_html_repeated_base(item.m_type, item.m_name);
            }
            lstream_parm2 << "\t\t\t);" << std::endl;
        }

        if (lmap[Label::LABEL_REPEATED].second.empty() == false)
        {
            lstream_parm2 << "\t\t\tlindex = lhelp.repeated_obj(this, lindex" << std::endl;
            for (auto& item : lmap[Label::LABEL_REPEATED].second)
            {
                lstream_parm2 << "\t\t\t\t, tolua_repeated_obj(" << item.m_type << ", " << item.m_name << ")" << std::endl;

                m_stream_doc << doc_html_repeated_obj(item.m_type, item.m_name);
            }
            lstream_parm2 << "\t\t\t);" << std::endl;

            for (auto& item : lmap[Label::LABEL_REPEATED].second)
            {
                lstream_parm3 << "\t\t\ttolua_repeated_obj_for("<< item.m_type <<", "<< item.m_name <<"))" << std::endl;
            }
            
        }

        if (lmap[(Label)(Label::MAX_LABEL + 1)].second.empty() == false)
        {
            lstream_parm2 << "\t\t\tlindex = lhelp.repeated_objmap(this, lindex" << std::endl;
            for (auto& item : lmap[(Label)(Label::MAX_LABEL + 1)].second)
            {
                lstream_parm2 << "\t\t\t\t, tolua_repeated_objmap(" << item.m_type << ", " << item.m_type2 << ", " << item.m_name << ")" << std::endl;

                //m_stream_doc << doc_html_repeated_obj(item.m_type, item.m_name);
            }
            lstream_parm2 << "\t\t\t);" << std::endl;

        }

        if (isprotocol)
        {
            g_stream_xml << "</" << messageDescriptor->name() << ">\n";
        }


        m_stream_doc << doc_html_finish();

        std::string lhtmlname = ".\\doc\\";
        lhtmlname += messageDescriptor->name() + ".html";
        ngl::writefile lhtml(lhtmlname);

        std::string lhtmldoc = m_stream_doc.str();
        ngl::conversion::to_utf8(lhtmldoc, lhtmldoc);
        lhtml.write(lhtmldoc);


        if (lmessage.find("db_") != std::string::npos)
      {
          db(m_stream, lmessage, apackname, lstream_parm1.str(), lstream_parm2.str(), lstream_parm4.str());

          g_stream_sql
              << "-- ----------------------------\r\n"
              << "--Table structure for " << lmessage << "\r\n"
              << "-- ----------------------------\r\n"
              << "DROP TABLE IF EXISTS `" << lmessage << "`;\r\n"
              << "CREATE TABLE `" << lmessage << "` (\r\n"
              << "    `id` bigint(63) NOT NULL,\r\n"
              << "    `data` LongBlob,\r\n"
              << "    PRIMARY KEY(`id`)\r\n"
              << ")ENGINE = InnoDB DEFAULT CHARSET = utf8; \r\n";
              
      }
      else
      {
          data(m_stream, lmessage, apackname, lstream_parm1.str(), lstream_parm2.str(), lstream_parm4.str());

          if (isprotocol)
          {
              if (lmessage.find("_RESPONSE") == std::string::npos)
              {
                  std::string lluafilename("./lua/");
                  lluafilename += lmessage + ".lua";

                  if (ngl::filetools::exist(lluafilename) == false)
                  {
                      std::stringstream lstream_lua;
                      lstream_lua << "function " << lmessage << "()" << std::endl;
                      lstream_lua << "  local pro = actor_lua.malloc(\"" << lmessage << "_RESPONSE\")" << std::endl;
                      lstream_lua << "  actor_lua.send2client(\"" << lmessage << "_RESPONSE\");" << std::endl;
                      lstream_lua << "end" << std::endl;
                      lstream_lua << "profun = {" << lmessage << " = " << lmessage << "};" << std::endl;

                      ngl::writefile lluafile(lluafilename);
                      lluafile.write(lstream_lua.str());
                  }
                 
              }              
          }
      }
    }


    g_stream_xml << "</root>\n";

    m_stream << "}//namespace ngl\n";

   lfile.write(m_stream.str());

   std::stringstream m_stream2;
   m_stream2 << "#include \"lua_" << aname << ".h\"" << std::endl;
   m_stream2 << "namespace ngl{\n";
   m_stream2 << "}//namespace ngl\n";
   lfilecpp.write(m_stream2.str());
   _sleep(1000);
   //delete lfile;
   //delete lfilecpp;
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

#include "localtime.h"


template <typename T>
class k1
{};

class abc
{
public:
    template <typename T>
    static void kkk()
    {
        std::cout << "kkk" << std::endl;
    }

    template <>
    void kkk<k1<int>>()
    {
        std::cout << "k1" << std::endl;
    }

    template <>
    void kkk<k1<float>>()
    {
        std::cout << "k1" << std::endl;
    }
};




int main(int argc, char** argv) {
   

    abc::kkk<int>();
    abc::kkk<k1<int>>();
    abc::kkk<k1<float>>();

    google::protobuf::compiler::DiskSourceTree sourceTree;
    sourceTree.MapPath("", "../../resources/protocolbuff/");
  
    g_stream << "#pragma once" << std::endl;
    g_stream << "#include \"actor_lua.h\"" << std::endl;
    g_stream << "#include \"lua_db.h\"" << std::endl;
    g_stream << "#include \"lua_net.h\"" << std::endl;
    g_stream << "namespace ngl{" << std::endl;

    g_stream << "   class lua_struct_register" << std::endl;
    g_stream << "   {" << std::endl;
    g_stream << "   public:" << std::endl;
    g_stream << "       static void init()" << std::endl;
    g_stream << "       {" << std::endl;

    g_stream_sql << "/*Date:" << ngl::localtime::time2msstr("%Y-%m-%d %H:%M:%S")<< "*/" << std::endl;
    g_stream_sql << std::endl;
    g_stream_sql << " DROP Database IF EXISTS `lbtest`;" << std::endl;
    g_stream_sql << " CREATE DATABASE lbtest default charset utf8 COLLATE utf8_general_ci;" << std::endl;
    g_stream_sql << " use lbtest;" << std::endl;
    g_stream_sql << std::endl;

   

    traverseProtobuf(sourceTree, "Template", "db");
    traverseProtobuf(sourceTree, "PB", "net");


    g_stream << "       }" << std::endl;
    g_stream << "   };" << std::endl;
    g_stream << "}" << std::endl;
    ngl::writefile lfilecpp("lua_struct_register.h");
    lfilecpp.write(g_stream.str());

    ngl::writefile lsql("create_db.sql");
    lsql.write(g_stream_sql.str());


    ngl::writefile lxml("lua.xml");
    lxml.write(g_stream_xml.str());

    
    
    ngl::xmlprotocol::load();
    int32_t lnumber = 0;
    foreachProtobuf(sourceTree, lnumber, "net");
    lnumber = 100000000;
    foreachProtobuf(sourceTree, lnumber, "gm");
   
    return 0;
}