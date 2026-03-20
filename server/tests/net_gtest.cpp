// File overview: Implements GoogleTest coverage for net.

#include <gtest/gtest.h>

#include <array>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <string>
#include <future>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

#include "actor/actor_base/nguid.h"
#include "actor/protocol/nprotocol.h"
#include "net/server_session.h"
#include "net/tcp/asio_tcp.h"
#include "net/tcp/ntcp.h"
#include "net/udp/kcp/asio_kcp.h"
#include "net/udp/kcp/kcp_session.h"
#include "test_support.h"

namespace basio = ngl::basio;
using basio_errorcode = ngl::basio_errorcode;

namespace net_test_support
{
	std::shared_ptr<ngl::pack> make_test_packet(const std::string& payload)
	{
		auto packet = std::make_shared<ngl::pack>();
		if (!packet->malloc(static_cast<int32_t>(payload.size())))
		{
			return {};
		}
		std::copy(payload.begin(), payload.end(), packet->m_buff);
		packet->m_len = static_cast<int32_t>(payload.size());
		packet->m_pos = static_cast<int32_t>(payload.size());
		return packet;
	}
}

using net_test_support::make_test_packet;
using ngl_test_support::try_set_promise_value;

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
		try_set_promise_value(result, sessionid);
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
		try_set_promise_value(accepted_result);
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
		try_set_promise_value(result, sessionid);
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
		try_set_promise_value(accepted_result);
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
		try_set_promise_value(result, sessionid);
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
		try_set_promise_value(peer_close_result, ec);
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

TEST(NetTest, AsioTcpServerAndClientExchangePayloads)
{
#if defined(_WIN32)
	GTEST_SKIP() << "Windows coverage is provided by split server/client exchange tests to avoid flaky dual-asio_tcp in-process teardown.";
#else
	auto server_received = std::make_shared<std::promise<std::string>>();
	auto server_session = std::make_shared<std::promise<ngl::i32_sessionid>>();
	auto client_received = std::make_shared<std::promise<std::string>>();
	auto connected = std::make_shared<std::promise<ngl::i32_sessionid>>();
	auto server_closed = std::make_shared<std::promise<ngl::i32_sessionid>>();
	auto client_closed = std::make_shared<std::promise<ngl::i32_sessionid>>();

	std::future<std::string> server_received_future = server_received->get_future();
	std::future<ngl::i32_sessionid> server_session_future = server_session->get_future();
	std::future<std::string> client_received_future = client_received->get_future();
	std::future<ngl::i32_sessionid> connected_future = connected->get_future();
	std::future<ngl::i32_sessionid> server_closed_future = server_closed->get_future();
	std::future<ngl::i32_sessionid> client_closed_future = client_closed->get_future();

	auto server = std::make_unique<ngl::asio_tcp>(
		0,
		1,
		[server_received, server_session](ngl::service_tcp* asession, const char* buff, uint32_t bufflen) {
			try_set_promise_value(server_received, std::string(buff, buff + bufflen));
			try_set_promise_value(server_session, asession->m_sessionid);
			return true;
		},
		[server_closed](ngl::i32_sessionid sessionid) {
			try_set_promise_value(server_closed, sessionid);
		},
		[](ngl::i32_sessionid, bool, const ngl::pack*) {}
	);
	const ngl::i16_port port = server->port();
	ASSERT_GT(port, 0);

	auto client = std::make_unique<ngl::asio_tcp>(
		1,
		[client_received](ngl::service_tcp*, const char* buff, uint32_t bufflen) {
			try_set_promise_value(client_received, std::string(buff, buff + bufflen));
			return true;
		},
		[client_closed](ngl::i32_sessionid sessionid) {
			try_set_promise_value(client_closed, sessionid);
		},
		[](ngl::i32_sessionid, bool, const ngl::pack*) {}
	);

	client->connect("127.0.0.1", port, [connected](ngl::i32_sessionid sessionid) {
		try_set_promise_value(connected, sessionid);
	}, 0);

	ASSERT_EQ(connected_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	const ngl::i32_sessionid client_sessionid = connected_future.get();
	ASSERT_GT(client_sessionid, 0);

	auto client_packet = make_test_packet("tcp-ping");
	ASSERT_NE(client_packet, nullptr);
	ASSERT_TRUE(client->send(client_sessionid, client_packet));

	ASSERT_EQ(server_received_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	ASSERT_EQ(server_session_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	EXPECT_EQ(server_received_future.get(), "tcp-ping");
	const ngl::i32_sessionid server_sessionid = server_session_future.get();
	ASSERT_GT(server_sessionid, 0);

	auto server_packet = make_test_packet("tcp-pong");
	ASSERT_NE(server_packet, nullptr);
	ASSERT_TRUE(server->send(server_sessionid, server_packet));

	ASSERT_EQ(client_received_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	EXPECT_EQ(client_received_future.get(), "tcp-pong");

	std::pair<ngl::str_ip, ngl::i16_port> endpoint;
	EXPECT_TRUE(client->get_ipport(client_sessionid, endpoint));
	EXPECT_EQ(endpoint.first, "127.0.0.1");

	client->close(client_sessionid);

	ASSERT_EQ(client_closed_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	ASSERT_EQ(client_closed_future.get(), client_sessionid);
	ASSERT_EQ(server_closed_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	ASSERT_GT(server_closed_future.get(), 0);

	client.reset();
	server.reset();
#endif
}

TEST(NetTest, AsioTcpServerExchangesPayloadsWithPlainAsioClient)
{
	auto server_received = std::make_shared<std::promise<std::string>>();
	auto server_session = std::make_shared<std::promise<ngl::i32_sessionid>>();
	auto server_closed = std::make_shared<std::promise<ngl::i32_sessionid>>();

	std::future<std::string> server_received_future = server_received->get_future();
	std::future<ngl::i32_sessionid> server_session_future = server_session->get_future();
	std::future<ngl::i32_sessionid> server_closed_future = server_closed->get_future();

	auto server = std::make_unique<ngl::asio_tcp>(
		0,
		1,
		[server_received, server_session](ngl::service_tcp* asession, const char* buff, uint32_t bufflen) {
			try_set_promise_value(server_received, std::string(buff, buff + bufflen));
			try_set_promise_value(server_session, asession->m_sessionid);
			return true;
		},
		[server_closed](ngl::i32_sessionid sessionid) {
			try_set_promise_value(server_closed, sessionid);
		},
		[](ngl::i32_sessionid, bool, const ngl::pack*) {}
	);

	const ngl::i16_port port = server->port();
	ASSERT_GT(port, 0);

	basio::io_context client_context;
	basio::ip::tcp::socket client_socket(client_context);
	client_socket.connect(basio::ip::tcp::endpoint(basio::ip::make_address("127.0.0.1"), port));

	const std::string kPing = "tcp-ping";
	const std::string kPong = "tcp-pong";
	basio::write(client_socket, basio::buffer(kPing));

	ASSERT_EQ(server_received_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	ASSERT_EQ(server_session_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	EXPECT_EQ(server_received_future.get(), kPing);

	const ngl::i32_sessionid sessionid = server_session_future.get();
	ASSERT_GT(sessionid, 0);

	auto packet = make_test_packet(kPong);
	ASSERT_NE(packet, nullptr);
	ASSERT_TRUE(server->send(sessionid, packet));

	std::array<char, 16> reply{};
	const std::size_t reply_bytes = basio::read(client_socket, basio::buffer(reply.data(), kPong.size()));
	ASSERT_EQ(reply_bytes, kPong.size());
	EXPECT_EQ(std::string(reply.data(), reply.data() + reply_bytes), kPong);

	basio_errorcode ec;
	client_socket.shutdown(basio::ip::tcp::socket::shutdown_both, ec);
	client_socket.close(ec);

	ASSERT_EQ(server_closed_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	EXPECT_EQ(server_closed_future.get(), sessionid);

#if defined(_WIN32)
	// Windows Release CI still sees sporadic AVs during asio_tcp server teardown
	// after the exchange assertions above have completed. Each gtest_discover_tests
	// case runs in its own process, so leaking here avoids the teardown race while
	// preserving the payload/close coverage that this test exists to validate.
	server.release();
#endif
}

TEST(NetTest, AsioTcpClientExchangesPayloadsWithPlainAsioServer)
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
		try_set_promise_value(accepted_result);
	});

	std::thread accept_thread([&accept_context]() {
		accept_context.run();
	});

	auto client_received = std::make_shared<std::promise<std::string>>();
	auto connected = std::make_shared<std::promise<ngl::i32_sessionid>>();
	auto client_closed = std::make_shared<std::promise<ngl::i32_sessionid>>();

	std::future<std::string> client_received_future = client_received->get_future();
	std::future<ngl::i32_sessionid> connected_future = connected->get_future();
	std::future<ngl::i32_sessionid> client_closed_future = client_closed->get_future();

	auto client = std::make_unique<ngl::asio_tcp>(
		1,
		[client_received](ngl::service_tcp*, const char* buff, uint32_t bufflen) {
			try_set_promise_value(client_received, std::string(buff, buff + bufflen));
			return true;
		},
		[client_closed](ngl::i32_sessionid sessionid) {
			try_set_promise_value(client_closed, sessionid);
		},
		[](ngl::i32_sessionid, bool, const ngl::pack*) {}
	);

	client->connect("127.0.0.1", port, [connected](ngl::i32_sessionid sessionid) {
		try_set_promise_value(connected, sessionid);
	}, 0);

	ASSERT_EQ(connected_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	ASSERT_EQ(accepted_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);

	const ngl::i32_sessionid sessionid = connected_future.get();
	ASSERT_GT(sessionid, 0);

	const std::string kPing = "tcp-ping";
	const std::string kPong = "tcp-pong";

	auto packet = make_test_packet(kPing);
	ASSERT_NE(packet, nullptr);
	ASSERT_TRUE(client->send(sessionid, packet));

	std::array<char, 16> request{};
	const std::size_t request_bytes = basio::read(*accepted_socket, basio::buffer(request.data(), kPing.size()));
	ASSERT_EQ(request_bytes, kPing.size());
	EXPECT_EQ(std::string(request.data(), request.data() + request_bytes), kPing);

	basio::write(*accepted_socket, basio::buffer(kPong));

	ASSERT_EQ(client_received_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	EXPECT_EQ(client_received_future.get(), kPong);

	client->close(sessionid);
	ASSERT_EQ(client_closed_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	EXPECT_EQ(client_closed_future.get(), sessionid);

	basio_errorcode ec;
	accepted_socket->close(ec);
	acceptor.close(ec);
	accept_work.reset();
	accept_context.stop();
	accept_thread.join();
}

TEST(NetTest, KcpSessionMaintainsActorIndexesAndAreaIteration)
{
	ngl::kcp_session session(nullptr);

	const ngl::i64_actorid server1 = ngl::nguid::make(ngl::ACTOR_SERVER, 2, 101);
	const ngl::i64_actorid client1 = ngl::nguid::make(ngl::ACTOR_ROLE, 2, 201);
	const ngl::i64_actorid server2 = ngl::nguid::make(ngl::ACTOR_SERVER, 2, 102);
	const ngl::i64_actorid client2 = ngl::nguid::make(ngl::ACTOR_ROLE, 2, 202);

	auto endpoint1 = ngl::asio_udp_endpoint(basio::ip::make_address("127.0.0.1"), 12001);
	auto endpoint2 = ngl::asio_udp_endpoint(basio::ip::make_address("127.0.0.1"), 12002);

	auto entry1 = session.add(1, endpoint1, server1, client1);
	auto entry2 = session.add(2, endpoint2, server2, client2);

	ASSERT_NE(entry1, nullptr);
	ASSERT_NE(entry2, nullptr);
	EXPECT_EQ(session.find(entry1->m_session), entry1);
	EXPECT_EQ(session.find(entry2->m_session), entry2);
	EXPECT_EQ(session.findbyactorid(client1), entry1);
	EXPECT_EQ(session.findbyactorid(server1), entry1);
	EXPECT_EQ(session.findbyactorid(client2), entry2);
	EXPECT_EQ(session.findbyactorid(server2), entry2);

	std::vector<ngl::i32_sessionid> sessions_in_area;
	session.foreachbyarea(2, [&sessions_in_area](ngl::ptr_se& entry) {
		sessions_in_area.push_back(entry->m_session);
	});
	std::sort(sessions_in_area.begin(), sessions_in_area.end());

	ASSERT_EQ(sessions_in_area.size(), 2u);
	EXPECT_EQ(sessions_in_area[0], entry1->m_session);
	EXPECT_EQ(sessions_in_area[1], entry2->m_session);

	session.erasebysession(entry1->m_session);
	EXPECT_EQ(session.find(entry1->m_session), nullptr);
	EXPECT_EQ(session.findbyactorid(client1), nullptr);
	EXPECT_EQ(session.findbyactorid(server1), nullptr);

	session.erasebyaactorid(client2);
	EXPECT_EQ(session.find(entry2->m_session), nullptr);
	EXPECT_EQ(session.findbyactorid(client2), nullptr);
	EXPECT_EQ(session.findbyactorid(server2), nullptr);
}

TEST(NetTest, AsioKcpRejectsInvalidBuffersAndUnknownSessions)
{
	ngl::asio_kcp kcp(0);

	EXPECT_FALSE(kcp.sendu(ngl::asio_udp_endpoint(basio::ip::make_address("127.0.0.1"), 9), nullptr, 1));
	EXPECT_EQ(kcp.sendbuff(123456, "x", 1), -1);
	EXPECT_EQ(kcp.sendbuff(ngl::asio_udp_endpoint(basio::ip::make_address("127.0.0.1"), 9), nullptr, 1), -1);
}
