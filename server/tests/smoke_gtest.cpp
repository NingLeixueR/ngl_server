// File overview: Implements GoogleTest coverage for smoke.

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "../libnglcore/core/bootstrap/server_main.h"
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
