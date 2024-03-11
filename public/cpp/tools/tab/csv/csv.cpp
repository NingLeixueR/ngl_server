#include "operator_file.h"
#include "splite.h"
#include "tools.h"
#include "nlog.h"
#include "csv.h"

#include <boost/lexical_cast.hpp>

namespace ngl
{
	csvpair::csvpair() :
		m_pos(0),
		m_doublequotationmarks(false),
		m_fg(',')
	{}

	void csvpair::clear()
	{
		m_data.clear();
		m_pos = 0;
		m_doublequotationmarks = false;
	}

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
			splite::division(lversion.c_str(), ",", lvec);
			Assert(lvec.empty() == false);
			std::string lver;
			splite::division(lvec[0].c_str(), ":", lver, aversion);
			Assert(lver == "version");
		}Catch;
		
		std::pair<char*, int> lpair = lrf.readcurrent();
		if (lpair.second > 0)
		{
			m_data = lpair.first;
		}
		delete[] lpair.first;
	}

	bool rcsv::readline(std::string& adata)
	{
		adata.clear();
		bool lbool = false;
		for (; m_pos < m_data.size(); ++m_pos)
		{
			if (!lbool)
			{
				if (m_data[m_pos] == '\r')	continue;
				if (m_data[m_pos] == '\n')
				{
					++m_pos;
					return true;
				}
			}
			if (m_data[m_pos] == '\"')	lbool = lbool ? false : true;;
			adata += m_data[m_pos];
		}
		return !adata.empty();
	}

	void rcsv::read(csvpair& apair, std::string& ltemp)
	{
		std::string& lret = apair.m_data;
		int& lpos = apair.m_pos;
		for (; lpos < lret.size(); ++lpos)
		{
			if (!apair.m_doublequotationmarks)
			{
				if (lret[lpos] == apair.m_fg)
				{
					++lpos;
					break;
				}
				if (lret[lpos] == '\n')
				{
					++lpos;
					break;
				}
				if (lret[lpos] == '\r') continue;
			}
			if (lret[lpos] == '\"')
			{
				apair.m_doublequotationmarks = apair.m_doublequotationmarks ? false : true;
				continue;
			}
			ltemp += lret[lpos];
		}
	}

	rcsv::rcsv(const std::string& afilename, int32_t& aversion) :
		m_pos(0)
	{
		read(afilename, aversion);
	}

	rcsv::rcsv() :
		m_pos(0) 
	{}

	bool rcsv::isok(csvpair& apair)
	{
		return apair.m_pos >= apair.m_data.size();
	}

	// 基本类型
	template <typename TNUMBER>
	static bool number_csv(csvpair& apair, TNUMBER& adata)
	{
		std::string ltemp;
		rcsv::read(apair, ltemp);
		if (ltemp != "")
			adata = boost::lexical_cast<TNUMBER>(ltemp);
		else
			adata = TNUMBER();
		return true;
	}

	bool rcsv::readcsv(csvpair& apair, int8_t& adata) 
	{ 
		return number_csv(apair, adata); 
	}

	bool rcsv::readcsv(csvpair& apair, int16_t& adata) 
	{ 
		return number_csv(apair, adata); 
	}

	bool rcsv::readcsv(csvpair& apair, int32_t& adata) 
	{ 
		return number_csv(apair, adata); 
	}

	bool rcsv::readcsv(csvpair& apair, int64_t& adata) 
	{ 
		return number_csv(apair, adata); 
	}

	bool rcsv::readcsv(csvpair& apair, uint8_t& adata) 
	{ 
		return number_csv(apair, adata); 
	}

	bool rcsv::readcsv(csvpair& apair, uint16_t& adata) 
	{ 
		return number_csv(apair, adata); 
	}

	bool rcsv::readcsv(csvpair& apair, uint32_t& adata) 
	{ 
		return number_csv(apair, adata); 
	}

	bool rcsv::readcsv(csvpair& apair, uint64_t& adata) 
	{ 
		return number_csv(apair, adata); 
	}

	bool rcsv::readcsv(csvpair& apair, float& adata) 
	{ 
		return number_csv(apair, adata); 
	}

	bool rcsv::readcsv(csvpair& apair, double& adata) 
	{ 
		return number_csv(apair, adata); 
	}

	bool rcsv::readcsv(csvpair& apair, bool& adata) 
	{ 
		return number_csv(apair, adata); 
	}

	bool rcsv::readcsv(csvpair& apair, std::string& adata) 
	{ 
		read(apair, adata); 
		return true; 
	}

	bool rcsv::readveccsv(csvpair& apair, int8_t& adata) 
	{ 
		return readcsv(apair, adata); 
	}
}// namespace ngl