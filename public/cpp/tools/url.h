#pragma once

#include <string>

namespace ngl
{
	class url
	{
	public:
		static std::string char2hex(char dec);
		static std::string encode(const std::string& c);
		static std::string decode(const std::string& c);
	};
}