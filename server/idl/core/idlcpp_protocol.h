#ifndef _IDLCPP_PROTOCOL_H_
#define _IDLCPP_PROTOCOL_H_

#include "idl.h"
#include "localtime.h"
#include "tools.h"
#include <algorithm>
#include <string>
#include <map>
#include <ranges>

class idlcppprotocol
{
	idlcppprotocol() 
	{
		char ltmbuff[1024] = { 0 };
		ngl::localtime::time2str(ltmbuff, 1024, ngl::localtime::gettime(), "// 创建时间 %y-%m-%d %H:%M:%S");
		m_tit = "// 注意【IDL 工具生成文件，不要手动修改】\n";
		m_tit += "// 创建时间 ";
		m_tit += ltmbuff;
		m_tit += "\n";
	}

	std::string m_tit;
public:
	static idlcppprotocol& instance()
	{
		static idlcppprotocol ltemp;
		return ltemp;
	}

	void _h()
	{
		std::map<std::string, idl_file>& lmap = idl::instance().data();
		for (std::pair<const std::string, idl_file>& item : lmap)
		{
			std::string lstr;
			lstr += m_tit;
			lstr += "#pragma once\n\n";
			Include lInclude(lstr, item.second.m_include);
			Define lDefine(lstr, item.second.m_define);
			Inamespace* lpInamespace = new Inamespace(lstr);
			Enum lEnum(lstr, item.second.m_enum);
			Struct lStruct(lstr, item.second.m_struct, item.second.m_enum);
			if (item.first == "csvtable")
			{
				lStruct._h_csv();
			}
			else
			{
				lStruct._h_protocol();
			}
			delete lpInamespace;
			ngl::writefile lfile("..\\idl\\idlfile\\" + item.first + ".h");
			lfile.write(lstr);
			//_h(".\\idlfile\\" + item.first, item.second, item.first == "game_db", item.first == "csvtable");
		}
	}

	
	void _cpp()
	{
		std::map<std::string, idl_file>& lmap = idl::instance().data();
		for (std::pair<const std::string, idl_file>& item : lmap)
		{
			std::string lstr;
			lstr += m_tit;
			{
				set<string> lset;
				lset.insert("#include \"" + item.first + ".h\"");
				Include lInclude(lstr, lset);

				Inamespace* lpInamespace = new Inamespace(lstr);
				Struct lStruct(lstr, item.second.m_struct, item.second.m_enum);
				if (item.first == "csvtable")
				{
					lStruct._cpp_csv();
				}
				else
				{
					lStruct._cpp_protocol();
				}

				delete lpInamespace;
			}

			
			ngl::writefile lfile("..\\idl\\idlfile\\" + item.first + ".cpp");
			lfile.write(lstr);
			//_cpp(".\\idlfile\\" + item.first, item.second, item.first == "game_db", item.first == "csvtable");
		}
	}

	std::string cs_get_type(Data& item2)
	{
		std::string ltempstr(item2.m_type);
		std::string ltempstr2;
		ngl::tools::replace_ret("map<", "Dictionary<", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::tools::replace_ret("list<", "List<", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::tools::replace_ret("vector<", "List<", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::tools::replace_ret("set<", "List<", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::tools::replace_ret("string", "string", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::tools::replace_ret("uint8_t", "byte", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::tools::replace_ret("int8_t", "sbyte", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::tools::replace_ret("uint16_t", "UInt16", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::tools::replace_ret("int16_t", "Int16", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::tools::replace_ret("UInt32", "uint32_t", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::tools::replace_ret("int32_t", "Int32", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::tools::replace_ret("uint64_t", "UInt64", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::tools::replace_ret("int64_t", "Int64", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		return ltempstr;
	}

	void _cs()
	{
		std::map<std::string, idl_file>& lmap = idl::instance().data();
		for (std::pair<const std::string, idl_file>& item : lmap)
		{
			if (item.first != "csvtable")
				continue;
			std::stringstream m_stream;
			m_stream << "namespace ngl" << std::endl;
			m_stream << "{" << std::endl;

			std::set<std::string> lsettype;
			lsettype.insert("byte");
			lsettype.insert("sbyte");
			lsettype.insert("string");
			lsettype.insert("Int16");
			lsettype.insert("UInt16");
			lsettype.insert("Int32");
			lsettype.insert("UInt32");
			lsettype.insert("Int64");
			lsettype.insert("UInt64");
			lsettype.insert("bool");
			lsettype.insert("float");
			lsettype.insert("double");
			
			std::set<std::string> lsetenum;
			for (auto& item : item.second.m_enum)
			{
				lsetenum.insert(item.name);
				m_stream << "	public enum " << item.name << "\n";
				m_stream << "	{\n";
				
				for (auto& itemdata : item.dataVec)
				{
					if (itemdata.m_values_name.empty())
					{
						m_stream << "		" << itemdata.m_type << ",\t" << itemdata.zhushi << "\n";
					}
					else
					{
						m_stream << "		" << itemdata.m_type << " = " + itemdata.m_values_name << ",\t" << itemdata.zhushi << "\n";
					}
				}
				m_stream << "	}\n";
				m_stream << "	partial class RCsv" << std::endl;
				m_stream << "	{" << std::endl;
				//m_stream << "		public static bool readcsv(csvpair apair, ref "<< item.name<<" adata)" << std::endl;
				//m_stream << "		{" << std::endl;
				//m_stream << "			Int32 ltemp = 0;" << std::endl;
				//m_stream << "			if(readcsv(apair, ref ltemp) == false)" << std::endl;
				//m_stream << "				return false;" << std::endl;
				//m_stream << "			adata = ("<< item.name <<")ltemp;" << std::endl;
				//m_stream << "			return true;" << std::endl;
				//m_stream << "		}" << std::endl;
				m_stream << "		public static bool ReadCsv(CsvPair apair, List<" << item.name << "> avec)" << std::endl;
				m_stream << "		{" << std::endl;
				m_stream << "			string ltempstr = Read(apair);" << std::endl;
				m_stream << "			CsvPair lpair = new CsvPair();" << std::endl;
				m_stream << "			lpair.m_data = ltempstr;" << std::endl;
				m_stream << "			lpair.m_fg = '*';" << std::endl;
				m_stream << "			for (; !IsOk(lpair);)" << std::endl;
				m_stream << "			{" << std::endl;
				m_stream << "				Int32 ltemp = 0;" << std::endl;
				m_stream << "				if (ReadCsv(lpair, ref ltemp))" << std::endl;
				m_stream << "					avec.Add(("<< item.name <<")ltemp);" << std::endl;
				m_stream << "			}" << std::endl;
				m_stream << "			return true;" << std::endl;
				m_stream << "		}" << std::endl;
				m_stream << "	}" << std::endl;
			}

			for (auto& item : item.second.m_struct)
			{
				std::string::size_type lv = item.name.find("tab_");
				bool lbool = lv != string::npos;
				m_stream << "	class " << item.name << " : ICsvRead" << (lbool ? ", ICsv" : "") << std::endl;
				m_stream << "	{" << std::endl;
				m_stream << "		/*********************************/" << std::endl;
				for (Data itemdata : item.dataVec)
				{
					itemdata.m_type = cs_get_type(itemdata);
					if (lsetenum.find(itemdata.m_type) != lsetenum.end())
					{
						m_stream
							<< "		public " << itemdata.m_type << "		" << itemdata.m_values_name << ";"
							<< "\t\t" << itemdata.zhushi << std::endl;
					}
					else if (lsettype.find(itemdata.m_type) != lsettype.end())
					{

						m_stream
							<< "		public " << itemdata.m_type << "		" << itemdata.m_values_name << ";"
							<< "\t\t" << itemdata.zhushi << std::endl;
					}
					else
					{
						m_stream
							<< "		public " << itemdata.m_type << "		" << itemdata.m_values_name << " = new " << itemdata.m_type << "();"
							<< "\t\t" << itemdata.zhushi << std::endl;

					}
					
				}
				m_stream << "		/*********************************/" << std::endl;
				if(lbool)
					m_stream << "		public Int32 Id(){return m_id;}"<<std::endl;
				m_stream << "		public bool Read(CsvPair apair)" << std::endl;
				m_stream << "		{" << std::endl;
				for (Data itemdata : item.dataVec)
				{
					itemdata.m_type = cs_get_type(itemdata);
					if (lsetenum.find(itemdata.m_type) != lsetenum.end())
					{
						m_stream << "			Int32 l" << itemdata.m_values_name << " = 0;" << std::endl;
						m_stream << "			if(RCsv.ReadCsv(apair, ref l" << itemdata.m_values_name << ") == false)" << std::endl;
						m_stream << "				return false;" << std::endl;
						m_stream << "			" << itemdata.m_values_name << " = ("<< itemdata.m_type <<")l"<< itemdata.m_values_name <<";" << std::endl;
					}
					else if(lsettype.find(itemdata.m_type) != lsettype.end())
					{
						m_stream << "			if(RCsv.ReadCsv(apair, ref " << itemdata.m_values_name << ") == false)" << std::endl;
						m_stream << "				return false;" << std::endl;
					}
					else
					{
						m_stream << "			if(RCsv.ReadCsv(apair, " << itemdata.m_values_name << ") == false)" << std::endl;
						m_stream << "				return false;" << std::endl;
					}
				}
				m_stream << "			return true;" << std::endl;
				m_stream << "		}" << std::endl;
				m_stream << "	}" << std::endl;
			}
			m_stream << "}" << std::endl;
			ngl::writefile lfile(".\\idlfile\\" + item.first + ".cs");
			lfile.write(m_stream.str());
			//_h(".\\idlfile\\" + item.first, item.second, item.first == "game_db", item.first == "csvtable");
		}
	}


	void _nprotocol_auto()
	{
		ngl::writefile lfile("nprotocol_auto.h");
		std::stringstream m_stream;
		m_stream << m_tit;
		m_stream << R"(#pragma once

#include "ndb_modular.h"
#include "nsp_client.h"
#include "nsp_server.h"
#include "actor_db.h"
#include "db.pb.h"


namespace ngl
{
	void tprotocol_customs_200000000()
	{
		tprotocol::set_customs_index(200000000);
		// 新增内部协议需要补充
		tprotocol::tp_customsjson::template func <
)";
		int lindex = 200000000;
		std::map<std::string, idl_file>& lmap = idl::instance().data();
		bool isdouhao = false;
		for (std::pair<const std::string, idl_file>& item : lmap)
		{
			for (const auto& item2 : item.second.m_struct)
			{
				if (item2.name.find("np_") != std::string::npos)
				{
					std::cout << item2.name << std::endl;
					if (isdouhao)
					{
						m_stream << "			/*" << ++lindex << "*/, " << item2.name << std::endl;
						m_stream << "			/*" << ++lindex << ", np_mass_actor<" << item2.name << ">*/" << std::endl;
					}
					else
					{
						m_stream << "			/*" << ++lindex << "*/" << item2.name << std::endl;
						m_stream << "			/*" << ++lindex << ", np_mass_actor<" << item2.name << ">*/" << std::endl;
						isdouhao = true;
					}
				}				
			}
		}

		m_stream << R"(		>();)" << std::endl;
		m_stream << "		tprotocol::set_customs_index(210000000);" << std::endl;
		m_stream << "		tprotocol::tp_customsjson::template func<" << std::endl;
		lindex = 210000000;
		isdouhao = false;
		for (std::pair<const std::string, idl_file>& item : lmap)
		{
			for (const auto& item2 : item.second.m_struct)
			{
				if (item2.name.find("np_") != std::string::npos)
				{
					std::cout << item2.name << std::endl;
					if (isdouhao)
					{
						m_stream << std::format("			/*{}*/, mforward<{}>", ++lindex, item2.name) << std::endl;
						m_stream << std::format("			/*{}, np_mass_actor<mforward<{}>>", ++lindex, item2.name) << ">*/" << std::endl;
					}
					else
					{
						m_stream << std::format("			/*{}*/mforward<{}>", ++lindex, item2.name) << std::endl;
						m_stream << std::format("			/*{}, np_mass_actor<mforward<{}>>", ++lindex, item2.name) << ">*/" << std::endl;
						isdouhao = true;
					}
				}
			}
		}
		m_stream << R"(		>();)" << std::endl;
		m_stream << "	}" << std::endl;
		m_stream << "}//namespace ngl" << std::endl;
		lfile.write(m_stream.str());
	}

	void _auto_actor_enum()
	{
		ngl::writefile lfile("auto_actor_enum.h");
		std::stringstream m_stream;
		m_stream << m_tit;
		m_stream << R"(#pragma once

#include "nactortype.h"

#define em_events_null(NAME) null<NAME>,(ENUM_ACTOR)(ACTOR_EVENTS+ NAME::id_index()), #NAME
#define dautoactor(NAME, DEF) null<NAME>, em_pram(DEF)

namespace ngl
{
	template <typename TACTOR>
	void _auto_actor(const TACTOR* aactor, ENUM_ACTOR aenum, const char* aname)
	{
		em<ENUM_ACTOR>::set(aenum, aname);
		nactor_type<TACTOR>::inits(aenum);
	}

	template <typename TACTOR, typename ...ARG>
	void _auto_actor(const TACTOR* aactor, ENUM_ACTOR aenum, const char* aname, const ARG&... arg)
	{
		_auto_actor<TACTOR>(aactor, aenum, aname);
		_auto_actor(arg...);
	}
}//namespace ngl
)";
		std::map<std::string, idl_file>& lmap = idl::instance().data();
		for (std::pair<const std::string, idl_file>& item : lmap)
		{
			for (const auto& item2 : item.second.m_enum)
			{
				if (item2.name == "ENUM_ACTOR")
				{
					for (const auto& item3 : item2.dataVec)
					{
						if (
							item3.m_type == "ACTOR_NONE" || 
							item3.m_type == "ACTOR_EVENTS" || 
							item3.m_type == "ACTOR_EVENTS_MAX_COUNT" || 
							item3.m_type == "ACTOR_DB" || 
							item3.m_type == "ACTOR_SIGNLE_FINISH" || 
							item3.m_type == "ACTOR_COUNT" ||
							item3.m_type == "ACTOR_SIGNLE_START" ||
							item3.m_type == "+"
							)
						{
							continue;
						}
						std::string lname = item3.m_type;
						std::transform(lname.begin(), lname.end(), lname.begin(), [](unsigned char c) { return std::tolower(c); });
						m_stream << "#include \"" << lname << ".h\"" << std::endl;
					}
				}
			}
		}
		m_stream << R"(namespace ngl
{
	void auto_actor_enum()
	{
		_auto_actor(
)";
		bool isdouhao = false;
		for (std::pair<const std::string, idl_file>& item : lmap)
		{
			for (const auto& item2 : item.second.m_enum)
			{
				if (item2.name == "ENUM_ACTOR")
				{
					for (const auto& item3 : item2.dataVec)
					{
						if (
							item3.m_type == "ACTOR_NONE" ||
							item3.m_type == "ACTOR_EVENTS" ||
							item3.m_type == "ACTOR_EVENTS_MAX_COUNT" ||
							item3.m_type == "ACTOR_DB" ||
							item3.m_type == "ACTOR_SIGNLE_FINISH" ||
							item3.m_type == "ACTOR_COUNT" ||
							item3.m_type == "ACTOR_SIGNLE_START" ||
							item3.m_type == "+"
							)
						{
							continue;
						}
						std::string lname = item3.m_type;
						std::transform(lname.begin(), lname.end(), lname.begin(), [](unsigned char c) { return std::tolower(c); });
						std::cout << lname << std::endl;
						if (isdouhao)
						{
							m_stream << "			, dautoactor(" << lname << ", " << item3.m_type << ")" << std::endl;
						}
						else
						{
							m_stream << "			dautoactor(" << lname << ", " << item3.m_type << ")" << std::endl;
							isdouhao = true;
						}
					}
				}
			}
		}

		m_stream << R"(		);
	}
}//namespace ngl)";

		lfile.write(m_stream.str());
	}

	void _ttab()
	{
		std::map<std::string, idl_file>& lmap = idl::instance().data();
		for (std::pair<const std::string, idl_file>& item : lmap)
		{
			//Struct lStruct(lstr, item.second.m_struct, item.second.m_enum);
			std::set<std::string> lset;
			if (item.first == "csvtable")
			{
				idl_file& ltemp = item.second;
				for (auto& struc : ltemp.m_struct)
				{
					if (struc.name.find("tab_") == std::string::npos)
					{
						continue;
					}
					if (ngl::tools::file_exists(std::format("../../public/cpp/actor/template_tab/t{}.h", struc.name)) == false)
					{
						lset.insert(struc.name);
						ngl::writefile lfile(std::format("../../public/cpp/actor/template_tab/t{}.h", struc.name));


						std::string lcsvpp = std::format(R"(#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	class t{} :
		public manage_csv<{}>
	{{
		t{}(const t{}&) = delete;
		t{}& operator=(const t{}&) = delete;

		t{}()
		{{
			allcsv::loadcsv(this);
		}}

		void reload()final
		{{
			std::cout << "{} reload" << std::endl;
		}}	

	public:
		using type_tab = {};

		static t{}& instance()
		{{
			static t{} ltemp;
			return ltemp;
		}}

		std::map<int, {}>& tablecsv()
		{{
			t{}* ttab = allcsv::get<t{}>();
			if(ttab == nullptr);
			{
				tools::no_core_dump();
			}
			return ttab->m_tablecsv;
		}}

		const {}* tab(int32_t aid)
		{{
			auto lmap = tablecsv();
			auto itor = lmap.find(aid);
			if (itor == lmap.end())
			{{
				return nullptr;
			}}
			return &itor->second;
		}}
	}};
}//namespace ngl)", struc.name);
						lfile.write(lcsvpp.c_str());
					}
				}
			}
		}
	}
};




#endif//_IDLCPP_PROTOCOL_H_