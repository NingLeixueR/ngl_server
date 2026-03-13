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
#include <string_view>
#include <functional>
#include <stdexcept>
#include <algorithm>
#include <charconv>
#include <iostream>
#include <utility>
#include <sstream>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <compare>
#include <format>
#include <string>
#include <cctype>
#include <atomic>
#include <regex>
#include <tuple>
#include <array>
#include <limits>
#include <map>
#include <set>
#include <bit>
#include <typeinfo>

#include <boost/lexical_cast.hpp>
#include <google/protobuf/util/json_util.h>

namespace ngl
{
	template <typename T>
	struct np_actormodule_forward;
	template <typename TDATA>
	using mforward = np_actormodule_forward<TDATA>;

	namespace detail
	{
		template <typename T>
		using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

		template <typename T>
		inline constexpr bool is_char_pointer_v =
			std::is_pointer_v<remove_cvref_t<T>> &&
			std::is_same_v<std::remove_cv_t<std::remove_pointer_t<remove_cvref_t<T>>>, char>;

		template <typename T>
		inline constexpr bool is_char_array_v =
			std::is_array_v<remove_cvref_t<T>> &&
			std::is_same_v<std::remove_cv_t<std::remove_extent_t<remove_cvref_t<T>>>, char>;

		template <typename T>
		inline constexpr bool is_string_like_v =
			std::is_same_v<remove_cvref_t<T>, std::string> ||
			std::is_same_v<remove_cvref_t<T>, std::string_view> ||
			is_char_pointer_v<T> ||
			is_char_array_v<T>;

		template <typename T>
		static std::string_view as_string_view(const T& avalue)
		{
			if constexpr (std::is_same_v<remove_cvref_t<T>, std::string> || std::is_same_v<remove_cvref_t<T>, std::string_view>)
			{
				return std::string_view(avalue);
			}
			else if constexpr (is_char_pointer_v<T>)
			{
				return avalue != nullptr ? std::string_view(avalue) : std::string_view{};
			}
			else
			{
				return std::string_view(avalue);
			}
		}

		inline char first_non_space(std::string_view avalue)
		{
			const auto liter = std::find_if_not(avalue.begin(), avalue.end(), [](unsigned char achar)
			{
				return std::isspace(achar) != 0;
			});
			return liter == avalue.end() ? '\0' : *liter;
		}
	}

	class tools
	{
		tools() = delete;
		tools(const tools&) = delete;
		tools& operator=(const tools&) = delete;

	public:
#pragma region map
		// map操作封装
		template <typename TMAP, typename TKEY>
		static auto findmap(TMAP& amap, const TKEY& akey)
		{
			auto itor = amap.find(akey);
			return itor == amap.end() ? nullptr : &itor->second;
		}

		template <typename TMAP, typename TKEY>
		static auto insertmap(TMAP& amap, const TKEY& akey, const typename TMAP::value_type::second& avalue)
		{
			auto lpair = amap.try_emplace(akey, avalue);
			return lpair.second ? &(lpair.first->second) : nullptr;
		}

		template <typename TMAP, typename TKEY>
		static bool erasemap(TMAP& amap, const TKEY& akey, typename TMAP::value_type::second_type& adata)
		{
			if (auto node = amap.extract(akey); node)
			{
				adata = std::move(node.mapped());
				return true;
			}
			return false;
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

		template <typename T>
		static bool json2custom(const std::string& json, T& adata);

		template <typename T>
		static bool custom2json(const T& adata, std::string& json);

		template <typename TKEY, typename TVAL>
		static void copy(const std::map<TKEY, TVAL>& asource, google::protobuf::Map<TKEY, TVAL>& atarget)
		{
			for(auto& [_key, _value] : asource)
			{
				atarget.insert({ _key, _value });
			}
		}

		template <typename TKEY, typename TVAL>
		static void copy(const google::protobuf::Map<TKEY, TVAL>& asource, std::map<TKEY, TVAL>& atarget)
		{
			for (auto& [_key, _value] : asource)
			{
				atarget.insert({ _key, _value });
			}
		}

		static void log_lexical_cast_error(
			const char* atotype,
			const char* afromtype,
			const char* aerror,
			const std::source_location& asource
		);

		template <typename To, typename From>
		static To lexical_cast(const From& from, const std::source_location& asource = std::source_location::current())
		{
			if constexpr (std::is_integral_v<To> && std::is_unsigned_v<To>)
			{
				if constexpr (std::is_integral_v<detail::remove_cvref_t<From>> && std::is_signed_v<detail::remove_cvref_t<From>>)
				{
					if (from < 0)
					{
						const boost::bad_lexical_cast lerror(typeid(From), typeid(To));
						log_lexical_cast_error(typeid(To).name(), typeid(From).name(), lerror.what(), asource);
						throw lerror;
					}
				}
				else if constexpr (detail::is_string_like_v<From>)
				{
					if (detail::first_non_space(detail::as_string_view(from)) == '-')
					{
						const boost::bad_lexical_cast lerror(typeid(From), typeid(To));
						log_lexical_cast_error(typeid(To).name(), typeid(From).name(), lerror.what(), asource);
						throw lerror;
					}
				}
			}

			try
			{
				return boost::lexical_cast<To>(from);
			}
			catch (const boost::bad_lexical_cast& aerror)
			{
				log_lexical_cast_error(typeid(To).name(), typeid(From).name(), aerror.what(), asource);
				throw;
			}
		}
		
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

		static int32_t utf8firstbyte(uint8_t firstbyte);
		// 是否是utf8编码
		static bool isutf8(const std::string& astr1);
		// utf8中是否包含mb4(utf8为mb3)
		static bool isincludeutf8mb4(const std::string& astr);

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
			std::vector<T> lparsed;
			lparsed.reserve(lvec.size());
			try
			{
				for (auto& item : lvec)
				{
					lparsed.emplace_back(tools::lexical_cast<T>(item.c_str()));
				}
			}
			catch (const boost::bad_lexical_cast&)
			{
				throw std::string("lexical_cast error");
			}
			avec = std::move(lparsed);
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
			std::set<T> lparsed;
			try
			{
				for (auto& item : lvec)
				{
					lparsed.emplace(tools::lexical_cast<T>(item.c_str()));
				}
			}
			catch (const boost::bad_lexical_cast&)
			{
				throw std::string("lexical_cast error");
			}
			aset = std::move(lparsed);
			return true;
		}

		static bool splite(const char* abuff, const char* afg, std::vector<std::string>& avec);

		template <typename T>
		static bool splite(int32_t aindex, const std::vector<std::string>& avec, T& adata)
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

		template <std::size_t... INDEX, typename TTUPLE>
		static bool splite_tuple(std::index_sequence<INDEX...>, const std::vector<std::string>& avec, TTUPLE& atuple)
		{
			return (splite<std::tuple_element_t<INDEX, TTUPLE>>(static_cast<int32_t>(INDEX), avec, std::get<INDEX>(atuple)) && ...);
		}

		template <std::size_t... INDEX, typename ...ARGS>
		static bool splite(std::index_sequence<INDEX...>, const char* abuff, const char* afg, ARGS&... args)
		{
			std::vector<std::string> lvec;
			if (splite(abuff, afg, lvec) == false)
			{
				return false;
			}

			std::tuple<std::decay_t<ARGS>...> lparsed;
			if (splite_tuple(std::index_sequence<INDEX...>{}, lvec, lparsed) == false)
			{
				return false;
			}

			((args = std::move(std::get<INDEX>(lparsed))), ...);
			return true;
		}

		template <typename ...ARGS>
		static bool splite(const char* abuff, const char* afg, ARGS&... args)
		{
			return 	splite(std::make_index_sequence<sizeof...(ARGS)>{}, abuff, afg, args...);
		}

		// 特殊分割:类似"接收邮件列表[邮件地址1:名字1]"
		// [348634371@qq.com:libo][libo1@youxigu.com:libo1]
		template <typename TFIRST = std::string, typename TSECOND = std::string>
		static bool splite_special(const char* astr, const char* akey1, const char* akey2, std::vector<std::pair<TFIRST, TSECOND>>& avec)
		{
			std::string ltemp = astr;
			ngl::tools::replace_ret(akey1, "", ltemp, ltemp);
			std::vector<std::string> lvec;
			if (ngl::tools::splite(ltemp.c_str(), akey2, lvec) == false)
			{
				return false;
			}
			std::vector<std::pair<TFIRST, TSECOND>> lmailvec;
			lmailvec.reserve(lvec.size());
			for (auto& item : lvec)
			{
				std::pair<TFIRST, TSECOND> lpair;
				if (ngl::tools::splite(item.c_str(), ":", lpair.first, lpair.second) == false)
				{
					return false;
				}
				lmailvec.emplace_back(std::move(lpair));
			}
			avec = std::move(lmailvec);
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
			std::map<TFIRST, TSECOND> lparsed;
			for (auto& item : lvec)
			{
				std::pair<TFIRST, TSECOND> lpair;
				if (ngl::tools::splite(item.c_str(), ":", lpair.first, lpair.second) == false)
				{
					return false;
				}
				lparsed.emplace(std::move(lpair));
			}
			amap.insert(lparsed.begin(), lparsed.end());
			return true;
		}
#pragma endregion

#pragma region splicing

		template <typename T>
		static std::function<std::string(const T&)> m_splicing;
		template <typename TKEY,typename TVALUE>
		static std::function<std::string(const TKEY&, const TVALUE&)> m_splicingmap;

		template <typename T>
		static bool splicing(const std::vector<T>& avec, const char* afg, std::string& astr, const std::function<std::string(const T&)> afunction = m_splicing<T>)
		{
			for (std::size_t i = 0; i < avec.size(); ++i)
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
			const std::size_t lfglen = std::strlen(afg);
			std::size_t lappend_size = avec.empty() ? 0 : lfglen * (avec.size() - 1);
			for (const auto& item : avec)
			{
				lappend_size += item.size();
			}
			astr.reserve(astr.size() + lappend_size);

			for (std::size_t i = 0; i < avec.size(); ++i)
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
		static bool splicing(const std::set<T>& avec, const char* afg, std::string& astr, const std::function<std::string(const T&)> afunction = m_splicing<T>)
		{
			for (auto itor = avec.begin(); itor != avec.end(); ++itor)
			{
				if (itor != avec.begin())
				{
					astr += afg;
				}
				
				astr += afunction(*itor);
			}
			return true;
		}

		template <typename TKEY, typename TVALUE>
		static bool splicing(const std::map<TKEY, TVALUE>& amap, const char* afg, std::string& astr, const std::function<std::string(const TKEY&, const TVALUE&)> afunction = m_splicingmap<TKEY,TVALUE>)
		{
			for(auto itor = amap.begin(); itor != amap.end();++itor)
			{
				if (itor != amap.begin())
				{
					astr += afg;
				}
				astr += afunction(itor->first, itor->second);
			}
			return true;
		}

		template <typename TKEY, typename TVALUE>
		static bool splicing(const google::protobuf::Map<TKEY, TVALUE>& amap, const char* afg, std::string& astr,const std::function<std::string(const TKEY&,const TVALUE&)> afunction = m_splicingmap<TKEY, TVALUE>)
		{
			for (auto itor = amap.begin(); itor != amap.end(); ++itor)
			{
				if (itor != amap.begin())
				{
					astr += afg;
				}
				astr += afunction(itor->first, itor->second);
			}
			return true;
		}

		template <typename T>
		static bool splicing(const std::set<std::string>& aset, const char* afg, std::string& astr)
		{
			const std::size_t lfglen = std::strlen(afg);
			std::size_t lappend_size = aset.empty() ? 0 : lfglen * (aset.size() - 1);
			for (const auto& item : aset)
			{
				lappend_size += item.size();
			}
			astr.reserve(astr.size() + lappend_size);

			for (auto itor = aset.begin(); itor != aset.end(); ++itor)
			{
				if (itor != aset.begin())
				{
					astr += afg;
				}
				astr += *itor;
			}
			return true;
		}

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
		
		template <std::size_t... INDEX, typename ...ARGS>
		static bool splicing(std::index_sequence<INDEX...>, const char* afg, std::string& astr, ARGS&... args)
		{
			return ((splicing<ARGS>(INDEX, afg, astr, args)) && ...);
		}

		template <typename ...ARGS>
		static bool splicing(const char* afg, std::string& astr, ARGS&... args)
		{
			return splicing(std::make_index_sequence<sizeof...(ARGS)>{}, afg, astr, args...);
		}
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

		static std::string& type_name_handle(std::string& aname);

		template <typename T>
		static std::string& type_name()
		{
			static std::string lname = typeid(T).name();
			static std::atomic lfirst = true;
			if (lfirst.exchange(false))
			{
				type_name_handle(lname);
			}
			return lname;
		}

		template <typename T>
		static std::string& type_name(const T*)
		{
			return type_name<T>();
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
		static bool create_dir(const std::string& apath);

		// 移除文件
		static bool file_remove(const std::string& afilename);

		// # 获取dir下所有文件
		// # aiteration是否循环迭代dir下的所有目录
		static void dir(const std::string& apath, std::vector<std::string>& afilevec, bool aiteration = false);

		// 不可恢复的异常，直接生成core dump
		static void no_core_dump(bool anocreate = false);

		static std::function<void()> send_mail(const std::string& acontent);

		// nguid str(actor_type#areaid#dataid) => nguid int64_t
		static int64_t nguidstr2int64(const char* anguidstr);

		static std::vector<const char*> split_str(char* apbuff, int32_t abuffcount);

		template <std::size_t N>
		static void split_str(char* apbuff, int32_t abuffcount, std::array<const char*, N>& aarrays)
		{
			std::vector<const char*> lvec = split_str(apbuff, abuffcount);
			aarrays.fill(nullptr);
			const auto lcopy_count = std::min<std::size_t>(lvec.size(), aarrays.size());
			std::copy_n(lvec.begin(), lcopy_count, aarrays.begin());
			return;
		}
		// 检查cjson类型
		static bool bit(int32_t atype, int32_t acjsontype)
		{
			return (atype & acjsontype) != 0;
		}

		//删除重复多余的ach
		static void erase_repeat(std::string& astrbuff, const char ach);

		// 比较是否相等
		template <typename ETYPE>
		static bool equal(ETYPE avalue1, ETYPE avalue2)
		{
			if (avalue1 == avalue2)
			{
				return true;
			}
			return false;
		}


		static int32_t less_member()
		{
			return 0;
		}

		template <typename T>
		static auto less_member(const T& lhs, const T& rhs)
		{
			return lhs <=> rhs;
		}

		template <typename T, typename... TARGS>
		static auto less_member(const T& lhs, const T& rhs, const TARGS&... members)
		{
			if (lhs != rhs)
			{
				return less_member(lhs, rhs);
			}
			return less_member(members...);
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
	struct has_protobuf_descriptor<T, std::void_t<decltype(T::descriptor())>> : std::is_same<
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
