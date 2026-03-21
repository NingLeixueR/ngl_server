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
	namespace RT_DTL
	{
		struct RT_PATH
		{
			std::string_view m_cfg_root;
			std::string_view m_csv_root;
		};

		inline bool CAN_USE(
			const RT_PATH& aPATH,
			std::string& aCFG_ROOT,
			std::string& aCSV_ROOT)
		{
			namespace fs = std::filesystem;

			std::error_code lEC;
			const fs::path lCFG_PATH(aPATH.m_cfg_root);
			if (!fs::is_directory(lCFG_PATH, lEC))
			{
				return false;
			}

			fs::path lCSV_PATH(aPATH.m_csv_root);
			if (!fs::is_directory(lCSV_PATH, lEC))
			{
				lCSV_PATH = lCFG_PATH / "csv";
			}

			const bool lHAS_CFG =
				fs::is_regular_file(lCFG_PATH / "config.xml", lEC) ||
				fs::is_regular_file(lCFG_PATH / "config" / "config.xml", lEC);
			const bool lHAS_CSV =
				fs::is_directory(lCSV_PATH, lEC) &&
				fs::is_regular_file(lCSV_PATH / "tab_servers.csv", lEC);
			if (!lHAS_CFG || !lHAS_CSV)
			{
				return false;
			}

			aCFG_ROOT = lCFG_PATH.string();
			aCSV_ROOT = lCSV_PATH.string();
			return true;
		}
	} // namespace RT_DTL

	inline bool FIND_RT(std::string& aCFG_ROOT, std::string& aCSV_ROOT, std::string& aERR)
	{
		namespace fs = std::filesystem;

		static constexpr std::array<RT_DTL::RT_PATH, 2> g_RT_PATHS = {{
			{ "./config", "./csv" },
			{ "./bin/configure/config", "./bin/configure/csv" },
		}};

		for (const auto& lPATH : g_RT_PATHS)
		{
			if (RT_DTL::CAN_USE(lPATH, aCFG_ROOT, aCSV_ROOT))
			{
				aERR.clear();
				return true;
			}
		}

		std::error_code lEC;
		const fs::path lCWD = fs::current_path(lEC);
		const std::string lCWD_TXT = lEC ? "<unknown>" : lCWD.string();
		aERR = std::format("unable to locate config/csv directories from cwd={}", lCWD_TXT);
		return false;
	}

	inline bool RELOAD_RT(
		std::string& aERR,
		std::string_view aNODE = "db",
		int aAREA = 1,
		int aTCOUNT = 1)
	{
		static std::mutex g_RT_MUTEX;
		std::lock_guard<std::mutex> lGUARD(g_RT_MUTEX);

		if (aNODE.empty())
		{
			aERR = "runtime bootstrap node_name is empty";
			return false;
		}

		std::string lCFG_ROOT;
		std::string lCSV_ROOT;
		if (!FIND_RT(lCFG_ROOT, lCSV_ROOT, aERR))
		{
			return false;
		}

		const std::string lSRV(aNODE);
		nconfig.init();
		if (!nconfig.set_server(lSRV.c_str()))
		{
			aERR = std::format("nconfig.set_server({}) failed", lSRV);
			return false;
		}

		if (!nconfig.load(lCFG_ROOT, std::format("{}_{}", lSRV, aTCOUNT)))
		{
			aERR = std::format("nconfig.load failed: {}", nconfig.config_file());
			return false;
		}

		ngl::csv_base::set_path(lCSV_ROOT, lSRV);
		const ngl::tab_servers* lTAB = ngl::ttab_servers::instance().const_tab(lSRV, aAREA);
		if (lTAB == nullptr)
		{
			aERR = std::format("ttab_servers::const_tab({}, {}) returned nullptr", lSRV, aAREA);
			return false;
		}

		nconfig.set_nodeid(lTAB->m_id, aTCOUNT);
		nconfig.set_servername(std::format("node_{}_{}_{}", lSRV, lTAB->m_area, aTCOUNT));
		aERR.clear();
		return true;
	}

	inline bool ENSURE_RT(std::string& aERR)
	{
		static std::once_flag g_RT_ONCE;
		static bool g_RT_OK = false;
		static std::string g_RT_ERR;

		std::call_once(g_RT_ONCE, []() {
			std::string lERR;
			g_RT_OK = RELOAD_RT(lERR);
			g_RT_ERR = std::move(lERR);
		});

		if (!g_RT_OK)
		{
			aERR = g_RT_ERR;
			return false;
		}

		aERR.clear();
		return true;
	}
}
