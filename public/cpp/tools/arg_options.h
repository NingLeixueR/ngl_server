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

#include <boost/program_options.hpp>

#include <algorithm>
#include <cctype>
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

		static std::string normalize_key(std::string_view akey)
		{
			const std::vector<std::string> option_names = split_option_names(akey);
			if (option_names.empty())
			{
				return {};
			}
			if (option_names.size() == 1)
			{
				return option_names.front();
			}

			std::size_t total_size = option_names.size() - 1;
			for (const std::string& option_name : option_names)
			{
				total_size += option_name.size();
			}

			std::string normalized_key;
			normalized_key.reserve(total_size);
			for (std::size_t index = 0; index < option_names.size(); ++index)
			{
				if (index != 0)
				{
					normalized_key.push_back(',');
				}
				normalized_key += option_names[index];
			}
			return normalized_key;
		}

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
			std::vector<std::string> option_names;
			option_names.reserve(static_cast<std::size_t>(std::count(aspec.begin(), aspec.end(), ',')) + 1);

			std::size_t token_begin = 0;
			while (token_begin <= aspec.size())
			{
				const std::size_t delimiter = aspec.find(',', token_begin);
				const std::size_t token_end = delimiter == std::string_view::npos ? aspec.size() : delimiter;
				const std::string option_name = trim_copy(aspec.substr(token_begin, token_end - token_begin));
				if (!option_name.empty())
				{
					option_names.push_back(option_name);
				}

				if (delimiter == std::string_view::npos)
				{
					break;
				}
				token_begin = delimiter + 1;
			}
			return option_names;
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

		static std::vector<std::string> split_args(std::string_view aargs)
		{
			if (aargs.empty())
			{
				return {};
			}

			std::vector<std::string> ltoks;
			ltoks.reserve(static_cast<std::size_t>(std::count_if(aargs.begin(), aargs.end(),
				[](char ach)
				{
					return std::isspace(static_cast<unsigned char>(ach)) != 0;
				})) + 1);
			std::string ltok;
			bool lin_q = false;
			char lq_ch = '\0';
			bool lesc = false;

			auto lflush = [&]()
				{
					if (!ltok.empty())
					{
						ltoks.emplace_back(std::move(ltok));
						ltok.clear();
					}
				};

			for (const char lch : aargs)
			{
				if (lesc)
				{
					ltok.push_back(lch);
					lesc = false;
					continue;
				}

				if (lch == '\\')
				{
					lesc = true;
					continue;
				}

				if (lin_q)
				{
					if (lch == lq_ch)
					{
						lin_q = false;
						lq_ch = '\0';
						continue;
					}

					ltok.push_back(lch);
					continue;
				}

				if (lch == '"' || lch == '\'')
				{
					lin_q = true;
					lq_ch = lch;
					continue;
				}

				if (std::isspace(static_cast<unsigned char>(lch)))
				{
					lflush();
					continue;
				}

				ltok.push_back(lch);
			}

			if (lesc)
			{
				ltok.push_back('\\');
			}

			lflush();
			return ltoks;
		}

		static bool is_help_arg(std::string_view aarg)
		{
			return aarg == "-h" || aarg == "--help";
		}

		void register_aliases(std::string_view aspec)
		{
			const std::vector<std::string> option_names = split_option_names(aspec);
			if (option_names.empty())
			{
				return;
			}

			const std::string& canonical_name = option_names.front();
			for (const std::string& option_name : option_names)
			{
				m_alias_to_key[option_name] = canonical_name;
			}
		}

		std::string resolve_key(std::string_view akey) const
		{
			const std::string normalized_key = normalize_key(akey);
			const auto itor = m_alias_to_key.find(normalized_key);
			if (itor != m_alias_to_key.end())
			{
				return itor->second;
			}
			return normalized_key;
		}

		bool help_enabled() const
		{
			return m_alias_to_key.contains("help");
		}

		std::vector<std::string> strip_help(const std::vector<std::string>& aargs, bool& ahelp_req) const
		{
			ahelp_req = false;
			if (!help_enabled())
			{
				return aargs;
			}

			std::vector<std::string> largs;
			largs.reserve(aargs.size());
			bool lend_opt = false;
			for (const std::string& larg : aargs)
			{
				if (!lend_opt && larg == "--")
				{
					lend_opt = true;
					largs.push_back(larg);
					continue;
				}

				if (!lend_opt && is_help_arg(larg))
				{
					ahelp_req = true;
					continue;
				}

				largs.push_back(larg);
			}
			return largs;
		}

		template <typename TParser>
		bool parse_with(TParser&& aparser, bool ahelp_req)
		{
			try
			{
				po::variables_map lvm;
				po::store(std::forward<TParser>(aparser)(), lvm);
				if (!ahelp_req)
				{
					po::notify(lvm);
				}
				m_vm = std::move(lvm);
				m_help_req = ahelp_req;
				return true;
			}
			catch (const std::exception&)
			{
				clear();
				return false;
			}
		}

		bool parse_args(const std::vector<std::string>& aargs)
		{
			bool lhelp_req = false;
			const std::vector<std::string> largs = strip_help(aargs, lhelp_req);

			std::vector<std::string> largv_buf;
			largv_buf.reserve(largs.size() + 1);
			largv_buf.push_back(m_cmdname.empty() ? "options" : m_cmdname);
			largv_buf.insert(largv_buf.end(), largs.begin(), largs.end());

			std::vector<const char*> largv;
			largv.reserve(largv_buf.size() + 1);
			for (const std::string& larg : largv_buf)
			{
				largv.push_back(larg.c_str());
			}
			largv.push_back(nullptr);

			return parse_with([this, &largv, &largv_buf]()
				{
					return po::command_line_parser(static_cast<int>(largv_buf.size()), largv.data())
						.options(m_options)
						.positional(m_positional)
						.run();
				}, lhelp_req);
		}

		template <typename T, typename TConfig>
		bool bind_typed_option(std::string_view akey, TConfig&& aconfig)
		{
			const std::string normalized_key = normalize_key(akey);
			auto itor = m_info.find(normalized_key);
			if (itor == m_info.end())
			{
				return false;
			}

			if (!m_registered_specs.insert(normalized_key).second)
			{
				return false;
			}

			register_aliases(normalized_key);
			using value_type = option_value_t<T>;
			auto* value_semantic = po::value<value_type>();
			std::forward<TConfig>(aconfig)(value_semantic);
			m_options.add_options()(normalized_key.c_str(), value_semantic, itor->second.c_str());
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
		bool											m_help_req = false;
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
			const std::string normalized_key = normalize_key(akey);
			m_info[normalized_key] = std::string(aremark);
			return bind_options(normalized_key, adata);
		}

		template <typename T>
		bool init_optional(std::string_view akey, std::string_view aremark)
		{
			const std::string normalized_key = normalize_key(akey);
			m_info[normalized_key] = std::string(aremark);
			return bind_typed_option<T>(normalized_key, [](auto*)
				{
					// Optional values intentionally keep Boost's default "value may be absent" behavior.
				});
		}

		template <typename T>
		bool init_required(std::string_view akey, std::string_view aremark)
		{
			const std::string normalized_key = normalize_key(akey);
			m_info[normalized_key] = std::string(aremark);
			return bind_typed_option<T>(normalized_key, [](auto* avalue)
				{
					avalue->required();
				});
		}

		template <typename T>
		bool init_multitoken(std::string_view akey, std::string_view aremark, bool arequired = false)
		{
			const std::string normalized_key = normalize_key(akey);
			m_info[normalized_key] = std::string(aremark);
			return bind_typed_option<T>(normalized_key, [arequired](auto* avalue)
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
			const std::string normalized_key = normalize_key(akey);
			if (!m_registered_specs.insert(normalized_key).second)
			{
				return false;
			}

			m_info[normalized_key] = std::string(aremark);
			register_aliases(normalized_key);
			m_options.add_options()(normalized_key.c_str(), m_info[normalized_key].c_str());
			return true;
		}

		bool init_help(std::string_view adata)
		{
			m_help_text = std::string(adata);
			return init_flag("help,h", "show help");
		}

		void positional(std::string_view akey, int32_t acount)
		{
			const std::string normalized_key = resolve_key(akey);
			m_positional.add(normalized_key.c_str(), acount);
		}

		bool parse(const char* aargs)
		{
			return parse(std::string_view(aargs == nullptr ? "" : aargs));
		}

		bool parse(std::string_view aargs)
		{
			return parse(split_args(aargs));
		}

		bool parse(const std::vector<std::string>& aargs)
		{
			return parse_args(aargs);
		}

		bool parse(int argc, const char* const argv[])
		{
			std::vector<std::string> largs;
			if (argc > 1 && argv != nullptr)
			{
				largs.reserve(static_cast<std::size_t>(argc - 1));
				for (int largi = 1; largi < argc; ++largi)
				{
					if (argv[largi] == nullptr)
					{
						break;
					}
					largs.emplace_back(argv[largi]);
				}
			}
			return parse_args(largs);
		}

		template <typename T>
		bool value(std::string_view akey, T& adata) const
		{
			const std::string lkey = resolve_key(akey);
			if constexpr (std::is_same_v<std::decay_t<T>, std::string>)
			{
				if ((lkey == "help") && has(lkey))
				{
					adata = help();
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
			const std::string lkey = resolve_key(akey);
			if (lkey == "help")
			{
				return m_help_req;
			}
			return m_vm.count(lkey) > 0;
		}

		bool help_requested() const
		{
			return m_help_req;
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
			m_help_req = false;
		}
	};


}//namespace ngl
