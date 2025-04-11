#ifndef _IDL_H_
#define _IDL_H_

#include <algorithm>
#include <iostream>
#include <fstream> 
#include <string>
#include <vector>
#include <regex>
#include <set>
#include <map>

#include "operator_file.h"
#include "tools.h"
using namespace std;

struct Data
{
	std::string m_modifier;			/* 修饰符 */
	std::string m_type;				/* 类型 **/
	std::string m_values_name;		/* 类型名称 **/
	std::string m_values_init;		/* 类型的初始值 **/
	std::string panduan;			/* 判断 **/
	std::string zhushi;				/* 注释 **/
	std::string m_typestr;			/* 数据库需要 给string添加长度 **/
	int m_index = 0;
	std::string m_load;

	bool operator<(const Data& ar)
	{
		return m_index < ar.m_index;
	}
};

struct EnumVec
{
	std::string name;				/* 枚举名称 **/
	std::vector<Data> dataVec;		/* 枚举数据 **/
};

struct StructVec
{
	std::string name;					/* 类名称 **/
	std::string derived1;
	std::string derived2;
	std::vector<Data> dataVec;			/* 类的数据 **/
};

struct DefVec
{
	std::string name;					/* define称 **/
	std::vector<Data> dataVec;			/* 类的数据 **/
};


class Inamespace
{
	std::string& m_str;
public:
	explicit Inamespace(std::string& astr):
		m_str(astr)
	{
		m_str += "namespace ngl{\n";
	}
	~Inamespace()
	{
		try
		{
			m_str += "}//namespace ngl\n";
		}
		catch (...)
		{
			return;
		}
	}
};

class Include
{
public:
	Include(std::string& astr, std::set<string>& ainclude)
	{
		std::stringstream m_stream;
		for (auto item : ainclude)
		{
			m_stream << item << "\n";
		}
		astr += m_stream.str();
	}
};

class Define
{
public:
	Define(std::string& astr, std::vector<DefVec>& adefine)
	{
		std::stringstream m_stream;
		for (auto item : adefine)
		{
			m_stream << item.name << "\n";
		}
	}
};

class Enum
{
	vector<EnumVec>& m_enum;
public:
	Enum(std::string& astr, vector<EnumVec>& aenum):
		m_enum(aenum)
	{
		std::stringstream m_stream;
		for (auto item : aenum)
		{
			m_stream << "enum " << item.name << "\n";
			m_stream << "{\n";
			for (Data item2 : item.dataVec)
			{
				if (item2.m_values_name.empty())
				{
					m_stream << "	" << item2.m_type << ",\t" << item2.zhushi << "\n";
				}
				else
				{
					m_stream << "	" << item2.m_type << " = " + item2.m_values_name << ",\t" << item2.zhushi << "\n";
				}
			}
			m_stream << "};\n";
		}
		astr += m_stream.str();
	}

	Enum(vector<EnumVec>& aenum) :
		m_enum(aenum)
	{
	}

	std::string _cs()
	{
		std::stringstream m_stream;
		for (auto item : m_enum)
		{
			m_stream << "	enum " << item.name << "\n";
			m_stream << "	{\n";
			for (Data item2 : item.dataVec)
			{
				if (item2.m_values_name.empty())
				{
					m_stream << "		" << item2.m_type << ",\t" << item2.zhushi << "\n";
				}
				else
				{
					std::string lvalues_name;
					ngl::tools::replace("::", ".", item2.m_values_name, lvalues_name);
					m_stream << "		" << item2.m_type << " = " + lvalues_name << ",\t" << item2.zhushi << "\n";
				}
			}
			m_stream << "	}\n\n";
		}
		return m_stream.str();
	}
};

class Struct
{
	std::string& m_str;
	std::vector<StructVec>& m_struct;
	std::vector<EnumVec>& m_enum;
public:
	Struct(std::string& astr, vector<StructVec>& astruct, vector<EnumVec>& aenum):
		m_struct(astruct),
		m_enum(aenum),
		m_str(astr)
	{
	}

	std::string get_type(Data& item2)
	{
		std::string ltempstr(item2.m_type);
		std::string ltempstr2;
		ngl::tools::replace_ret("map<", "std::map<", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::tools::replace_ret("list<", "std::list<", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::tools::replace_ret("vector<", "std::vector<", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::tools::replace_ret("set<", "std::set<", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		ngl::tools::replace_ret("string", "std::string", ltempstr, ltempstr2);
		ltempstr = ltempstr2;
		return ltempstr;
	}

	bool is_version(std::vector<Data>& dataVec)
	{
		for (Data item2 : dataVec)
		{
			if (item2.m_values_name == "m_version")
			{
				return true;
			}
		}
		return false;
	}
	
	void _h_serialize(std::stringstream& astream, StructVec& astruct)
	{
		astream << "	// 序列化反序列化相关" << std::endl;
		astream << "	def_portocol("<< astruct.name;
		for (Data item_ : astruct.dataVec)
		{
			if (item_.m_load == "n")
				continue;
			astream << ", " << item_.m_values_name;
		}
		astream << ")" << std::endl;

		if (astruct.name.find("tab_") == std::string::npos)
		{
			return;
		}
		// 生成csv表头
		std::stringstream lstreamcsv;
		for (int i = 0; i < astruct.dataVec.size(); ++i)
		{
			lstreamcsv << astruct.dataVec[i].m_type << ",";
		}
		lstreamcsv << std::endl;
		for (int i = 0; i < astruct.dataVec.size(); ++i)
		{
			lstreamcsv << astruct.dataVec[i].m_values_name << ",";
		}
		lstreamcsv << std::endl;
		for (int i = 0; i < astruct.dataVec.size(); ++i)
		{
			lstreamcsv << "\"" << (&astruct.dataVec[i].zhushi.c_str()[2]) << "\",";
		}
		lstreamcsv << std::endl;
		//lstreamcsv << "version:0";

		ngl::writefile lfilecsv("./idlfile/"+ astruct.name + ".csv");

		//## 转换为utf8
		std::string lstr;
		ngl::tools::to_utf8(lstreamcsv.str(), lstr);

		lfilecsv.write(lstr.c_str());
	}

	void _h_member(std::stringstream& astream, StructVec& astruct)
	{
		astream << "\t/*********************************/" << std::endl;
		for (Data item_ : astruct.dataVec)
		{
			item_.m_type = get_type(item_);
			if (item_.m_load == "n")
			{
				astream<< "//";
			}
			astream
				<< std::format(
					"\t{:32} {:32}; {}"
					, item_.m_type, item_.m_values_name, item_.zhushi
				)
				<< std::endl;
			
		}
		astream << "\t/*********************************/" << std::endl;

		astream << "	" << astruct.name << "();" << std::endl;

	}

	void _h_csv()
	{
		std::stringstream lstream;
		for (auto& item : m_struct)
		{
			lstream << "struct " << item.name << std::endl;
			lstream << "{" << std::endl;
			_h_member(lstream, item);
			_h_serialize(lstream, item);
			lstream << "	// csv相关" << std::endl;
			lstream << "	inline bool rcsv(ngl::csvpair& apair)" << std::endl;
			lstream << "	{" << std::endl;
			for (int i = 0; i < item.dataVec.size(); ++i)
			{
				if (item.dataVec[i].m_load == "n")
				{
					lstream << std::format("		{} l{};"
						, get_type(item.dataVec[i])
						, item.dataVec[i].m_values_name
					) << std::endl;
				}
			}
			lstream << "		def_rcsv2(";
			for (int i = 0, index = 0; i < item.dataVec.size(); ++i)
			{
				if (index != 0)
					lstream << ",";
				if (item.dataVec[i].m_load == "y" || item.dataVec[i].m_load == "")
				{
					lstream << item.dataVec[i].m_values_name;
					++index;
				}
				else
				{
					lstream << "l" << item.dataVec[i].m_values_name;
				}
			}
			lstream << ");"<< std::endl;
			lstream << "	}" << std::endl;
			lstream << "};\n";
		}
		m_str += lstream.str();
	}

	void _h_protocol()
	{
		std::stringstream lstream;
		for (auto& item : m_struct)
		{
			lstream << "struct " << item.name << std::endl;
			lstream << "{" << std::endl;
			_h_member(lstream, item);
			lstream << "	" << item.name << "();" << std::endl;
			_h_serialize(lstream, item);
			lstream << "};\n";
		}
		m_str += lstream.str();
	}

	void _cpp_constructor(std::stringstream& astream, StructVec& astruct)
	{
		astream << astruct.name << "::" << astruct.name << "()" << std::endl;
		astream << "{" << std::endl;
		for (Data item : astruct.dataVec)
		{
			if (item.m_values_init.empty() != true)
			{
				astream << "	" << item.m_values_name << item.m_values_init << ";\n";
			}
		}
		astream << "}" << std::endl;
	}

	void _cpp_serialize(std::stringstream& astream, StructVec& astruct)
	{
	}

	void _cpp_csv()
	{
		std::stringstream lstream;
		for (auto item : m_struct)
		{
			_cpp_constructor(lstream, item);
			_cpp_serialize(lstream, item);
		}
		m_str += lstream.str();
	}

	void _cpp_protocol()
	{
		std::stringstream lstream;
		for (auto& item : m_struct)
		{
			_cpp_constructor(lstream, item);
			_cpp_serialize(lstream, item);
		}
		m_str += lstream.str();
	}
};


struct idl_file
{
	vector<StructVec> m_struct;
	vector<EnumVec> m_enum;
	set<string> m_include;
	vector<DefVec> m_define;
	string m_fname;
};

class idl
{
	std::map<std::string, idl_file> m_data;
public:
	static idl& getInstance(bool arecreate = false)
	{
		static idl ltemp;
		if (arecreate)
		{
			ltemp = idl();
		}
		return ltemp;
	}

	std::map<std::string, idl_file>& data()
	{
		return m_data;
	}

	void analysis(std::string apth)
	{
		//// ----- 去掉文件后缀
		bool lsssbool = true;
		std::string lname;
		std::ranges::for_each(apth, [&lsssbool, &lname](char item)
			{
				if (item == '/' || item == '\\')
				{
					lname = "";
					lsssbool = true;
					return;
				}
				if (item == '.')
				{
					lsssbool = false;
					return;
				}
				if (lsssbool)
				{
					lname += item;
				}
			});
		m_data[lname].m_fname = lname;

		//// ---- 读取文件
		ngl::readfile lfile(apth);
		string ldata;
		lfile.read(ldata);

		_helpDefine(lname,ldata);
		_helpInclude(lname, ldata);
		_helpEnum(lname, ldata);
		_helpStruct(lname, ldata);
	}

	void _helpDefine(std::string& aname,string& ldata)
	{
		static string pattern("#define[ \t]+[a-zA-Z0-9_ ]+[ \t]+[(]+[^)^(]+[)]+[ \t]+[/][/][^\r^\n]*");
		ngl::tools::sregex(pattern, ldata, [this, aname](std::string& adata)
		{
			static string expression("#define[ \t]+([a-zA-Z0-9_ ]+)[ \t]+[(]+([^)]+)[)]+[ \t]+[/][/][^\r^\n]*");
			DefVec lDefVec;
			lDefVec.name = adata;
			ngl::tools::smatch(expression, adata, [&lDefVec](std::smatch& awhat)
			{
				Data ldataStr;
				ldataStr.m_values_name = awhat[1];
				ldataStr.m_values_init = awhat[2];
				lDefVec.dataVec.push_back(ldataStr);
			});
			m_data[aname].m_define.push_back(lDefVec);
		});
	}

	void _helpInclude(std::string& aname, string& ldata)
	{
		static string pattern("#include[ ]+[\"<][a-zA-Z0-9/]+(.[a-zA-Z]+)*[\">]");
		ngl::tools::sregex(pattern, ldata, [aname,this](std::string& adata)
		{
			m_data[aname].m_include.insert(adata);
		});
	}

	void _helpEnum(std::string& aname, string& ldata)
	{
		static string pattern("enum[ ][^{}]*[{][^{}]*[}][^;]*[;]");
		ngl::tools::sregex(pattern, ldata, [aname,this](std::string& adata)
		{
			EnumVec lenumString;
		//获取结构名称
		static string lpattern("enum[ \n\r\t]+([^ \n\r\t]+)");
		ngl::tools::smatch(lpattern, adata, [&lenumString](std::smatch& awhat)
			{
				lenumString.name = awhat[1];
			});
		static string lpattern2;
		if (lpattern2.empty())
		{
			lpattern2 += "[ \n\r\t]*";//空白
			lpattern2 += "([^ \n\r\t{}]*)";//type
			lpattern2 += "[ \t]*";//空白
			lpattern2 += "[=]*";
			lpattern2 += "[ \t]*";//空白
			lpattern2 += "([^ \t,{}]*)";//values
			lpattern2 += "[ \t]*";//空白
			lpattern2 += "[,]";
			lpattern2 += "[ \t]*";//空白
			lpattern2 += "([//]*[^\r\n]*)";//注释
		}
		ngl::tools::smatch(lpattern2, adata, [&lenumString](std::smatch& awhat)
			{
				Data ldataStr;
				ldataStr.m_type = awhat[1];
				ldataStr.m_values_name = awhat[2];
				ldataStr.m_values_init = "";
				ldataStr.zhushi = awhat[3];
				lenumString.dataVec.push_back(ldataStr);
			});

		m_data[aname].m_enum.push_back(lenumString);
			});
	}

	void _helpStruct(std::string& aname, string& ldata)
	{
		static string pattern("struct[^{}]*[{][^{}]*[}][^;]*[;]");
		ngl::tools::sregex(pattern, ldata, [aname,this](std::string& adata)
		{
			StructVec lstructString;
			//获取结构名称
			static string lpattern("struct[ \t]+([^ \t\n\r]+)");
			ngl::tools::smatch(lpattern, adata, [&lstructString](std::smatch& awhat)
			{
				lstructString.name = awhat[1];
				if (lstructString.name.size() > sizeof("DERIVED<")-1)
				{
					if (memcmp("DERIVED<", lstructString.name.c_str(), sizeof("DERIVED<")-1) == 0)
					{
						int lcount = 0;
						for (int i = 0; i < lstructString.name.size(); ++i)
						{
							if (lstructString.name[i] == '<')
							{
								++lcount;
							}
							else if (lstructString.name[i] != '>')
							{
								if (lcount == 1)
								{
									lstructString.derived1 += lstructString.name[i];
								}
								else if(lcount == 2)
								{
									lstructString.derived2 += lstructString.name[i];
								}
							}
							
						}
					}
				}
				
			});

			static std::string lpattern2;
			static std::string lpattern3;
			if (lpattern2.empty())
			{
				string lkb = "[ \t]*";
				string lhh = "[ \t\r\n]*";
				//  required关键字
				//	顾名思义，就是必须的意思，数据发送方和接收方都必须处理这个字段，不然还怎么通讯呢
				//	optional关键字
				//  字面意思是可选的意思

				lpattern2
					+= lkb + lhh + lkb
					//+ "(if[\(][^\(\)]*[\)])*"
					+ "(if[(][^)]*[)])*"
					+= lkb + lhh + lkb
					+ "(required|optional)*"	//修饰符
					+ lkb
					+ "([^ <>\r\n};()]+)" //类型
					+ "[ ]"
					+ "([^ \t<>\r\n};]+)" //类型名
					+ lkb
					+ "([ ]+[=][ ][^;]*)*"
					+ "[;]"
					+ lkb
					+ "([//]*[^\r\n]*)" //注释
					+ "[\r\n]*[\r\n]*";

				lpattern3
					+= lkb + lhh + lkb
					+ "(if[(][^)]*[)])*"
					+ lkb + lhh + lkb
					+ "(required|optional)*"	//修饰符
					+ lkb
					+ "(derived_class[<][^>]+[>]|map[<][^>]+[>]|vector[<][^>]+[>]|list[<][^>]+[>]|set[<][^>]+[>]|string[<][^>]+[>]|int[<][^>]+[>])" //类型
					+ "[ ]"
					+ "([^ \t<>\r\n};]+)" //类型名
					+ lkb
					+ "([=][^;]+)*"
					+ "[;]"
					+ lkb
					+ "([//]*[^\r\n]*)" //注释
					+ "[\r\n]*[\r\n]*";
			}

			ngl::tools::smatch(lpattern2, adata, [&lstructString](std::smatch& awhat)
			{
				Data ldataStr;
				ldataStr.panduan = awhat[1];
				if (awhat[2] == "")
				{
					ldataStr.m_modifier = "required";
				}
				else
				{
					ldataStr.m_modifier = awhat[2];
				}
				ldataStr.m_type = awhat[3];
				ldataStr.m_values_name = awhat[4];
				ldataStr.m_values_init = awhat[5];
				ldataStr.zhushi = awhat[6];

				ngl::tools::smatch("index:([0-9]+)", ldataStr.zhushi, [&ldataStr](std::smatch& awhat)
					{
						ldataStr.m_index = ngl::tools::lexical_cast<int32_t>(awhat[1].str().c_str());
					});
				ngl::tools::smatch("load:([yn]+)", ldataStr.zhushi, [&ldataStr](std::smatch& awhat)
					{
						ldataStr.m_load = awhat[1].str().c_str();
					});

				lstructString.dataVec.push_back(ldataStr);
			});

			ngl::tools::smatch(lpattern3, adata, [&lstructString](std::smatch& awhat)
			{
				Data ldataStr;
				ldataStr.panduan = awhat[1];
				if (awhat[2] == "")
				{
					ldataStr.m_modifier = "required";
				}
				else
				{
					ldataStr.m_modifier = awhat[2];
				}
				ldataStr.m_type = awhat[3];
				ldataStr.m_values_name = awhat[4];
				ldataStr.m_values_init = awhat[5];
				ldataStr.zhushi = awhat[6];

				ngl::tools::smatch("index:([0-9]+)", ldataStr.zhushi, [&ldataStr](std::smatch& awhat)
					{
						ldataStr.m_index = ngl::tools::lexical_cast<int32_t>(awhat[1].str().c_str());
					});
				ngl::tools::smatch("load:([yn]+)", ldataStr.zhushi, [&ldataStr](std::smatch& awhat)
					{
						ldataStr.m_load = awhat[1].str().c_str();
					});


				if (ldataStr.m_type.size() > sizeof("derived_class<") - 1)
				{
					if (memcmp("derived_class<", ldataStr.m_type.c_str(), sizeof("derived_class<") - 1) == 0)
					{
						ldataStr.m_type = "ngl::" + ldataStr.m_type;
					}
				}
				lstructString.dataVec.push_back(ldataStr);

				
			});

			m_data[aname].m_struct.push_back(lstructString);
		});
		for (int i = 0; i < m_data[aname].m_struct.size(); ++i)
		{
			size_t lpos = m_data[aname].m_struct[i].name.find("tab_");
			if (lpos >= 0)
			{
				
				std::set<int32_t> lset;
				bool lbool = true;
				std::ranges::for_each(m_data[aname].m_struct[i].dataVec, [&lset,&lbool](const auto& adata)
					{
						if (lbool)
						{
							lbool = lset.insert(adata.m_index).second;
						}				
					});

				//assert(lbool);

				std::sort(m_data[aname].m_struct[i].dataVec.begin(), m_data[aname].m_struct[i].dataVec.end());

			}			
		}
	}

};


#endif//_IDL_H_