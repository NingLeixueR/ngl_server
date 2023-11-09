#pragma once

#include <vector>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

namespace ngl
{
	class splite
	{
		
	public:
		// ·Ö¸î×Ö·û´®
		static void division(const char* abuff, const char* afg, std::vector<int>& avec);
		static void division(const char* abuff, const char* afg, std::vector<float>& avec);
		static void division(const char* abuff, const char* afg, std::vector<double>& avec);
		static void division(const char* abuff, const char* afg, std::vector<std::string>& avec);

		template <typename TYPE>
		static void division(std::vector<std::string>& avec, int apos, TYPE& aval)
		{
			if (apos >= avec.size())
				return;
			aval = boost::lexical_cast<TYPE>(avec[apos]);
		}

		template <typename TYPE, typename ...ARG>
		static void division(std::vector<std::string>& avec, int apos, TYPE& aval, ARG&... arg)
		{
			if (apos >= avec.size())
				return;
			aval = boost::lexical_cast<TYPE>(avec[apos]);
			division(avec, ++apos, arg...);
		}


		template <typename ...ARG>
		static void division(const char* abuff, const char* afg, ARG&... arg)
		{
			std::vector<std::string> lvec;
			boost::split(lvec, abuff, boost::is_any_of(afg));

			division(lvec, 0, arg...);
		}

		template <typename TYPE>
		static void recast(std::stringstream& astream, const TYPE& arg)
		{
			astream << arg;
		}

		template <typename TYPE, typename ...ARG>
		static void recast(std::stringstream& astream, const TYPE& aval, const ARG&... arg)
		{
			recast(astream, aval);
			recast(astream, arg...);
		}

		template <typename ...ARG>
		static void recast(std::string& astr, const ARG&... arg)
		{
			std::stringstream m_stream;
			recast(m_stream, arg...);
			astr += m_stream.str();
		}

		static void recast(const std::vector<std::string>& avec, const char afg, std::string& astr);
		static void recast(const std::vector<int>& avec, const char afg, std::string& astr);
		static void recast(const std::vector<float>& avec, const char afg, std::string& astr);
		static void recast(const std::vector<double>& avec, const char afg, std::string& astr);

		template <typename T>
		static void division(
			const char* abuff, 
			const char* afg1, 
			const char* afg2, 
			std::vector<std::vector<T>>& avec
		)
		{
			std::vector<std::string> lvec;
			division(abuff, afg1, lvec);
			avec.resize(lvec.size());
			for (int i = 0; i < lvec.size(); ++i)
			{
				division(lvec[i].c_str(), afg2, avec[i]);
			}
		}
	};
}