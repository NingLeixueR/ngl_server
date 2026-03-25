// File overview: Implements GoogleTest coverage for runtime.

#include <gtest/gtest.h>

#include <array>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "runtime_helpers.h"
#include "test_support.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
#include "tools/db/sql/postgresql/npostgresql_manage.h"
#include "tools/tab/xml/xml.h"
#include "tools/arg_options.h"
#include "tools/tools.h"

namespace ngl::robot_manage_cmd
{
	std::vector<std::string> split_cmd(std::string_view aargs);
	bool strip_help(std::vector<std::string>& aargs);
}

namespace runtime_test_case
{
std::filesystem::path write_cfg(std::string_view apublic)
{
	const std::filesystem::path ldir = ngl_test_support::make_tmp_dir("db_cfg", "ngl_test", true);
	std::filesystem::create_directories(ldir / "config");
	std::ofstream lfile(ldir / "config" / "config.xml");
	lfile
		<< "<xmlnode>\n"
		<< "  <db db=\"0\" ip=\"127.0.0.1\" port=\"3306\" account=\"root\" passworld=\"123456\" dbname=\"lbtest\"/>\n"
		<< "  <public>\n"
		<< apublic
		<< "  </public>\n"
		<< "</xmlnode>\n";
	return ldir;
}

template <std::size_t tcount>
struct argv_buf
{
	std::array<std::string, tcount> m_store;
	std::array<char*, tcount> m_argv{};

	explicit argv_buf(std::array<std::string, tcount> astore) :
		m_store(std::move(astore))
	{
		for (std::size_t li = 0; li < tcount; ++li)
		{
			m_argv[li] = m_store[li].data();
		}
	}

	int argc() const
	{
		return static_cast<int>(m_argv.size());
	}

	char** argv()
	{
		return m_argv.data();
	}
};

template <std::size_t tcount>
argv_buf<tcount> make_argv(const std::array<std::string, tcount>& aargs)
{
	return argv_buf<tcount>(aargs);
}

TEST(RuntimeHelpersTest, SplitCommandLineNormalizesRepeatedSpaces)
{
	const std::vector<std::string> ltokens =
		ngl::arg_options::split_command_line("test   1500   login alice");
	ASSERT_EQ(ltokens.size(), 4u);
	EXPECT_EQ(ltokens[0], "test");
	EXPECT_EQ(ltokens[1], "1500");
	EXPECT_EQ(ltokens[2], "login");
	EXPECT_EQ(ltokens[3], "alice");
}

TEST(RuntimeHelpersTest, RobotManageSplitCommandKeepsQuotedPayload)
{
	const std::vector<std::string> ltokens =
		ngl::robot_manage_cmd::split_cmd("protocol Demo \"{\\\"x\\\": 1, \\\"name\\\": \\\"alpha beta\\\"}\"");
	ASSERT_EQ(ltokens.size(), 3u);
	EXPECT_EQ(ltokens[0], "protocol");
	EXPECT_EQ(ltokens[1], "Demo");
	EXPECT_EQ(ltokens[2], "{\"x\": 1, \"name\": \"alpha beta\"}");
}

TEST(RuntimeHelpersTest, RobotManageStripHelpRemovesHelpFlags)
{
	std::vector<std::string> largs = { "alice", "--help", "tcp", "-h" };

	EXPECT_TRUE(ngl::robot_manage_cmd::strip_help(largs));
	ASSERT_EQ(largs.size(), 2u);
	EXPECT_EQ(largs[0], "alice");
	EXPECT_EQ(largs[1], "tcp");
}

TEST(RuntimeHelpersTest, PushServerConfigParamEncodesStructuredNetPayload)
{
	ngl::tab_servers lsrv{};
	lsrv.m_id = 42;
	lsrv.m_name = "gateway alpha";
	lsrv.m_area = 3;
	lsrv.m_type = ngl::GATEWAY;

	ngl::net_works ltcp{};
	ltcp.m_type = ngl::ENET_TCP;
	ltcp.m_ip = "127.0.0.1";
	ltcp.m_nip = "public.example.com";
	ltcp.m_port = 9000;
	lsrv.m_net.push_back(ltcp);

	std::string lparam;
	ASSERT_TRUE(ngl_runtime::build_push_cfg(lsrv, lparam));
	EXPECT_NE(lparam.find("id=42"), std::string::npos);
	EXPECT_NE(lparam.find("name=gateway%20alpha"), std::string::npos);

	const std::size_t lnet_pos = lparam.find("net=");
	ASSERT_NE(lnet_pos, std::string::npos);
	const std::string lnet = ngl::tools::url_decode(lparam.substr(lnet_pos + 4));
	EXPECT_NE(lnet.find("\"tcp\""), std::string::npos);
	EXPECT_NE(lnet.find("127.0.0.1"), std::string::npos);
	EXPECT_NE(lnet.find("public.example.com"), std::string::npos);
}

TEST(RuntimeHelpersTest, PushServerConfigParamRejectsUnknownNetworkType)
{
	ngl::tab_servers lsrv{};
	lsrv.m_id = 7;
	lsrv.m_name = "broken";
	lsrv.m_area = 1;
	lsrv.m_type = ngl::GATEWAY;

	ngl::net_works lbad{};
	lbad.m_type = static_cast<ngl::ENET_PROTOCOL>(ngl::ENET_COUNT);
	lbad.m_ip = "127.0.0.1";
	lbad.m_port = 8000;
	lsrv.m_net.push_back(lbad);

	std::string lparam = "preset";
	EXPECT_FALSE(ngl_runtime::build_push_cfg(lsrv, lparam));
	EXPECT_TRUE(lparam.empty());
}

TEST(RuntimeHelpersTest, DbManageInitRejectsMissingDbProtoBinary)
{
	const std::filesystem::path ldir = write_cfg("    <data key=\"consumings\">100</data>\n");
	nconfig.init();
	ASSERT_TRUE(nconfig.set_server("db"));
	ASSERT_TRUE(nconfig.load(ldir.string(), "db_1"));

	EXPECT_FALSE(ngl::nmysql_manage::init());
	EXPECT_FALSE(ngl::npostgresql_manage::init());
}

TEST(RuntimeHelpersTest, DbManageInitAcceptsDbProtoBinary)
{
	const std::filesystem::path ldir = write_cfg(
		"    <data key=\"consumings\">100</data>\n"
		"    <data key=\"dbprotobinary\">true</data>\n");
	nconfig.init();
	ASSERT_TRUE(nconfig.set_server("db"));
	ASSERT_TRUE(nconfig.load(ldir.string(), "db_1"));

	EXPECT_TRUE(ngl::nmysql_manage::init());
	EXPECT_TRUE(ngl::npostgresql_manage::init());
}

} // namespace runtime_test_case
