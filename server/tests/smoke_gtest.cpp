// File overview: Implements GoogleTest coverage for smoke.

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "../libnglcore/core/bootstrap/startup_support.h"
#include "../libnglcore/core/bootstrap/server_main.h"
#include "actor/protocol/nprotocol.h"
#include "actor/tab/ttab_servers.h"
#include "test_support.h"

TEST(A01Smoke, StartupInvalidArgs)
{
	char program_name[] = "ngl_test";
	char* argv[] = { program_name };

	const int rc = ngl_main(1, argv);
	EXPECT_EQ(rc, static_cast<int>(startup_error::invalid_args));
}

TEST(A02Smoke, StartupInvalidNodeType)
{
	char program_name[] = "ngl_test";
	char invalid_node[] = "invalid_node";
	char area[] = "1";
	char tcount[] = "1";
	char* argv[] = { program_name, invalid_node, area, tcount };

	const int rc = ngl_main(4, argv);
	EXPECT_EQ(rc, static_cast<int>(startup_error::invalid_node_type));
}

TEST(A02Smoke, StartupRejectsNonNumericArea)
{
	char program_name[] = "ngl_test";
	char db_node[] = "db";
	char area[] = "area";
	char tcount[] = "1";
	char* argv[] = { program_name, db_node, area, tcount };

	const int rc = ngl_main(4, argv);
	EXPECT_EQ(rc, static_cast<int>(startup_error::invalid_args));
}

TEST(A02Smoke, StartupRejectsNonNumericThreadCount)
{
	char program_name[] = "ngl_test";
	char db_node[] = "db";
	char area[] = "1";
	char tcount[] = "thread";
	char* argv[] = { program_name, db_node, area, tcount };

	const int rc = ngl_main(4, argv);
	EXPECT_EQ(rc, static_cast<int>(startup_error::invalid_args));
}

TEST(A03Smoke, StartupReturnsConfigNotFoundWhenConfigRootMissing)
{
	const std::filesystem::path temp_dir = ngl_test_support::make_tmp_dir("config_missing", "ngl_test", false);
	ngl_test_support::scoped_path cwd_guard(temp_dir);

	char program_name[] = "ngl_test";
	char db_node[] = "db";
	char area[] = "1";
	char tcount[] = "1";
	char* argv[] = { program_name, db_node, area, tcount };

	const int rc = ngl_main(4, argv);
	EXPECT_EQ(rc, static_cast<int>(startup_error::config_not_found));
}

TEST(A03Smoke, PrepCtxRestoresXmlNodeRuntimeState)
{
	ngl_test_support::scoped_path lcwd_guard(std::filesystem::path("bin/configure"));

	char lprog[] = "ngl_test";
	char lnode[] = "db";
	char larea[] = "1";
	char ltcount[] = "1";
	char* largv[] = { lprog, lnode, larea, ltcount };

	start_ctx lctx{};
	const prep_res lres = prep_ctx(4, largv, lctx);
	ASSERT_EQ(lres.code, startup_error::ok) << lres.reason;

	const ngl::tab_servers* ltab = ngl::ttab_servers::instance().const_tab("db", 1);
	ASSERT_NE(ltab, nullptr);
	EXPECT_EQ(lctx.node_type, static_cast<int>(ngl::DB));
	EXPECT_EQ(nconfig.nodename(), "db");
	EXPECT_EQ(nconfig.nodetype(), ngl::DB);
	EXPECT_EQ(nconfig.config_name(), "db_1");
	EXPECT_EQ(nconfig.tid(), ltab->m_id);
	EXPECT_EQ(nconfig.tcount(), 1);
	EXPECT_EQ(nconfig.nodeid(), ngl::nnodeid::nodeid(ltab->m_id, 1));
	EXPECT_EQ(nconfig.servername(), std::format("node_db_{}_1", ltab->m_area));
}
