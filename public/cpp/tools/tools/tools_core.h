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
// File overview: Declares shared helpers and templates for tools.

#pragma once

#include <source_location>
#include <unordered_map>
#include <type_traits>
#include <string_view>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include <typeinfo>
#include <charconv>
#include <utility>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <compare>
#include <vector>
#include <string>
#include <format>
#include <atomic>
#include <cctype>
#include <tuple>
#include <regex>
#include <array>
#include <bit>
#include <map>
#include <set>

#include <google/protobuf/util/json_util.h>
#include <google/protobuf/message_lite.h>
#include <google/protobuf/descriptor.h>
#include <boost/lexical_cast.hpp>
#include <google/protobuf/map.h>

namespace ngl
{
	template <typename T>
	struct np_actormodule_forward;

	template <typename TDATA>
	using mforward = np_actormodule_forward<TDATA>;

	namespace detail
	{
		inline constexpr std::string_view g_hex_tab = "0123456789ABCDEF";
		inline constexpr std::size_t g_tn_npos = std::string_view::npos;

		template <typename T>
		using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

		constexpr std::string_view ext_type(
			std::string_view asign,
			std::size_t aprefix,
			std::size_t asize,
			std::size_t aend = g_tn_npos) noexcept
		{
			const std::size_t lbeg = aprefix == g_tn_npos ? 0 : aprefix + asize;
			const bool lhas_pre = aprefix != g_tn_npos;
			const bool lhas_end = aend != g_tn_npos && aend > lbeg;
			return !lhas_pre ? asign : (lhas_end ? asign.substr(lbeg, aend - lbeg) : asign.substr(lbeg));
		}

		constexpr std::size_t find_tend(
			std::string_view asign,
			std::size_t abeg,
			std::string_view asuf) noexcept
		{
			return asign.find(asuf, abeg);
		}

		constexpr std::size_t find_tend(
			std::string_view asign,
			std::size_t abeg,
			char asuf) noexcept
		{
			return asign.find(asuf, abeg);
		}

		constexpr std::size_t find_tend(
			std::string_view asign,
			std::size_t abeg,
			char asuf1,
			char asuf2) noexcept
		{
			const std::size_t lend1 = asign.find(asuf1, abeg);
			const std::size_t lend2 = asign.find(asuf2, abeg);
			return lend1 == g_tn_npos ? lend2
				: (lend2 == g_tn_npos ? lend1 : std::min(lend1, lend2));
		}

		template <typename T>
		constexpr std::string_view comp_type() noexcept
		{
#if defined(_MSC_VER)
			constexpr std::string_view lsign = __FUNCSIG__;
			constexpr std::string_view lpre = "comp_type<";
			constexpr std::size_t lpos = lsign.find(lpre);
			constexpr std::size_t lbeg = lpos == g_tn_npos ? 0 : lpos + lpre.size();
			constexpr std::size_t lend = find_tend(lsign, lbeg, ">(void");
			return ext_type(lsign, lpos, lpre.size(), lend);
#elif defined(__clang__)
			constexpr std::string_view lsign = __PRETTY_FUNCTION__;
			constexpr std::string_view lpre = "T = ";
			constexpr std::size_t lpos = lsign.find(lpre);
			constexpr std::size_t lbeg = lpos == g_tn_npos ? 0 : lpos + lpre.size();
			constexpr std::size_t lend = find_tend(lsign, lbeg, ']');
			return ext_type(lsign, lpos, lpre.size(), lend);
#elif defined(__GNUC__)
			constexpr std::string_view lsign = __PRETTY_FUNCTION__;
			constexpr std::string_view lpre = "with T = ";
			constexpr std::size_t lpos = lsign.find(lpre);
			constexpr std::size_t lbeg = lpos == g_tn_npos ? 0 : lpos + lpre.size();
			constexpr std::size_t lend = find_tend(lsign, lbeg, ';', ']');
			return ext_type(lsign, lpos, lpre.size(), lend);
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
		std::string_view to_sv(const T& avalue)
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

		inline bool parse_ip4(std::string_view aip, std::array<int, 4>& aoct)
		{
			std::size_t lbeg = 0;
			for (std::size_t lidx = 0; lidx < aoct.size(); ++lidx)
			{
				const bool llast = lidx + 1 == aoct.size();
				const std::size_t ldot = aip.find('.', lbeg);
				if ((ldot == std::string_view::npos) != llast)
				{
					return false;
				}

				const std::size_t lend = llast ? aip.size() : ldot;
				if (lend <= lbeg || lend - lbeg > 3)
				{
					return false;
				}

				int lval = 0;
				const auto [lptr, lec] = std::from_chars(aip.data() + lbeg, aip.data() + lend, lval);
				if (lec != std::errc() || lptr != aip.data() + lend || lval < 0 || lval > 255)
				{
					return false;
				}

				aoct[lidx] = lval;
				lbeg = lend + 1;
			}
			return lbeg == aip.size() + 1;
		}
	}

	namespace tools
	{
#pragma region map
		template <typename TMAP, typename TKEY>
		auto findmap(TMAP& amap, const TKEY& akey)
		{
			auto liter = amap.find(akey);
			return liter == amap.end() ? nullptr : &liter->second;
		}

		template <typename TMAP, typename TKEY>
		auto insertmap(TMAP& amap, const TKEY& akey, const typename TMAP::mapped_type& avalue)
		{
			auto lpair = amap.try_emplace(akey, avalue);
			return lpair.second ? &lpair.first->second : nullptr;
		}

		template <typename TMAP, typename TKEY>
		bool erasemap(TMAP& amap, const TKEY& akey, typename TMAP::mapped_type& adata)
		{
			if (auto lnode = amap.extract(akey); lnode)
			{
				adata = std::move(lnode.mapped());
				return true;
			}
			return false;
		}
#pragma endregion

		template <typename T>
		bool proto2json(const T& adata, std::string& ajson)
		{
			google::protobuf::util::JsonPrintOptions lopt;
			lopt.add_whitespace = false;
			lopt.always_print_primitive_fields = false;
			lopt.preserve_proto_field_names = false;
			return google::protobuf::util::MessageToJsonString(adata, &ajson, lopt).ok();
		}

		template <typename T>
		void print_json(const T& adata, bool aislog = false);

		template <typename T>
		bool json2proto(const std::string& ajson, T& adata)
		{
			google::protobuf::util::Status lstat = google::protobuf::util::JsonStringToMessage(ajson, &adata);
			return lstat.ok();
		}

		template <typename T>
		bool json2custom(const std::string& ajson, T& adata);

		template <typename T>
		bool custom2json(const T& adata, std::string& ajson);

		template <typename TKEY, typename TVAL>
		void copy(const std::map<TKEY, TVAL>& asrc, google::protobuf::Map<TKEY, TVAL>& adst)
		{
			for (const auto& [lkey, lval] : asrc)
			{
				adst.insert({ lkey, lval });
			}
		}

		template <typename TKEY, typename TVAL>
		void copy(const google::protobuf::Map<TKEY, TVAL>& asrc, std::map<TKEY, TVAL>& adst)
		{
			for (const auto& [lkey, lval] : asrc)
			{
				adst.insert({ lkey, lval });
			}
		}

		void log_lex_err(
			const char* atotype,
			const char* afromtype,
			const char* aerror,
			const std::source_location& asource
		);

		template <typename To, typename From>
		To lexical_cast(const From& afrom, const std::source_location& asource = std::source_location::current())
		{
			if constexpr (std::is_integral_v<To> && std::is_unsigned_v<To>)
			{
				if constexpr (std::is_integral_v<detail::remove_cvref_t<From>> && std::is_signed_v<detail::remove_cvref_t<From>>)
				{
					if (afrom < 0)
					{
						const boost::bad_lexical_cast lerr(typeid(From), typeid(To));
						log_lex_err(typeid(To).name(), typeid(From).name(), lerr.what(), asource);
						throw lerr;
					}
				}
				else if constexpr (detail::is_str_v<From>)
				{
					if (detail::first_non_space(detail::to_sv(afrom)) == '-')
					{
						const boost::bad_lexical_cast lerr(typeid(From), typeid(To));
						log_lex_err(typeid(To).name(), typeid(From).name(), lerr.what(), asource);
						throw lerr;
					}
				}
			}

			try
			{
				return boost::lexical_cast<To>(afrom);
			}
			catch (const boost::bad_lexical_cast& aerror)
			{
				log_lex_err(typeid(To).name(), typeid(From).name(), aerror.what(), asource);
				throw;
			}
		}

		template <typename To, typename From>
		bool try_lexical_cast(const From& afrom, To& ato, const std::source_location& asource = std::source_location::current())
		{
			try
			{
				ato = lexical_cast<To>(afrom, asource);
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

			explicit parm(T avalue) :
				m_value(avalue)
			{}

			parm() = delete;
			parm(const parm&) = delete;
			parm& operator=(const parm&) = delete;
		};
	}
}//namespace ngl
