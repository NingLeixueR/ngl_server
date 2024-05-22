#include "splite.h"

namespace ngl
{
	bool splite::func(const char* abuff, const char* afg, std::vector<std::string>& avec)
	{
		std::string_view buff(abuff);
		std::string_view fg(afg);
		for (const auto& token : buff | std::views::split(fg)) 
		{
			avec.emplace_back(token.begin(), token.end());
		}
		return true;
	}
	/*{
		std::string_view buff(abuff);
		std::string_view fg(afg);
		for (const std::string_view sv : std::ranges::split_view(buff, fg))
		{
			avec.emplace_back(sv);
		}

		return true;
	}*/
}