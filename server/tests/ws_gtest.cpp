// File overview: Implements GoogleTest coverage for ws.

#include <gtest/gtest.h>

#include <openssl/buffer.h>
#include <openssl/bio.h>
#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>

#include <array>
#include <atomic>
#include <chrono>
#include <cstring>
#include <future>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <string>

#include "net/tcp/ws/asio_ws.h"
#include "test_support.h"

namespace ws_test_case
{
void trace_step(const char* alabel)
{
	if (alabel == nullptr)
	{
		return;
	}
	ngl_test_support::trace(std::string("[ws-test] ") + alabel);
}

std::string bio_to_string(BIO* abio)
{
	BUF_MEM* lbuffer = nullptr;
	BIO_get_mem_ptr(abio, &lbuffer);
	if (lbuffer == nullptr || lbuffer->data == nullptr || lbuffer->length == 0)
	{
		return {};
	}
	return std::string(lbuffer->data, lbuffer->length);
}

struct tls_identity
{
	std::string m_certificate_chain;
	std::string m_private_key;
};

tls_identity make_tls_identity()
{
	std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> lpkey(nullptr, &EVP_PKEY_free);
	std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> lctx(EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr), &EVP_PKEY_CTX_free);
	std::unique_ptr<X509, decltype(&X509_free)> lcert(X509_new(), &X509_free);
	std::unique_ptr<BIO, decltype(&BIO_free)> lcert_bio(BIO_new(BIO_s_mem()), &BIO_free);
	std::unique_ptr<BIO, decltype(&BIO_free)> lkey_bio(BIO_new(BIO_s_mem()), &BIO_free);

	if (!lctx || !lcert || !lcert_bio || !lkey_bio)
	{
		throw std::runtime_error("failed to allocate OpenSSL objects");
	}
	if (EVP_PKEY_keygen_init(lctx.get()) != 1)
	{
		throw std::runtime_error("failed to initialize RSA key generation");
	}
	if (EVP_PKEY_CTX_set_rsa_keygen_bits(lctx.get(), 2048) != 1)
	{
		throw std::runtime_error("failed to set RSA key size");
	}
	EVP_PKEY* lraw_key = nullptr;
	if (EVP_PKEY_keygen(lctx.get(), &lraw_key) != 1)
	{
		throw std::runtime_error("failed to generate RSA key");
	}
	lpkey.reset(lraw_key);

	if (X509_set_version(lcert.get(), 2) != 1)
	{
		throw std::runtime_error("failed to set certificate version");
	}
	if (ASN1_INTEGER_set(X509_get_serialNumber(lcert.get()), 1) != 1)
	{
		throw std::runtime_error("failed to set certificate serial");
	}
	if (X509_gmtime_adj(X509_get_notBefore(lcert.get()), 0) == nullptr)
	{
		throw std::runtime_error("failed to set certificate notBefore");
	}
	if (X509_gmtime_adj(X509_get_notAfter(lcert.get()), 60 * 60) == nullptr)
	{
		throw std::runtime_error("failed to set certificate notAfter");
	}
	if (X509_set_pubkey(lcert.get(), lpkey.get()) != 1)
	{
		throw std::runtime_error("failed to attach certificate public key");
	}

	X509_NAME* lname = X509_get_subject_name(lcert.get());
	const unsigned char kCommonName[] = "localhost";
	if (X509_NAME_add_entry_by_txt(lname, "CN", MBSTRING_ASC, kCommonName, -1, -1, 0) != 1)
	{
		throw std::runtime_error("failed to populate certificate subject");
	}
	if (X509_set_issuer_name(lcert.get(), lname) != 1)
	{
		throw std::runtime_error("failed to set certificate issuer");
	}
	if (X509_sign(lcert.get(), lpkey.get(), EVP_sha256()) == 0)
	{
		throw std::runtime_error("failed to sign certificate");
	}

	if (PEM_write_bio_X509(lcert_bio.get(), lcert.get()) != 1)
	{
		throw std::runtime_error("failed to encode certificate");
	}
	if (PEM_write_bio_PrivateKey(lkey_bio.get(), lpkey.get(), nullptr, nullptr, 0, nullptr, nullptr) != 1)
	{
		throw std::runtime_error("failed to encode private key");
	}

	return {
		.m_certificate_chain = bio_to_string(lcert_bio.get()),
		.m_private_key = bio_to_string(lkey_bio.get()),
	};
}
TEST(WsTest, AsioWsServerAndClientExchangeTextFrames)
{
	trace_step("text: begin");
	auto server_received = std::make_shared<std::promise<std::string>>();
	auto server_text = std::make_shared<std::promise<bool>>();
	auto server_session = std::make_shared<std::promise<ngl::i32_sessionid>>();
	auto client_received = std::make_shared<std::promise<std::string>>();
	auto client_text = std::make_shared<std::promise<bool>>();
	auto connected = std::make_shared<std::promise<ngl::i32_sessionid>>();
	auto server_closed = std::make_shared<std::promise<ngl::i32_sessionid>>();
	auto client_closed = std::make_shared<std::promise<ngl::i32_sessionid>>();

	std::future<std::string> server_received_future = server_received->get_future();
	std::future<bool> server_text_future = server_text->get_future();
	std::future<ngl::i32_sessionid> server_session_future = server_session->get_future();
	std::future<std::string> client_received_future = client_received->get_future();
	std::future<bool> client_text_future = client_text->get_future();
	std::future<ngl::i32_sessionid> connected_future = connected->get_future();
	std::future<ngl::i32_sessionid> server_closed_future = server_closed->get_future();
	std::future<ngl::i32_sessionid> client_closed_future = client_closed->get_future();

	std::unique_ptr<ngl::asio_ws> server;
	server = std::make_unique<ngl::asio_ws>(
		0,
		1,
		false,
		[server_received, server_text, server_session](ngl::service_ws* asession, const char* abuff, uint32_t alen) {
			ngl_test_support::try_set(server_received, std::string(abuff, abuff + alen));
			ngl_test_support::try_set(server_text, asession->message_is_text());
			ngl_test_support::try_set(server_session, asession->m_sessionid);
			return true;
		},
		[server_closed](ngl::i32_sessionid asessionid) {
			ngl_test_support::try_set(server_closed, asessionid);
		},
		[](ngl::i32_sessionid, bool, const ngl::pack*) {
			// This text-frame exchange only asserts message delivery and close events.
		}
	);

	std::unique_ptr<ngl::asio_ws> client;
	client = std::make_unique<ngl::asio_ws>(
		1,
		false,
		[client_received, client_text](ngl::service_ws* asession, const char* abuff, uint32_t alen) {
			ngl_test_support::try_set(client_received, std::string(abuff, abuff + alen));
			ngl_test_support::try_set(client_text, asession->message_is_text());
			return true;
		},
		[client_closed](ngl::i32_sessionid asessionid) {
			ngl_test_support::try_set(client_closed, asessionid);
		},
		[](ngl::i32_sessionid, bool, const ngl::pack*) {
			// This text-frame exchange only asserts message delivery and close events.
		}
	);

	client->connect("127.0.0.1", server->port(), "/", [connected](ngl::i32_sessionid asession) {
		ngl_test_support::try_set(connected, asession);
		ngl_test_support::trace("ws client success!!!");
	});

	trace_step("text: wait connect");
	ASSERT_EQ(connected_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	const ngl::i32_sessionid sessionid = connected_future.get();
	ASSERT_GT(sessionid, 0);
	ASSERT_TRUE(client->send_msg(sessionid, "ping"));

	trace_step("text: wait recv");
	ASSERT_EQ(server_received_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	ASSERT_EQ(server_text_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	ASSERT_EQ(server_session_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	EXPECT_EQ(server_received_future.get(), "ping");
	EXPECT_TRUE(server_text_future.get());
	const ngl::i32_sessionid server_sessionid = server_session_future.get();
	ASSERT_GT(server_sessionid, 0);
	ASSERT_TRUE(server->send_msg(server_sessionid, "pong"));
	ASSERT_EQ(client_received_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	ASSERT_EQ(client_text_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	EXPECT_EQ(client_received_future.get(), "pong");
	EXPECT_TRUE(client_text_future.get());

	std::pair<ngl::str_ip, ngl::i16_port> endpoint;
	EXPECT_TRUE(client->get_ipport(sessionid, endpoint));
	EXPECT_EQ(endpoint.first, "127.0.0.1");

	trace_step("text: close");
	client->close(sessionid);
	trace_step("text: wait close");
	ASSERT_EQ(client_closed_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	ASSERT_EQ(client_closed_future.get(), sessionid);
	ASSERT_EQ(server_closed_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	ASSERT_GT(server_closed_future.get(), 0);
	trace_step("text: end");
	trace_step("text: reset client");
	client.reset();
	trace_step("text: reset server");
	server.reset();
	trace_step("text: cleanup done");
}

TEST(WsTest, AsioWsSendsPackPayloadAsBinaryFrame)
{
	trace_step("binary: begin");
	auto server_received = std::make_shared<std::promise<std::string>>();
	auto connected = std::make_shared<std::promise<ngl::i32_sessionid>>();
	auto server_closed = std::make_shared<std::promise<ngl::i32_sessionid>>();
	auto client_closed = std::make_shared<std::promise<ngl::i32_sessionid>>();

	std::future<std::string> server_received_future = server_received->get_future();
	std::future<ngl::i32_sessionid> connected_future = connected->get_future();
	std::future<ngl::i32_sessionid> server_closed_future = server_closed->get_future();
	std::future<ngl::i32_sessionid> client_closed_future = client_closed->get_future();

	ngl::asio_ws server(
		0,
		1,
		false,
		[server_received](ngl::service_ws* asession, const char* abuff, uint32_t alen) {
			EXPECT_FALSE(asession->message_is_text());
			ngl_test_support::try_set(server_received, std::string(abuff, abuff + alen));
			return true;
		},
		[server_closed](ngl::i32_sessionid asessionid) {
			ngl_test_support::try_set(server_closed, asessionid);
		},
		[](ngl::i32_sessionid, bool, const ngl::pack*) {
			// This binary-frame exchange only asserts payload transport.
		}
	);

	ngl::asio_ws client(
		1,
		false,
		[](ngl::service_ws*, const char*, uint32_t) {
			return true;
		},
		[client_closed](ngl::i32_sessionid asessionid) {
			ngl_test_support::try_set(client_closed, asessionid);
		},
		[](ngl::i32_sessionid, bool, const ngl::pack*) {
			// This binary-frame exchange only asserts payload transport.
		}
	);

	client.connect("127.0.0.1", server.port(), "/", [connected](ngl::i32_sessionid asession) {
		ngl_test_support::try_set(connected, asession);
	});

	trace_step("binary: wait connect");
	ASSERT_EQ(connected_future.wait_for(std::chrono::seconds(10)), std::future_status::ready);
	const ngl::i32_sessionid sessionid = connected_future.get();
	ASSERT_GT(sessionid, 0);

	auto packet = std::make_shared<ngl::pack>();
	ASSERT_TRUE(packet->malloc(5));
	std::memcpy(packet->m_buff, "hello", 5);
	packet->m_len = 5;
	packet->m_pos = 5;

	ASSERT_TRUE(client.send(sessionid, packet));
	trace_step("binary: wait recv");
	ASSERT_EQ(server_received_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	EXPECT_EQ(server_received_future.get(), "hello");
	EXPECT_EQ(packet->m_protocol, ngl::ENET_WS);

	trace_step("binary: close");
	client.close(sessionid);
	trace_step("binary: wait close");
	ASSERT_EQ(client_closed_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	ASSERT_EQ(client_closed_future.get(), sessionid);
	ASSERT_EQ(server_closed_future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
	ASSERT_GT(server_closed_future.get(), 0);
	trace_step("binary: end");
}

TEST(WsTest, AsioWssServerAndClientExchangeTextFrames)
{
	trace_step("wss: begin");
	const tls_identity identity = make_tls_identity();
	ngl::ws_tls_options server_tls{
		.m_certificate_chain = identity.m_certificate_chain,
		.m_private_key = identity.m_private_key,
	};
	ngl::ws_tls_options client_tls{
		.m_verify_peer = false,
	};

	auto server_received = std::make_shared<std::promise<std::string>>();
	auto server_text = std::make_shared<std::promise<bool>>();
	auto server_session = std::make_shared<std::promise<ngl::i32_sessionid>>();
	auto client_received = std::make_shared<std::promise<std::string>>();
	auto client_text = std::make_shared<std::promise<bool>>();
	auto connected = std::make_shared<std::promise<ngl::i32_sessionid>>();
	auto server_closed = std::make_shared<std::promise<ngl::i32_sessionid>>();
	auto client_closed = std::make_shared<std::promise<ngl::i32_sessionid>>();

	std::future<std::string> server_received_future = server_received->get_future();
	std::future<bool> server_text_future = server_text->get_future();
	std::future<ngl::i32_sessionid> server_session_future = server_session->get_future();
	std::future<std::string> client_received_future = client_received->get_future();
	std::future<bool> client_text_future = client_text->get_future();
	std::future<ngl::i32_sessionid> connected_future = connected->get_future();
	std::future<ngl::i32_sessionid> server_closed_future = server_closed->get_future();
	std::future<ngl::i32_sessionid> client_closed_future = client_closed->get_future();

	std::unique_ptr<ngl::asio_ws> server;
	server = std::make_unique<ngl::asio_ws>(
		0,
		1,
		true,
		[server_received, server_text, server_session](ngl::service_ws* asession, const char* abuff, uint32_t alen) {
			ngl_test_support::try_set(server_received, std::string(abuff, abuff + alen));
			ngl_test_support::try_set(server_text, asession->message_is_text());
			ngl_test_support::try_set(server_session, asession->m_sessionid);
			return true;
		},
		[server_closed](ngl::i32_sessionid asessionid) {
			ngl_test_support::try_set(server_closed, asessionid);
		},
		[](ngl::i32_sessionid, bool, const ngl::pack*) {
			// This secure text-frame exchange only asserts message delivery and close events.
		},
		server_tls
	);

	std::unique_ptr<ngl::asio_ws> client;
	client = std::make_unique<ngl::asio_ws>(
		1,
		true,
		[client_received, client_text](ngl::service_ws* asession, const char* abuff, uint32_t alen) {
			ngl_test_support::try_set(client_received, std::string(abuff, abuff + alen));
			ngl_test_support::try_set(client_text, asession->message_is_text());
			return true;
		},
		[client_closed](ngl::i32_sessionid asessionid) {
			ngl_test_support::try_set(client_closed, asessionid);
		},
		[](ngl::i32_sessionid, bool, const ngl::pack*) {
			// This secure text-frame exchange only asserts message delivery and close events.
		},
		client_tls
	);

	client->connect("127.0.0.1", server->port(), "/", [connected](ngl::i32_sessionid asession) {
		ngl_test_support::try_set(connected, asession);
	});

	trace_step("wss: wait connect");
	ASSERT_EQ(connected_future.wait_for(std::chrono::seconds(10)), std::future_status::ready);
	const ngl::i32_sessionid sessionid = connected_future.get();
	ASSERT_GT(sessionid, 0);
	ASSERT_TRUE(client->send_msg(sessionid, "secure-ping"));

	trace_step("wss: wait recv");
	ASSERT_EQ(server_received_future.wait_for(std::chrono::seconds(5)), std::future_status::ready);
	ASSERT_EQ(server_text_future.wait_for(std::chrono::seconds(5)), std::future_status::ready);
	ASSERT_EQ(server_session_future.wait_for(std::chrono::seconds(5)), std::future_status::ready);
	EXPECT_EQ(server_received_future.get(), "secure-ping");
	EXPECT_TRUE(server_text_future.get());
	const ngl::i32_sessionid server_sessionid = server_session_future.get();
	ASSERT_GT(server_sessionid, 0);
	ASSERT_TRUE(server->send_msg(server_sessionid, "secure-pong"));

	ASSERT_EQ(client_received_future.wait_for(std::chrono::seconds(5)), std::future_status::ready);
	ASSERT_EQ(client_text_future.wait_for(std::chrono::seconds(5)), std::future_status::ready);
	EXPECT_EQ(client_received_future.get(), "secure-pong");
	EXPECT_TRUE(client_text_future.get());

	std::pair<ngl::str_ip, ngl::i16_port> endpoint;
	EXPECT_TRUE(client->get_ipport(sessionid, endpoint));
	EXPECT_EQ(endpoint.first, "127.0.0.1");

	trace_step("wss: close");
	client->close(sessionid);
	trace_step("wss: wait close");
	ASSERT_EQ(client_closed_future.wait_for(std::chrono::seconds(5)), std::future_status::ready);
	ASSERT_EQ(client_closed_future.get(), sessionid);
	ASSERT_EQ(server_closed_future.wait_for(std::chrono::seconds(5)), std::future_status::ready);
	ASSERT_GT(server_closed_future.get(), 0);
	trace_step("wss: end");
	trace_step("wss: reset client");
	client.reset();
	trace_step("wss: reset server");
	server.reset();
	trace_step("wss: cleanup done");
}

} // namespace ws_test_case
