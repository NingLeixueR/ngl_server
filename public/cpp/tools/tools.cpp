#include "sysconfig.h"
#include "tools.h"

#include <cassert>
#include <cstdint>
#include <codecvt>
#include <cstring>
#include <cstdio>
#include <string>
#include <vector>
#include <locale>

#if defined(_WIN32)
#include <objbase.h>
#else
#include <uuid/uuid.h>
#endif

#define GUID_LEN 64

namespace ngl
{ 
	//A类地址：10.0.0.0--10.255.255.255
	bool is_a_address(const std::vector<std::string>& lvec)
	{
		if (lvec.empty())
			return false;
		return lvec[0] == "10";
	}

	//B类地址：172.16.0.0--172.31.255.255
	bool is_b_address(const std::vector<std::string>& lvec)
	{
		if (lvec.size() < 2)
			return false;
		if (lvec[0] != "172")
			return false;
		int lnum = tools::lexical_cast<int>(lvec[1]);
		return lnum >= 16 && lnum <= 31;
	}

	//C类地址：192.168.0.0--192.168.255.255
	bool is_c_address(const std::vector<std::string>& lvec)
	{
		if (lvec.size() < 2)
			return false;
		return lvec[0] == "192" && lvec[1] == "168";
	}

	bool tools::is_lanip(const std::string& aip)
	{
		if (aip == "127.0.0.1")
			return true;
		std::vector<std::string> lvec;
		if (splite(aip.c_str(), ".", lvec) == false)
			return false;
		return is_a_address(lvec) || is_b_address(lvec) || is_c_address(lvec);
	}

	bool tools::is_equal(const char* astr1, const char* astr2)
	{
		int llen1 = strlen(astr1);
		int llen2 = strlen(astr2);
		if (llen1 != llen2)
			return false;
		return is_equal(astr1, astr2, llen1);
	}

	bool tools::is_equal(const char* astr1, const char* astr2, int32_t abyte)
	{
		return memcmp(astr1, astr2, abyte) == 0;
	}

	int16_t tools::transformlittle(parm<int16_t>& avalues)
	{
		if constexpr (islittle())
			return avalues.m_value;
		unsigned char* value_p = reinterpret_cast<unsigned char*>(&avalues.m_value);
		avalues.m_value =
			(static_cast<int16_t>(value_p[0]) << 8)
			| static_cast<int16_t>(value_p[1]);
		return avalues.m_value;
	}

	uint16_t tools::transformlittle(parm<uint16_t>& avalues)
	{
		if constexpr (islittle())
			return avalues.m_value;
		unsigned char* value_p = reinterpret_cast<unsigned char*>(&avalues.m_value);
		avalues.m_value =
			(static_cast<uint16_t>(value_p[0]) << 8)
			| static_cast<uint16_t>(value_p[1]);
		return avalues.m_value;
	}

	int32_t tools::transformlittle(parm<int32_t>& avalues)
	{
		if constexpr (islittle())
			return avalues.m_value;
		unsigned char* value_p = reinterpret_cast<unsigned char*>(&avalues.m_value);
		avalues.m_value = (static_cast<int32_t>(value_p[0]) << 24)
			| (static_cast<int32_t>(value_p[1]) << 16)
			| (static_cast<int32_t>(value_p[2]) << 8)
			| static_cast<int32_t>(value_p[3]);
		return avalues.m_value;
	}

	uint32_t tools::transformlittle(parm<uint32_t>& avalues)
	{
		if constexpr (islittle())
			return avalues.m_value;
		unsigned char* value_p = reinterpret_cast<unsigned char*>(&avalues.m_value);
		avalues.m_value = (static_cast<uint32_t>(value_p[0]) << 24)
			| (static_cast<uint32_t>(value_p[1]) << 16)
			| (static_cast<uint32_t>(value_p[2]) << 8)
			| static_cast<uint32_t>(value_p[3]);
		return avalues.m_value;
	}

	int64_t tools::transformlittle(parm<int64_t>& avalues)
	{
		if constexpr (islittle())
			return avalues.m_value;
		unsigned char* value_p = reinterpret_cast<unsigned char*>(&avalues);
		avalues.m_value = (static_cast<uint64_t>(value_p[0]) << 56)
			| (static_cast<int64_t>(value_p[1]) << 48)
			| (static_cast<int64_t>(value_p[2]) << 40)
			| (static_cast<int64_t>(value_p[3]) << 32)
			| (static_cast<int64_t>(value_p[4]) << 24)
			| (static_cast<int64_t>(value_p[5]) << 16)
			| (static_cast<int64_t>(value_p[6]) << 8)
			| static_cast<int64_t>(value_p[7]);
		return avalues.m_value;
	}

	uint64_t tools::transformlittle(parm<uint64_t>& avalues)
	{
		if constexpr (islittle())
			return avalues.m_value;
		unsigned char* value_p = reinterpret_cast<unsigned char*>(&avalues);
		avalues.m_value = (static_cast<uint64_t>(value_p[0]) << 56)
			| (static_cast<uint64_t>(value_p[1]) << 48)
			| (static_cast<uint64_t>(value_p[2]) << 40)
			| (static_cast<uint64_t>(value_p[3]) << 32)
			| (static_cast<uint64_t>(value_p[4]) << 24)
			| (static_cast<uint64_t>(value_p[5]) << 16)
			| (static_cast<uint64_t>(value_p[6]) << 8)
			| static_cast<uint64_t>(value_p[7]);
		return avalues.m_value;
	}

    class base64_impl
    {
    public:
        static char const* get_alphabet()
        {
            static char constexpr tab[] = {
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" };
            return &tab[0];
        }

        static signed char const* get_inverse()
        {
            static signed char constexpr tab[] = {
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //   0-15
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //  16-31
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63, //  32-47
                52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1, //  48-63
                -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,           //  64-79
                15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, //  80-95
                -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, //  96-111
                41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1, // 112-127
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 128-143
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 144-159
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 160-175
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 176-191
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 192-207
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 208-223
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 224-239
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1  // 240-255
            };
            return &tab[0];
        }

        static std::size_t constexpr encoded_size(std::size_t n)
        {
            return 4 * ((n + 2) / 3);
        }

        static std::size_t constexpr decoded_size(std::size_t n)
        {
            return n / 4 * 3; // requires n&3==0, smaller
            //return 3 * n / 4;
        }

        /** Encode a series of octets as a padded, base64 string.

            The resulting string will not be null terminated.

            @par Requires

            The memory pointed to by `out` points to valid memory
            of at least `encoded_size(len)` bytes.

            @return The number of characters written to `out`. This
            will exclude any null termination.
        */
        static std::size_t encode(void* dest, void const* src, std::size_t len)
        {
            char* out = static_cast<char*>(dest);
            char const* in = static_cast<char const*>(src);
            auto const tab = get_alphabet();

            for (auto n = len / 3; n--;)
            {
                *out++ = tab[(in[0] & 0xfc) >> 2];
                *out++ = tab[((in[0] & 0x03) << 4) + ((in[1] & 0xf0) >> 4)];
                *out++ = tab[((in[2] & 0xc0) >> 6) + ((in[1] & 0x0f) << 2)];
                *out++ = tab[in[2] & 0x3f];
                in += 3;
            }

            switch (len % 3)
            {
            case 2:
                *out++ = tab[(in[0] & 0xfc) >> 2];
                *out++ = tab[((in[0] & 0x03) << 4) + ((in[1] & 0xf0) >> 4)];
                *out++ = tab[(in[1] & 0x0f) << 2];
                *out++ = '=';
                break;

            case 1:
                *out++ = tab[(in[0] & 0xfc) >> 2];
                *out++ = tab[((in[0] & 0x03) << 4)];
                *out++ = '=';
                *out++ = '=';
                break;

            case 0:
                break;
            }

            return out - static_cast<char*>(dest);
        }

        /** Decode a padded base64 string into a series of octets.

            @par Requires

            The memory pointed to by `out` points to valid memory
            of at least `decoded_size(len)` bytes.

            @return The number of octets written to `out`, and
            the number of characters read from the input string,
            expressed as a pair.
        */
        static std::pair<std::size_t, std::size_t> decode(void* dest, char const* src, std::size_t len)
        {
            char* out = static_cast<char*>(dest);
            auto in = reinterpret_cast<unsigned char const*>(src);
            unsigned char c3[3], c4[4];
            int i = 0;
            int j = 0;

            auto const inverse = get_inverse();

            while (len-- && *in != '=')
            {
                auto const v = inverse[*in];
                if (v == -1)
                    break;
                ++in;
                c4[i] = v;
                if (++i == 4)
                {
                    c3[0] = (c4[0] << 2) + ((c4[1] & 0x30) >> 4);
                    c3[1] = ((c4[1] & 0xf) << 4) + ((c4[2] & 0x3c) >> 2);
                    c3[2] = ((c4[2] & 0x3) << 6) + c4[3];

                    for (i = 0; i < 3; i++)
                        *out++ = c3[i];
                    i = 0;
                }
            }

            if (i)
            {
                c3[0] = (c4[0] << 2) + ((c4[1] & 0x30) >> 4);
                c3[1] = ((c4[1] & 0xf) << 4) + ((c4[2] & 0x3c) >> 2);
                c3[2] = ((c4[2] & 0x3) << 6) + c4[3];

                for (j = 0; j < i - 1; j++)
                    *out++ = c3[j];
            }

            return { out - static_cast<char*>(dest),
                    in - reinterpret_cast<unsigned char const*>(src) };
        }

    };

    std::string tools::base64_encode(std::uint8_t const* data, std::size_t len)
    {
        std::string dest;
        dest.resize(base64_impl::encoded_size(len));
        dest.resize(base64_impl::encode(&dest[0], data, len));
        return dest;
    }

    std::string tools::base64_encode(std::string const& s)
    {
        return base64_encode(reinterpret_cast<
            std::uint8_t const*>(s.data()),
            s.size());
    }

    std::string tools::base64_decode(char const* data, std::size_t len)
    {
        std::string dest;
        dest.resize(base64_impl::decoded_size(len));
        auto const result = base64_impl::decode(
            &dest[0], data, len);
        dest.resize(result.first);
        return dest;
    }

    std::string tools::base64_decode(std::string const& data)
    {
        return base64_decode(data.data(), data.size());
    }

    bool tools::uuid_make(std::string& astr)
    {
#if defined(_WIN32)
            char buf[GUID_LEN] = { 0 };
            GUID guid;

            if (CoCreateGuid(&guid))
            {
                return false;
            }

            sprintf(buf,
                "%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",
                guid.Data1, guid.Data2, guid.Data3,
                guid.Data4[0], guid.Data4[1], guid.Data4[2],
                guid.Data4[3], guid.Data4[4], guid.Data4[5],
                guid.Data4[6], guid.Data4[7]);

            astr = buf;
            return true;
#else
            uuid_t uuid;
            uuid_generate(uuid);

            char str[37];
            uuid_unparse(uuid, str);

            astr = str;
            return true;
#endif
    }

	bool tools::wasscii2asscii(const std::wstring& awstr, std::string& astr)
	{
		if (awstr.empty())
			return true;

		std::locale sys_locale("");

		const wchar_t* data_from = awstr.c_str();
		const wchar_t* data_from_end = awstr.c_str() + awstr.size();
		const wchar_t* data_from_next = 0;

		int wchar_size = 4;
		char* data_to = new char[(awstr.size() + 1) * wchar_size];
		char* data_to_end = data_to + (awstr.size() + 1) * wchar_size;
		char* data_to_next = 0;

		memset(data_to, 0, (awstr.size() + 1) * wchar_size);

		typedef std::codecvt<wchar_t, char, mbstate_t> convert_facet;
		mbstate_t out_state;
		auto result = std::use_facet<convert_facet>(sys_locale).out(
			out_state, data_from, data_from_end, data_from_next,
			data_to, data_to_end, data_to_next);
		if (result == convert_facet::ok)
		{
			astr = data_to;
			delete[] data_to;
			return true;
		}
		else
		{
			delete[] data_to;
			astr.clear();
			return false;
		}
	}

	bool tools::asscii2wasscii(const std::string& astr, std::wstring& awstr)
	{
		if (astr.empty())
			return true;

		std::locale sys_locale("");

		const char* data_from = astr.c_str();
		const char* data_from_end = astr.c_str() + astr.size();
		const char* data_from_next = 0;

		wchar_t* data_to = new wchar_t[astr.size() + 1];
		wchar_t* data_to_end = data_to + astr.size() + 1;
		wchar_t* data_to_next = 0;

		wmemset(data_to, 0, astr.size() + 1);

		typedef std::codecvt<wchar_t, char, mbstate_t> convert_facet;
		mbstate_t in_state;
		auto result = std::use_facet<convert_facet>(sys_locale).in(
			in_state, data_from, data_from_end, data_from_next,
			data_to, data_to_end, data_to_next);
		if (result == convert_facet::ok)
		{
			awstr = data_to;
			delete[] data_to;
			return true;
		}
		else
		{
			delete[] data_to;
			awstr.clear();
			return false;
		}
	}

	bool tools::wasscii2utf8(const std::wstring& awstr, std::string& astr)
	{
		if (awstr.empty())
			return true;
		std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
		astr = conv.to_bytes(awstr);
		return true;
	}

	bool tools::utf82wasscii(const std::string& astr, std::wstring& awstr)
	{
		if (astr.empty())
			return false;
		std::wstring_convert<std::codecvt_utf8<wchar_t> > conv;
		awstr = conv.from_bytes(astr);
		return true;
	}

	bool tools::to_utf8(const std::string& astr1, std::string& astr2)
	{
		std::wstring lwsql;
		if (ngl::tools::asscii2wasscii(astr1, lwsql) == false)
			return false;
		if (ngl::tools::wasscii2utf8(lwsql, astr2) == false)
			return false;
		return true;
	}

	bool tools::to_asscii(const std::string& astr1, std::string& astr2)
	{
		std::wstring lwsql;
		if (ngl::tools::utf82wasscii(astr1, lwsql) == false)
			return false;
		if (ngl::tools::wasscii2asscii(lwsql, astr2) == false)
			return false;
		return true;
	}

	void tools::sregex(const std::string& apattern, const std::string& adata, const std::function<void(std::string&)>& afun)
	{
		std::regex r(apattern);
		std::sregex_iterator it(adata.begin(), adata.end(), r);
		std::sregex_iterator end;
		for (; it != end; ++it)
		{
			std::string lstr = it->str();
			afun(lstr);
		}
	}

	///---- values = std::smatch[1];
	void tools::smatch(const std::string& aexpression, const std::string& adata, const std::function<void(std::smatch&)>& afun)
	{
		std::smatch what;
		std::regex expression(aexpression);
		std::string::const_iterator start = adata.begin();
		std::string::const_iterator end = adata.end();
		for (; std::regex_search(start, end, what, expression);)
		{
			afun(what);
			start = what[0].second;
		}
	}

	void tools::replace(const std::string& aexpression, const std::string& areleace, const std::string& adata, std::string& aret)
	{
		std::regex reg(aexpression);
		aret = std::regex_replace(adata, reg, areleace);
	}

	bool tools::replace_ret(const std::string& aexpression, const std::string& areleace, std::string& adata, std::string& aret)
	{
		replace(aexpression, areleace, adata, aret);
		return aret != adata;
	}

	bool tools::splite(const char* abuff, const char* afg, std::vector<std::string>& avec)
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
		if (ltemp.empty() == false)
			avec.push_back(ltemp);
		return !avec.empty();
	}

	std::string tools::char2hex(char dec)
	{
		char dig1 = (dec & 0xF0) >> 4;
		char dig2 = (dec & 0x0F);
		if (0 <= dig1 && dig1 <= 9) dig1 += 48;    //0,48 in ascii
		if (10 <= dig1 && dig1 <= 15) dig1 += 65 - 10; //A,65 in ascii
		if (0 <= dig2 && dig2 <= 9) dig2 += 48;
		if (10 <= dig2 && dig2 <= 15) dig2 += 65 - 10;

		std::string r;
		r.append(&dig1, 1);
		r.append(&dig2, 1);
		return r;
	}

	std::string tools::url_encode(const std::string& c)
	{

		std::string escaped;
		int max = (int)c.length();
		for (int i = 0; i < max; i++)
		{
			if ((48 <= c[i] && c[i] <= 57) ||//0-9
				(65 <= c[i] && c[i] <= 90) ||//ABC...XYZ
				(97 <= c[i] && c[i] <= 122) || //abc...xyz
				(c[i] == '-' || c[i] == '_' || c[i] == '.')//(c[i]=='~' || c[i]=='-' || c[i]=='_' || c[i]=='.')
				)
			{
				escaped.append(&c[i], 1);
			}
			else
			{
				escaped.append("%");
				escaped.append(char2hex(c[i]));//converts char 255 to string "FF"
			}
		}
		return std::move(escaped);
	}

	std::string tools::url_decode(const std::string& szToDecode)
	{
		std::string result;
		int hex = 0;
		for (size_t i = 0; i < szToDecode.length(); ++i)
		{
			switch (szToDecode[i])
			{
			case '+':
				result += ' ';
				break;
			case '%':
				if (isxdigit(szToDecode[i + 1]) && isxdigit(szToDecode[i + 2]))
				{
					std::string hexStr = szToDecode.substr(i + 1, 2);
					hex = strtol(hexStr.c_str(), 0, 16);
					//字母和数字[0-9a-zA-Z]、一些特殊符号[$-_.+!*'(),] 、以及某些保留字[$&+,/:;=?@]
					//可以不经过编码直接用于URL
					if (!((hex >= 48 && hex <= 57) ||	//0-9
						(hex >= 97 && hex <= 122) ||	//a-z
						(hex >= 65 && hex <= 90) ||	//A-Z
						//一些特殊符号及保留字[$-_.+!*'(),]  [$&+,/:;=?@]
						hex == 0x21 || hex == 0x24 || hex == 0x26 || hex == 0x27 || hex == 0x28 || hex == 0x29
						|| hex == 0x2a || hex == 0x2b || hex == 0x2c || hex == 0x2d || hex == 0x2e || hex == 0x2f
						|| hex == 0x3A || hex == 0x3B || hex == 0x3D || hex == 0x3f || hex == 0x40 || hex == 0x5f
						))
					{
						result += char(hex);
						i += 2;
					}
					else result += '%';
				}
				else {
					result += '%';
				}
				break;
			default:
				result += szToDecode[i];
				break;
			}
		}
		return std::move(result);
	}


	struct varint_impl
	{
		static const int32_t m_64maxpos/* = 10*/;
		static const int32_t m_32maxpos/* = 5*/;

		static uint64_t zigzag64encode(int64_t n)
		{
			return (uint64_t)((n >> 63) ^ (n << 1));
		}

		static int64_t zigzag64decode(uint64_t n)
		{
			return (uint64_t)((n >> 1) ^ -(n & 1));
		}

		static uint32_t zigzag32encode(int32_t n)
		{
			return (uint32_t)((n >> 31) ^ (n << 1));
		}

		static int32_t zigzag32decode(uint32_t n)
		{
			return (int32_t)((n >> 1) ^ -(n & 1));
		}
	};

	const int32_t varint_impl::m_64maxpos = 10;
	const int32_t varint_impl::m_32maxpos = 5;

	int tools::varint_length(tools::parm<int64_t>& avalue)
	{
		if (sysconfig::varint())
		{
			uint64_t ln = varint_impl::zigzag64encode(avalue.m_value);
			if ((ln & (0xffffffffffff << 7)) == 0)
				return 1;
			else if ((ln & (0xffffffffffff << 14)) == 0)
				return 2;
			else if ((ln & (0xffffffffffff << 21)) == 0)
				return 3;
			else if ((ln & (0xffffffffffff << 28)) == 0)
				return 4;
			else if ((ln & (0xffffffffffff << 35)) == 0)
				return 5;
			else if ((ln & (0xffffffffffff << 42)) == 0)
				return 6;
			else if ((ln & (0xffffffffffff << 49)) == 0)
				return 7;
			else if ((ln & (0xffffffffffff << 56)) == 0)
				return 8;
			else if ((ln & (0xffffffffffff << 63)) == 0)
				return 9;
			else
				return 10;
		}
		else
			return sizeof(int64_t);
	}

	int tools::varint_length(tools::parm<int32_t>& avalue)
	{
		if (sysconfig::varint())
		{
			uint64_t ln = varint_impl::zigzag64encode(avalue.m_value);
			if ((ln & (0xffffffffffff << 7)) == 0)
				return 1;
			else if ((ln & (0xffffffffffff << 14)) == 0)
				return 2;
			else if ((ln & (0xffffffffffff << 21)) == 0)
				return 3;
			else if ((ln & (0xffffffffffff << 28)) == 0)
				return 4;
			else
				return 5;
		}
		else
			return sizeof(int32_t);
	}

	template <typename T>
	bool encodebasetype(char* buf, int len, const T* adata, int32_t adatalen, int32_t* bytes)
	{
		if (sizeof(T) > len)
			return false;
		memcpy(buf, adata, sizeof(T) * adatalen);
		*bytes += sizeof(T) * adatalen;
		return true;
	}

	bool tools::varint_encode(tools::varint_parm<int64_t>& aparm)
	{
		if (sysconfig::varint() == false)
		{
			if (encodebasetype(aparm.m_buf, aparm.m_len, &aparm.m_value, 1, aparm.m_bytes) == false)
				return false;
			return true;
		}
		else
		{
			uint64_t ln = varint_impl::zigzag64encode(aparm.m_value);
			int32_t index = 0;
			while (true)
			{
				if (index > aparm.m_len || index > varint_impl::m_64maxpos)
					return false;
				//0x7F 即　0111 1111
				//~ 表示取反  
				if ((ln & ~0x7F) == 0)
				{
					aparm.m_buf[index] = (uint8_t)(ln & 0x7F);
					break;
				}
				else
				{
					//取出7位并在第8位加上标记1
					aparm.m_buf[index] = (uint8_t)((ln & 0x7F) | 0x80);
					index++;
					//已经处理的舍去
					ln = ln >> 7;
				}
			}
			*aparm.m_bytes += index + 1;
			return true;
		}
	}

	bool tools::varint_encode(tools::varint_parm<int32_t>& aparm)
	{
		if (sysconfig::varint() == false)
		{
			if (encodebasetype(aparm.m_buf, aparm.m_len, &aparm.m_value, 1, aparm.m_bytes) == false)
				return false;
			return true;
		}
		else
		{
			uint32_t ln = varint_impl::zigzag32encode(aparm.m_value);
			int index = 0;
			while (true)
			{
				if (index > aparm.m_len || index > varint_impl::m_32maxpos)
					return false;
				//0x7F 即　0111 1111
				//~ 表示取反  
				if ((ln & ~0x7F) == 0)
				{
					aparm.m_buf[index] = (uint8_t)(ln & 0x7F);
					break;
				}
				else
				{
					//取出7位并在第8位加上标记1
					aparm.m_buf[index] = (uint8_t)((ln & 0x7F) | 0x80);
					index++;
					//已经处理的舍去
					ln = ln >> 7;
				}
			}
			*aparm.m_bytes += index + 1;
			return true;
		}
	}

	template <typename T>
	bool decodebasetype(const char* buf, int len, int32_t* bytes, T* adata, int32_t adatalen)
	{
		int llen = sizeof(T) * adatalen;
		if (llen > len)
			return false;
		memcpy(adata, buf, llen);
		*bytes += llen;
		return true;
	}

	bool tools::varint_decode(varint_parm<int64_t>& aparm)
	{
		if (sysconfig::varint() == false)
		{
			if (decodebasetype(aparm.m_buf, aparm.m_len, aparm.m_bytes, &aparm.m_value, 1) == false)
				return false;
			return true;
		}
		else
		{
			aparm.m_value = 0;
			int i = 0;
			for (; i < aparm.m_len; i++)
			{
				uint8_t b = aparm.m_buf[i];
				//0x7F 即　0111 1111
				//取出7位然后右移，因为是小端存储
				uint8_t c = (b & 0x7F);
				aparm.m_value |= (int64_t)c << (i * 7);
				// 0x80 即 1000 0000
				//第8位是0 说明后面没有字节了
				if ((uint8_t)(b & 0x80) == 0)
				{
					break;
				}
				if (i > varint_impl::m_64maxpos)
					break;
			}
			*aparm.m_bytes += i + 1;
			aparm.m_value = varint_impl::zigzag64decode(aparm.m_value);
			return true;
		}
	}

	bool tools::varint_decode(varint_parm<int32_t>& aparm)
	{
		if (sysconfig::varint() == false)
		{
			int32_t lbytes = 0;
			if (decodebasetype(aparm.m_buf, aparm.m_len, &lbytes, &aparm.m_value, 1) == false)
				return false;
			*aparm.m_bytes += lbytes;
			return true;
		}
		else
		{
			aparm.m_value = 0;
			int i = 0;
			for (; i < aparm.m_len; i++)
			{
				uint8_t b = aparm.m_buf[i];
				//0x7F 即　0111 1111
				//取出7位然后右移，因为是小端存储
				aparm.m_value |= (b & 0x7F) << (i * 7);
				// 0x80 即 1000 0000
				//第8位是0 说明后面没有字节了
				if ((uint8_t)(b & 0x80) == 0)
				{
					break;
				}
				if (i > varint_impl::m_32maxpos)
					break;
			}
			*aparm.m_bytes += i + 1;
			aparm.m_value = varint_impl::zigzag32decode(aparm.m_value);
			return true;
		}
	}

	unsigned char _hexget(unsigned char asource, unsigned char& atarget)
	{
		if (asource >= 'a' && asource <= 'f')
		{
			unsigned char lret = 10;
			atarget = lret + (asource - 'a');
			return true;
		}
		if (asource >= '0' && asource <= '9')
		{
			atarget = (asource - '0');
			return true;
		}
		return false;
	}

	void _bh(unsigned char& apso, unsigned char& apto1, unsigned char& apto2)
	{
		static const char arr[] = { '0','1','2','3','4','5', '6','7','8', '9','a','b', 'c','d','e', 'f' };
		apto1 = arr[apso >> 4];
		apto2 = arr[0x0f & apso];
	}

	bool _hb(unsigned char* apso, unsigned char* apto)
	{
		(*apto) = 0;
		unsigned char ltarget = 0;
		if (_hexget(*apso, ltarget) == false)
			return false;
		(*apto) |= ltarget << 4;
		if (_hexget(*&apso[1], ltarget) == false)
			return false;
		(*apto) |= ltarget;
		return true;
	}

	int tools::to_hex(void* apso, int alen, void* apto)
	{
		int llen = 0;
		unsigned char* lp1;
		unsigned char* lp2;
		for (int i = 0; i < alen; ++i)
		{
			lp1 = (unsigned char*)apso + i;
			lp2 = (unsigned char*)apto + (i * 2);
			_bh(*lp1, *lp2, *(lp2 + 1));
			llen += 2;
		}
		return llen;
	}

	bool tools::to_bytes(void* apso, int alen, void* apto, int& apotlen)
	{
		int llen = 0;
		for (int i = 0; i < alen / 2; ++i)
		{
			if (_hb(&((unsigned char*)apso)[i * 2], &((unsigned char*)apto)[i]) == false)
				return false;
			llen += 2;
		}
		apotlen = alen / 2;
		return true;
	}

	void tools::bytexor(char* ap, int32_t aplen, int apos)
	{
		if (sysconfig::isxor() == false)
			return;
		if (sysconfig::xorkeynum() < 1 || sysconfig::xorkey().empty())
			return;
		for (int i = 0, j = apos; i < aplen; ++i, ++j)
		{
			ap[i] = ap[i] ^ sysconfig::xorkey()[j & sysconfig::xorkeynum()];
		}
	}

}//namespace ngl