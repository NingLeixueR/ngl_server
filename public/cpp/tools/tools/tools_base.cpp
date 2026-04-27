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
// File overview: Implements base, endian, and base64 helpers for tools.

#include "tools/tools/tools_base.h"

#if defined(_WIN32)
#include <objbase.h>
#else
#include <uuid/uuid.h>
#endif
#include <cstdio>

namespace ngl::tools
{
	class base64_impl
	{
	public:
		static const char* get_alpha()
		{
			static constexpr char g_tab[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
			return &g_tab[0];
		}

		static const signed char* get_inv()
		{
			static constexpr signed char g_tab[] = {
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
				52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
				-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
				15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
				-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
			};
			return &g_tab[0];
		}

		static constexpr std::size_t enc_sz(std::size_t alen)
		{
			return 4 * ((alen + 2) / 3);
		}

		static constexpr std::size_t dec_sz(std::size_t alen)
		{
			return alen / 4 * 3;
		}

		static std::size_t enc(char* adst, const char* asrc, std::size_t alen)
		{
			char* lout = adst;
			const char* lin = asrc;
			const auto* ltab = get_alpha();

			for (auto ln = alen / 3; ln-- != 0;)
			{
				*lout++ = ltab[(lin[0] & 0xfc) >> 2];
				*lout++ = ltab[((lin[0] & 0x03) << 4) + ((lin[1] & 0xf0) >> 4)];
				*lout++ = ltab[((lin[2] & 0xc0) >> 6) + ((lin[1] & 0x0f) << 2)];
				*lout++ = ltab[lin[2] & 0x3f];
				lin += 3;
			}

			switch (alen % 3)
			{
			case 2:
				*lout++ = ltab[(lin[0] & 0xfc) >> 2];
				*lout++ = ltab[((lin[0] & 0x03) << 4) + ((lin[1] & 0xf0) >> 4)];
				*lout++ = ltab[(lin[1] & 0x0f) << 2];
				*lout++ = '=';
				break;
			case 1:
				*lout++ = ltab[(lin[0] & 0xfc) >> 2];
				*lout++ = ltab[(lin[0] & 0x03) << 4];
				*lout++ = '=';
				*lout++ = '=';
				break;
			default:
				break;
			}

			return static_cast<std::size_t>(lout - adst);
		}

		static std::pair<std::size_t, std::size_t> dec(char* adst, const char* asrc, std::size_t alen)
		{
			char* lout = adst;
			auto* lin = reinterpret_cast<const unsigned char*>(asrc);
			unsigned char lc3[3]{};
			unsigned char lc4[4]{};
			int li = 0;
			int lj = 0;
			const auto* linv = get_inv();

			while (alen-- != 0 && *lin != '=')
			{
				const auto lval = linv[*lin];
				if (lval == -1)
				{
					break;
				}
				++lin;
				lc4[li] = static_cast<unsigned char>(lval);
				if (++li == 4)
				{
					lc3[0] = static_cast<unsigned char>((lc4[0] << 2) + ((lc4[1] & 0x30) >> 4));
					lc3[1] = static_cast<unsigned char>(((lc4[1] & 0x0f) << 4) + ((lc4[2] & 0x3c) >> 2));
					lc3[2] = static_cast<unsigned char>(((lc4[2] & 0x03) << 6) + lc4[3]);
					for (li = 0; li < 3; ++li)
					{
						*lout++ = static_cast<char>(lc3[li]);
					}
					li = 0;
				}
			}

			if (li != 0)
			{
				lc3[0] = static_cast<unsigned char>((lc4[0] << 2) + ((lc4[1] & 0x30) >> 4));
				lc3[1] = static_cast<unsigned char>(((lc4[1] & 0x0f) << 4) + ((lc4[2] & 0x3c) >> 2));
				lc3[2] = static_cast<unsigned char>(((lc4[2] & 0x03) << 6) + lc4[3]);
				for (lj = 0; lj < li - 1; ++lj)
				{
					*lout++ = static_cast<char>(lc3[lj]);
				}
			}

			return {
				static_cast<std::size_t>(lout - adst),
				static_cast<std::size_t>(lin - reinterpret_cast<const unsigned char*>(asrc))
			};
		}
	};

	bool is_lanip(const std::string& aip)
	{
		if (aip == "127.0.0.1")
		{
			return true;
		}
		std::array<int, 4> loct{};
		if (!detail::parse_ip4(aip, loct))
		{
			return false;
		}
		return loct[0] == 10
			|| (loct[0] == 172 && loct[1] >= 16 && loct[1] <= 31)
			|| (loct[0] == 192 && loct[1] == 168);
	}

	int16_t transformlittle(parm<int16_t>& avalues)
	{
		if constexpr (islittle())
		{
			return avalues.m_value;
		}
		else
		{
			auto* lbuf = reinterpret_cast<unsigned char*>(&avalues.m_value);
			avalues.m_value = static_cast<int16_t>((static_cast<int16_t>(lbuf[0]) << 8) | static_cast<int16_t>(lbuf[1]));
			return avalues.m_value;
		}
	}

	uint16_t transformlittle(parm<uint16_t>& avalues)
	{
		if constexpr (islittle())
		{
			return avalues.m_value;
		}
		else
		{
			auto* lbuf = reinterpret_cast<unsigned char*>(&avalues.m_value);
			avalues.m_value = static_cast<uint16_t>((static_cast<uint16_t>(lbuf[0]) << 8) | static_cast<uint16_t>(lbuf[1]));
			return avalues.m_value;
		}
	}

	int32_t transformlittle(parm<int32_t>& avalues)
	{
		if constexpr (islittle())
		{
			return avalues.m_value;
		}
		else
		{
			auto* lbuf = reinterpret_cast<unsigned char*>(&avalues.m_value);
			avalues.m_value = (static_cast<int32_t>(lbuf[0]) << 24)
				| (static_cast<int32_t>(lbuf[1]) << 16)
				| (static_cast<int32_t>(lbuf[2]) << 8)
				| static_cast<int32_t>(lbuf[3]);
			return avalues.m_value;
		}
	}

	uint32_t transformlittle(parm<uint32_t>& avalues)
	{
		if constexpr (islittle())
		{
			return avalues.m_value;
		}
		else
		{
			auto* lbuf = reinterpret_cast<unsigned char*>(&avalues.m_value);
			avalues.m_value = (static_cast<uint32_t>(lbuf[0]) << 24)
				| (static_cast<uint32_t>(lbuf[1]) << 16)
				| (static_cast<uint32_t>(lbuf[2]) << 8)
				| static_cast<uint32_t>(lbuf[3]);
			return avalues.m_value;
		}
	}

	int64_t transformlittle(parm<int64_t>& avalues)
	{
		if constexpr (islittle())
		{
			return avalues.m_value;
		}
		else
		{
			auto* lbuf = reinterpret_cast<unsigned char*>(&avalues.m_value);
			avalues.m_value = static_cast<int64_t>(
				(static_cast<uint64_t>(lbuf[0]) << 56)
				| (static_cast<uint64_t>(lbuf[1]) << 48)
				| (static_cast<uint64_t>(lbuf[2]) << 40)
				| (static_cast<uint64_t>(lbuf[3]) << 32)
				| (static_cast<uint64_t>(lbuf[4]) << 24)
				| (static_cast<uint64_t>(lbuf[5]) << 16)
				| (static_cast<uint64_t>(lbuf[6]) << 8)
				| static_cast<uint64_t>(lbuf[7]));
			return avalues.m_value;
		}
	}

	uint64_t transformlittle(parm<uint64_t>& avalues)
	{
		if constexpr (islittle())
		{
			return avalues.m_value;
		}
		else
		{
			auto* lbuf = reinterpret_cast<unsigned char*>(&avalues.m_value);
			avalues.m_value = (static_cast<uint64_t>(lbuf[0]) << 56)
				| (static_cast<uint64_t>(lbuf[1]) << 48)
				| (static_cast<uint64_t>(lbuf[2]) << 40)
				| (static_cast<uint64_t>(lbuf[3]) << 32)
				| (static_cast<uint64_t>(lbuf[4]) << 24)
				| (static_cast<uint64_t>(lbuf[5]) << 16)
				| (static_cast<uint64_t>(lbuf[6]) << 8)
				| static_cast<uint64_t>(lbuf[7]);
			return avalues.m_value;
		}
	}

	std::string base64_encode(const char* adata, std::size_t alen)
	{
		if (adata == nullptr || alen == 0)
		{
			return {};
		}

		std::string ldst;
		ldst.resize(base64_impl::enc_sz(alen));
		ldst.resize(base64_impl::enc(ldst.data(), adata, alen));
		return ldst;
	}

	std::string base64_encode(const std::string& astr)
	{
		return base64_encode(astr.data(), astr.size());
	}

	std::string base64_decode(const char* adata, std::size_t alen)
	{
		if (adata == nullptr || alen == 0)
		{
			return {};
		}

		std::string ldst;
		ldst.resize(base64_impl::dec_sz(alen));
		const auto lpair = base64_impl::dec(ldst.data(), adata, alen);
		ldst.resize(lpair.first);
		return ldst;
	}

	std::string base64_decode(const std::string& adata)
	{
		return base64_decode(adata.data(), adata.size());
	}

	bool uuid_make(std::string& astr)
	{
#if defined(_WIN32)
		constexpr std::size_t g_guid_len = 64;
		char lbuf[g_guid_len] = { 0 };
		GUID lguid{};
		if (CoCreateGuid(&lguid) != S_OK)
		{
			return false;
		}
		std::snprintf(
			lbuf,
			sizeof(lbuf),
			"%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",
			lguid.Data1,
			lguid.Data2,
			lguid.Data3,
			lguid.Data4[0],
			lguid.Data4[1],
			lguid.Data4[2],
			lguid.Data4[3],
			lguid.Data4[4],
			lguid.Data4[5],
			lguid.Data4[6],
			lguid.Data4[7]);
		astr = lbuf;
		return true;
#else
		uuid_t luuid{};
		uuid_generate(luuid);
		char lstr[37]{};
		uuid_unparse(luuid, lstr);
		astr = lstr;
		return true;
#endif
	}
}//namespace ngl::tools
