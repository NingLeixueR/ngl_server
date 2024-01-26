#include <codecvt>
#include <locale>
#include <cstring>
#include "conversion.h"


namespace ngl
{
	bool conversion::wasscii2asscii(const std::wstring& awstr, std::string& astr)
	{
		if (awstr.empty())
			return true;

		std::locale sys_locale("");

		const wchar_t* data_from = awstr.c_str();
		const wchar_t* data_from_end = awstr.c_str() + awstr.size();
		const wchar_t* data_from_next = 0;

		int wchar_size = 4;
		char* data_to = new char[(awstr.size() + 1) * wchar_size];
		char* data_to_end = data_to + (awstr.size() + 1) * wchar_size;
		char* data_to_next = 0;

		memset(data_to, 0, (awstr.size() + 1) * wchar_size);

		typedef std::codecvt<wchar_t, char, mbstate_t> convert_facet;
		mbstate_t out_state;
		auto result = std::use_facet<convert_facet>(sys_locale).out(
			out_state, data_from, data_from_end, data_from_next,
			data_to, data_to_end, data_to_next);
		if (result == convert_facet::ok)
		{
			astr = data_to;
			delete[] data_to;
			return true;
		}
		else
		{
			delete[] data_to;
			astr.clear();
			return false;
		}
	}

	bool conversion::asscii2wasscii(const std::string& astr, std::wstring& awstr)
	{
		if (astr.empty())
			return true;

		std::locale sys_locale("");

		const char* data_from = astr.c_str();
		const char* data_from_end = astr.c_str() + astr.size();
		const char* data_from_next = 0;

		wchar_t* data_to = new wchar_t[astr.size() + 1];
		wchar_t* data_to_end = data_to + astr.size() + 1;
		wchar_t* data_to_next = 0;

		wmemset(data_to, 0, astr.size() + 1);

		typedef std::codecvt<wchar_t, char, mbstate_t> convert_facet;
		mbstate_t in_state;
		auto result = std::use_facet<convert_facet>(sys_locale).in(
			in_state, data_from, data_from_end, data_from_next,
			data_to, data_to_end, data_to_next);
		if (result == convert_facet::ok)
		{
			awstr = data_to;
			delete[] data_to;
			return true;
		}
		else
		{
			delete[] data_to;
			awstr.clear();
			return false;
		}
	}

	bool conversion::wasscii2utf8(const std::wstring& awstr, std::string& astr)
	{
		if (awstr.empty())
			return true;
		std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
		astr = conv.to_bytes(awstr);
		return true;
	}

	bool conversion::utf82wasscii(const std::string& astr, std::wstring& awstr)
	{
		if (astr.empty())
			return false;
		std::wstring_convert<std::codecvt_utf8<wchar_t> > conv;
		awstr = conv.from_bytes(astr);
		return true;
	}

	bool conversion::to_utf8(const std::string& astr1, std::string& astr2)
	{
		std::wstring lwsql;
		if (ngl::conversion::asscii2wasscii(astr1, lwsql) == false)
			return false;
		if (ngl::conversion::wasscii2utf8(lwsql, astr2) == false)
			return false;
		return true;
	}
	
	bool conversion::to_asscii(const std::string& astr1, std::string& astr2)
	{
		std::wstring lwsql;
		if (ngl::conversion::utf82wasscii(astr1, lwsql) == false)
			return false;
		if (ngl::conversion::wasscii2asscii(lwsql, astr2) == false)
			return false;
		return true;
	}

}