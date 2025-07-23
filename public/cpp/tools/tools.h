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

#include "json_read.h"
#include "json_write.h"


#ifdef WIN32
#define snprintf _snprintf
#endif //WIN32

enum EPROTOCOL_TYPE
{
	EPROTOCOL_TYPE_CUSTOM,			// 自定义二进制协议
	EPROTOCOL_TYPE_PROTOCOLBUFF,	// protobuff协议
	EPROTOCOL_TYPE_COUNT,
	EPROTOCOL_TYPE_ERROR,
};

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

	template <>
	struct lexical_cast2<int32_t>
	{
		static int32_t fun(const std::string& source)
		{
			return atoi(source.c_str());
		}
		static int32_t fun(const char* source)
		{
			return atoi(source);
		}
	};

	template <>
	struct lexical_cast2<uint32_t>
	{
		static uint32_t fun(const std::string& source)
		{
			return atoi(source.c_str());
		}
		static uint32_t fun(const char* source)
		{
			return atoi(source);
		}
	};

	template <>
	struct lexical_cast2<int16_t>
	{
		static int16_t fun(const std::string& source)
		{
			return atoi(source.c_str());
		}
		static int16_t fun(const char* source)
		{
			return atoi(source);
		}
	};

	template <>
	struct lexical_cast2<uint16_t>
	{
		static uint16_t fun(const std::string& source)
		{
			return atoi(source.c_str());
		}
		static uint16_t fun(const char* source)
		{
			return atoi(source);
		}
	};

	template <>
	struct lexical_cast2<int8_t>
	{
		static int8_t fun(const std::string& source)
		{
			return atoi(source.c_str());
		}
		static int8_t fun(const char* source)
		{
			return atoi(source);
		}
	};

	template <>
	struct lexical_cast2<uint8_t>
	{
		static uint8_t fun(const std::string& source)
		{
			return atoi(source.c_str());
		}
		static uint8_t fun(const char* source)
		{
			return atoi(source);
		}
	};

	template <>
	struct lexical_cast2<int64_t>
	{
		static int64_t fun(const std::string& source)
		{
			return atoll(source.c_str());
		}

		static int64_t fun(const char* source)
		{
			return atoll(source);
		}
	};

	template <>
	struct lexical_cast2<uint64_t>
	{
		static uint64_t fun(const std::string& source)
		{
			return atoll(source.c_str());
		}

		static uint64_t fun(const char* source)
		{
			return atoll(source);
		}
	};

	template <>
	struct lexical_cast2<float>
	{
		static float fun(const std::string& source)
		{
			return (float)std::atof(source.c_str());
		}

		static float fun(const char* source)
		{
			return (float)std::atof(source);
		}
	};

	template <>
	struct lexical_cast2<double>
	{
		static double fun(const std::string& source)
		{
			return std::atof(source.c_str());
		}

		static double fun(const char* source)
		{
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

		template <typename T>
		static bool proto2json(const mforward<T>& adata, std::string& json);

		template <typename TKEY, typename TVALUE>
		static bool proto2json(const std::map<TKEY, TVALUE>& adata, std::string& json)
		{
			json = "{\"";
			json += tools::type_name<TVALUE>();
			json += "\":{";
			bool lfirst = true;
			for (const auto item : adata)
			{
				if (lfirst)
				{
					lfirst = false;
				}
				else
				{
					json += ",";
				}
				std::string ltemp;
				if (!proto2json(item.second, ltemp))
				{
					continue;
				}
				json += "\"";
				json += tools::lexical_cast<std::string>(item.first);
				json += "\":";
				json += ltemp;
			}
			json += "}}";
			return true;
		}

		template <typename TKEY, typename TVALUE>
		static bool proto2json(const std::map<TKEY, TVALUE*>& adata, std::string& json)
		{
			json = "{\"";
			json += tools::type_name<TVALUE>();
			json += "\":{";
			bool lfirst = true;
			for (const auto item : adata)
			{
				if (lfirst)
				{
					lfirst = false;
				}
				else
				{
					json += ",";
				}
				std::string ltemp;
				if (!proto2json(*item.second, ltemp))
				{
					continue;
				}
				json += "\"";
				json += tools::lexical_cast<std::string>(item.first);
				json += "\":";
				json += ltemp;
			}
			json += "}}";
			return true;
		}


		// 以json格式打印pb数据
		template <typename T>
		static void print_json2proto(const T& adata, bool aislog = false);

		// 通过json获取结构
		template <typename T>
		static bool json2proto(const std::string& json, T& adata)
		{
			google::protobuf::util::Status status = google::protobuf::util::JsonStringToMessage(json, &adata);
			return status.ok();
		}

		template <typename TKEY, typename TVALUE>
		static bool json2proto(
			const std::string& json
			, std::map<TKEY, TVALUE>& adata
		)
		{
			json_read ltempread(json.c_str());
			std::map<std::string, json_read> lmap;
			if (!ltempread.read(tools::type_name<TVALUE>().c_str(), lmap))
			{
				return false;
			}

			for (const auto& item :lmap)
			{
				std::string ltemp = item.second.get();
				TVALUE ltempt;
				if (json2proto(ltemp, ltempt))
				{
					adata[tools::lexical_cast<TKEY>(item.first)] = ltempt;
				}
			}
			return true;
		}

		//EPROTOCOL_TYPE_CUSTOM
		template <typename T>
		static bool json2custom(const std::string& json, T& adata)
		{
			json_read ljread(json);
			return ljread.read(tools::type_name<T>().c_str(), adata);
		}

		template <typename TKEY, typename TVALUE>
		static bool json2custom(const std::string& json, std::map<TKEY, TVALUE>& adata)
		{
			json_read ljread(json);
			return ljread.read(tools::type_name<TVALUE>().c_str(), adata);
		}

		template <typename T>
		static void custom2json(const T& adata, std::string& ajson)
		{
			json_write ljwrite;
			ljwrite.write(tools::type_name<T>().c_str(), adata);
			ljwrite.get(ajson);
		}

		template <typename TKEY, typename TVALUE>
		static bool custom2json(const std::map<TKEY, TVALUE>& adata, std::string& ajson)
		{
			json_write ljwrite;
			ljwrite.write(tools::type_name<TVALUE>().c_str(), adata);
			ljwrite.get(ajson);
			return true;
		}

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
			splite(0, lvec, args...);
			return true;
		}

	private:
		template <typename T>
		static bool splite(int32_t aindex, std::vector<std::string>& avec, T& adata)
		{
			if (aindex > avec.size())
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

		template <typename T>
		class protobuf_tabname
		{
			static std::string m_name;
		public:
			static std::string& name()
			{
				if (m_name.empty())
				{
					m_name = T().descriptor()->full_name();
					ngl::tools::replace("pbdb.", "", m_name, m_name);
					ngl::tools::replace("pbnet.", "", m_name, m_name);
					ngl::tools::replace("pbexample.", "", m_name, m_name);
				}
				return m_name;
			}
		};

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
	};

	template <typename T>
	std::string tools::protobuf_tabname<T>::m_name;

	template <typename T>
	std::function<std::string(const T&)> tools::m_splicing = [](const T& adata)
		{
			return tools::lexical_cast<std::string>(adata);
		};
}//namespace ngl

namespace ngl
{
	template <typename KEY, typename VAL>
	void json_write::write_mapnumber(const char* akey, const std::map<KEY, VAL>& aval)
	{
		json_write ltemp;
		for (const auto& item : aval)
		{
			ltemp.write(tools::lexical_cast<std::string>(item.first).c_str(), item.second);
		}
		write(akey, ltemp);
	}

	template <typename KEY, typename VAL>
	bool json_read::read_mapnumber(const char* akey, std::map<KEY, VAL>& aval) const
	{
		json_read ltemp;
		if (read(akey, ltemp) == false)
		{
			return false;
		}

		for (cJSON* child = ltemp.m_json->child; child != nullptr; child = child->next)
		{
			KEY lkey = tools::lexical_cast<KEY>(child->string);
			if (!ltemp.read(child->string, aval[lkey]))
			{
				return false;
			}				
		}
		return true;
	}
}