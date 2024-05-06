#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <string>

namespace ngl
{
	class splite
	{
	public:
		template <typename T>
		static bool func(const char* abuff, const char* afg, std::vector<T>& avec)
		{
			std::vector<std::string> lvec;
			func(abuff, afg, lvec);
			for (std::string& item : lvec)
			{
				avec.push_back(boost::lexical_cast<T>(item));
			}
			return true;
		}

		static bool func(const char* abuff, const char* afg, std::vector<std::string>& avec)
		{
			boost::split(avec, abuff, boost::is_any_of(afg));
			return true;
		}

		template <typename ...ARGS>
		static bool func(const char* abuff, const char* afg, ARGS&... args)
		{
			std::vector<std::string> lvec;
			func(abuff, afg, lvec);
			func(0, lvec, args...);
			return true;
		}

	private:
		template <typename T>
		static void cast(std::string& astr, T& adata)
		{
			adata = boost::lexical_cast<T>(astr);
		}

		template <typename T>
		static bool func(int32_t aindex, std::vector<std::string>& avec, T& adata)
		{
			if (aindex > avec.size())
				return false;
			try
			{
				cast(avec[aindex], adata);
			}
			catch (...)
			{
				return false;
			}
			return true;
		}

		template <typename T,typename ...ARGS>
		static bool func(int32_t aindex, std::vector<std::string>& avec, T& adata, ARGS&... args)
		{
			if (aindex > avec.size())
				return false;
			try
			{
				cast(avec[aindex], adata);
			}
			catch (...)
			{
				return false;
			}
			return func(++aindex, avec, args...);
		}
	};

	class splicing
	{
	public:
		template <typename T>
		static bool func(const std::vector<T>& avec, const char* afg, std::string& astr)
		{
			for (int i = 0; i < avec.size(); ++i)
			{
				if (i != 0)
					astr += afg;
				astr += boost::lexical_cast<std::string>(avec[i]);
			}
			return true;
		}

		static bool func(const std::vector<std::string>& avec, const char* afg, std::string& astr)
		{
			for (int i = 0; i < avec.size(); ++i)
			{
				if (i != 0)
					astr += afg;
				astr += avec[i];
			}
			return true;
		}

		template <typename ...ARGS>
		static bool func(const char* afg, std::string& astr, ARGS... args)
		{
			return func(0, afg, astr, args...);
		}

	private:
		template <typename T>
		static bool func(int32_t aindex, const char* afg, std::string& astr, T& adata)
		{
			try
			{
				if (aindex != 0)
					astr += afg;
				astr += boost::lexical_cast<std::string>(adata);
			}
			catch (...)
			{
				return false;
			}
			return true;
		}

		template <typename T, typename ...ARGS>
		static bool func(int32_t aindex, const char* afg, std::string& astr, T& adata, ARGS&... args)
		{
			try
			{
				if (aindex != 0)
					astr += afg;
				astr += boost::lexical_cast<std::string>(adata);
			}
			catch (...)
			{
				return false;
			}
			return func(++aindex, afg, astr, args...);
		}
	};
}// namespace ngl