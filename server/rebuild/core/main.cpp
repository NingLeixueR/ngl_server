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
#include "tools.h"

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
	std::map<std::string, int>& avec1,
	std::set<std::string>& avec2
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
			if (lpath.find("third_party") != std::string::npos)
			{
				continue;
			}

			std::string lpath2 = &lpath.c_str()[sizeof("../../")-1];
			std::cout << "dir:[" << lpath2 << "]" << std::endl;
			adir.insert(lpath2);
			find(awz, atxt, lpath, adir, avec1, avec2);
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
					avec1.insert(std::make_pair(targetPath + '/' + lname, lmaxline));
				}
				else
				{
					avec1.insert(std::make_pair(lname, lmaxline));
				}
				avec2.insert(targetPath + '/' + lname);
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
	std::set<std::string> ldic;

	std::set<std::string> lset_head;

	std::stringstream m_streamtxt;
	for (int i = 2; i < argc; ++i)
	{
		std::string targetPath = argv[i];
		ldic.insert(targetPath);
		find(false, ".cpp", targetPath, ldic, lvec1, lset_head);
		find(false, ".c", targetPath, ldic, lvec5, lset_head);
		find(false, ".h", targetPath, ldic, lvec3, lset_head);
	}

	std::map<int, std::map<std::string,int>> lmap;
	std::map<int, std::map<std::string, int>> lmap2;
	for (auto& item : lvec1)
	{
		if (item.first.find("actor_") == std::string::npos) {
			lmap[item.first.size()].insert(item);
		}
		else
		{
			lmap2[item.first.size()].insert(item);
		}		
	}

	auto lsavefun = [&argv](int aindex, std::stringstream& astream)
	{
			std::string cname = std::format("{}_{}.cpp", argv[1], aindex);
			ngl::writefile lfile(cname);
			lfile.write(astream.str());
	};
	auto lmalloc = [/*&lmapinclude*/]()->std::stringstream*
		{
			std::stringstream* m_stream = new std::stringstream();
			*m_stream << "// 注意【rebuild.bat 工具生成文件，不要手动修改】" << std::endl;
			char ltmbuff[1024] = { 0 };
			ngl::localtime::time2str(ltmbuff, 1024, ngl::localtime::gettime(), "// 创建时间 %y-%m-%d %H:%M:%S");
			*m_stream << ltmbuff << std::endl;
			*m_stream << "#include \"pb_field.cpp\"" << std::endl;
			return m_stream;
		};

	int lindex = 0; 
	auto foreach_map = [&](std::stringstream* astream, std::map<int, std::map<std::string, int>>& amap)
		{
			int llinecount = 0;
			for (auto itor1 = amap.rbegin(); itor1 != amap.rend(); ++itor1)
			{
				for (auto itor2 = itor1->second.rbegin(); itor2 != itor1->second.rend(); ++itor2)
				{
					//if (llinecount > 2000000000)
					//{
					//	lsavefun(++lindex, *astream);
					//	std::cout << std::format("index[{}] line:[{}] ", lindex, llinecount) << std::endl;
					//	astream = lmalloc();
					//	llinecount = 0;
					//}
					//
					*astream << "#include \"" << itor2->first << "\"\n";
					llinecount += itor2->second;
				}
			}
			//if (llinecount > 0)
			//{
			//	lsavefun(++lindex, *astream);
			//}
		};

	std::stringstream* lstream = lmalloc();
	foreach_map(lstream, lmap);
	foreach_map(lstream, lmap2);
	
	//std::stringstream* m_stream = lmalloc();
	*lstream << "extern \"C\"{\n";
	for (const auto& item : lvec5)
		*lstream << "#include \"" << item.first << "\"\n";
	*lstream << "}//extern \"C\"\n";
	lsavefun(++lindex, *lstream);

	for (const auto& item : ldic)
		m_streamtxt << "INCLUDE_DIRECTORIES(" << item << ")\n";

	std::string cname = argv[1];
	cname += ".txt";
	ngl::writefile lfiletxt(cname);
	lfiletxt.write(m_streamtxt.str());
	std::cout << "##############" << cname << std::endl;

	int32_t lsumline = 0;
	std::ranges::for_each(lvec1, [&lsumline](const auto& apair)
		{
			lsumline += apair.second;
		});
	std::ranges::for_each(lvec3, [&lsumline](const auto& apair)
		{
			lsumline += apair.second;
		});
	std::ranges::for_each(lvec5, [&lsumline](const auto& apair)
		{
			lsumline += apair.second;
		});
	std::cout << "当前源码行数：" << lsumline << std::endl;


	std::string lhead = R"(/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
)";
	lhead += "\r\n";
	for (const std::string& item : lset_head)
	{
		ngl::readfile lfiletxt(item);
		std::string lneirong;
		lfiletxt.read(lneirong);

		if (!lneirong.empty())
		{
			if (lneirong.find("Copyright (c) [2020-2025] NingLeixueR") != std::string::npos)
			{
				continue;
			}
			if (item.find("third_party") != std::string::npos)
			{
				continue;
			}
			if (item.find("pb.h") != std::string::npos)
			{
				continue;
			}
			std::cout << "+head [" << item << std::endl;
			ngl::writefile lwritetxt(item);
			lwritetxt.write(lhead);
			lwritetxt.write(lneirong);
		}
	}

	// 将所有文件转换为utf8格式
	std::cout << "to utf8..." << std::endl;
	for (const std::string& item : lset_head)
	{
		std::string lneirong;
		{
			ngl::readfile lfiletxt(item);
			lfiletxt.read(lneirong);
			ngl::tools::to_utf8(lneirong, lneirong);
		}
		{
			ngl::writefile lwritetxt(item);
			lwritetxt.write(lneirong);
		}
		std::cout << item << ":fnish" << std::endl;
	}

	return 0;
}
