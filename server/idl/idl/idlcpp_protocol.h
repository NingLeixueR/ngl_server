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
	
};




#endif//_IDLCPP_PROTOCOL_H_