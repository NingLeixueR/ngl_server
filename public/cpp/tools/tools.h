/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include <unordered_map>
#include <type_traits>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>
#include <cctype>
#include <format>
#include <string>
#include <regex>
#include <map>

#include <google/protobuf/util/json_util.h>

#ifdef WIN32
#define snprintf _snprintf
#endif //WIN32

namespace ngl
{
	template <typename Target>
	struct lexical_cast2
	{
	};
	
	union nguid;
	template <typename T>
	struct np_actormodule_forward;
	template <typename TDATA>
	using mforward = np_actormodule_forward<TDATA>;

	template <>
	struct lexical_cast2<nguid>;

	template <>
	struct lexical_cast2<std::string>;

	class lexical_check
	{
	public:
		static bool func_number(const std::string& source)
		{
			return func_number(source.c_str());
		}
		static bool func_number(const char* source)
		{
			for (auto lpchar = source; *lpchar != '\0'; ++lpchar)
			{
				if (source == lpchar)
				{
					if (*lpchar == '-' || *lpchar == '+')
					{
						continue;
					}
				}
				if (!std::isdigit(*lpchar))
				{
					throw std::string("isdigit fail");
				}
			}
			return true;
		}

		static bool func_float(const std::string& source)
		{
			return func_float(source.c_str());
		}
		static bool func_float(const char* source)
		{
			bool ldian = false;
			for (auto lpchar = source; *lpchar != '\0'; ++lpchar)
			{
				if (source == lpchar)
				{
					if (*lpchar == '-' || *lpchar == '+')
					{
						continue;
					}
				}
				if (*lpchar == '.')
				{
					if (ldian)
					{
						throw std::string("isdigit fail");
					}
					ldian = true;
				}
				if (!std::isdigit(*lpchar))
				{
					throw std::string("isdigit fail");
				}
			}
			return true;
		}
	};

	template <>
	struct lexical_cast2<int32_t>
	{
		static int32_t fun(const std::string& source)
		{
			lexical_check::func_number(source);
			return atoi(source.c_str());
		}
		static int32_t fun(const char* source)
		{
			lexical_check::func_number(source);
			return atoi(source);
		}
	};

	template <>
	struct lexical_cast2<uint32_t>
	{
		static uint32_t fun(const std::string& source)
		{
			lexical_check::func_number(source);
			return atoi(source.c_str());
		}
		static uint32_t fun(const char* source)
		{
			lexical_check::func_number(source);
			return atoi(source);
		}
	};

	template <>
	struct lexical_cast2<int16_t>
	{
		static int16_t fun(const std::string& source)
		{
			lexical_check::func_number(source);
			return atoi(source.c_str());
		}
		static int16_t fun(const char* source)
		{
			lexical_check::func_number(source);
			return atoi(source);
		}
	};

	template <>
	struct lexical_cast2<uint16_t>
	{
		static uint16_t fun(const std::string& source)
		{
			lexical_check::func_number(source);
			return atoi(source.c_str());
		}
		static uint16_t fun(const char* source)
		{
			lexical_check::func_number(source);
			return atoi(source);
		}
	};

	template <>
	struct lexical_cast2<int8_t>
	{
		static int8_t fun(const std::string& source)
		{
			lexical_check::func_number(source);
			return atoi(source.c_str());
		}
		static int8_t fun(const char* source)
		{
			lexical_check::func_number(source);
			return atoi(source);
		}
	};

	template <>
	struct lexical_cast2<uint8_t>
	{
		static uint8_t fun(const std::string& source)
		{
			lexical_check::func_number(source);
			return atoi(source.c_str());
		}
		static uint8_t fun(const char* source)
		{
			lexical_check::func_number(source);
			return atoi(source);
		}
	};

	template <>
	struct lexical_cast2<int64_t>
	{
		static int64_t fun(const std::string& source)
		{
			lexical_check::func_number(source);
			return atoll(source.c_str());
		}

		static int64_t fun(const char* source)
		{
			lexical_check::func_number(source);
			return atoll(source);
		}
	};

	template <>
	struct lexical_cast2<uint64_t>
	{
		static uint64_t fun(const std::string& source)
		{
			lexical_check::func_number(source);
			return atoll(source.c_str());
		}

		static uint64_t fun(const char* source)
		{
			lexical_check::func_number(source);
			return atoll(source);
		}
	};

	template <>
	struct lexical_cast2<float>
	{
		static float fun(const std::string& source)
		{
			lexical_check::func_float(source);
			return (float)std::atof(source.c_str());
		}

		static float fun(const char* source)
		{
			lexical_check::func_float(source);
			return (float)std::atof(source);
		}
	};

	template <>
	struct lexical_cast2<double>
	{
		static double fun(const std::string& source)
		{
			lexical_check::func_float(source);
			return std::atof(source.c_str());
		}

		static double fun(const char* source)
		{
			lexical_check::func_float(source);
			return std::atof(source);
		}
	};

	template <>
	struct lexical_cast2<bool>
	{
		static bool fun(const std::string& source)
		{
			if (source == "true" || source == "TRUE")
			{
				return true;
			}
			else
			{
				if (source != "0" && source != "")
				{
					return true;
				}
				return false;
			}
		}

		static bool fun(const char* source)
		{
			std::string lstr(source);
			return (lstr == "0" || lstr.empty()) ? false : true;
		}

		static bool fun(int64_t source)
		{
			return source != 0;
		}
	};

	class tools
	{
		tools() = delete;
		tools(const tools&) = delete;
		tools& operator=(const tools&) = delete;

	public:
#pragma region map
		// map操作封装
		template <typename TE, typename T>
		static const T* findmap(const std::map<TE, T>& amap, const TE& aid)
		{
			auto itor = amap.find(aid);
			return itor == amap.end() ? nullptr : &itor->second;
		}

		template <typename TE, typename T>
		static T* findmap(std::map<TE, T>& amap, const TE& aid)
		{
			auto itor = amap.find(aid);
			return itor == amap.end() ? nullptr : &itor->second;
		}

		template <typename TE, typename T>
		static T* findmap(std::unordered_map<TE, T>& amap, const TE& aid)
		{
			auto itor = amap.find(aid);
			return itor == amap.end() ? nullptr : &itor->second;
		}

		template <typename TE, typename T>
		static T* insertmap(std::map<TE, T>& amap, const TE& akey, const T& avalue)
		{
			auto lpair = amap.try_emplace(akey, avalue);
			return lpair.second ? &(lpair.first->second) : nullptr;
		}

		template <typename TE, typename T>
		static T* insertmap(std::unordered_map<TE, T>& amap, const TE& akey, const T& avalue)
		{
			auto lpair = amap.try_emplace(akey, avalue);
			return lpair.second ? &(lpair.first->second) : nullptr;
		}
#pragma endregion 

		// 是否是内网地址
		static bool is_lanip(const std::string& aip);

		template <typename T>
		static bool proto2json(const T& adata, std::string& json)
		{
			google::protobuf::util::JsonPrintOptions options;
			options.add_whitespace = false;
			options.always_print_primitive_fields = false;
			options.preserve_proto_field_names = false;
			return google::protobuf::util::MessageToJsonString(adata, &json, options).ok();		
		}

		// 以json格式打印pb数据
		template <typename T>
		static void print_json(const T& adata, bool aislog = false);

		// 通过json获取结构
		template <typename T>
		static bool json2proto(const std::string& json, T& adata)
		{
			google::protobuf::util::Status status = google::protobuf::util::JsonStringToMessage(json, &adata);
			return status.ok();
		}

		//EPROTOCOL_TYPE_CUSTOM
		template <typename T>
		static bool json2custom(const std::string& json, T& adata);

		template <typename T>
		static bool custom2json(T& adata, std::string& json);

		template <typename TKEY, typename TVAL>
		static void copy(const std::map<TKEY, TVAL>& asource, google::protobuf::Map<TKEY, TVAL>& atarget)
		{
			for (const std::pair<const TKEY, TVAL>& item : asource)
			{
				atarget.insert({ item.first, item.second });
			}
		}

		template <typename TKEY, typename TVAL>
		static void copy(const google::protobuf::Map<TKEY, TVAL>& asource, std::map<TKEY, TVAL>& atarget)
		{
			for (const google::protobuf::MapPair<const TKEY, TVAL>& item : asource)
			{
				atarget.insert({ item.first, item.second });
			}
		}
		
		template <typename To, typename From>
		static To lexical_cast(const From& from);

		template <typename To>
		static To& lexical_cast(To& from);

		template <typename T>
		struct parm
		{
			T m_value;

			parm(T avalue):
				m_value(avalue)
			{}

			parm() = delete;
			parm(const parm&) = delete;
			parm& operator=(const parm&) = delete;
		};

#pragma region bytesorder
		// # 是否是小端?
		static constexpr bool islittle()
		{
			return std::endian::native == std::endian::little;
		}

		// # 转换为小端
		static int16_t	transformlittle(parm<int16_t>& avalues);
		static uint16_t transformlittle(parm<uint16_t>& avalues);
		static int32_t	transformlittle(parm<int32_t>& avalues);
		static uint32_t transformlittle(parm<uint32_t>& avalues);
		static int64_t	transformlittle(parm<int64_t>& avalues);
		static uint64_t	transformlittle(parm<uint64_t>& avalues);
#pragma endregion

#pragma region base64
		static std::string base64_encode(const char* data, std::size_t len);
		static std::string base64_encode(std::string const& s);
		static std::string base64_decode(char const* data, std::size_t len);
		static std::string base64_decode(std::string const& data);
#pragma endregion

		static bool uuid_make(std::string& astr);

#pragma region asscii_utf8
		// 宽字符与ASCII相互转换  utf8
		static bool wasscii2asscii(const std::wstring& awstr, std::string& astr);
		static bool asscii2wasscii(const std::string& astr, std::wstring& awstr);
		static bool wasscii2utf8(const std::wstring& awstr, std::string& astr);
		static bool utf82wasscii(const std::string& astr, std::wstring& awstr);

		static bool to_asscii(const std::string& astr1, std::string& astr2);
		static bool to_utf8(const std::string& astr1, std::string& astr2);
#pragma endregion

#pragma region regular
		static void sregex(const std::string& apattern, const std::string& adata, const std::function<void(std::string&)>& afun);

		static void smatch(const std::string& aexpression, const std::string& adata, const std::function<void(std::smatch&)>& afun);

		static void replace(const std::string& aexpression, const std::string& areleace, const std::string& adata, std::string& aret);

		static void replace_ret(const std::string& aexpression, const std::string& areleace, std::string& adata, std::string& aret);
#pragma endregion

#pragma region splite
		template <typename T>
		static bool splite(const char* abuff, const char* afg, std::vector<T>& avec)
		{
			std::vector<std::string> lvec;
			if (splite(abuff, afg, lvec) == false)
			{
				return false;
			}
			for (const auto& item : lvec)
			{
				avec.push_back(tools::lexical_cast<T>(item.c_str()));
			}
			return true;
		}

		template <typename T>
		static bool splite(const char* abuff, const char* afg, std::set<T>& aset)
		{
			std::vector<std::string> lvec;
			if (splite(abuff, afg, lvec) == false)
			{
				return false;
			}
			for (const auto& item : lvec)
			{
				aset.insert(tools::lexical_cast<T>(item.c_str()));
			}
			return true;
		}

		static bool splite(const char* abuff, const char* afg, std::vector<std::string>& avec);

		template <typename ...ARGS>
		static bool splite(const char* abuff, const char* afg, ARGS&... args)
		{
			std::vector<std::string> lvec;
			splite(abuff, afg, lvec);
			return splite(0, lvec, args...);
		}

	private:
		template <typename T>
		static bool splite(int32_t aindex, std::vector<std::string>& avec, T& adata)
		{
			if (aindex >= avec.size())
			{
				return false;
			}
			try
			{
				adata = tools::lexical_cast<T>(avec[aindex].c_str());
			}
			catch (...)
			{
				return false;
			}
			return true;
		}

		template <typename T, typename ...ARGS>
		static bool splite(int32_t aindex, std::vector<std::string>& avec, T& adata, ARGS&... args)
		{
			if (aindex >= avec.size())
			{
				return false;
			}
			try
			{
				adata = tools::lexical_cast<T>(avec[aindex].c_str());
			}
			catch (...)
			{
				return false;
			}
			return splite(++aindex, avec, args...);
		}
	public:

		// 特殊分割:类似"接收邮件列表[邮件地址1:名字1]"
		// [348634371@qq.com:libo][libo1@youxigu.com:libo1]
		template <typename TFIRST = std::string, typename TSECOND = std::string>
		static bool splite_special(
			const char* astr
			, const char* akey1
			, const char* akey2
			, std::vector<std::pair<TFIRST, TSECOND>>& avec
		)
		{
			std::string ltemp = astr;
			ngl::tools::replace_ret(akey1, "", ltemp, ltemp);
			std::vector<std::string> lvec;
			if (ngl::tools::splite(ltemp.c_str(), akey2, lvec) == false)
			{
				return false;
			}
			std::vector<std::pair<std::string, std::string>> lmailvec;
			for (const auto& item : lvec)
			{
				std::pair<TFIRST, TSECOND> lpair;
				if (ngl::tools::splite(item.c_str(), ":", lpair.first, lpair.second) == false)
				{
					return false;
				}
				lmailvec.push_back(lpair);
			}
			return true;
		}

		template <typename TFIRST = std::string, typename TSECOND = std::string>
		static bool splite_special(const char* astr, const char* akey1, const char* akey2, std::map<TFIRST, TSECOND>& amap)
		{
			std::string ltemp = astr;
			ngl::tools::replace_ret(akey1, "", ltemp, ltemp);
			std::vector<std::string> lvec;
			if (ngl::tools::splite(ltemp.c_str(), akey2, lvec) == false)
			{
				return false;
			}
			for (const auto& item : lvec)
			{
				std::pair<TFIRST, TSECOND> lpair;
				if (ngl::tools::splite(item.c_str(), ":", lpair.first, lpair.second) == false)
				{
					return false;
				}
				amap.insert(lpair);
			}
			return true;
		}
#pragma endregion

#pragma region splicing

		template <typename T>
		static std::function<std::string(const T&)> m_splicing;
		template <typename TKEY,typename TVALUE>
		static std::function<std::string(const TKEY&, const TVALUE&)> m_splicingmap;

		template <typename T>
		static bool splicing(
			const std::vector<T>& avec
			, const char* afg
			, std::string& astr
			, const std::function<std::string(const T&)> afunction = m_splicing<T>
		)
		{
			for (int i = 0; i < avec.size(); ++i)
			{
				if (i != 0)
				{
					astr += afg;
				}
				astr += afunction(avec[i]);
			}
			return true;
		}

		static bool splicing(const std::vector<std::string>& avec, const char* afg, std::string& astr)
		{
			for (int i = 0; i < avec.size(); ++i)
			{
				if (i != 0)
				{
					astr += afg;
				}
				astr += avec[i];
			}
			return true;
		}

		template <typename T>
		static bool splicing(
			const std::set<T>& avec, const char* afg, std::string& astr, const std::function<std::string(const T&)> afunction = m_splicing<T>
		)
		{
			int i = 0;
			for (auto itor = avec.begin(); i < avec.size(); ++i,++itor)
			{
				if (i != 0)
				{
					astr += afg;
				}
				astr += afunction(*itor);
			}
			return true;
		}

		template <typename TKEY, typename TVALUE>
		static bool splicing(
			const std::map<TKEY, TVALUE>& avec, const char* afg, std::string& astr, const std::function<std::string(const TKEY&, const TVALUE&)> afunction = m_splicingmap<TKEY,TVALUE>
		)
		{
			bool lbool = false;
			for (const auto& item : avec)
			{
				if (lbool)
				{
					astr += afg;
				}
				afunction(item.first, item.second);
				lbool = true;
			}
			return true;
		}

		template <typename TKEY, typename TVALUE>
		static bool splicing(
			const google::protobuf::Map<TKEY, TVALUE>& avec, const char* afg, std::string& astr,const std::function<std::string(const TKEY&,const TVALUE&)> afunction = m_splicingmap<TKEY, TVALUE>
		)
		{
			bool lbool = false;
			for (const auto& item : avec)
			{
				if (lbool)
				{
					astr += afg;
				}
				afunction(item.first, item.second);
				lbool = true;
			}
			return true;
		}

		template <typename T>
		static bool splicing(const std::set<std::string>& avec, const char* afg, std::string& astr)
		{
			int i = 0;
			for (auto itor = avec.begin(); i < avec.size(); ++i, ++itor)
			{
				if (i != 0)
				{
					astr += afg;
				}
				astr += *itor;
			}
			return true;
		}

		template <typename ...ARGS>
		static bool splicing(const char* afg, std::string& astr, ARGS... args)
		{
			return splicing(0, afg, astr, args...);
		}

	private:
		template <typename T>
		static bool splicing(int32_t aindex, const char* afg, std::string& astr, T& adata)
		{
			try
			{
				if (aindex != 0)
				{
					astr += afg;
				}
				astr += tools::lexical_cast<std::string>(adata);
			}
			catch (...)
			{
				return false;
			}
			return true;
		}

		template <typename T, typename ...ARGS>
		static bool splicing(int32_t aindex, const char* afg, std::string& astr, T& adata, ARGS&... args)
		{
			try
			{
				if (aindex != 0)
				{
					astr += afg;
				}
				astr += tools::lexical_cast<std::string>(adata);
			}
			catch (...)
			{
				return false;
			}
			return splicing(++aindex, afg, astr, args...);
		}
	public:
#pragma endregion

#pragma region url
		static std::string char2hex(char dec);
		static std::string url_encode(const std::string& c);
		static std::string url_decode(const std::string& c);
#pragma endregion

#pragma region varint
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// Varint是一种使用一个或多个字节序列化整数的方法会把整数编码为变长字节。
		// 对于32位整型数据经过Varint编码后需要1~5个字节，
		// 小的数字使用1个byte，大的数字使用5个bytes。
		// 64位整型数据编码后占用1~10个字节。
		// 在实际场景中小数字的使用率远远多于大数字，
		// 因此通过Varint编码对于大部分场景都可以起到很好的压缩效果。
	
		static int varint_length(parm<int32_t>& avalue);
		static int varint_length(parm<int64_t>& avalue);

		template <typename T>
		struct varint_parm
		{
			T m_value;
			char* m_buf = nullptr;
			int m_len = 0;
			int32_t* m_bytes = 0;
		};
		static bool varint_encode(varint_parm<int64_t>& aparm);
		static bool	varint_decode(varint_parm<int64_t>& aparm);
		static bool	varint_encode(varint_parm<int32_t>& aparm);
		static bool	varint_decode(varint_parm<int32_t>& aparm);

	#pragma endregion

#pragma region hexbytes
		static int	to_hex(void* apso, int alen, void* apto);
		static bool to_bytes(void* apso, int alen, void* apto, int& apotlen);
#pragma endregion

#pragma region encryption
		static void bytexor(char* ap, int32_t aplen, int apos);
#pragma endregion

		static std::string& type_name_handle(std::string& aname)
		{
			ngl::tools::replace("struct ", "", aname, aname);
			ngl::tools::replace("class ", "", aname, aname);
			ngl::tools::replace("ngl::", "", aname, aname);
			ngl::tools::replace("pbdb::", "", aname, aname);
			ngl::tools::replace("pbnet::", "", aname, aname);
			ngl::tools::replace("pbexample::", "", aname, aname);
			ngl::tools::replace(" ", "", aname, aname);
			return aname;
		}

		template <typename T>
		static std::string& type_name()
		{
			static std::string lname = typeid(T).name();
			static std::atomic<bool> lfirst = true;
			if (lfirst.exchange(false))
			{
				type_name_handle(lname);
			}
			return lname;
		}

		static std::string md5(const std::string& text);

		static std::string sh1(std::string_view text);

		static std::string hmac_sha1(const std::string& key, const std::string& text);

		static std::string time2str(int autc, const char* aformat = "%y/%m/%d %H:%M:%S");

		static std::vector<std::string_view> get_line(const char* apbuff, size_t abuffsize);

		static bool isnumber(const std::string& anumber);

		static const std::string& server_name();

		static int rand();

		// 字符串大小写转换
		static void transform_tolower(std::string& adata);
		static void transform_toupper(std::string& adata);

		// 判断文件夹是否存在
		static bool directories_exists(const std::string& apath);

		// 判断文件是否存在
		static bool file_exists(const std::string& apath);

		// 创建目录
		static bool create_directories(const std::string& apath);

		// 移除文件
		static bool file_remove(const std::string& afilename);

		// # 获取dir下所有文件
		// # aiteration是否循环迭代dir下的所有目录
		static void dir(const std::string& apath, std::vector<std::string>& afilevec, bool aiteration = false);

		static std::string& format_json(std::string& ajson);

		// 不可恢复的异常，直接生成core dump
		static void no_core_dump(bool anocreate = false);

		// nguid str(actor_type#areaid#dataid) => nguid int64_t
		static int64_t nguidstr2int64(const char* anguidstr);

		static std::vector<const char*> split_str(char* apbuff, int32_t abuffcount);

		// 检查cjson类型
		static bool check_cjson_or(int32_t atype, int32_t acjsontype)
		{
			return (atype & acjsontype) != 0;
		}

		template <typename ...TARGS>
		static bool check_cjson_or(int32_t atype, int32_t acjsontype, TARGS... args)
		{
			return check_cjson_or(atype, acjsontype) || check_cjson_or(atype, args...);
		}

		static bool check_cjson_and(int32_t atype, int32_t acjsontype)
		{
			return (atype & acjsontype) == 0;
		}

		template <typename ...TARGS>
		static bool check_cjson_and(int32_t atype, int32_t acjsontype, TARGS... args)
		{
			return check_cjson_and(atype, acjsontype) && check_cjson_and(atype, args...);
		}
	};

	template <typename T>
	std::function<std::string(const T&)> tools::m_splicing = [](const T& adata)
		{
			return tools::lexical_cast<std::string>(adata);
		};


	template <typename T, typename = void>
	struct has_protobuf_descriptor;

	template <typename T>
	struct has_protobuf_descriptor<
		T,
		std::void_t<decltype(T::descriptor())>
	> : std::is_same<
		decltype(T::descriptor()),
		const google::protobuf::Descriptor*
	> {};

	template <typename T, typename>
	struct has_protobuf_descriptor : std::false_type {};

	template <typename T>
	struct is_protobuf_message : std::conjunction<
		std::is_base_of<google::protobuf::MessageLite, std::remove_cv_t<std::remove_reference_t<T>>>,
		has_protobuf_descriptor<std::remove_cv_t<std::remove_reference_t<T>>>
	> {};


}//namespace ngl