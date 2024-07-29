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

void find(
	bool awz, 
	const std::string& atxt, 
	const std::string& targetPath, 
	std::set<std::string>& adir, 
	std::map<std::string, int>& avec1
)
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
			find(awz, atxt, lpath, adir, avec1);
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
				// 文件行数
				ngl::readfile lrf(lpath);
				int lmaxline = lrf.get_maxline();


				if (awz)
				{
					avec1.insert(std::make_pair(targetPath + lname, lmaxline));
				}
				else
				{
					avec1.insert(std::make_pair(lname, lmaxline));
				}
				continue;
			}
		}
	}
}

int main(int argc, char** argv)
{
	std::map<std::string, int> lvec1;
	std::map<std::string, int> lvec3;
	std::map<std::string, int> lvec5;
	std::map<std::string, int> lvec6;
	std::set<std::string> ldic;

	std::stringstream m_streamtxt;
	for (int i = 2; i < argc; ++i)
	{
		std::string targetPath = argv[i];
		ldic.insert(targetPath);
		find(false, ".cpp", targetPath, ldic, lvec1);
		find(false, ".c", targetPath, ldic, lvec5);
		//find(false, ".h", targetPath, ldic, lvec3);
	}

	std::map<int, std::map<std::string,int>> lmap;
	for (auto& item : lvec1)
	{
		lmap[item.first.size()].insert(item);
	}
	int llinecount = 0;
	auto lsavefun = [&argv](int aindex, std::stringstream& astream)
	{
			std::string cname = std::format("{}_{}.cpp", argv[1], aindex);
			ngl::writefile lfile(cname);
			lfile.write(astream.str());
	};
	auto lmalloc = []()->std::stringstream*
		{
			std::stringstream* m_stream = new std::stringstream();
			*m_stream << "// 注意【rebuild.bat 工具生成文件，不要手动修改】" << std::endl;
			char ltmbuff[1024] = { 0 };
			ngl::localtime::time2str(ltmbuff, 1024, ngl::localtime::gettime(), "// 创建时间 %y-%m-%d %H:%M:%S");
			*m_stream << ltmbuff << std::endl;
			return m_stream;
		};

	std::stringstream* m_stream = lmalloc();
	int lindex = 0;
	for (auto itor1 = lmap.rbegin(); itor1 != lmap.rend(); ++itor1)
	{
		for (auto itor2 = itor1->second.rbegin(); itor2!= itor1->second.rend();++itor2)
		{
			if (llinecount > 100000)
			{
				lsavefun(++lindex, *m_stream);
				m_stream = lmalloc();
				llinecount = 0;
			}
			
			*m_stream << "#include \"" << itor2->first << "\"\n";
			llinecount += itor2->second;
		}	
	}
	lsavefun(++lindex, *m_stream);

	m_stream = lmalloc();

	*m_stream << "extern \"C\"{\n";
	for (auto item : lvec5)
		*m_stream << "#include \"" << item.first << "\"\n";
	*m_stream << "}//extern \"C\"\n";
	lsavefun(++lindex, *m_stream);

	for (auto item : ldic)
		m_streamtxt << "INCLUDE_DIRECTORIES(" << item << ")\n";

	std::string cname = argv[1];
	cname += ".txt";
	ngl::writefile lfiletxt(cname);
	lfiletxt.write(m_streamtxt.str());
	std::cout << "##############" << cname << std::endl;

	return 0;
}
