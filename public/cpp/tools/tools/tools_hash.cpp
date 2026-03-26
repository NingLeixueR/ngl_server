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
// File overview: Implements hashing helpers for tools.

#include "tools_hash.h"

#include <cstdio>

namespace ngl
{
	namespace hash_detail
	{
		inline constexpr int g_s11 = 7;
		inline constexpr int g_s12 = 12;
		inline constexpr int g_s13 = 17;
		inline constexpr int g_s14 = 22;
		inline constexpr int g_s21 = 5;
		inline constexpr int g_s22 = 9;
		inline constexpr int g_s23 = 14;
		inline constexpr int g_s24 = 20;
		inline constexpr int g_s31 = 4;
		inline constexpr int g_s32 = 11;
		inline constexpr int g_s33 = 16;
		inline constexpr int g_s34 = 23;
		inline constexpr int g_s41 = 6;
		inline constexpr int g_s42 = 10;
		inline constexpr int g_s43 = 15;
		inline constexpr int g_s44 = 21;

		class md5_sum
		{
		public:
			using size_type = unsigned int;

			md5_sum()
			{
				init();
			}

			explicit md5_sum(const std::string& atext)
			{
				init();
				update(atext.c_str(), static_cast<size_type>(atext.size()));
				finalize();
			}

			void update(const unsigned char* abuf, size_type alen)
			{
				size_type lidx = m_cnt[0] / 8 % g_blk_sz;
				if ((m_cnt[0] += (alen << 3)) < (alen << 3))
				{
					++m_cnt[1];
				}
				m_cnt[1] += (alen >> 29);
				const size_type lpart = g_blk_sz - lidx;
				size_type li = 0;
				if (alen >= lpart)
				{
					std::memcpy(&m_buf[lidx], abuf, lpart);
					transform(m_buf);
					for (li = lpart; li + g_blk_sz <= alen; li += g_blk_sz)
					{
						transform(&abuf[li]);
					}
					lidx = 0;
				}
				std::memcpy(&m_buf[lidx], &abuf[li], alen - li);
			}

			void update(const char* abuf, size_type alen)
			{
				update(reinterpret_cast<const unsigned char*>(abuf), alen);
			}

			md5_sum& finalize()
			{
				static const unsigned char g_pad[64] = {
					0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
				};

				if (!m_fin)
				{
					unsigned char lbits[8];
					enc(lbits, m_cnt, 8);
					const size_type lidx = m_cnt[0] / 8 % 64;
					const size_type lpad = (lidx < 56) ? (56 - lidx) : (120 - lidx);
					update(g_pad, lpad);
					update(lbits, 8);
					enc(m_dig, m_state, 16);
					std::memset(m_buf, 0, sizeof(m_buf));
					std::memset(m_cnt, 0, sizeof(m_cnt));
					m_fin = true;
				}
				return *this;
			}

			std::string hexdigest() const
			{
				if (!m_fin)
				{
					return {};
				}

				char lbuf[33] = { 0 };
				for (int lidx = 0; lidx < 16; ++lidx)
				{
					std::snprintf(lbuf + (lidx * 2), 33 - (lidx * 2), "%02x", m_dig[lidx]);
				}
				lbuf[32] = 0;
				return lbuf;
			}

		private:
			using uint1 = unsigned char;
			using uint4 = unsigned int;
			inline static constexpr int g_blk_sz = 64;

			void init()
			{
				m_fin = false;
				m_cnt[0] = 0;
				m_cnt[1] = 0;
				m_state[0] = 0x67452301;
				m_state[1] = 0xefcdab89;
				m_state[2] = 0x98badcfe;
				m_state[3] = 0x10325476;
			}

			static void dec(uint4 aout[], const uint1 ain[], size_type alen)
			{
				for (unsigned int li = 0, lj = 0; lj < alen; ++li, lj += 4)
				{
					aout[li] = static_cast<uint4>(ain[lj])
						| (static_cast<uint4>(ain[lj + 1]) << 8)
						| (static_cast<uint4>(ain[lj + 2]) << 16)
						| (static_cast<uint4>(ain[lj + 3]) << 24);
				}
			}

			static void enc(uint1 aout[], const uint4 ain[], size_type alen)
			{
				for (size_type li = 0, lj = 0; lj < alen; ++li, lj += 4)
				{
					aout[lj] = static_cast<uint1>(ain[li] & 0xff);
					aout[lj + 1] = static_cast<uint1>((ain[li] >> 8) & 0xff);
					aout[lj + 2] = static_cast<uint1>((ain[li] >> 16) & 0xff);
					aout[lj + 3] = static_cast<uint1>((ain[li] >> 24) & 0xff);
				}
			}

			static uint4 f(uint4 ax, uint4 ay, uint4 az)
			{
				return (ax & ay) | ((~ax) & az);
			}

			static uint4 g(uint4 ax, uint4 ay, uint4 az)
			{
				return (ax & az) | (ay & ~az);
			}

			static uint4 h(uint4 ax, uint4 ay, uint4 az)
			{
				return ax ^ ay ^ az;
			}

			static uint4 i(uint4 ax, uint4 ay, uint4 az)
			{
				return ay ^ (ax | ~az);
			}

			static uint4 rot(uint4 ax, int an)
			{
				return (ax << an) | (ax >> (32 - an));
			}

			static void ff(uint4& aa, uint4 ab, uint4 ac, uint4 ad, uint4 ax, uint4 as, uint4 aac)
			{
				aa = rot(aa + f(ab, ac, ad) + ax + aac, as) + ab;
			}

			static void gg(uint4& aa, uint4 ab, uint4 ac, uint4 ad, uint4 ax, uint4 as, uint4 aac)
			{
				aa = rot(aa + g(ab, ac, ad) + ax + aac, as) + ab;
			}

			static void hh(uint4& aa, uint4 ab, uint4 ac, uint4 ad, uint4 ax, uint4 as, uint4 aac)
			{
				aa = rot(aa + h(ab, ac, ad) + ax + aac, as) + ab;
			}

			static void ii(uint4& aa, uint4 ab, uint4 ac, uint4 ad, uint4 ax, uint4 as, uint4 aac)
			{
				aa = rot(aa + i(ab, ac, ad) + ax + aac, as) + ab;
			}

			void transform(const uint1 ablk[g_blk_sz])
			{
				uint4 la = m_state[0];
				uint4 lb = m_state[1];
				uint4 lc = m_state[2];
				uint4 ld = m_state[3];
				uint4 lx[16]{};
				dec(lx, ablk, g_blk_sz);

				ff(la, lb, lc, ld, lx[0], g_s11, 0xd76aa478); ff(ld, la, lb, lc, lx[1], g_s12, 0xe8c7b756);
				ff(lc, ld, la, lb, lx[2], g_s13, 0x242070db); ff(lb, lc, ld, la, lx[3], g_s14, 0xc1bdceee);
				ff(la, lb, lc, ld, lx[4], g_s11, 0xf57c0faf); ff(ld, la, lb, lc, lx[5], g_s12, 0x4787c62a);
				ff(lc, ld, la, lb, lx[6], g_s13, 0xa8304613); ff(lb, lc, ld, la, lx[7], g_s14, 0xfd469501);
				ff(la, lb, lc, ld, lx[8], g_s11, 0x698098d8); ff(ld, la, lb, lc, lx[9], g_s12, 0x8b44f7af);
				ff(lc, ld, la, lb, lx[10], g_s13, 0xffff5bb1); ff(lb, lc, ld, la, lx[11], g_s14, 0x895cd7be);
				ff(la, lb, lc, ld, lx[12], g_s11, 0x6b901122); ff(ld, la, lb, lc, lx[13], g_s12, 0xfd987193);
				ff(lc, ld, la, lb, lx[14], g_s13, 0xa679438e); ff(lb, lc, ld, la, lx[15], g_s14, 0x49b40821);

				gg(la, lb, lc, ld, lx[1], g_s21, 0xf61e2562); gg(ld, la, lb, lc, lx[6], g_s22, 0xc040b340);
				gg(lc, ld, la, lb, lx[11], g_s23, 0x265e5a51); gg(lb, lc, ld, la, lx[0], g_s24, 0xe9b6c7aa);
				gg(la, lb, lc, ld, lx[5], g_s21, 0xd62f105d); gg(ld, la, lb, lc, lx[10], g_s22, 0x2441453);
				gg(lc, ld, la, lb, lx[15], g_s23, 0xd8a1e681); gg(lb, lc, ld, la, lx[4], g_s24, 0xe7d3fbc8);
				gg(la, lb, lc, ld, lx[9], g_s21, 0x21e1cde6); gg(ld, la, lb, lc, lx[14], g_s22, 0xc33707d6);
				gg(lc, ld, la, lb, lx[3], g_s23, 0xf4d50d87); gg(lb, lc, ld, la, lx[8], g_s24, 0x455a14ed);
				gg(la, lb, lc, ld, lx[13], g_s21, 0xa9e3e905); gg(ld, la, lb, lc, lx[2], g_s22, 0xfcefa3f8);
				gg(lc, ld, la, lb, lx[7], g_s23, 0x676f02d9); gg(lb, lc, ld, la, lx[12], g_s24, 0x8d2a4c8a);

				hh(la, lb, lc, ld, lx[5], g_s31, 0xfffa3942); hh(ld, la, lb, lc, lx[8], g_s32, 0x8771f681);
				hh(lc, ld, la, lb, lx[11], g_s33, 0x6d9d6122); hh(lb, lc, ld, la, lx[14], g_s34, 0xfde5380c);
				hh(la, lb, lc, ld, lx[1], g_s31, 0xa4beea44); hh(ld, la, lb, lc, lx[4], g_s32, 0x4bdecfa9);
				hh(lc, ld, la, lb, lx[7], g_s33, 0xf6bb4b60); hh(lb, lc, ld, la, lx[10], g_s34, 0xbebfbc70);
				hh(la, lb, lc, ld, lx[13], g_s31, 0x289b7ec6); hh(ld, la, lb, lc, lx[0], g_s32, 0xeaa127fa);
				hh(lc, ld, la, lb, lx[3], g_s33, 0xd4ef3085); hh(lb, lc, ld, la, lx[6], g_s34, 0x4881d05);
				hh(la, lb, lc, ld, lx[9], g_s31, 0xd9d4d039); hh(ld, la, lb, lc, lx[12], g_s32, 0xe6db99e5);
				hh(lc, ld, la, lb, lx[15], g_s33, 0x1fa27cf8); hh(lb, lc, ld, la, lx[2], g_s34, 0xc4ac5665);

				ii(la, lb, lc, ld, lx[0], g_s41, 0xf4292244); ii(ld, la, lb, lc, lx[7], g_s42, 0x432aff97);
				ii(lc, ld, la, lb, lx[14], g_s43, 0xab9423a7); ii(lb, lc, ld, la, lx[5], g_s44, 0xfc93a039);
				ii(la, lb, lc, ld, lx[12], g_s41, 0x655b59c3); ii(ld, la, lb, lc, lx[3], g_s42, 0x8f0ccc92);
				ii(lc, ld, la, lb, lx[10], g_s43, 0xffeff47d); ii(lb, lc, ld, la, lx[1], g_s44, 0x85845dd1);
				ii(la, lb, lc, ld, lx[8], g_s41, 0x6fa87e4f); ii(ld, la, lb, lc, lx[15], g_s42, 0xfe2ce6e0);
				ii(lc, ld, la, lb, lx[6], g_s43, 0xa3014314); ii(lb, lc, ld, la, lx[13], g_s44, 0x4e0811a1);
				ii(la, lb, lc, ld, lx[4], g_s41, 0xf7537e82); ii(ld, la, lb, lc, lx[11], g_s42, 0xbd3af235);
				ii(lc, ld, la, lb, lx[2], g_s43, 0x2ad7d2bb); ii(lb, lc, ld, la, lx[9], g_s44, 0xeb86d391);

				m_state[0] += la;
				m_state[1] += lb;
				m_state[2] += lc;
				m_state[3] += ld;
				std::memset(lx, 0, sizeof(lx));
			}

			bool m_fin = false;
			uint1 m_buf[g_blk_sz]{};
			uint4 m_cnt[2]{};
			uint4 m_state[4]{};
			uint1 m_dig[16]{};
		};

		struct sha1_ctx
		{
			uint32_t m_state[5]{};
			uint32_t m_cnt[2]{};
			uint8_t m_buf[64]{};
		};

		inline constexpr std::size_t g_sha1_sz = 20;
		inline constexpr std::size_t g_hmac_sz = 64;

		uint32_t rol32(uint32_t aval, int abits)
		{
			return (aval << abits) | (aval >> (32 - abits));
		}

		void sha1_tran(uint32_t astate[5], const uint8_t abuf[64])
		{
			std::array<uint32_t, 80> lw{};
			for (std::size_t lidx = 0; lidx < 16; ++lidx)
			{
				const std::size_t lpos = lidx * 4;
				lw[lidx] = (static_cast<uint32_t>(abuf[lpos]) << 24)
					| (static_cast<uint32_t>(abuf[lpos + 1]) << 16)
					| (static_cast<uint32_t>(abuf[lpos + 2]) << 8)
					| static_cast<uint32_t>(abuf[lpos + 3]);
			}
			for (std::size_t lidx = 16; lidx < lw.size(); ++lidx)
			{
				lw[lidx] = rol32(lw[lidx - 3] ^ lw[lidx - 8] ^ lw[lidx - 14] ^ lw[lidx - 16], 1);
			}

			uint32_t la = astate[0];
			uint32_t lb = astate[1];
			uint32_t lc = astate[2];
			uint32_t ld = astate[3];
			uint32_t le = astate[4];

			for (std::size_t lidx = 0; lidx < lw.size(); ++lidx)
			{
				uint32_t lf = 0;
				uint32_t lk = 0;
				if (lidx < 20)
				{
					lf = (lb & lc) | ((~lb) & ld);
					lk = 0x5A827999;
				}
				else if (lidx < 40)
				{
					lf = lb ^ lc ^ ld;
					lk = 0x6ED9EBA1;
				}
				else if (lidx < 60)
				{
					lf = (lb & lc) | (lb & ld) | (lc & ld);
					lk = 0x8F1BBCDC;
				}
				else
				{
					lf = lb ^ lc ^ ld;
					lk = 0xCA62C1D6;
				}
				const uint32_t ltmp = rol32(la, 5) + lf + le + lk + lw[lidx];
				le = ld;
				ld = lc;
				lc = rol32(lb, 30);
				lb = la;
				la = ltmp;
			}

			astate[0] += la;
			astate[1] += lb;
			astate[2] += lc;
			astate[3] += ld;
			astate[4] += le;
		}

		void sha1_init(sha1_ctx* actx)
		{
			actx->m_state[0] = 0x67452301;
			actx->m_state[1] = 0xEFCDAB89;
			actx->m_state[2] = 0x98BADCFE;
			actx->m_state[3] = 0x10325476;
			actx->m_state[4] = 0xC3D2E1F0;
			actx->m_cnt[0] = 0;
			actx->m_cnt[1] = 0;
		}

		void sha1_upd(sha1_ctx* actx, const uint8_t* adata, std::size_t alen)
		{
			std::size_t lj = (actx->m_cnt[0] >> 3) & 63;
			const uint32_t llen_lo = static_cast<uint32_t>(alen << 3);
			const uint32_t llen_hi = static_cast<uint32_t>(alen >> 29);
			if ((actx->m_cnt[0] += llen_lo) < llen_lo)
			{
				++actx->m_cnt[1];
			}
			actx->m_cnt[1] += llen_hi;

			std::size_t li = 0;
			if ((lj + alen) > 63)
			{
				std::memcpy(&actx->m_buf[lj], adata, (li = 64 - lj));
				sha1_tran(actx->m_state, actx->m_buf);
				for (; li + 63 < alen; li += 64)
				{
					sha1_tran(actx->m_state, adata + li);
				}
				lj = 0;
			}
			std::memcpy(&actx->m_buf[lj], &adata[li], alen - li);
		}

		void sha1_fin(sha1_ctx* actx, uint8_t adig[g_sha1_sz])
		{
			uint8_t lcnt[8]{};
			const uint8_t lpad_80[1] = { 0x80 };
			const uint8_t lpad_00[1] = { 0x00 };
			for (uint32_t lidx = 0; lidx < 8; ++lidx)
			{
				lcnt[lidx] = static_cast<unsigned char>((actx->m_cnt[(lidx >= 4 ? 0 : 1)] >> ((3 - (lidx & 3)) * 8)) & 255);
			}
			sha1_upd(actx, lpad_80, 1);
			while ((actx->m_cnt[0] & 504) != 448)
			{
				sha1_upd(actx, lpad_00, 1);
			}
			sha1_upd(actx, lcnt, 8);
			for (std::size_t lidx = 0; lidx < g_sha1_sz; ++lidx)
			{
				adig[lidx] = static_cast<uint8_t>((actx->m_state[lidx >> 2] >> ((3 - (lidx & 3)) * 8)) & 255);
			}
			std::memset(actx->m_buf, 0, sizeof(actx->m_buf));
			std::memset(actx->m_state, 0, sizeof(actx->m_state));
			std::memset(actx->m_cnt, 0, sizeof(actx->m_cnt));
			std::memset(lcnt, 0, sizeof(lcnt));
		}

		void sha1_raw(std::string_view atext, std::string& aout)
		{
			sha1_ctx lctx{};
			sha1_init(&lctx);
			sha1_upd(&lctx, reinterpret_cast<const uint8_t*>(atext.data()), atext.size());
			uint8_t ldig[g_sha1_sz]{};
			sha1_fin(&lctx, ldig);
			aout.assign(reinterpret_cast<const char*>(ldig), g_sha1_sz);
		}

		void xor_key(uint8_t akey[g_hmac_sz], uint32_t axor)
		{
			for (std::size_t lidx = 0; lidx < g_hmac_sz; lidx += sizeof(uint32_t))
			{
				uint32_t lval = 0;
				std::memcpy(&lval, &akey[lidx], sizeof(lval));
				lval ^= axor;
				std::memcpy(&akey[lidx], &lval, sizeof(lval));
			}
		}

		void hmac_raw(const std::string& akey, const std::string& atext, std::string& aout)
		{
			sha1_ctx lctx1{};
			sha1_ctx lctx2{};
			uint8_t ldig1[g_sha1_sz]{};
			uint8_t ldig2[g_sha1_sz]{};
			uint8_t lrkey[g_hmac_sz]{};
			std::size_t lkey_sz = akey.size();

			if (lkey_sz > g_hmac_sz)
			{
				sha1_ctx lctx{};
				sha1_init(&lctx);
				sha1_upd(&lctx, reinterpret_cast<const uint8_t*>(akey.data()), lkey_sz);
				sha1_fin(&lctx, lrkey);
				lkey_sz = g_sha1_sz;
			}
			else
			{
				std::memcpy(lrkey, akey.data(), lkey_sz);
			}

			xor_key(lrkey, 0x5c5c5c5c);
			sha1_init(&lctx1);
			sha1_upd(&lctx1, lrkey, g_hmac_sz);

			xor_key(lrkey, 0x5c5c5c5c ^ 0x36363636);
			sha1_init(&lctx2);
			sha1_upd(&lctx2, lrkey, g_hmac_sz);
			sha1_upd(&lctx2, reinterpret_cast<const uint8_t*>(atext.data()), atext.size());
			sha1_fin(&lctx2, ldig2);

			sha1_upd(&lctx1, ldig2, g_sha1_sz);
			sha1_fin(&lctx1, ldig1);
			aout.assign(reinterpret_cast<const char*>(ldig1), g_sha1_sz);
		}
	}

	namespace tools
	{
		std::string md5(const std::string& atext)
		{
			hash_detail::md5_sum lsum(atext);
			return lsum.hexdigest();
		}

		std::string sh1(std::string_view atext)
		{
			std::string lret;
			hash_detail::sha1_raw(atext, lret);
			return lret;
		}

		std::string hmac_sha1(const std::string& akey, const std::string& atext)
		{
			std::string lret;
			hash_detail::hmac_raw(akey, atext, lret);
			return lret;
		}
	}
}
