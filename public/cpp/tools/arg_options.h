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
#pragma once

#include "tools/log/nlog.h"

#include <boost/program_options.hpp>

#include <cstdint>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace ngl
{
	namespace po = boost::program_options;

	class arg_options
	{
		template <typename T>
		using option_value_t = std::conditional_t<std::is_convertible_v<T, std::string_view>, std::string, std::decay_t<T>>;

		static std::string trim_copy(std::string_view avalue)
		{
			const std::size_t lbegin = avalue.find_first_not_of(" \t\r\n");
			if (lbegin == std::string_view::npos)
			{
				return {};
			}

			const std::size_t lend = avalue.find_last_not_of(" \t\r\n");
			return std::string(avalue.substr(lbegin, lend - lbegin + 1));
		}

		static std::vector<std::string> split_option_names(std::string_view aspec)
		{
			std::vector<std::string> lnames;
			std::size_t lbegin = 0;
			while (lbegin <= aspec.size())
			{
				const std::size_t lend = aspec.find(',', lbegin);
				const std::string lname = trim_copy(aspec.substr(
					lbegin,
					lend == std::string_view::npos ? std::string_view::npos : lend - lbegin
				));
				if (!lname.empty())
				{
					lnames.push_back(lname);
				}

				if (lend == std::string_view::npos)
				{
					break;
				}
				lbegin = lend + 1;
			}
			return lnames;
		}

		template <typename T>
		static option_value_t<T> normalize_default_value(const T& avalue)
		{
			if constexpr (std::is_convertible_v<T, std::string_view>)
			{
				return std::string(std::string_view(avalue));
			}
			else
			{
				return avalue;
			}
		}

		static std::vector<std::string> split_args(const std::string& aargs)
		{
			if (aargs.empty())
			{
				return {};
			}
#ifdef _WIN32
			return po::split_winmain(aargs);
#else
			return po::split_unix(aargs);
#endif
		}

		void register_aliases(std::string_view aspec)
		{
			const std::vector<std::string> lnames = split_option_names(aspec);
			if (lnames.empty())
			{
				return;
			}

			const std::string& lcanonical = lnames.front();
			for (const auto& lname : lnames)
			{
				m_alias_to_key[lname] = lcanonical;
			}
		}

		std::string resolve_key(std::string_view akey) const
		{
			const std::string lkey = trim_copy(akey);
			const auto itor = m_alias_to_key.find(lkey);
			if (itor != m_alias_to_key.end())
			{
				return itor->second;
			}
			return lkey;
		}

		template <typename TParser>
		bool parse_with(TParser&& aparser)
		{
			try
			{
				po::variables_map lvm;
				po::store(std::forward<TParser>(aparser)(), lvm);
				po::notify(lvm);
				m_vm = std::move(lvm);
				return true;
			}
			catch (const std::exception& ex)
			{
				m_vm.clear();
				log_warn()->print("arg_options [{}] parse fail [{}]", m_cmdname, ex.what());
				return false;
			}
		}

		template <typename T, typename TConfig>
		bool bind_typed_option(std::string_view akey, TConfig&& aconfig)
		{
			const std::string lkey = trim_copy(akey);
			auto itor = m_info.find(lkey);
			if (itor == m_info.end())
			{
				return false;
			}

			if (!m_registered_specs.insert(lkey).second)
			{
				return false;
			}

			register_aliases(lkey);
			using value_type = option_value_t<T>;
			auto* lvalue = po::value<value_type>();
			std::forward<TConfig>(aconfig)(lvalue);
			m_options.add_options()(lkey.c_str(), lvalue, itor->second.c_str());
			return true;
		}

		po::options_description							m_options;
		po::positional_options_description				m_positional;
		std::unordered_map<std::string, std::string>	m_info;
		std::unordered_map<std::string, std::string>	m_alias_to_key;
		std::unordered_set<std::string>					m_registered_specs;
		std::string										m_cmdname;
		std::string										m_help_text;
		po::variables_map								m_vm;
	public:
		explicit arg_options(std::string_view acmdname) :
			m_options(acmdname.empty() ? "options" : std::string(acmdname)),
			m_cmdname(acmdname)
		{}

		static std::vector<std::string> split_command_line(std::string_view aargs)
		{
			return split_args(std::string(aargs));
		}

		template <typename T>
		bool bind_options(std::string_view akey, const T& avalue)
		{
			using value_type = option_value_t<T>;
			const value_type ldefault = normalize_default_value(avalue);
			return bind_typed_option<T>(akey, [&ldefault](auto* avalue)
				{
					avalue->default_value(ldefault);
				});
		}

		template <typename T>
		bool init_options(std::string_view akey, std::string_view aremark, const T& adata)
		{
			const std::string lkey = trim_copy(akey);
			m_info[lkey] = std::string(aremark);
			return bind_options(lkey, adata);
		}

		template <typename T>
		bool init_optional(std::string_view akey, std::string_view aremark)
		{
			const std::string lkey = trim_copy(akey);
			m_info[lkey] = std::string(aremark);
			return bind_typed_option<T>(lkey, [](auto*)
				{
				});
		}

		template <typename T>
		bool init_required(std::string_view akey, std::string_view aremark)
		{
			const std::string lkey = trim_copy(akey);
			m_info[lkey] = std::string(aremark);
			return bind_typed_option<T>(lkey, [](auto* avalue)
				{
					avalue->required();
				});
		}

		template <typename T>
		bool init_multitoken(std::string_view akey, std::string_view aremark, bool arequired = false)
		{
			const std::string lkey = trim_copy(akey);
			m_info[lkey] = std::string(aremark);
			return bind_typed_option<T>(lkey, [arequired](auto* avalue)
				{
					avalue->multitoken();
					if (arequired)
					{
						avalue->required();
					}
				});
		}

		bool init_flag(std::string_view akey, std::string_view aremark)
		{
			const std::string lkey = trim_copy(akey);
			if (!m_registered_specs.insert(lkey).second)
			{
				return false;
			}

			m_info[lkey] = std::string(aremark);
			register_aliases(lkey);
			m_options.add_options()(lkey.c_str(), m_info[lkey].c_str());
			return true;
		}

		bool init_help(std::string_view adata)
		{
			m_help_text = std::string(adata);
			return init_flag("help,h", "show help");
		}

		void positional(std::string_view akey, int32_t acount)
		{
			const std::string lkey = resolve_key(akey);
			m_positional.add(lkey.c_str(), acount);
		}

		bool parse(const char* aargs)
		{
			return parse(std::string_view(aargs == nullptr ? "" : aargs));
		}

		bool parse(std::string_view aargs)
		{
			const std::string largs(aargs);
			return parse(split_args(largs));
		}

		bool parse(const std::vector<std::string>& aargs)
		{
			return parse_with([&]()
				{
					return po::command_line_parser(aargs)
						.options(m_options)
						.positional(m_positional)
						.run();
				});
		}

		bool parse(int argc, const char* const argv[])
		{
			return parse_with([&]()
				{
					return po::command_line_parser(argc, argv)
						.options(m_options)
						.positional(m_positional)
						.run();
				});
		}

		template <typename T>
		bool value(std::string_view akey, T& adata) const
		{
			const std::string lkey = resolve_key(akey);
			if constexpr (std::is_same_v<std::decay_t<T>, std::string>)
			{
				if ((lkey == "help") && has(lkey) && !m_help_text.empty())
				{
					adata = m_help_text;
					return true;
				}
			}

			auto itor = m_vm.find(lkey);
			if (itor == m_vm.end())
			{
				return false;
			}

			adata = itor->second.as<T>();
			return true;
		}

		bool has(std::string_view akey) const
		{
			return m_vm.count(resolve_key(akey)) > 0;
		}

		bool help_requested() const
		{
			return has("help");
		}

		std::string help() const
		{
			if (!m_help_text.empty())
			{
				return m_help_text;
			}

			std::ostringstream lstream;
			lstream << m_options;
			return lstream.str();
		}

		void clear()
		{
			m_vm.clear();
		}
	};


}//namespace ngl
