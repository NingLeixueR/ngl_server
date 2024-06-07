#include "operator_file.h"
#include "tools.h"
#include "nlog.h"
#include "csv.h"


namespace ngl
{
	
	void rcsv::read(const std::string& aname, int32_t& aversion)
	{
		//# 读取文件
		readfile lrf(aname);
		//# 跳过前3行表头
		lrf.jumpbegin(3, true);

		//# 读取第4行reload csv version
		std::string lversion;
		lrf.readline(lversion);

		Try
		{
			std::vector<std::string>  lvec;
			if (tools::splite(lversion.c_str(), ",", lvec) == false)
				return;
			Assert(lvec.empty() == false);
			std::string lver;
			if (tools::splite(lvec[0].c_str(), ":", lver, aversion) == false)
				return;
			Assert(lver == "version");
		}Catch;


		if (lrf.readcurrent(m_data) == false)
		{
			log_error()->print("!!!! {} !!!!", aname);
		}
	}
}// namespace ngl