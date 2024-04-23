#include <type_traits>
#include <filesystem>
#include <functional>
#include <algorithm>
#include <iostream>
#include <typeinfo>
#include <fstream> 
#include <ranges>
#include <memory> 
#include <string>
#include <vector> 
#include <regex>
#include <set>
#include <map>

#include <boost/lexical_cast.hpp> 
#include <boost/filesystem.hpp>

#include "operator_file.h"
#include "localtime.h"
#include "impl.h"


bool is_sname(const std::string& astrname, const std::string& akey)
{
	auto itor1 = astrname.rbegin();
	auto itor2 = akey.rbegin();
	for (; itor1 != astrname.rend() && itor2 != akey.rend(); ++itor1, ++itor2)
	{
		if (*itor1 != *itor2)
		{
			break;
		}
	}
	if (itor2 == akey.rend())
		return true;
	return false;
}

void find(bool awz, const std::string& atxt, const std::string& targetPath, std::set<std::string>& adir, std::set<std::string>& avec1, std::set<std::string>& avec2)
{
	std::filesystem::path myPath(targetPath);
	std::filesystem::recursive_directory_iterator endIter;
	for (std::filesystem::recursive_directory_iterator iter(myPath); iter != endIter; iter++)
	{
		if (std::filesystem::is_directory(*iter))
		{
			std::string lpath = "";
			for (char ch : iter->path().string())
			{
				lpath += (ch == '\\' ? '/' : ch);
			}

			std::string lpath2 = &lpath.c_str()[sizeof("../../")-1];
			std::cout << "dir:[" << lpath2 << "]" << std::endl;
			adir.insert(lpath2);
			find(awz, atxt, iter->path().string(), adir, avec1, avec2);
		}
		else
		{
			std::cout << "file:[" << iter->path().string() << "]" << std::endl;
			std::string lpath = iter->path().string();
			std::string lname;
			for (auto itor = lpath.begin(); itor != lpath.end(); ++itor)
			{
				if (*itor == '/' || *itor == '\\')
					lname = "";
				else
					lname += *itor;
			}
			
			if (is_sname(lname, atxt))
			{
				if (awz)
					avec1.insert(targetPath + lname);
				else
					avec1.insert(lname);
				continue;
			}
		}
	}
}


int main(int argc, char** argv)
{
	std::set<std::string> lvec1;
	std::set<std::string> lvec2;
	std::set<std::string> lvec3;
	std::set<std::string> lvec4;
	std::set<std::string> lvec5;
	std::set<std::string> lvec6;
	std::set<std::string> ldic;

	std::stringstream m_stream;
	std::stringstream m_streamtxt;
	for (int i = 2; i < argc; ++i)
	{
		std::string targetPath = argv[i];
		ldic.insert(targetPath);
		find(false, ".cpp", targetPath, ldic, lvec1, lvec2);
		find(false, ".c", targetPath, ldic, lvec5, lvec6);
		find(false, ".h", targetPath, ldic, lvec3, lvec4);
	}

	m_stream << "// 注意【rebuild.bat 工具生成文件，不要手动修改】" << std::endl;
	char ltmbuff[1024] = { 0 };
	ngl::localtime::time2str(ltmbuff, 1024, ngl::localtime::gettime(), "// 创建时间 %y-%m-%d %H:%M:%S");
	m_stream << ltmbuff << std::endl;

	for (auto item : lvec1)
		m_stream << "#include \"" << item << "\"\n";
	for (auto item : lvec2)
		m_stream << "#include \"" << item << "\"\n";

	m_stream << "extern \"C\"{\n";
	for (auto item : lvec5)
		m_stream << "#include \"" << item << "\"\n";
	m_stream << "}//extern \"C\"\n";

	for (auto item : ldic)
		m_streamtxt << "INCLUDE_DIRECTORIES(" << item << ")\n";

	std::string cname = argv[1];
	ngl::writefile lfile(cname);
	lfile.write(m_stream.str());
	cname += ".txt";
	ngl::writefile lfiletxt(cname);
	lfiletxt.write(m_streamtxt.str());
	std::cout << "##############" << cname << std::endl;

	return 0;
}
