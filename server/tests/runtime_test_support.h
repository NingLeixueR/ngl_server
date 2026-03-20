// File overview: Shared runtime bootstrap helpers for server GoogleTest cases.

#pragma once

#include <array>
#include <filesystem>
#include <format>
#include <mutex>
#include <string>
#include <string_view>

#include "actor/tab/ttab_servers.h"
#include "tools/tab/csv/ncsv.h"
#include "tools/tab/xml/xml.h"

namespace ngl_test_support
{
	namespace runtime_support_detail
	{
		struct RuntimePathCandidate
		{
			std::string_view config_root;
			std::string_view csv_root;
		};

		inline bool candidate_usable(
			const RuntimePathCandidate& candidate,
			std::string& config_root,
			std::string& csv_root)
		{
			namespace fs = std::filesystem;

			std::error_code ec;
			const fs::path candidate_config(candidate.config_root);
			if (!fs::is_directory(candidate_config, ec))
			{
				return false;
			}

			fs::path candidate_csv(candidate.csv_root);
			if (!fs::is_directory(candidate_csv, ec))
			{
				candidate_csv = candidate_config / "csv";
			}

			const bool has_config =
				fs::is_regular_file(candidate_config / "config.xml", ec) ||
				fs::is_regular_file(candidate_config / "config" / "config.xml", ec);
			const bool has_servers_csv =
				fs::is_directory(candidate_csv, ec) &&
				fs::is_regular_file(candidate_csv / "tab_servers.csv", ec);
			if (!has_config || !has_servers_csv)
			{
				return false;
			}

			config_root = candidate_config.string();
			csv_root = candidate_csv.string();
			return true;
		}
	} // namespace runtime_support_detail

	inline bool resolve_runtime_paths(std::string& config_root, std::string& csv_root, std::string& err)
	{
		namespace fs = std::filesystem;

		static constexpr std::array<runtime_support_detail::RuntimePathCandidate, 2> kCandidates = {{
			{ "./config", "./csv" },
			{ "./bin/configure/config", "./bin/configure/csv" },
		}};

		for (const auto& candidate : kCandidates)
		{
			if (runtime_support_detail::candidate_usable(candidate, config_root, csv_root))
			{
				err.clear();
				return true;
			}
		}

		std::error_code ec;
		const fs::path cwd = fs::current_path(ec);
		const std::string cwd_text = ec ? "<unknown>" : cwd.string();
		err = std::format("unable to locate config/csv directories from cwd={}", cwd_text);
		return false;
	}

	inline bool reload_bootstrap_runtime(
		std::string& err,
		std::string_view node_name = "db",
		int area = 1,
		int tcount = 1)
	{
		static std::mutex runtime_mutex;
		std::lock_guard<std::mutex> guard(runtime_mutex);

		if (node_name.empty())
		{
			err = "runtime bootstrap node_name is empty";
			return false;
		}

		std::string config_root;
		std::string csv_root;
		if (!resolve_runtime_paths(config_root, csv_root, err))
		{
			return false;
		}

		const std::string server_name(node_name);
		nconfig.init();
		if (!nconfig.set_server(server_name.c_str()))
		{
			err = std::format("nconfig.set_server({}) failed", server_name);
			return false;
		}

		if (!nconfig.load(config_root, std::format("{}_{}", server_name, tcount)))
		{
			err = std::format("nconfig.load failed: {}", nconfig.config_file());
			return false;
		}

		ngl::csv_base::set_path(csv_root, server_name);
		const ngl::tab_servers* tab = ngl::ttab_servers::instance().const_tab(server_name, area);
		if (tab == nullptr)
		{
			err = std::format("ttab_servers::const_tab({}, {}) returned nullptr", server_name, area);
			return false;
		}

		nconfig.set_nodeid(tab->m_id, tcount);
		nconfig.set_servername(std::format("node_{}_{}_{}", server_name, tab->m_area, tcount));
		err.clear();
		return true;
	}

	inline bool ensure_bootstrap_runtime(std::string& err)
	{
		static std::once_flag once;
		static bool ok = false;
		static std::string cached_err;

		std::call_once(once, []() {
			std::string local_err;
			ok = reload_bootstrap_runtime(local_err);
			cached_err = std::move(local_err);
		});

		if (!ok)
		{
			err = cached_err;
			return false;
		}

		err.clear();
		return true;
	}
}
