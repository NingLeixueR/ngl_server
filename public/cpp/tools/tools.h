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
// File overview: Declares interfaces for tools.

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
		inline constexpr std::string_view url_hex_digits = "0123456789ABCDEF";
		inline constexpr std::size_t g_TN_NPOS = std::string_view::npos;

		template <typename T>
		using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

		constexpr std::string_view ext_type(
			std::string_view signature,
			std::size_t prefix_start,
			std::size_t prefix_size,
			std::size_t type_end = g_TN_NPOS) noexcept
		{
			const std::size_t type_begin = prefix_start == g_TN_NPOS ? 0 : prefix_start + prefix_size;
			const bool has_prefix = prefix_start != g_TN_NPOS;
			const bool has_end = type_end != g_TN_NPOS && type_end > type_begin;
			return !has_prefix ? signature : (has_end ? signature.substr(type_begin, type_end - type_begin) : signature.substr(type_begin));
		}

		constexpr std::size_t find_tend(std::string_view signature, std::size_t type_begin, std::string_view suffix) noexcept
		{
			return signature.find(suffix, type_begin);
		}

		constexpr std::size_t find_tend(std::string_view signature, std::size_t type_begin, char suffix) noexcept
		{
			return signature.find(suffix, type_begin);
		}

		constexpr std::size_t find_tend(
			std::string_view signature,
			std::size_t type_begin,
			char first_suffix,
			char second_suffix) noexcept
		{
			const std::size_t first_end = signature.find(first_suffix, type_begin);
			const std::size_t second_end = signature.find(second_suffix, type_begin);
			return first_end == g_TN_NPOS ? second_end
				: (second_end == g_TN_NPOS ? first_end : std::min(first_end, second_end));
		}

		template <typename T>
		constexpr std::string_view comp_type() noexcept
		{
#if defined(_MSC_VER)
			// Parse the instantiated type directly from the compiler signature so
			// forward-declared types still produce stable names.
			constexpr std::string_view signature = __FUNCSIG__;
			constexpr std::string_view prefix = "comp_type<";
			constexpr std::size_t prefix_start = signature.find(prefix);
			constexpr std::size_t type_begin = prefix_start == g_TN_NPOS ? 0 : prefix_start + prefix.size();
			constexpr std::size_t type_end = find_tend(signature, type_begin, ">(void");
			return ext_type(signature, prefix_start, prefix.size(), type_end);
#elif defined(__clang__)
			constexpr std::string_view signature = __PRETTY_FUNCTION__;
			constexpr std::string_view prefix = "T = ";
			constexpr std::size_t prefix_start = signature.find(prefix);
			constexpr std::size_t type_begin = prefix_start == g_TN_NPOS ? 0 : prefix_start + prefix.size();
			constexpr std::size_t type_end = find_tend(signature, type_begin, ']');
			return ext_type(signature, prefix_start, prefix.size(), type_end);
#elif defined(__GNUC__)
			constexpr std::string_view signature = __PRETTY_FUNCTION__;
			constexpr std::string_view prefix = "with T = ";
			constexpr std::size_t prefix_start = signature.find(prefix);
			constexpr std::size_t type_begin = prefix_start == g_TN_NPOS ? 0 : prefix_start + prefix.size();
			constexpr std::size_t type_end = find_tend(signature, type_begin, ';', ']');
			return ext_type(signature, prefix_start, prefix.size(), type_end);
#else
			return "unknown";
#endif
		}

		template <typename T>
		inline constexpr bool is_char_ptr_v =
			std::is_pointer_v<remove_cvref_t<T>> &&
			std::is_same_v<std::remove_cv_t<std::remove_pointer_t<remove_cvref_t<T>>>, char>;

		template <typename T>
		inline constexpr bool is_char_arr_v =
			std::is_array_v<remove_cvref_t<T>> &&
			std::is_same_v<std::remove_cv_t<std::remove_extent_t<remove_cvref_t<T>>>, char>;

		template <typename T>
		inline constexpr bool is_str_v =
			std::is_same_v<remove_cvref_t<T>, std::string> ||
			std::is_same_v<remove_cvref_t<T>, std::string_view> ||
			is_char_ptr_v<T> ||
			is_char_arr_v<T>;

		template <typename T>
		static std::string_view to_sv(const T& avalue)
		{
			if constexpr (std::is_same_v<remove_cvref_t<T>, std::string> || std::is_same_v<remove_cvref_t<T>, std::string_view>)
			{
				return std::string_view(avalue);
			}
			else if constexpr (is_char_ptr_v<T>)
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

		inline bool is_url_safe(unsigned char achar)
		{
			return (achar >= '0' && achar <= '9')
				|| (achar >= 'A' && achar <= 'Z')
				|| (achar >= 'a' && achar <= 'z')
				|| achar == '-'
				|| achar == '_'
				|| achar == '.';
		}

		inline int hex_value(char achar)
		{
			if (achar >= '0' && achar <= '9')
			{
				return achar - '0';
			}
			if (achar >= 'A' && achar <= 'F')
			{
				return achar - 'A' + 10;
			}
			if (achar >= 'a' && achar <= 'f')
			{
				return achar - 'a' + 10;
			}
			return -1;
		}

		inline bool keep_pct(unsigned char ahex)
		{
			return (ahex >= '0' && ahex <= '9')
				|| (ahex >= 'a' && ahex <= 'z')
				|| (ahex >= 'A' && ahex <= 'Z')
				|| ahex == 0x21 || ahex == 0x24 || ahex == 0x26 || ahex == 0x27 || ahex == 0x28 || ahex == 0x29
				|| ahex == 0x2a || ahex == 0x2b || ahex == 0x2c || ahex == 0x2d || ahex == 0x2e || ahex == 0x2f
				|| ahex == 0x3A || ahex == 0x3B || ahex == 0x3D || ahex == 0x3F || ahex == 0x40 || ahex == 0x5F;
		}

		inline bool parse_ip4(std::string_view aip, std::array<int, 4>& aoctets)
		{
			std::size_t begin = 0;
			for (std::size_t index = 0; index < aoctets.size(); ++index)
			{
				const bool is_last = index + 1 == aoctets.size();
				const std::size_t dot = aip.find('.', begin);
				if ((dot == std::string_view::npos) != is_last)
				{
					return false;
				}

				const std::size_t end = is_last ? aip.size() : dot;
				if (end <= begin || end - begin > 3)
				{
					return false;
				}

				int value = 0;
				const auto [ptr, ec] = std::from_chars(aip.data() + begin, aip.data() + end, value);
				if (ec != std::errc() || ptr != aip.data() + end || value < 0 || value > 255)
				{
					return false;
				}

				aoctets[index] = value;
				begin = end + 1;
			}
			return begin == aip.size() + 1;
		}
	}

	namespace tools
	{
		std::size_t strlen(const char*);

#pragma region map
		// Map
		template <typename TMAP, typename TKEY>
		auto findmap(TMAP& amap, const TKEY& akey)
		{
			auto itor = amap.find(akey);
			return itor == amap.end() ? nullptr : &itor->second;
		}

		template <typename TMAP, typename TKEY>
		auto insertmap(TMAP& amap, const TKEY& akey, const typename TMAP::value_type::second& avalue)
		{
			auto lpair = amap.try_emplace(akey, avalue);
			return lpair.second ? &(lpair.first->second) : nullptr;
		}

		template <typename TMAP, typename TKEY>
		bool erasemap(TMAP& amap, const TKEY& akey, typename TMAP::value_type::second_type& adata)
		{
			if (auto node = amap.extract(akey); node)
			{
				adata = std::move(node.mapped());
				return true;
			}
			return false;
		}
#pragma endregion 

		// Checks whether an IPv4 address belongs to a private/LAN range.
		bool is_lanip(const std::string& aip);

		template <typename T>
		bool proto2json(const T& adata, std::string& json)
		{
			google::protobuf::util::JsonPrintOptions options;
			options.add_whitespace = false;
			options.always_print_primitive_fields = false;
			options.preserve_proto_field_names = false;
			return google::protobuf::util::MessageToJsonString(adata, &json, options).ok();		
		}

		// Convenience logger for protobuf messages during debugging.
		template <typename T>
		void print_json(const T& adata, bool aislog = false);

		// Parses JSON into protobuf messages or custom types that implement the
		// project `json_pop/json_push` contract.
		template <typename T>
		bool json2proto(const std::string& json, T& adata)
		{
			google::protobuf::util::Status status = google::protobuf::util::JsonStringToMessage(json, &adata);
			return status.ok();
		}

		template <typename T>
		bool json2custom(const std::string& json, T& adata);

		template <typename T>
		bool custom2json(const T& adata, std::string& json);

		template <typename TKEY, typename TVAL>
		void copy(const std::map<TKEY, TVAL>& asource, google::protobuf::Map<TKEY, TVAL>& atarget)
		{
			for(auto& [_key, _value] : asource)
			{
				atarget.insert({ _key, _value });
			}
		}

		template <typename TKEY, typename TVAL>
		void copy(const google::protobuf::Map<TKEY, TVAL>& asource, std::map<TKEY, TVAL>& atarget)
		{
			for (auto& [_key, _value] : asource)
			{
				atarget.insert({ _key, _value });
			}
		}

		void log_lex_err(
			const char* atotype,
			const char* afromtype,
			const char* aerror,
			const std::source_location& asource
		);

		template <typename To, typename From>
		To lexical_cast(const From& from, const std::source_location& asource = std::source_location::current())
		{
			if constexpr (std::is_integral_v<To> && std::is_unsigned_v<To>)
			{
				if constexpr (std::is_integral_v<detail::remove_cvref_t<From>> && std::is_signed_v<detail::remove_cvref_t<From>>)
				{
					if (from < 0)
					{
						const boost::bad_lexical_cast lerror(typeid(From), typeid(To));
						log_lex_err(typeid(To).name(), typeid(From).name(), lerror.what(), asource);
						throw lerror;
					}
				}
				else if constexpr (detail::is_str_v<From>)
				{
					if (detail::first_non_space(detail::to_sv(from)) == '-')
					{
						const boost::bad_lexical_cast lerror(typeid(From), typeid(To));
						log_lex_err(typeid(To).name(), typeid(From).name(), lerror.what(), asource);
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
				log_lex_err(typeid(To).name(), typeid(From).name(), aerror.what(), asource);
				throw;
			}
		}

		template <typename To, typename From>
		bool try_lexical_cast(const From& from, To& to, const std::source_location& asource = std::source_location::current())
		{
			try
			{
				to = lexical_cast<To>(from, asource);
				return true;
			}
			catch (const boost::bad_lexical_cast&)
			{
				return false;
			}
			catch (const std::string&)
			{
				return false;
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
		// Whether the host platform uses little-endian byte order.
		constexpr bool islittle()
		{
			return std::endian::native == std::endian::little;
		}

		// Converts values read from little-endian wire/storage format.
		int16_t	transformlittle(parm<int16_t>& avalues);
		uint16_t transformlittle(parm<uint16_t>& avalues);
		int32_t	transformlittle(parm<int32_t>& avalues);
		uint32_t transformlittle(parm<uint32_t>& avalues);
		int64_t	transformlittle(parm<int64_t>& avalues);
		uint64_t	transformlittle(parm<uint64_t>& avalues);
#pragma endregion

#pragma region base64
		std::string base64_encode(const char* data, std::size_t len);
		std::string base64_encode(std::string const& s);
		std::string base64_decode(char const* data, std::size_t len);
		std::string base64_decode(std::string const& data);
#pragma endregion

		bool uuid_make(std::string& astr);

#pragma region asscii_utf8
		// Locale-based ANSI/wide conversions and explicit UTF-8 helpers.
		bool wasscii2asscii(const std::wstring& awstr, std::string& astr);
		bool asscii2wasscii(const std::string& astr, std::wstring& awstr);
		bool wasscii2utf8(const std::wstring& awstr, std::string& astr);
		bool utf82wasscii(const std::string& astr, std::wstring& awstr);

		bool to_asscii(const std::string& astr1, std::string& astr2);
		bool to_utf8(const std::string& astr1, std::string& astr2);

		int32_t utf8firstbyte(uint8_t firstbyte);
		// Validates UTF-8 and detects code points that require utf8mb4 storage.
		bool isutf8(const std::string& astr1);
		bool isincludeutf8mb4(const std::string& astr);

#pragma endregion

#pragma region regular
		void sregex(const std::string& apattern, const std::string& adata, const std::function<void(std::string&)>& afun);

		void smatch(const std::string& aexpression, const std::string& adata, const std::function<void(std::smatch&)>& afun);

		void replace(const std::string& aexpression, const std::string& areleace, const std::string& adata, std::string& aret);

		void replace_ret(const std::string& aexpression, const std::string& areleace, std::string& adata, std::string& aret);
#pragma endregion

#pragma region splite
		template <typename T>
		bool splite(const char* abuff, const char* afg, std::vector<T>& avec)
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
				for (const std::string& litem : lvec)
				{
					lparsed.emplace_back(tools::lexical_cast<T>(litem.c_str()));
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
		bool splite(const char* abuff, const char* afg, std::set<T>& aset)
		{
			std::vector<std::string> lvec;
			if (splite(abuff, afg, lvec) == false)
			{
				return false;
			}
			std::set<T> lparsed;
			try
			{
				for (const std::string& litem : lvec)
				{
					lparsed.emplace(tools::lexical_cast<T>(litem.c_str()));
				}
			}
			catch (const boost::bad_lexical_cast&)
			{
				throw std::string("lexical_cast error");
			}
			aset = std::move(lparsed);
			return true;
		}

		bool splite(const char* abuff, const char* afg, std::vector<std::string>& avec);

		template <typename T>
		bool splite(int32_t aindex, const std::vector<std::string>& avec, T& adata)
		{
			if (aindex < 0)
			{
				return false;
			}
			const std::size_t lidx = static_cast<std::size_t>(aindex);
			if (lidx >= avec.size())
			{
				return false;
			}
			try
			{
				adata = tools::lexical_cast<T>(avec[lidx].c_str());
			}
			catch (...)
			{
				return false;
			}
			return true;
		}

		template <typename T>
		bool splite(int32_t aindex, const std::vector<std::string>& avec, std::vector<T>& adata)
		{
			if (aindex < 0)
			{
				return false;
			}
			for (int32_t i = aindex;i< avec.size();++i)
			{
				adata.push_back(tools::lexical_cast<T>(avec[i].c_str()));
			}
			return true;
		}

		template <typename T>
		bool splite(int32_t aindex, const std::vector<std::string>& avec, std::vector<std::string>& adata)
		{
			if (aindex < 0)
			{
				return false;
			}
			adata.insert(adata.end(), avec.begin() + aindex, avec.end());
			return true;
		}

		template <std::size_t... INDEX, typename TTUPLE>
		bool splite_tuple(std::index_sequence<INDEX...>, const std::vector<std::string>& avec, TTUPLE& atuple)
		{
			return (splite<std::tuple_element_t<INDEX, TTUPLE>>(static_cast<int32_t>(INDEX), avec, std::get<INDEX>(atuple)) && ...);
		}

		template <std::size_t... INDEX, typename ...ARGS>
		bool splite(std::index_sequence<INDEX...>, const char* abuff, const char* afg, ARGS&... args)
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
		bool splite(const char* abuff, const char* afg, ARGS&... args)
		{
			return 	splite(std::make_index_sequence<sizeof...(ARGS)>{}, abuff, afg, args...);
		}

		// Parses compact key/value collections such as
		// "[mail@example.com:name][other@example.com:other]".
		template <typename TFIRST = std::string, typename TSECOND = std::string>
		bool splite_special(const char* astr, const char* akey1, const char* akey2, std::vector<std::pair<TFIRST, TSECOND>>& avec)
		{
			if (astr == nullptr || akey1 == nullptr || akey2 == nullptr)
			{
				return false;
			}
			std::string ltemp = astr;
			ngl::tools::replace_ret(akey1, "", ltemp, ltemp);
			std::vector<std::string> lvec;
			if (ngl::tools::splite(ltemp.c_str(), akey2, lvec) == false)
			{
				return false;
			}
			std::vector<std::pair<TFIRST, TSECOND>> lmailvec;
			lmailvec.reserve(lvec.size());
			for (const std::string& litem : lvec)
			{
				std::pair<TFIRST, TSECOND> lpair;
				if (ngl::tools::splite(litem.c_str(), ":", lpair.first, lpair.second) == false)
				{
					return false;
				}
				lmailvec.emplace_back(std::move(lpair));
			}
			avec = std::move(lmailvec);
			return true;
		}

		template <typename TFIRST = std::string, typename TSECOND = std::string>
		bool splite_special(const char* astr, const char* akey1, const char* akey2, std::map<TFIRST, TSECOND>& amap)
		{
			if (astr == nullptr || akey1 == nullptr || akey2 == nullptr)
			{
				return false;
			}
			std::string ltemp = astr;
			ngl::tools::replace_ret(akey1, "", ltemp, ltemp);
			std::vector<std::string> lvec;
			if (ngl::tools::splite(ltemp.c_str(), akey2, lvec) == false)
			{
				return false;
			}
			std::map<TFIRST, TSECOND> lparsed;
			for (const std::string& litem : lvec)
			{
				std::pair<TFIRST, TSECOND> lpair;
				if (ngl::tools::splite(litem.c_str(), ":", lpair.first, lpair.second) == false)
				{
					return false;
				}
				lparsed.emplace(std::move(lpair));
			}
			amap = std::move(lparsed);
			return true;
		}
#pragma endregion

#pragma region splicing

		template <typename T>
		inline std::function<std::string(const T&)> m_splicing = [](const T& adata)
		{
			return tools::lexical_cast<std::string>(adata);
		};
		template <typename TKEY,typename TVALUE>
		inline std::function<std::string(const TKEY&, const TVALUE&)> m_splicingmap = [](const TKEY& akey, const TVALUE& avalue)
		{
			std::string lret = tools::lexical_cast<std::string>(akey);
			lret += ':';
			lret += tools::lexical_cast<std::string>(avalue);
			return lret;
		};

		template <typename T>
		bool splicing(const std::vector<T>& avec, const char* afg, std::string& astr, const std::function<std::string(const T&)> afunction = m_splicing<T>)
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

		bool splicing(const std::vector<std::string>& avec, const char* afg, std::string& astr, int32_t apos = 0)
		{
			const std::size_t lfglen = tools::strlen(afg);
			for (std::size_t i = apos; i < avec.size(); ++i)
			{
				if (i != apos)
				{
					astr.append(afg, lfglen);
				}
				astr += avec[i];
			}
			return true;
		}

		template <typename T>
		bool splicing(const std::set<T>& avec, const char* afg, std::string& astr, const std::function<std::string(const T&)> afunction = m_splicing<T>)
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
		bool splicing(const std::map<TKEY, TVALUE>& amap, const char* afg, std::string& astr, const std::function<std::string(const TKEY&, const TVALUE&)> afunction = m_splicingmap<TKEY,TVALUE>)
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
		bool splicing(const google::protobuf::Map<TKEY, TVALUE>& amap, const char* afg, std::string& astr,const std::function<std::string(const TKEY&,const TVALUE&)> afunction = m_splicingmap<TKEY, TVALUE>)
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
		bool splicing(const std::set<std::string>& aset, const char* afg, std::string& astr)
		{
			const std::size_t lfglen = tools::strlen(afg);
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
					astr.append(afg, lfglen);
				}
				astr += *itor;
			}
			return true;
		}

		template <typename T>
		bool splicing(int32_t aindex, const char* afg, std::string& astr, T& adata)
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
		bool splicing(std::index_sequence<INDEX...>, const char* afg, std::string& astr, ARGS&... args)
		{
			return ((splicing<ARGS>(INDEX, afg, astr, args)) && ...);
		}

		template <typename ...ARGS>
		bool splicing(const char* afg, std::string& astr, ARGS&... args)
		{
			return splicing(std::make_index_sequence<sizeof...(ARGS)>{}, afg, astr, args...);
		}
#pragma endregion

#pragma region url
		std::string char2hex(char dec);
		std::string url_encode(const std::string& c);
		std::string url_decode(const std::string& c);
#pragma endregion

#pragma region varint
		// Varint helpers used by the custom serializer. `m_bytes` receives the
		// number of encoded/decoded bytes when the caller needs it.
	
		int varint_length(parm<int32_t>& avalue);
		int varint_length(parm<int64_t>& avalue);

		template <typename T>
		struct varint_parm
		{
			T m_value;
			char* m_buf = nullptr;
			int m_len = 0;
			int32_t* m_bytes = 0;
		};
		bool varint_encode(varint_parm<int64_t>& aparm);
		bool	varint_decode(varint_parm<int64_t>& aparm);
		bool	varint_encode(varint_parm<int32_t>& aparm);
		bool	varint_decode(varint_parm<int32_t>& aparm);

	#pragma endregion

#pragma region hexbytes
		int	to_hex(void* apso, int alen, void* apto);
		bool to_bytes(void* apso, int alen, void* apto, int& apotlen);
#pragma endregion

#pragma region encryption
		void bytexor(char* ap, int32_t aplen, int apos);
#pragma endregion

		std::string& type_name_handle(std::string& aname);

		template <typename T>
		std::string& type_name()
		{
			// Derive the name from the compiler signature so incomplete forward-declared
			// types still work when unity builds change template instantiation order.
			static std::string lname = std::string(detail::comp_type<T>());
			static std::atomic lfirst = true;
			if (lfirst.exchange(false))
			{
				type_name_handle(lname);
			}
			return lname;
		}

		template <typename T>
		std::string& type_name(const T*)
		{
			return type_name<T>();
		}

		std::string md5(const std::string& text);

		std::string sh1(std::string_view text);

		std::string hmac_sha1(const std::string& key, const std::string& text);

		std::string time2str(int autc, const char* aformat = "%y/%m/%d %H:%M:%S");

		std::vector<std::string_view> get_line(const char* apbuff, size_t abuffsize);

		bool isnumber(const std::string& anumber);

		const std::string& server_name();

		int rand();

		// ASCII case conversion for configuration keys and protocol names.
		void transform_tolower(std::string& adata);
		void transform_toupper(std::string& adata);

		// Filesystem helpers return false on errors instead of throwing.
		bool directories_exists(const std::string& apath);
		bool file_exists(const std::string& apath);
		bool create_dir(const std::string& apath);
		bool file_remove(const std::string& afilename);
		// Enumerates regular files in one directory or recursively.
		void dir(const std::string& apath, std::vector<std::string>& afilevec, bool aiteration = false);

		// Intentionally crashes the process when the project treats the current
		// state as unrecoverable.
		void no_core_dump(bool anocreate = false);

		std::function<void()> send_mail(const std::string& acontent);

		// Converts `actor_type#area#dataid` text into the packed nguid format.
		int64_t nguidstr2int64(const char* anguidstr);

		// Splits a mutable comma-separated buffer in place and returns pointers
		// into the original storage.
		std::vector<const char*> split_str(char* apbuff, int32_t abuffcount);

		template <std::size_t N>
		void split_str(char* apbuff, int32_t abuffcount, std::array<const char*, N>& aarrays)
		{
			aarrays.fill(nullptr);
			if (apbuff == nullptr || abuffcount <= 0)
			{
				return;
			}

			std::size_t lidx = 0;
			int32_t lbeg = 0;
			for (int32_t li = 0; li < abuffcount; ++li)
			{
				if (apbuff[li] != ',')
				{
					continue;
				}

				apbuff[li] = '\0';
				if (lidx < N)
				{
					aarrays[lidx++] = &apbuff[lbeg];
				}
				lbeg = li + 1;
				if (lbeg < abuffcount && apbuff[lbeg] == ' ')
				{
					++lbeg;
				}
			}

			if (lidx < N)
			{
				aarrays[lidx] = &apbuff[lbeg];
			}
		}
		// Cjsontype
		bool bit(int32_t atype, int32_t acjsontype)
		{
			return (atype & acjsontype) != 0;
		}

		// Collapses repeated copies of the same separator character.
		void erase_repeat(std::string& astrbuff, const char ach);

		// Comparewhether
		template <typename ETYPE>
		bool equal(ETYPE avalue1, ETYPE avalue2)
		{
			if (avalue1 == avalue2)
			{
				return true;
			}
			return false;
		}


		int32_t less_member()
		{
			return 0;
		}

		template <typename T>
		auto less_member(const T& lhs, const T& rhs)
		{
			return lhs <=> rhs;
		}

		template <typename T, typename... TARGS>
		auto less_member(const T& lhs, const T& rhs, const TARGS&... members)
		{
			if (lhs != rhs)
			{
				return less_member(lhs, rhs);
			}
			return less_member(members...);
		}

		std::string_view trim_ascii_spaces(std::string_view avalue);

	} // namespace tools


	template <typename T, typename = void>
	struct has_pb_desc;

	template <typename T>
	struct has_pb_desc<T, std::void_t<decltype(T::descriptor())>> : std::is_same<
		decltype(T::descriptor()),
		const google::protobuf::Descriptor*
	> {};

	template <typename T, typename>
	struct has_pb_desc : std::false_type {};

	template <typename T>
	struct is_protobuf_message : std::conjunction<
		std::is_base_of<google::protobuf::MessageLite, std::remove_cv_t<std::remove_reference_t<T>>>,
		has_pb_desc<std::remove_cv_t<std::remove_reference_t<T>>>
	> {};
}//namespace ngl
