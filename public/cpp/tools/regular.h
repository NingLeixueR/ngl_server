#pragma once

#include <string>
#include <functional>
#include <regex>

namespace ngl
{
	class regular
	{
	public:
		static void sregex(
			const std::string& apattern
			, const std::string& adata
			, const std::function<void(std::string&)>& afun
		);

		static void smatch(
			const std::string& aexpression
			, const std::string& adata
			, const std::function<void(std::smatch&)>& afun
		);

		static void replace(
			const std::string& aexpression
			, const std::string& areleace
			, const std::string& adata
			, std::string& aret
		);

		static bool replace_ret(
			const std::string& aexpression
			, const std::string& areleace
			, std::string& adata
			, std::string& aret
		);
	};
}