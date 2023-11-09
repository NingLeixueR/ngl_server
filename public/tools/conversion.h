#pragma once
#include <string>

namespace ngl
{
	class conversion
	{
	public:
		// ¿í×Ö·ûÓëASCIIÏà»¥×ª»»  utf8
		static bool wasscii2asscii(const std::wstring& awstr, std::string& astr);
		static bool asscii2wasscii(const std::string& astr, std::wstring& awstr);
		static bool wasscii2utf8(const std::wstring& awstr, std::string& astr);
		static bool utf82wasscii(const std::string& astr, std::wstring& awstr);

		static bool to_asscii(const std::string& astr1, std::string& astr2);
		static bool to_utf8(const std::string& astr1, std::string& astr2);
	};
}