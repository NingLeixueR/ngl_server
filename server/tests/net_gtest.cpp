#include <gtest/gtest.h>

#include <array>
#include <atomic>
#include <chrono>
#include <future>
#include <memory>
#include <thread>
#include <vector>

#include "actor/protocol/nprotocol.h"
#include "net/server_session.h"
#include "net/tcp/asio_tcp.h"
#include "net/tcp/ntcp.h"

namespace basio = ngl::basio;
using basio_errorcode = ngl::basio_errorcode;

TEST(NetTest, ServerSessionReplacesExistingMappingsBidirectionally)
{
	constexpr ngl::i32_serverid kServer1 = 91001;
	constexpr ngl::i32_serverid kServer2 = 91002;
	constexpr ngl::i32_sessionid kSession1 = 92001;
	constexpr ngl::i32_sessionid kSession2 = 92002;

	ngl::server_session::remove(kSession1);
	ngl::server_session::remove(kSession2);

	ngl::server_session::add(kServer1, kSession1);
	EXPECT_EQ(ngl::server_session::sessionid(kServer1), kSession1);
	EXPECT_EQ(ngl::server_session::serverid(kSession1), kServer1);

	ngl::server_session::add(kServer1, kSession2);
	EXPECT_EQ(ngl::server_session::sessionid(kServer1), kSession2);
	EXPECT_EQ(ngl::server_session::serverid(kSession1), -1);
	EXPECT_EQ(ngl::server_session::serverid(kSession2), kServer1);

	ngl::server_session::add(kServer2, kSession2);
	EXPECT_EQ(ngl::server_session::sessionid(kServer1), -1);
	EXPECT_EQ(ngl::server_session::sessionid(kServer2), kSession2);
	EXPECT_EQ(ngl::server_session::serverid(kSession2), kServer2);

	ngl::server_session::remove(kSession1);
	ngl::server_session::remove(kSession2);
	EXPECT_EQ(ngl::server_session::sessionid(kServer1), -1);
	EXPECT_EQ(ngl::server_session::sessionid(kServer2), -1);
	EXPECT_EQ(ngl::server_session::serverid(kSession1), -1);
	EXPECT_EQ(ngl::server_session::serverid(kSession2), -1);
}

TEST(NetTest, SendClientReportsFailureWhenGatewayIsUnavailable)
{
	ngl::np_testlua payload;
	payload.m_name = "net";
	payload.m_data.insert({ 1, "value" });

	EXPECT_FALSE(ngl::ntcp::instance().send_client(1001, 1, 0, payload));

	const std::vector<std::pair<ngl::i32_actordataid, ngl::i16_area>> recipients = {
		{ 1001, 1 },
		{ 1002, 1 },
	};
	EXPECT_FALSE(ngl::ntcp::instance().send_client(recipients, 990001, payload));
}

TEST(NetTest, BatchSendReportsFailureWhenAnySessionIsUnavailable)
{
	auto packet = ngl::pack::make_pack(nullptr, 0);
	ASSERT_NE(packet, nullptr);

	const std::map<ngl::i32_sessionid, ngl::i64_actorid> session_to_actor = {
		{ 123456, 1001 },
		{ 123457, 1002 },
	};
	EXPECT_FALSE(ngl::ntcp::instance().send(session_to_actor, 2001, packet));

	const std::set<ngl::i32_sessionid> sessions = {
		123456,
		123457,
	};
	EXPECT_FALSE(ngl::ntcp::instance().send(sessions, 2001, 3001, packet));
}

TEST(NetTest, AsioTcpConnectReportsFinalFailure)
{
	basio::io_context probe_context;
	basio::ip::tcp::acceptor probe_acceptor(probe_context, basio::ip::tcp::endpoint(basio::ip::tcp::v4(), 0));
	const ngl::i16_port port = probe_acceptor.local_endpoint().port();
	probe_acceptor.close();

	auto result = std::make_shared<std::promise<ngl::i32_sessionid>>();
	std::future<ngl::i32_sessionid> future = result->get_future();

	auto client = std::make_unique<ngl::asio_tcp>(
		1,
		[](ngl::service_tcp*, const char*, uint32_t) { return true; },
		[](ngl::i32_sessionid) {},
		[](ngl::i32_sessionid, bool, const ngl::pack*) {}
	);

	client->connect("127.0.0.1", port, [result](ngl::i32_sessionid sessionid) {
		try
		{
			result->set_value(sessionid);
		}
		catch (const std::future_error&)
		{
		}
	}, 0);

	ASSERT_EQ(future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	EXPECT_EQ(future.get(), -1);
}

TEST(NetTest, AsioTcpCloseNetRemovesIpPortEntry)
{
	basio::io_context accept_context;
	basio::ip::tcp::acceptor acceptor(accept_context, basio::ip::tcp::endpoint(basio::ip::tcp::v4(), 0));
	const ngl::i16_port port = acceptor.local_endpoint().port();

	auto accepted_socket = std::make_shared<basio::ip::tcp::socket>(accept_context);
	auto accepted_result = std::make_shared<std::promise<void>>();
	std::future<void> accepted_future = accepted_result->get_future();

	acceptor.async_accept(*accepted_socket, [accepted_result](const basio_errorcode& ec) {
		EXPECT_FALSE(ec);
		try
		{
			accepted_result->set_value();
		}
		catch (const std::future_error&)
		{
		}
	});

	std::thread accept_thread([&accept_context]() {
		accept_context.run();
	});

	auto result = std::make_shared<std::promise<ngl::i32_sessionid>>();
	std::future<ngl::i32_sessionid> future = result->get_future();

	auto client = std::make_unique<ngl::asio_tcp>(
		1,
		[](ngl::service_tcp*, const char*, uint32_t) { return true; },
		[](ngl::i32_sessionid) {},
		[](ngl::i32_sessionid, bool, const ngl::pack*) {}
	);

	client->connect("127.0.0.1", port, [result](ngl::i32_sessionid sessionid) {
		try
		{
			result->set_value(sessionid);
		}
		catch (const std::future_error&)
		{
		}
	}, 0);

	ASSERT_EQ(future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	ASSERT_EQ(accepted_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);

	const ngl::i32_sessionid sessionid = future.get();
	ASSERT_GT(sessionid, 0);

	std::pair<ngl::str_ip, ngl::i16_port> endpoint;
	EXPECT_TRUE(client->get_ipport(sessionid, endpoint));

	client->close_net(sessionid);
	EXPECT_FALSE(client->get_ipport(sessionid, endpoint));

	basio_errorcode ec;
	accepted_socket->close(ec);
	acceptor.close(ec);
	accept_context.stop();
	accept_thread.join();
}

TEST(NetTest, AsioTcpCloseDisconnectsPeerAndNotifiesOnce)
{
	basio::io_context accept_context;
	auto accept_work = basio::make_work_guard(accept_context);
	basio::ip::tcp::acceptor acceptor(accept_context, basio::ip::tcp::endpoint(basio::ip::tcp::v4(), 0));
	const ngl::i16_port port = acceptor.local_endpoint().port();

	auto accepted_socket = std::make_shared<basio::ip::tcp::socket>(accept_context);
	auto accepted_result = std::make_shared<std::promise<void>>();
	std::future<void> accepted_future = accepted_result->get_future();

	acceptor.async_accept(*accepted_socket, [accepted_result](const basio_errorcode& ec) {
		EXPECT_FALSE(ec);
		try
		{
			accepted_result->set_value();
		}
		catch (const std::future_error&)
		{
		}
	});

	std::thread accept_thread([&accept_context]() {
		accept_context.run();
	});

	auto close_count = std::make_shared<std::atomic<int>>(0);
	auto recv_close_count = std::make_shared<std::atomic<int>>(0);
	auto result = std::make_shared<std::promise<ngl::i32_sessionid>>();
	std::future<ngl::i32_sessionid> future = result->get_future();

	auto client = std::make_unique<ngl::asio_tcp>(
		1,
		[recv_close_count](ngl::service_tcp*, const char* buff, uint32_t bufflen) {
			if (buff == nullptr && bufflen == 0)
			{
				recv_close_count->fetch_add(1, std::memory_order_relaxed);
			}
			return true;
		},
		[close_count](ngl::i32_sessionid) {
			close_count->fetch_add(1, std::memory_order_relaxed);
		},
		[](ngl::i32_sessionid, bool, const ngl::pack*) {}
	);

	client->connect("127.0.0.1", port, [result](ngl::i32_sessionid sessionid) {
		try
		{
			result->set_value(sessionid);
		}
		catch (const std::future_error&)
		{
		}
	}, 0);

	ASSERT_EQ(future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	ASSERT_EQ(accepted_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);

	const ngl::i32_sessionid sessionid = future.get();
	ASSERT_GT(sessionid, 0);

	auto peer_close_result = std::make_shared<std::promise<basio_errorcode>>();
	std::future<basio_errorcode> peer_close_future = peer_close_result->get_future();
	auto peer_buffer = std::make_shared<std::array<char, 16>>();

	accepted_socket->async_read_some(basio::buffer(*peer_buffer), [peer_close_result](const basio_errorcode& ec, std::size_t bytes_transferred) {
		EXPECT_EQ(bytes_transferred, 0U);
		try
		{
			peer_close_result->set_value(ec);
		}
		catch (const std::future_error&)
		{
		}
	});

	client->close(sessionid);

	ASSERT_EQ(peer_close_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	const basio_errorcode peer_close_error = peer_close_future.get();
	EXPECT_TRUE(peer_close_error == basio::error::eof || peer_close_error == basio::error::connection_reset);
	EXPECT_EQ(close_count->load(std::memory_order_relaxed), 1);
	EXPECT_EQ(recv_close_count->load(std::memory_order_relaxed), 0);

	std::pair<ngl::str_ip, ngl::i16_port> endpoint;
	EXPECT_FALSE(client->get_ipport(sessionid, endpoint));

	basio_errorcode ec;
	accepted_socket->close(ec);
	acceptor.close(ec);
	accept_work.reset();
	accept_context.stop();
	accept_thread.join();
}
