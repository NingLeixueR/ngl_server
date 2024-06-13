#include "operator_file.h"
#include "tools.h"
#include "nlog.h"
#include "csv.h"
#include "md5.h"

namespace ngl
{
	void rcsv::read(const std::string& aname, std::string& averify)
	{
		//# 读取文件
		readfile lrf(aname);
		//# 跳过前3行表头
		lrf.jumpbegin(3, true);

		if (lrf.readcurrent(m_data) == false)
		{
			log_error()->print("!!!! {} !!!!", aname);
		}
		md5 lmd5(m_data);
		averify = lmd5.values();
		std::cout << std::format("loadcsv#{}#{}", aname, averify) << std::endl;
	}
}// namespace ngl