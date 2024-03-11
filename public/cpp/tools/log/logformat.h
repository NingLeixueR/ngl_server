#pragma once

#include <functional>
#include <string>
#include <map>

#include "logserialize.h"

namespace ngl
{
	class logformat
	{
	public:
		template <typename T>
		bool out(const char* aformat, char* abuff, int abufflen, const T& avalue)
		{
			for (int i = 0; ; ++i)
			{
				if (aformat[i] == '\0')
				{
					*abuff = '\0';
					++abuff;
					--abufflen;
					break;
				}
				if (aformat[i] == '%')
				{
					logserialize ltstr(abuff, abufflen);
					if (!ltstr(avalue)) return false;
					abufflen = ltstr.len();
					abuff = ltstr.buff();
					if (abufflen < 0) return false;
				}
				else
				{
					*abuff = aformat[i];
					++abuff;
					--abufflen;
				}
			}
			return true;
		}

		template <typename T, typename ...ARG>
		bool out(const char* aformat, char* abuff, int abufflen, const T& avalue, const ARG& ... arg)
		{
			for (int i = 0; ; ++i)
			{
				if (abufflen <= 0)
					return false;
				if (aformat[i] == '\0')
				{
					*abuff = '\0';
					++abuff;
					--abufflen;
					break;
				}
				if (aformat[i] == '%')
				{
					logserialize ltstr(abuff, abufflen);
					if (!ltstr(avalue)) return false;
					abufflen = ltstr.len();
					abuff = ltstr.buff();
					if (abufflen <= 0)
						return false;
					return out(&aformat[i + 1], abuff, abufflen, arg...);
				}
				else
				{
					*abuff = aformat[i];
					++abuff;
					--abufflen;
				}
			}
			return true;
		}

		template <int NUM, typename ...ARG>
		bool out_num(std::string& astr, const char* aformat, const ARG&... arg)
		{
			char lbuff[NUM] = { 0x0 };
			if (out(aformat, lbuff, NUM, arg...))
			{
				astr = lbuff;
				return true;
			}
			return false;
		}

		std::map<std::string, std::string> m_data;
	public:
		std::string& data(const char* aname)
		{
			return m_data[aname];
		}

		template <typename ...ARG>
		std::string& format(const char* aname, const char* aformat, const ARG& ... arg)
		{
			std::string& lstr = m_data[aname];
			lstr = "";
			out_num<1024>(lstr, aformat, arg...)
				|| out_num<2048>(lstr, aformat, arg...)
				|| out_num<4096>(lstr, aformat, arg...)
				|| out_num<8192>(lstr, aformat, arg...)
				|| out_num<10240>(lstr, aformat, arg...)
				|| out_num<20480>(lstr, aformat, arg...);
			return lstr;
		}

		template <typename ...ARG>
		std::string& format(const char* aname, const char* aformat)
		{
			std::string& lstr = m_data[aname];
			lstr = aformat;
			return lstr;
		}
	};
}// namespace ngl
