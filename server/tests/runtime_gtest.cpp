// File overview: Implements GoogleTest coverage for runtime.

#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "runtime_helpers.h"
#include "tools/tools.h"

namespace
{
char kProgram[] = "ngl_test";
char kRobot[] = "robot";
char kArea[] = "1";
char kTcount[] = "1";
char kAccount[] = "alice";
char kBegin[] = "10";
char kCount[] = "20";
}

TEST(RuntimeHelpersTest, RobotLaunchRequestSupportsInteractiveAndBatchModes)
{
	char* interactive_argv[] = { kProgram, kRobot, kArea, kTcount };
	const auto interactive = ngl_runtime::build_robot_launch_request(4, interactive_argv);
	EXPECT_EQ(interactive.mode, ngl_runtime::robot_launch_mode::interactive);
	EXPECT_TRUE(interactive.command.empty());

	char* login_argv[] = { kProgram, kRobot, kArea, kTcount, kAccount };
	const auto login = ngl_runtime::build_robot_launch_request(5, login_argv);
	EXPECT_EQ(login.mode, ngl_runtime::robot_launch_mode::login);
	EXPECT_EQ(login.command, "login alice");

	char* logins_argv[] = { kProgram, kRobot, kArea, kTcount, kAccount, kBegin, kCount };
	const auto logins = ngl_runtime::build_robot_launch_request(7, logins_argv);
	EXPECT_EQ(logins.mode, ngl_runtime::robot_launch_mode::logins);
	EXPECT_EQ(logins.command, "logins alice 10 20");
}

TEST(RuntimeHelpersTest, RobotLaunchRequestRejectsPartialBatchArgs)
{
	char* argv[] = { kProgram, kRobot, kArea, kTcount, kAccount, kBegin };
	const auto request = ngl_runtime::build_robot_launch_request(6, argv);
	EXPECT_EQ(request.mode, ngl_runtime::robot_launch_mode::invalid);
	EXPECT_TRUE(request.command.empty());
}

TEST(RuntimeHelpersTest, SplitCommandLineNormalizesRepeatedSpaces)
{
	const std::vector<std::string> tokens = ngl_runtime::split_command_line("test   1500   login alice");
	ASSERT_EQ(tokens.size(), 4u);
	EXPECT_EQ(tokens[0], "test");
	EXPECT_EQ(tokens[1], "1500");
	EXPECT_EQ(tokens[2], "login");
	EXPECT_EQ(tokens[3], "alice");
}

TEST(RuntimeHelpersTest, PushServerConfigParamEncodesStructuredNetPayload)
{
	ngl::tab_servers server{};
	server.m_id = 42;
	server.m_name = "gateway alpha";
	server.m_area = 3;
	server.m_type = ngl::GATEWAY;

	ngl::net_works tcp{};
	tcp.m_type = ngl::ENET_TCP;
	tcp.m_ip = "127.0.0.1";
	tcp.m_nip = "public.example.com";
	tcp.m_port = 9000;
	server.m_net.push_back(tcp);

	std::string param;
	ASSERT_TRUE(ngl_runtime::build_push_server_config_param(server, param));
	EXPECT_NE(param.find("id=42"), std::string::npos);
	EXPECT_NE(param.find("name=gateway%20alpha"), std::string::npos);

	const std::size_t net_pos = param.find("net=");
	ASSERT_NE(net_pos, std::string::npos);
	const std::string decoded_net = ngl::tools::url_decode(param.substr(net_pos + 4));
	EXPECT_NE(decoded_net.find("\"tcp\""), std::string::npos);
	EXPECT_NE(decoded_net.find("127.0.0.1"), std::string::npos);
	EXPECT_NE(decoded_net.find("public.example.com"), std::string::npos);
}

TEST(RuntimeHelpersTest, PushServerConfigParamRejectsUnknownNetworkType)
{
	ngl::tab_servers server{};
	server.m_id = 7;
	server.m_name = "broken";
	server.m_area = 1;
	server.m_type = ngl::GATEWAY;

	ngl::net_works invalid{};
	invalid.m_type = static_cast<ngl::ENET_PROTOCOL>(ngl::ENET_COUNT);
	invalid.m_ip = "127.0.0.1";
	invalid.m_port = 8000;
	server.m_net.push_back(invalid);

	std::string param = "preset";
	EXPECT_FALSE(ngl_runtime::build_push_server_config_param(server, param));
	EXPECT_TRUE(param.empty());
}