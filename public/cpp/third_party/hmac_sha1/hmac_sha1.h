#pragma once

#include "sha1.h"

namespace ngl
{
	constexpr auto BLOCKSIZE = 64;

	static inline void xor_key(uint8_t key[BLOCKSIZE], uint32_t xor_)
	{
		for (int i = 0; i < BLOCKSIZE; i += sizeof(uint32_t))
		{
			auto k = (uint32_t*)&key[i];
			*k ^= xor_;
		}
	}

	void hmac_sha1(const std::string& key, const std::string& text, std::string& aret)
	{
		SHA1_CTX ctx1, ctx2;
		uint8_t digest1[SHA1_DIGEST_SIZE];
		uint8_t digest2[SHA1_DIGEST_SIZE];
		uint8_t rkey[BLOCKSIZE];
		memset(rkey, 0, BLOCKSIZE);
		size_t key_sz = key.size();
		size_t text_sz = text.size();

		if (key_sz > BLOCKSIZE) {
			SHA1_CTX ctx;
			sat_SHA1_Init(&ctx);
			sat_SHA1_Update(&ctx, (const uint8_t*)key.data(), key_sz);
			sat_SHA1_Final(&ctx, rkey);
			key_sz = SHA1_DIGEST_SIZE;
		}
		else {
			memcpy(rkey, key.data(), key_sz);
		}

		xor_key(rkey, 0x5c5c5c5c);
		sat_SHA1_Init(&ctx1);
		sat_SHA1_Update(&ctx1, rkey, BLOCKSIZE);

		xor_key(rkey, 0x5c5c5c5c ^ 0x36363636);
		sat_SHA1_Init(&ctx2);
		sat_SHA1_Update(&ctx2, rkey, BLOCKSIZE);
		sat_SHA1_Update(&ctx2, (const uint8_t*)text.data(), text_sz);
		sat_SHA1_Final(&ctx2, digest2);

		sat_SHA1_Update(&ctx1, digest2, SHA1_DIGEST_SIZE);
		sat_SHA1_Final(&ctx1, digest1);

		aret.assign((char*)digest1, SHA1_DIGEST_SIZE);
	}
}