#pragma once

#include "operator_file.h"
#include "nprotocol.h"
#include "tools.h"
#include "nlog.h"

#include <functional>
#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>

namespace ngl
{
	struct csvpair
	{
		std::string m_data;
		int m_pos;
		bool m_doublequotationmarks; // 双引号
		char m_fg;//默认,

		csvpair() :
			m_pos(0),
			m_doublequotationmarks(false),
			m_fg(',')
		{}

		void clear()
		{
			m_data.clear();
			m_pos = 0;
			m_doublequotationmarks = false;
		}
	};
	
	template <typename E, bool IS_ENUM>
	class enum_operator_readcsv
	{
	public:
		static bool operator_readcsv(csvpair& apair, E& adata)
		{
			return adata.rcsv(apair);
		}
	};

	class rcsv
	{
	public:
		std::string m_data;
		int m_pos;
	public:
		bool read(const std::string& aname, std::string& averify);

		bool readline(std::string& adata)
		{
			adata.clear();
			bool lbool = false;
			int lsize = m_data.size();
			for (; m_pos < lsize; ++m_pos)
			{
				if (!lbool)
				{
					if (m_data[m_pos] == '\r')	
						continue;
					if (m_data[m_pos] == '\n')
					{
						++m_pos;
						return true;
					}
				}
				if (m_data[m_pos] == '\"')	
					lbool = lbool ? false : true;
				adata += m_data[m_pos];
			}
			return !adata.empty();
		}

		static void read(csvpair& apair, std::string& ltemp)
		{
			std::string& lret = apair.m_data;
			int& lpos = apair.m_pos;
			int lsize = lret.size();
			for (; lpos < lsize; ++lpos)
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
					if (lret[lpos] == '\r')
						continue;
				}
				if (lret[lpos] == '\"')
				{
					apair.m_doublequotationmarks = apair.m_doublequotationmarks ? false : true;
					continue;
				}
				ltemp += lret[lpos];
			}
		}

		rcsv() :
			m_pos(0)
		{
		}

		// 基本类型
		template <typename TNUMBER>
		static bool number_csv(csvpair& apair, TNUMBER& adata)
		{
			std::string ltemp;
			rcsv::read(apair, ltemp);
			if (ltemp != "")
				adata = tools::lexical_cast<TNUMBER>(ltemp.c_str());
			else
				adata = TNUMBER();
			return true;
		}

		static bool readcsv(csvpair& apair, int8_t& adata)
		{
			return number_csv(apair, adata);
		}

		static bool readcsv(csvpair& apair, int16_t& adata)
		{
			return number_csv(apair, adata);
		}

		static bool readcsv(csvpair& apair, int32_t& adata)
		{
			return number_csv(apair, adata);
		}

		static bool readcsv(csvpair& apair, int64_t& adata)
		{
			return number_csv(apair, adata);
		}

		static bool readcsv(csvpair& apair, uint8_t& adata)
		{
			return number_csv(apair, adata);
		}

		static bool readcsv(csvpair& apair, uint16_t& adata)
		{
			return number_csv(apair, adata);
		}

		static bool readcsv(csvpair& apair, uint32_t& adata)
		{
			return number_csv(apair, adata);
		}

		static bool readcsv(csvpair& apair, uint64_t& adata)
		{
			return number_csv(apair, adata);
		}

		static bool readcsv(csvpair& apair, float& adata)
		{
			return number_csv(apair, adata);
		}

		static bool readcsv(csvpair& apair, double& adata)
		{
			return number_csv(apair, adata);
		}

		static bool readcsv(csvpair& apair, bool& adata)
		{
			return number_csv(apair, adata);
		}

		static bool readcsv(csvpair& apair, std::string& adata)
		{
			read(apair, adata);
			return true;
		}

		template <typename T, bool BASE_TYPE>
		class readcsv_stl
		{
		public:
			static bool fun(csvpair& apair, const std::function<void(T&)>& afun)
			{
				std::string ltempstr;
				read(apair, ltempstr);
				csvpair lpair;
				lpair.m_data = ltempstr;
				for (; lpair.m_pos < lpair.m_data.size();)
				{
					T ltemp;
					int32_t lret = readveccsv(lpair, ltemp);
					if (lret == -1)
						return false;
					if (lret == 0)
						afun(ltemp);
					else if (lret == 1)
						break;
					else
						return false;
				}
				return true;
			}
		};

		template <typename T>
		class readcsv_stl<T, false>
		{
		public:
			static bool fun(csvpair& apair, const std::function<void(T&)>& afun)
			{
				std::string ltempstr;
				read(apair, ltempstr);
				csvpair lpair;
				lpair.m_data = ltempstr;
				for (; lpair.m_pos < lpair.m_data.size();)
				{
					T ltemp;
					if (readcsv(lpair, ltemp))
						afun(ltemp);
				}
				return true;
			}
		};
		
		template <typename T>
		static bool readcsv(csvpair& apair, std::vector<T>& avec)
		{
			return readcsv_stl<T, std::is_class<T>::value>::fun(apair, [&avec](T& adata)
				{
					avec.push_back(adata);
				});
		}

		template <typename T>
		static bool readcsv(csvpair& apair, std::list<T>& avec)
		{
			return readcsv_stl<T, std::is_class<T>::value>::fun(apair, [&avec](T& adata)
				{
					avec.push_back(adata);
				});
		}

		template <typename T>
		static bool readcsv(csvpair& apair, std::set<T>& avec)
		{
			return readcsv_stl<T, std::is_class<T>::value>::fun(apair, [&avec](T& adata)
				{
					avec.insert(adata);
				});
			return true;
		}

		template <typename T>
		static bool readcsv(csvpair& apair, T& adata)
		{
			ngl::csvpair lcpair;
			lcpair.m_fg = '*';
			if (readcsv(apair, lcpair.m_data) == false)
				return false;
			if (lcpair.m_data == "")
				return true;
			return enum_operator_readcsv<T, std::is_enum<T>::value>::operator_readcsv(lcpair, adata);
		}

		template <typename T>
		static int readveccsv(csvpair& apair, T& adata)
		{
			ngl::csvpair lcpair;
			lcpair.m_fg = '*';
			if (readcsv(apair, lcpair.m_data) == false)
				return -1;
			if (lcpair.m_data == "")
				return 1;
			if (enum_operator_readcsv<T, std::is_enum<T>::value>::operator_readcsv(lcpair, adata) == false)
				return -1;
			//adata.rcsv(lcpair);
			return 0;
		}
		
		static bool readveccsv(csvpair& apair, int8_t& adata)
		{
			return readcsv(apair, adata);
		}

		static bool readveccsv(csvpair& apair, int16_t& adata)
		{
			return readcsv(apair, adata);
		}

		static bool readveccsv(csvpair& apair, int32_t& adata)
		{
			return readcsv(apair, adata);
		}

		static bool readveccsv(csvpair& apair, int64_t& adata)
		{
			return readcsv(apair, adata);
		}

		static bool readveccsv(csvpair& apair, uint8_t& adata)
		{
			return readcsv(apair, adata);
		}

		static bool readveccsv(csvpair& apair, uint16_t& adata)
		{
			return readcsv(apair, adata);
		}

		static bool readveccsv(csvpair& apair, uint32_t& adata)
		{
			return readcsv(apair, adata);
		}

		static bool readveccsv(csvpair& apair, uint64_t& adata)
		{
			return readcsv(apair, adata);
		}

		static bool readveccsv(csvpair& apair, float& adata)
		{
			return readcsv(apair, adata);
		}

		static bool readveccsv(csvpair& apair, double& adata)
		{
			return readcsv(apair, adata);
		}

		static bool readveccsv(csvpair& apair, std::string& adata)
		{
			return readcsv(apair, adata);
		}

		template <typename TKEY, typename TVALUE>
		static bool readcsv(csvpair& apair, std::map<TKEY, TVALUE>& amap)
		{
			std::string ltempstr;
			read(apair, ltempstr);
			csvpair lpair;
			lpair.m_data = ltempstr;
			int lsize = apair.m_data.size();
			for (; apair.m_pos < lsize;)
			{
				TKEY lkey;
				TVALUE lvalue;
				if (readcsv(lpair, lkey) == false)
					return false;
				if (readcsv(lpair, lvalue) == false)
					return false;
				amap.insert(std::make_pair(lkey, lvalue));
			}
			return true;
		}

		template <typename T, typename ...ARG>
		static bool readcsv(csvpair& apair, T& adata, ARG& ...arg)
		{
			if (apair.m_data.empty())
				return false;
			return readcsv(apair, adata) && readcsv(apair, arg...);
		}

		template <typename T>
		bool readcsv(std::map<int, T>& adata)
		{
			csvpair lpair;
			for (; readline(lpair.m_data);)
			{
				if (lpair.m_data.empty() != true)
				{
					if (lpair.m_data[0] == '#')
						continue;
					T ltemp;
					if (ltemp.rcsv(lpair))
					{
						adata.insert(std::make_pair(ltemp.m_id, ltemp));
						lpair.clear();
					}
				}
			}
			return true;
		}
		
		template <typename T>
		bool readcsv(std::vector<T>& adata)
		{
			csvpair lpair;
			for (; readline(lpair.m_data);)
			{
				T ltemp;
				ltemp.rcsv(lpair);
				adata.push_back(ltemp);
				lpair.clear();
			}
			return true;
		}
	};

	template <typename E>
	class enum_operator_readcsv<E, true>
	{
	public:
		static bool operator_readcsv(csvpair& apair, E& adata)
		{
			int ltemp = 0;
			if (rcsv::readcsv(apair, ltemp))
			{
				adata = (E)ltemp;
				return true;
			}
			return false;
		}
	};
}//namespace ngl