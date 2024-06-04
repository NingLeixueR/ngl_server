#include "splite.h"

namespace ngl
{
	bool splite::func(const char* abuff, const char* afg, std::vector<std::string>& avec)
	{
        if (abuff == nullptr || afg == nullptr)
            return false;
        std::string ltemp;
        for (const char* lp1 = abuff; *lp1 != '\0';)
        {
            int lpos = 0;
            bool lfg = true;
            const char* lp2 = lp1;
            for (; lp2[lpos] != '\0' && afg[lpos] != '\0'; ++lpos)
            {
                if (lp2[lpos] != afg[lpos])
                {
                    lfg = false;
                    break;
                }
            }
            if (lfg)
            {
                avec.push_back(ltemp);
                ltemp = "";
                lp1 += lpos;
            }
            else
            {
                ltemp += *lp1;
                ++lp1;
            }
        }
        if(ltemp.empty() == false)
            avec.push_back(ltemp);
        return true;
    }
}