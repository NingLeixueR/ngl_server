#include "operator_file.h"
#include "tools.h"
#include "nlog.h"
#include "csv.h"
#include "md5.h"

namespace ngl
{
	bool rcsv::read(const std::string& aname, std::string& averify)
	{
		//# 读取文件
		readfile lrf(aname);
		//# 跳过前3行表头
		lrf.jumpbegin(3, true);

		if (lrf.readcurrent(m_data) == false)
		{
			std::cout << std::format("loadcsv fail #{}", aname) << std::endl;
			return false;
		}
		averify = tools::md5(m_data);
		std::cout << std::format("loadcsv#{}#{}", aname, averify) << std::endl;
		return true;
	}
}// namespace ngl