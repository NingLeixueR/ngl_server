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
	namespace rt_dtl
	{
		struct rt_path
		{
			std::string_view m_cfg_root;
			std::string_view m_csv_root;
		};

		inline bool can_use(
			const rt_path& apath,
			std::string& acfg_root,
			std::string& acsv_root)
		{
			namespace fs = std::filesystem;

			std::error_code lec;
			const fs::path lcfg_path(apath.m_cfg_root);
			if (!fs::is_directory(lcfg_path, lec))
			{
				return false;
			}

			fs::path lcsv_path(apath.m_csv_root);
			if (!fs::is_directory(lcsv_path, lec))
			{
				lcsv_path = lcfg_path / "csv";
			}

			const bool lhas_cfg =
				fs::is_regular_file(lcfg_path / "config.xml", lec) ||
				fs::is_regular_file(lcfg_path / "config" / "config.xml", lec);
			const bool lhas_csv =
				fs::is_directory(lcsv_path, lec) &&
				fs::is_regular_file(lcsv_path / "tab_servers.csv", lec);
			if (!lhas_cfg || !lhas_csv)
			{
				return false;
			}

			acfg_root = lcfg_path.string();
			acsv_root = lcsv_path.string();
			return true;
		}
	} // namespace rt_dtl

	inline bool find_rt(std::string& acfg_root, std::string& acsv_root, std::string& aerr)
	{
		namespace fs = std::filesystem;

		static constexpr std::array<rt_dtl::rt_path, 2> g_rt_paths = {{
			{ "./config", "./csv" },
			{ "./bin/configure/config", "./bin/configure/csv" },
		}};

		for (const auto& lpath : g_rt_paths)
		{
			if (rt_dtl::can_use(lpath, acfg_root, acsv_root))
			{
				aerr.clear();
				return true;
			}
		}

		std::error_code lec;
		const fs::path lcwd = fs::current_path(lec);
		const std::string lcwd_txt = lec ? "<unknown>" : lcwd.string();
		aerr = std::format("unable to locate config/csv directories from cwd={}", lcwd_txt);
		return false;
	}

	inline bool reload_rt(
		std::string& aerr,
		std::string_view anode = "db",
		int aarea = 1,
		int atcount = 1)
	{
		static std::mutex g_rt_mutex;
		std::scoped_lock lguard(g_rt_mutex);

		if (anode.empty())
		{
			aerr = "runtime bootstrap node_name is empty";
			return false;
		}

		std::string lcfg_root;
		std::string lcsv_root;
		if (!find_rt(lcfg_root, lcsv_root, aerr))
		{
			return false;
		}

		const std::string lnode(anode);
		nconfig.init();
		if (!nconfig.set_server(lnode.c_str()))
		{
			aerr = std::format("nconfig.set_server({}) failed", lnode);
			return false;
		}

		if (!nconfig.load(lcfg_root, std::format("{}_{}", lnode, atcount)))
		{
			aerr = std::format("nconfig.load failed: {}", nconfig.config_file());
			return false;
		}

		ngl::csv_base::set_path(lcsv_root, lnode);
		const ngl::tab_servers* ltab = ngl::ttab_servers::instance().const_tab(lnode, aarea);
		if (ltab == nullptr)
		{
			aerr = std::format("ttab_servers::const_tab({}, {}) returned nullptr", lnode, aarea);
			return false;
		}

		nconfig.set_nodeid(ltab->m_id, atcount);
		nconfig.set_servername(std::format("node_{}_{}_{}", lnode, ltab->m_area, atcount));
		aerr.clear();
		return true;
	}

	inline bool ensure_rt(std::string& aerr)
	{
		static std::once_flag g_rt_once;
		static bool g_rt_ok = false;
		static std::string g_rt_err;

		std::call_once(g_rt_once, []() {
			std::string lerr;
			g_rt_ok = reload_rt(lerr);
			g_rt_err = std::move(lerr);
		});

		if (!g_rt_ok)
		{
			aerr = g_rt_err;
			return false;
		}

		aerr.clear();
		return true;
	}
}
