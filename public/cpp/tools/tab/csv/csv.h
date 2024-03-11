#pragma once

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
		bool m_doublequotationmarks; // Ë«ÒıºÅ
		char m_fg;//Ä¬ÈÏ,

		csvpair();
		void clear();
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
		void read(const std::string& aname, int32_t& aversion);
		bool readline(std::string& adata);

		static void read(csvpair& apair, std::string& ltemp);

		rcsv(const std::string& afilename, int32_t& aversion);
		rcsv();

		static bool isok(csvpair& apair);

		static bool readcsv(csvpair& apair, int8_t& adata);
		static bool readcsv(csvpair& apair, int16_t& adata);
		static bool readcsv(csvpair& apair, int32_t& adata);
		static bool readcsv(csvpair& apair, int64_t& adata);
		static bool readcsv(csvpair& apair, uint8_t& adata);
		static bool readcsv(csvpair& apair, uint16_t& adata);
		static bool readcsv(csvpair& apair, uint32_t& adata);
		static bool readcsv(csvpair& apair, uint64_t& adata);
		static bool readcsv(csvpair& apair, float& adata);
		static bool readcsv(csvpair& apair, double& adata);
		static bool readcsv(csvpair& apair, bool& adata);
		static bool readcsv(csvpair& apair, std::string& adata);

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
				for (; !isok(lpair);)
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
				for (; !isok(lpair);)
				{
					T ltemp;
					if (readcsv(lpair, ltemp))
						afun(ltemp);
					//if (readveccsv(lpair, ltemp))
					//	afun(ltemp);
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
		
		static bool readveccsv(csvpair& apair, int8_t& adata);
		static bool readveccsv(csvpair& apair, int16_t& adata);
		static bool readveccsv(csvpair& apair, int32_t& adata);
		static bool readveccsv(csvpair& apair, int64_t& adata);
		static bool readveccsv(csvpair& apair, uint8_t& adata);
		static bool readveccsv(csvpair& apair, uint16_t& adata);
		static bool readveccsv(csvpair& apair, uint32_t& adata);
		static bool readveccsv(csvpair& apair, uint64_t& adata);
		static bool readveccsv(csvpair& apair, float& adata);
		static bool readveccsv(csvpair& apair, double& adata);
		static bool readveccsv(csvpair& apair, std::string& adata);

		template <typename TKEY, typename TVALUE>
		static bool readcsv(csvpair& apair, std::map<TKEY, TVALUE>& amap)
		{
			std::string ltempstr;
			read(apair, ltempstr);
			csvpair lpair;
			lpair.m_data = ltempstr;
			for (; !isok(lpair);)
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
						adata[ltemp.m_id] = ltemp;
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