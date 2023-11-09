#include "url.h"

namespace ngl
{
	std::string url::char2hex(char dec)
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
	std::string url::encode(const std::string& c)
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


	std::string url::decode(const std::string& szToDecode)
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
}