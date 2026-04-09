/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements text, regex, url, and casing helpers for tools.

#include "tools/tools/tools_text.h"
#include "utf8cpp/utf8.h"

#include <codecvt>
#include <locale>

namespace ngl
{
	namespace tools
	{
		bool wasscii2asscii(const std::wstring& awstr, std::string& astr)
		{
			if (awstr.empty())
			{
				astr.clear();
				return true;
			}

			std::locale lloc("");
			const wchar_t* lfrom = awstr.c_str();
			const wchar_t* lfrom_end = awstr.c_str() + awstr.size();
			const wchar_t* lfrom_next = nullptr;
			const std::size_t lwchar_sz = 4;
			std::vector<char> lbuf((awstr.size() + 1) * lwchar_sz, '\0');
			char* lto = lbuf.data();
			char* lto_end = lto + lbuf.size();
			char* lto_next = nullptr;
			using conv_t = std::codecvt<wchar_t, char, mbstate_t>;
			mbstate_t lstate{};
			auto lret = std::use_facet<conv_t>(lloc).out(
				lstate,
				lfrom,
				lfrom_end,
				lfrom_next,
				lto,
				lto_end,
				lto_next);
			if (lret == conv_t::ok)
			{
				astr = lto;
				return true;
			}
			astr.clear();
			return false;
		}

		bool asscii2wasscii(const std::string& astr, std::wstring& awstr)
		{
			if (astr.empty())
			{
				return true;
			}

			std::locale lloc("");
			const char* lfrom = astr.c_str();
			const char* lfrom_end = astr.c_str() + astr.size();
			const char* lfrom_next = nullptr;
			std::vector<wchar_t> lbuf(astr.size() + 1, L'\0');
			wchar_t* lto = lbuf.data();
			wchar_t* lto_end = lto + lbuf.size();
			wchar_t* lto_next = nullptr;
			using conv_t = std::codecvt<wchar_t, char, mbstate_t>;
			mbstate_t lstate{};
			auto lret = std::use_facet<conv_t>(lloc).in(
				lstate,
				lfrom,
				lfrom_end,
				lfrom_next,
				lto,
				lto_end,
				lto_next);
			if (lret == conv_t::ok)
			{
				awstr = lto;
				return true;
			}
			awstr.clear();
			return false;
		}

		bool wasscii2utf8(const std::wstring& awstr, std::string& astr)
		{
			if (awstr.empty())
			{
				astr.clear();
				return true;
			}
			try
			{
				std::wstring_convert<std::codecvt_utf8<wchar_t>> lconv;
				astr = lconv.to_bytes(awstr);
				return true;
			}
			catch (...)
			{
				astr.clear();
				return false;
			}
		}

		bool utf82wasscii(const std::string& astr, std::wstring& awstr)
		{
			if (astr.empty())
			{
				awstr.clear();
				return true;
			}
			try
			{
				std::wstring_convert<std::codecvt_utf8<wchar_t>> lconv;
				awstr = lconv.from_bytes(astr);
				return true;
			}
			catch (...)
			{
				awstr.clear();
				return false;
			}
		}

		bool to_asscii(const std::string& astr1, std::string& astr2)
		{
			std::wstring lwstr;
			if (!utf82wasscii(astr1, lwstr))
			{
				return false;
			}
			return wasscii2asscii(lwstr, astr2);
		}

		bool to_utf8(const std::string& astr1, std::string& astr2)
		{
			std::wstring lwstr;
			if (!asscii2wasscii(astr1, lwstr))
			{
				return false;
			}
			return wasscii2utf8(lwstr, astr2);
		}

		int32_t utf8firstbyte(uint8_t afirst)
		{
			int32_t lcnt = 0;
			unsigned char lmask = 0x80;
			if ((afirst & lmask) == 0)
			{
				return 1;
			}
			while ((afirst & lmask) != 0)
			{
				++lcnt;
				lmask >>= 1;
				if (lcnt > 4)
				{
					return 0;
				}
			}
			return (lcnt < 2 || lcnt > 4) ? 0 : lcnt;
		}

		bool isutf8(const std::string& astr)
		{
			return utf8::is_valid(astr.begin(), astr.end());
		}

		bool isincludeutf8mb4(const std::string& astr)
		{
			if (!isutf8(astr))
			{
				return false;
			}
			try
			{
				auto liter = astr.begin();
				while (liter != astr.end())
				{
					const auto lcode = static_cast<char32_t>(utf8::next(liter, astr.end()));
					if (lcode > 0xFFFF)
					{
						return true;
					}
				}
			}
			catch (...)
			{
				return false;
			}
			return false;
		}

		void sregex(const std::string& apattern, const std::string& adata, const std::function<void(std::string&)>& afun)
		{
			std::regex lreg(apattern);
			std::sregex_iterator liter(adata.begin(), adata.end(), lreg);
			std::sregex_iterator lend;
			for (; liter != lend; ++liter)
			{
				std::string lstr = liter->str();
				afun(lstr);
			}
		}

		void smatch(const std::string& aexpr, const std::string& adata, const std::function<void(std::smatch&)>& afun)
		{
			std::smatch lwhat;
			std::regex lexpr(aexpr);
			auto lstart = adata.begin();
			const auto lend = adata.end();
			while (std::regex_search(lstart, lend, lwhat, lexpr))
			{
				afun(lwhat);
				lstart = lwhat[0].second;
			}
		}

		void replace(const std::string& aexpr, const std::string& arepl, const std::string& adata, std::string& aret)
		{
			std::regex lreg(aexpr);
			aret = std::regex_replace(adata, lreg, arepl);
		}

		void replace_ret(const std::string& aexpr, const std::string& arepl, std::string& adata, std::string& aret)
		{
			replace(aexpr, arepl, adata, aret);
		}

		std::string char2hex(char adec)
		{
			const auto lval = static_cast<unsigned char>(adec);
			std::string lret(2, '\0');
			lret[0] = detail::g_hex_tab[lval >> 4];
			lret[1] = detail::g_hex_tab[lval & 0x0f];
			return lret;
		}

		std::string url_encode(const std::string& astr)
		{
			std::string lret;
			lret.reserve(astr.size() * 3);
			for (unsigned char lch : astr)
			{
				if (detail::is_url_safe(lch))
				{
					lret.push_back(static_cast<char>(lch));
				}
				else
				{
					lret.push_back('%');
					lret.push_back(detail::g_hex_tab[lch >> 4]);
					lret.push_back(detail::g_hex_tab[lch & 0x0f]);
				}
			}
			return lret;
		}

		std::string url_decode(const std::string& astr)
		{
			std::string lret;
			lret.reserve(astr.size());
			for (std::size_t lidx = 0; lidx < astr.size(); ++lidx)
			{
				const char lch = astr[lidx];
				if (lch == '+')
				{
					lret.push_back(' ');
					continue;
				}
				if (lch != '%')
				{
					lret.push_back(lch);
					continue;
				}
				if (lidx + 2 >= astr.size())
				{
					lret.push_back('%');
					continue;
				}
				const int lhi = detail::hex_value(astr[lidx + 1]);
				const int llo = detail::hex_value(astr[lidx + 2]);
				if (lhi < 0 || llo < 0)
				{
					lret.push_back('%');
					continue;
				}
				const auto ldec = static_cast<unsigned char>((lhi << 4) | llo);
				if (detail::keep_pct(ldec))
				{
					lret.push_back('%');
					continue;
				}
				lret.push_back(static_cast<char>(ldec));
				lidx += 2;
			}
			return lret;
		}

		std::vector<std::string_view> get_line(const char* apbuff, std::size_t abuff_sz)
		{
			std::vector<std::string_view> lvec;
			if (apbuff == nullptr || abuff_sz == 0)
			{
				return lvec;
			}

			std::size_t lline_cnt = 1;
			for (std::size_t lidx = 0; lidx + 1 < abuff_sz; ++lidx)
			{
				if (apbuff[lidx] == '\r' && apbuff[lidx + 1] == '\n')
				{
					++lline_cnt;
					++lidx;
				}
			}
			lvec.reserve(lline_cnt);

			const char* lline = apbuff;
			const char* lcur = apbuff;
			const char* lend = apbuff + abuff_sz;
			while (lcur + 1 < lend)
			{
				if (lcur[0] == '\r' && lcur[1] == '\n')
				{
					lvec.emplace_back(lline, static_cast<std::size_t>(lcur - lline));
					lcur += 2;
					lline = lcur;
					continue;
				}
				++lcur;
			}
			if (lline < lend)
			{
				lvec.emplace_back(lline, static_cast<std::size_t>(lend - lline));
			}
			return lvec;
		}

		bool isnumber(const std::string& anum)
		{
			return !anum.empty() && std::all_of(anum.begin(), anum.end(), [](unsigned char ach)
			{
				return std::isdigit(ach) != 0;
			});
		}

		void transform_tolower(std::string& adata)
		{
			std::ranges::transform(adata, adata.begin(), [](unsigned char ach)
			{
				return static_cast<char>(std::tolower(ach));
			});
		}

		void transform_toupper(std::string& adata)
		{
			std::ranges::transform(adata, adata.begin(), [](unsigned char ach)
			{
				return static_cast<char>(std::toupper(ach));
			});
		}
	}//namespace tools
}//namespace ngl
