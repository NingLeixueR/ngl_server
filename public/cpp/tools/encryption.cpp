#include "encryption.h"
#include "sysconfig.h"
#include "hexbytes.h"

#include <cryptopp/cryptlib.h>
#include <cryptopp/secblock.h>
#include <cryptopp/filters.h>
#include <cryptopp/base64.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/files.h>
#include <cryptopp/cmac.h>
#include <cryptopp/hmac.h>
#include <cryptopp/aes.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>
#include <cryptopp/aes.h>

#include <iostream>
#include <stdio.h>

using namespace CryptoPP;
typedef unsigned long long uint64;
typedef long long int64;
typedef unsigned char byte;

namespace ngl
{
	void encryption::hmac_sha256(const std::string& key, const std::string& plain, std::string& value)
	{
		//std::string mac, encoded;
		std::string mac;
		HMAC< SHA256 > hmac((byte*)key.c_str(), key.length());
		//CryptoPP::StringSource(plain, true, new HashFilter(hmac, new StringSink(mac)));
		StringSink lStringSink(mac);
		HashFilter lHashFilter(hmac, &lStringSink);
		//const char *buf = mac.c_str();
		char lbuff[1024] = { 0 };
		hexbytes::to_hex((void*)mac.c_str(), (int)mac.size(), lbuff);
		value = lbuff;
	}

	void encryption::function_sha256(char* abuff, int alen, std::string& value)
	{
		SHA256 sha256;
		char byDigest[32 + 1];
		memset(byDigest, 0, sizeof(byDigest));
		sha256.CalculateDigest((byte*)byDigest, (const byte*)abuff, alen);
		value = byDigest;
	}

	void encryption::bytexor(char* ap, int32_t aplen, int apos)
	{
		if (sysconfig::isxor() == false)
			return;
		if (sysconfig::xorkeynum() < 1 || sysconfig::xorkey().empty())
			return;
		for (int i = 0, j = apos; i < aplen; ++i,++j)
		{
			ap[i] = ap[i] ^ sysconfig::xorkey()[j & sysconfig::xorkeynum()];
		}
	}
}// namespace ngl