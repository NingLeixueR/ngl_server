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
// File overview: Implements binary codec helpers for tools.

#include "tools/tools_code.h"

#include "tools/tab/xml/sysconfig.h"

namespace ngl
{
	namespace code_detail
	{
		struct varint_impl
		{
			inline static constexpr int32_t g_64max = 10;
			inline static constexpr int32_t g_32max = 5;

			static uint64_t zigzag64e(int64_t an)
			{
				return static_cast<uint64_t>((an >> 63) ^ (an << 1));
			}

			static int64_t zigzag64d(uint64_t an)
			{
				const uint64_t lmask = 0ULL - (an & 1ULL);
				return static_cast<int64_t>((an >> 1) ^ lmask);
			}

			static uint32_t zigzag32e(int32_t an)
			{
				return static_cast<uint32_t>((an >> 31) ^ (an << 1));
			}

			static int32_t zigzag32d(uint32_t an)
			{
				const uint32_t lmask = 0U - (an & 1U);
				return static_cast<int32_t>((an >> 1) ^ lmask);
			}
		};

		template <typename T>
		bool enc_base(char* abuf, int alen, const T* adata, int32_t acnt, int32_t* abytes)
		{
			if (abuf == nullptr || adata == nullptr || abytes == nullptr)
			{
				return false;
			}
			const int lsize = static_cast<int>(sizeof(T) * acnt);
			if (lsize > alen)
			{
				return false;
			}
			std::memcpy(abuf, adata, lsize);
			*abytes += lsize;
			return true;
		}

		template <typename T>
		bool dec_base(const char* abuf, int alen, int32_t* abytes, T* adata, int32_t acnt)
		{
			if (abuf == nullptr || abytes == nullptr || adata == nullptr)
			{
				return false;
			}
			const int lsize = static_cast<int>(sizeof(T) * acnt);
			if (lsize > alen)
			{
				return false;
			}
			std::memcpy(adata, abuf, lsize);
			*abytes += lsize;
			return true;
		}

		bool hex_get(unsigned char asrc, unsigned char& adst)
		{
			if (asrc >= 'a' && asrc <= 'f')
			{
				adst = static_cast<unsigned char>(10 + (asrc - 'a'));
				return true;
			}
			if (asrc >= '0' && asrc <= '9')
			{
				adst = static_cast<unsigned char>(asrc - '0');
				return true;
			}
			return false;
		}

		void hex_pair(unsigned char asrc, unsigned char& adst1, unsigned char& adst2)
		{
			static constexpr char g_hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
			adst1 = static_cast<unsigned char>(g_hex[asrc >> 4]);
			adst2 = static_cast<unsigned char>(g_hex[asrc & 0x0f]);
		}

		bool hex_byte(const unsigned char* asrc, unsigned char* adst)
		{
			if (asrc == nullptr || adst == nullptr)
			{
				return false;
			}
			*adst = 0;
			unsigned char lval = 0;
			if (!hex_get(asrc[0], lval))
			{
				return false;
			}
			*adst |= static_cast<unsigned char>(lval << 4);
			if (!hex_get(asrc[1], lval))
			{
				return false;
			}
			*adst |= lval;
			return true;
		}
	}

	namespace tools
	{
		int varint_length(parm<int64_t>& avalues)
		{
			if (!sysconfig::varint())
			{
				return sizeof(int64_t);
			}

			const uint64_t lval = code_detail::varint_impl::zigzag64e(avalues.m_value);
			if ((lval & (0xffffffffffffULL << 7)) == 0) return 1;
			if ((lval & (0xffffffffffffULL << 14)) == 0) return 2;
			if ((lval & (0xffffffffffffULL << 21)) == 0) return 3;
			if ((lval & (0xffffffffffffULL << 28)) == 0) return 4;
			if ((lval & (0xffffffffffffULL << 35)) == 0) return 5;
			if ((lval & (0xffffffffffffULL << 42)) == 0) return 6;
			if ((lval & (0xffffffffffffULL << 49)) == 0) return 7;
			if ((lval & (0xffffffffffffULL << 56)) == 0) return 8;
			if ((lval & (0xffffffffffffULL << 63)) == 0) return 9;
			return 10;
		}

		int varint_length(parm<int32_t>& avalues)
		{
			if (!sysconfig::varint())
			{
				return sizeof(int32_t);
			}

			const uint32_t lval = code_detail::varint_impl::zigzag32e(avalues.m_value);
			if ((lval & (0xffffffffU << 7)) == 0) return 1;
			if ((lval & (0xffffffffU << 14)) == 0) return 2;
			if ((lval & (0xffffffffU << 21)) == 0) return 3;
			if ((lval & (0xffffffffU << 28)) == 0) return 4;
			return 5;
		}

		bool varint_encode(varint_parm<int64_t>& aparm)
		{
			if (!sysconfig::varint())
			{
				return code_detail::enc_base(aparm.m_buf, aparm.m_len, &aparm.m_value, 1, aparm.m_bytes);
			}
			if (aparm.m_buf == nullptr || aparm.m_bytes == nullptr || aparm.m_len <= 0)
			{
				return false;
			}

			uint64_t lval = code_detail::varint_impl::zigzag64e(aparm.m_value);
			int32_t lidx = 0;
			while (true)
			{
				if (lidx >= aparm.m_len || lidx >= code_detail::varint_impl::g_64max)
				{
					return false;
				}
				if ((lval & ~0x7fULL) == 0)
				{
					aparm.m_buf[lidx] = static_cast<char>(lval & 0x7f);
					break;
				}
				aparm.m_buf[lidx] = static_cast<char>((lval & 0x7f) | 0x80);
				++lidx;
				lval >>= 7;
			}
			*aparm.m_bytes += lidx + 1;
			return true;
		}

		bool varint_encode(varint_parm<int32_t>& aparm)
		{
			if (!sysconfig::varint())
			{
				return code_detail::enc_base(aparm.m_buf, aparm.m_len, &aparm.m_value, 1, aparm.m_bytes);
			}
			if (aparm.m_buf == nullptr || aparm.m_bytes == nullptr || aparm.m_len <= 0)
			{
				return false;
			}

			uint32_t lval = code_detail::varint_impl::zigzag32e(aparm.m_value);
			int32_t lidx = 0;
			while (true)
			{
				if (lidx >= aparm.m_len || lidx >= code_detail::varint_impl::g_32max)
				{
					return false;
				}
				if ((lval & ~0x7fU) == 0)
				{
					aparm.m_buf[lidx] = static_cast<char>(lval & 0x7f);
					break;
				}
				aparm.m_buf[lidx] = static_cast<char>((lval & 0x7f) | 0x80);
				++lidx;
				lval >>= 7;
			}
			*aparm.m_bytes += lidx + 1;
			return true;
		}

		bool varint_decode(varint_parm<int64_t>& aparm)
		{
			if (!sysconfig::varint())
			{
				return code_detail::dec_base(aparm.m_buf, aparm.m_len, aparm.m_bytes, &aparm.m_value, 1);
			}
			if (aparm.m_buf == nullptr || aparm.m_bytes == nullptr || aparm.m_len <= 0)
			{
				return false;
			}

			uint64_t lraw = 0;
			int32_t lidx = 0;
			for (; lidx < aparm.m_len; ++lidx)
			{
				const uint8_t lbyte = static_cast<uint8_t>(aparm.m_buf[lidx]);
				lraw |= static_cast<uint64_t>(lbyte & 0x7fU) << (lidx * 7);
				if ((lbyte & 0x80U) == 0)
				{
					break;
				}
				if (lidx >= code_detail::varint_impl::g_64max)
				{
					return false;
				}
			}
			if (lidx >= aparm.m_len)
			{
				return false;
			}
			*aparm.m_bytes += lidx + 1;
			aparm.m_value = code_detail::varint_impl::zigzag64d(lraw);
			return true;
		}

		bool varint_decode(varint_parm<int32_t>& aparm)
		{
			if (!sysconfig::varint())
			{
				return code_detail::dec_base(aparm.m_buf, aparm.m_len, aparm.m_bytes, &aparm.m_value, 1);
			}
			if (aparm.m_buf == nullptr || aparm.m_bytes == nullptr || aparm.m_len <= 0)
			{
				return false;
			}

			uint32_t lraw = 0;
			int32_t lidx = 0;
			for (; lidx < aparm.m_len; ++lidx)
			{
				const uint8_t lbyte = static_cast<uint8_t>(aparm.m_buf[lidx]);
				lraw |= static_cast<uint32_t>(lbyte & 0x7fU) << (lidx * 7);
				if ((lbyte & 0x80U) == 0)
				{
					break;
				}
				if (lidx >= code_detail::varint_impl::g_32max)
				{
					return false;
				}
			}
			if (lidx >= aparm.m_len)
			{
				return false;
			}
			*aparm.m_bytes += lidx + 1;
			aparm.m_value = code_detail::varint_impl::zigzag32d(lraw);
			return true;
		}

		int to_hex(void* apso, int alen, void* apto)
		{
			if (apso == nullptr || apto == nullptr || alen <= 0)
			{
				return 0;
			}
			int llen = 0;
			for (int lidx = 0; lidx < alen; ++lidx)
			{
				auto* lsrc = static_cast<unsigned char*>(apso) + lidx;
				auto* ldst = static_cast<unsigned char*>(apto) + (lidx * 2);
				code_detail::hex_pair(*lsrc, *ldst, *(ldst + 1));
				llen += 2;
			}
			return llen;
		}

		bool to_bytes(void* apso, int alen, void* apto, int& aout_len)
		{
			if (apso == nullptr || apto == nullptr || alen < 0)
			{
				return false;
			}
			for (int lidx = 0; lidx < alen / 2; ++lidx)
			{
				if (!code_detail::hex_byte(&static_cast<unsigned char*>(apso)[lidx * 2], &static_cast<unsigned char*>(apto)[lidx]))
				{
					return false;
				}
			}
			aout_len = alen / 2;
			return true;
		}

		void bytexor(char* ap, int32_t aplen, int apos)
		{
			if (ap == nullptr || aplen <= 0 || !sysconfig::isxor())
			{
				return;
			}
			const std::string& lkey = sysconfig::xorkey();
			const int32_t lkey_len = sysconfig::xorkeynum();
			if (lkey_len < 1 || lkey.empty())
			{
				return;
			}

			int32_t lkey_idx = apos % lkey_len;
			if (lkey_idx < 0)
			{
				lkey_idx += lkey_len;
			}

			for (int32_t lidx = 0; lidx < aplen; ++lidx)
			{
				ap[lidx] = static_cast<char>(ap[lidx] ^ lkey[static_cast<std::size_t>(lkey_idx)]);
				++lkey_idx;
				if (lkey_idx == lkey_len)
				{
					lkey_idx = 0;
				}
			}
		}
	}
}
