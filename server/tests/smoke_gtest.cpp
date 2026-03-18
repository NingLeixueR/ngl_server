// File overview: Implements GoogleTest coverage for smoke.

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "server_main.h"

TEST(A01Smoke, StartupInvalidArgs)
{
	char program_name[] = "ngl_test";
	char* argv[] = { program_name };

	const int rc = ngl_main(1, argv);
	EXPECT_EQ(rc, static_cast<int>(startup_error::invalid_args));
}

namespace smoke_test_support
{
class ScopedCurrentPath
{
public:
	explicit ScopedCurrentPath(const std::filesystem::path& target)
		: original_(std::filesystem::current_path())
	{
		std::filesystem::current_path(target);
	}

	~ScopedCurrentPath()
	{
		std::filesystem::current_path(original_);
	}

private:
	std::filesystem::path original_;
};

std::filesystem::path make_temp_test_dir(const std::string& test_name)
{
	const std::filesystem::path dir =
		std::filesystem::temp_directory_path() /
		std::filesystem::path("ngl_test_" + test_name);
	std::error_code ec;
	std::filesystem::remove_all(dir, ec);
	std::filesystem::create_directories(dir);
	return dir;
}
} // namespace smoke_test_support

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
	const std::filesystem::path temp_dir = smoke_test_support::make_temp_test_dir("config_missing");
	smoke_test_support::ScopedCurrentPath cwd_guard(temp_dir);

	char program_name[] = "ngl_test";
	char db_node[] = "db";
	char area[] = "1";
	char tcount[] = "1";
	char* argv[] = { program_name, db_node, area, tcount };

	const int rc = ngl_main(4, argv);
	EXPECT_EQ(rc, static_cast<int>(startup_error::config_not_found));
}

TEST(A04Smoke, StartupReturnsConfigLoadFailedForMalformedConfig)
{
	const std::filesystem::path temp_dir = smoke_test_support::make_temp_test_dir("config_invalid");
	std::filesystem::create_directories(temp_dir / "config");
	std::ofstream(temp_dir / "config" / "config.xml") << "<config>";
	smoke_test_support::ScopedCurrentPath cwd_guard(temp_dir);

	char program_name[] = "ngl_test";
	char db_node[] = "db";
	char area[] = "1";
	char tcount[] = "1";
	char* argv[] = { program_name, db_node, area, tcount };

	const int rc = ngl_main(4, argv);
	EXPECT_EQ(rc, static_cast<int>(startup_error::config_load_failed));
}
