// File overview: Implements GoogleTest coverage for runtime.

#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "runtime_helpers.h"
#include "tools/arg_options.h"
#include "tools/tools.h"

namespace runtime_test_case
{
char g_prog[] = "ngl_test";
char g_robot[] = "robot";
char g_area[] = "1";
char g_tcount[] = "1";
char g_account[] = "alice";
char g_begin[] = "10";
char g_count[] = "20";
TEST(RuntimeHelpersTest, RobotLaunchRequestSupportsInteractiveAndBatchModes)
{
	char* linter_argv[] = { g_prog, g_robot, g_area, g_tcount };
	const auto linter = ngl_runtime::build_robot_req(4, linter_argv);
	EXPECT_EQ(linter.mode, ngl_runtime::robot_launch_mode::interactive);
	EXPECT_TRUE(linter.command.empty());

	char* llogin_argv[] = { g_prog, g_robot, g_area, g_tcount, g_account };
	const auto llogin = ngl_runtime::build_robot_req(5, llogin_argv);
	EXPECT_EQ(llogin.mode, ngl_runtime::robot_launch_mode::login);
	EXPECT_EQ(llogin.command, "login alice");

	char* llogins_argv[] = { g_prog, g_robot, g_area, g_tcount, g_account, g_begin, g_count };
	const auto llogins = ngl_runtime::build_robot_req(7, llogins_argv);
	EXPECT_EQ(llogins.mode, ngl_runtime::robot_launch_mode::logins);
	EXPECT_EQ(llogins.command, "logins alice 10 20");
}

TEST(RuntimeHelpersTest, RobotLaunchRequestRejectsPartialBatchArgs)
{
	char* largv[] = { g_prog, g_robot, g_area, g_tcount, g_account, g_begin };
	const auto lreq = ngl_runtime::build_robot_req(6, largv);
	EXPECT_EQ(lreq.mode, ngl_runtime::robot_launch_mode::invalid);
	EXPECT_TRUE(lreq.command.empty());
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

} // namespace runtime_test_case
