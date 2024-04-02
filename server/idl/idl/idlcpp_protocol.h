#ifndef _IDLCPP_PROTOCOL_H_
#define _IDLCPP_PROTOCOL_H_

#include "idl.h"
#include "localtime.h"

class idlcppprotocol
{
	idlcppprotocol() 
	{
		char ltmbuff[1024] = { 0 };
		ngl::localtime::time2str(ltmbuff, 1024, ngl::localtime::gettime(), "// 创建时间 %y-%m-%d %H:%M:%S");
		m_tit = "// 注意【rebuild.bat 工具生成文件，不要手动修改】\n";
		m_tit += "// 创建时间 ";
		m_tit += ltmbuff;
		m_tit += "\n";
	}

	std::string m_tit;
public:
	static idlcppprotocol& getInstance()
	{
		static idlcppprotocol ltemp;
		return ltemp;
	}

	void _h()
	{
		std::map<std::string, idl_file>& lmap = idl::getInstance().data();
		for (std::pair<const std::string, idl_file>& item : lmap)
		{
			std::string lstr;
			lstr += m_tit;
			Ifndef* lIfndef = new Ifndef(lstr, item.first);
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
			delete lIfndef;
			ngl::writefile lfile(".\\idlfile\\" + item.first + ".h");
			lfile.write(lstr);
			//_h(".\\idlfile\\" + item.first, item.second, item.first == "game_db", item.first == "csvtable");
		}
	}

	
	void _cpp()
	{
		std::map<std::string, idl_file>& lmap = idl::getInstance().data();
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

			
			ngl::writefile lfile(".\\idlfile\\" + item.first + ".cpp");
			lfile.write(lstr);
			//_cpp(".\\idlfile\\" + item.first, item.second, item.first == "game_db", item.first == "csvtable");
		}
	}

	std::string cs_get_type(Data& item2)
	{
		std::string ltempstr(item2.m_type);
		std::string ltempstr2;
		ngl::regular::replace_ret("map<", "Dictionary<", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::regular::replace_ret("list<", "List<", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::regular::replace_ret("vector<", "List<", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::regular::replace_ret("set<", "List<", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::regular::replace_ret("string", "string", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::regular::replace_ret("uint8_t", "byte", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::regular::replace_ret("int8_t", "sbyte", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::regular::replace_ret("uint16_t", "UInt16", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::regular::replace_ret("int16_t", "Int16", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::regular::replace_ret("UInt32", "uint32_t", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::regular::replace_ret("int32_t", "Int32", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::regular::replace_ret("uint64_t", "UInt64", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::regular::replace_ret("int64_t", "Int64", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		return ltempstr;
	}




	void _cs()
	{
		std::map<std::string, idl_file>& lmap = idl::getInstance().data();
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
	
};




#endif//_IDLCPP_PROTOCOL_H_