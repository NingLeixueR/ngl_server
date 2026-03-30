// File overview: Implements GoogleTest coverage for serialize.

#include <gtest/gtest.h>

#include <array>
#include <cstring>
#include <list>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "actor/actor_base/core/actor_base.h"
#include "actor/actor_base/core/nguid.h"
#include "actor/protocol/tprotocol.h"
#include "tools/db/sql/db_buff.h"
#include "tools/serialize/nserialize.h"
#include "tools/serialize/netbuff_pool.h"
#include "tools/serialize/pack.h"
#include "tools/serialize/pack_head.h"
#include "tools/serialize/segpack.h"
#include "tools/serialize/structbytes.h"

namespace serialize_test_case
{
struct OverestimatedPayload
{
	int32_t value = 0;

	bool push_format(ngl::ser::serialize_push* aser) const
	{
		return ngl::ser::nserialize::push(aser, value);
	}

	bool pop_format(ngl::ser::serialize_pop* aser)
	{
		return ngl::ser::nserialize::pop(aser, value);
	}

	void bytes_format(ngl::ser::serialize_byte* aser) const
	{
		ngl::ser::nserialize::bytes(aser, value, value);
	}
};

void RegisterSerializeCustomTypes()
{
	static bool registered = [] {
		ngl::tprotocol::tp_customs<OverestimatedPayload>();
		return true;
	}();
	(void)registered;
}
TEST(SerializeTest, Uint32RoundTripsWithoutSignLoss)
{
	constexpr uint32_t kValue = 0xFEDCBA98u;
	std::array<char, sizeof(uint32_t)> buffer = {};

	ngl::ser::serialize_push push(buffer.data(), static_cast<int32_t>(buffer.size()));
	ASSERT_TRUE(ngl::ser::serialize_format<uint32_t>::push(&push, kValue));
	EXPECT_EQ(push.pos(), static_cast<int32_t>(buffer.size()));

	uint32_t roundtrip = 0;
	ngl::ser::serialize_pop pop(buffer.data(), static_cast<int32_t>(buffer.size()));
	ASSERT_TRUE(ngl::ser::serialize_format<uint32_t>::pop(&pop, roundtrip));
	EXPECT_EQ(roundtrip, kValue);
}

TEST(SerializeTest, BasetypeRejectsNegativeByteCount)
{
	std::array<char, sizeof(int32_t)> buffer = {};
	int32_t value = 123;

	ngl::ser::serialize_push push(buffer.data(), static_cast<int32_t>(buffer.size()));
	EXPECT_FALSE(push.basetype(&value, -1));
	EXPECT_EQ(push.pos(), 0);

	int32_t output = 0;
	ngl::ser::serialize_pop pop(buffer.data(), static_cast<int32_t>(buffer.size()));
	EXPECT_FALSE(pop.basetype(&output, -1));
	EXPECT_EQ(pop.pos(), 0);
}

TEST(SerializeTest, BasetypeRejectsNullBufferWithPositiveLength)
{
	int32_t value = 123;

	ngl::ser::serialize_push push(nullptr, sizeof(value));
	EXPECT_FALSE(push.basetype(&value, sizeof(value)));
	EXPECT_EQ(push.pos(), 0);

	int32_t output = 0;
	ngl::ser::serialize_pop pop(nullptr, sizeof(output));
	EXPECT_FALSE(pop.basetype(&output, sizeof(output)));
	EXPECT_EQ(pop.pos(), 0);
}

TEST(SerializeTest, StringPopRejectsNegativeLengthWithoutMutation)
{
	std::array<char, sizeof(int32_t)> buffer = {};

	ngl::ser::serialize_push push(buffer.data(), static_cast<int32_t>(buffer.size()));
	ASSERT_TRUE(ngl::ser::serialize_format<int32_t>::push(&push, -1));

	std::string value = "keep";
	ngl::ser::serialize_pop pop(buffer.data(), static_cast<int32_t>(buffer.size()));
	EXPECT_FALSE(ngl::ser::serialize_format<std::string>::pop(&pop, value));
	EXPECT_EQ(value, "keep");
}

TEST(SerializeTest, StringPushRejectsPayloadLargerThanLimit)
{
	std::string value(102401, 'x');
	std::vector<char> buffer(sizeof(int32_t) + 16, '\0');

	ngl::ser::serialize_push push(buffer.data(), static_cast<int32_t>(buffer.size()));
	EXPECT_FALSE(ngl::ser::serialize_format<std::string>::push(&push, value));
	EXPECT_EQ(push.pos(), 0);
}

TEST(SerializeTest, StringPushDoesNotAdvanceOnInsufficientBuffer)
{
	std::array<char, sizeof(int32_t) + 2> buffer = {};
	const std::string value = "abcd";

	ngl::ser::serialize_push push(buffer.data(), static_cast<int32_t>(buffer.size()));
	EXPECT_FALSE(ngl::ser::serialize_format<std::string>::push(&push, value));
	EXPECT_EQ(push.pos(), 0);
}

TEST(SerializeTest, VectorPopDoesNotMutateOutputOnFailure)
{
	std::array<char, sizeof(int32_t) * 2> buffer = {};

	ngl::ser::serialize_push push(buffer.data(), static_cast<int32_t>(buffer.size()));
	ASSERT_TRUE(ngl::ser::serialize_format<int32_t>::push(&push, 2));
	ASSERT_TRUE(ngl::ser::serialize_format<int32_t>::push(&push, 11));

	std::vector<int32_t> values = {
		99,
	};
	ngl::ser::serialize_pop pop(buffer.data(), push.pos());
	EXPECT_FALSE(ngl::ser::serialize_format<std::vector<int32_t>>::pop(&pop, values));
	ASSERT_EQ(values.size(), 1u);
	EXPECT_EQ(values[0], 99);
}

TEST(SerializeTest, VectorPushDoesNotAdvanceOnFailure)
{
	std::array<char, sizeof(int32_t) * 2> buffer = {};
	const std::vector<int32_t> values = {
		11,
		22,
	};

	ngl::ser::serialize_push push(buffer.data(), static_cast<int32_t>(buffer.size()));
	EXPECT_FALSE(ngl::ser::serialize_format<std::vector<int32_t>>::push(&push, values));
	EXPECT_EQ(push.pos(), 0);
}

TEST(SerializeTest, SetPopRejectsNegativeSizeWithoutMutation)
{
	std::array<char, sizeof(int32_t)> buffer = {};

	ngl::ser::serialize_push push(buffer.data(), static_cast<int32_t>(buffer.size()));
	ASSERT_TRUE(ngl::ser::serialize_format<int32_t>::push(&push, -1));

	std::set<int32_t> values = {
		42,
	};
	ngl::ser::serialize_pop pop(buffer.data(), static_cast<int32_t>(buffer.size()));
	EXPECT_FALSE(ngl::ser::serialize_format<std::set<int32_t>>::pop(&pop, values));
	ASSERT_EQ(values.size(), 1u);
	EXPECT_TRUE(values.contains(42));
}

TEST(SerializeTest, SetPopRejectsDuplicateValueWithoutMutation)
{
	std::array<char, sizeof(int32_t) * 3> buffer = {};

	ngl::ser::serialize_push push(buffer.data(), static_cast<int32_t>(buffer.size()));
	ASSERT_TRUE(ngl::ser::serialize_format<int32_t>::push(&push, 2));
	ASSERT_TRUE(ngl::ser::serialize_format<int32_t>::push(&push, 5));
	ASSERT_TRUE(ngl::ser::serialize_format<int32_t>::push(&push, 5));

	std::set<int32_t> values = {
		42,
	};
	ngl::ser::serialize_pop pop(buffer.data(), push.pos());
	EXPECT_FALSE(ngl::ser::serialize_format<std::set<int32_t>>::pop(&pop, values));
	ASSERT_EQ(values.size(), 1u);
	EXPECT_TRUE(values.contains(42));
}

TEST(SerializeTest, ListPopDoesNotMutateOutputOnFailure)
{
	std::array<char, sizeof(int32_t) * 2> buffer = {};

	ngl::ser::serialize_push push(buffer.data(), static_cast<int32_t>(buffer.size()));
	ASSERT_TRUE(ngl::ser::serialize_format<int32_t>::push(&push, 2));
	ASSERT_TRUE(ngl::ser::serialize_format<int32_t>::push(&push, 11));

	std::list<int32_t> values = {
		77,
	};
	ngl::ser::serialize_pop pop(buffer.data(), push.pos());
	EXPECT_FALSE(ngl::ser::serialize_format<std::list<int32_t>>::pop(&pop, values));
	ASSERT_EQ(values.size(), 1u);
	EXPECT_EQ(values.front(), 77);
}

TEST(SerializeTest, MapPopRejectsNegativeSizeWithoutMutation)
{
	std::array<char, sizeof(int32_t)> buffer = {};

	ngl::ser::serialize_push push(buffer.data(), static_cast<int32_t>(buffer.size()));
	ASSERT_TRUE(ngl::ser::serialize_format<int32_t>::push(&push, -1));

	std::map<int32_t, int32_t> values = {
		{ 7, 8 },
	};
	ngl::ser::serialize_pop pop(buffer.data(), static_cast<int32_t>(buffer.size()));
	EXPECT_FALSE((ngl::ser::serialize_format<std::map<int32_t, int32_t>>::pop(&pop, values)));
	ASSERT_EQ(values.size(), 1u);
	EXPECT_EQ(values.at(7), 8);
}

TEST(SerializeTest, MapPopRejectsDuplicateKeyWithoutMutation)
{
	std::array<char, sizeof(int32_t) * 5> buffer = {};

	ngl::ser::serialize_push push(buffer.data(), static_cast<int32_t>(buffer.size()));
	ASSERT_TRUE(ngl::ser::serialize_format<int32_t>::push(&push, 2));
	ASSERT_TRUE(ngl::ser::serialize_format<int32_t>::push(&push, 7));
	ASSERT_TRUE(ngl::ser::serialize_format<int32_t>::push(&push, 8));
	ASSERT_TRUE(ngl::ser::serialize_format<int32_t>::push(&push, 7));
	ASSERT_TRUE(ngl::ser::serialize_format<int32_t>::push(&push, 9));

	std::map<int32_t, int32_t> values = {
		{ 1, 2 },
	};
	ngl::ser::serialize_pop pop(buffer.data(), push.pos());
	EXPECT_FALSE((ngl::ser::serialize_format<std::map<int32_t, int32_t>>::pop(&pop, values)));
	ASSERT_EQ(values.size(), 1u);
	EXPECT_EQ(values.at(1), 2);
}

TEST(SerializeTest, ProtobufPopRejectsNegativeLengthWithoutMutation)
{
	std::array<char, sizeof(int32_t)> buffer = {};

	ngl::ser::serialize_push push(buffer.data(), static_cast<int32_t>(buffer.size()));
	ASSERT_TRUE(ngl::ser::serialize_format<int32_t>::push(&push, -1));

	pbnet::PROBUFF_NET_ACOUNT_LOGIN value;
	value.set_maccount("keep");
	value.set_mpassword("secret");
	value.set_marea(12);

	ngl::ser::serialize_pop pop(buffer.data(), static_cast<int32_t>(buffer.size()));
	EXPECT_FALSE(ngl::ser::serialize_format<pbnet::PROBUFF_NET_ACOUNT_LOGIN>::pop(&pop, value));
	EXPECT_EQ(value.maccount(), "keep");
	EXPECT_EQ(value.mpassword(), "secret");
	EXPECT_EQ(value.marea(), 12);
}

TEST(SerializeTest, ProtobufPushDoesNotAdvanceOnInsufficientBuffer)
{
	std::array<char, sizeof(int32_t) + 2> buffer = {};

	pbnet::PROBUFF_NET_ACOUNT_LOGIN value;
	value.set_maccount("keep");
	value.set_mpassword("secret");
	value.set_marea(12);

	ngl::ser::serialize_push push(buffer.data(), static_cast<int32_t>(buffer.size()));
	EXPECT_FALSE(ngl::ser::serialize_format<pbnet::PROBUFF_NET_ACOUNT_LOGIN>::push(&push, value));
	EXPECT_EQ(push.pos(), 0);
}

TEST(SerializeTest, VariadicPushRollsBackWhenLaterFieldFails)
{
	std::array<char, sizeof(int32_t) + 2> buffer = {};
	const int32_t head = 7;
	const std::string tail = "abcd";

	ngl::ser::serialize_push push(buffer.data(), static_cast<int32_t>(buffer.size()));
	EXPECT_FALSE(ngl::ser::nserialize::push(&push, head, tail));
	EXPECT_EQ(push.pos(), 0);
}

TEST(SerializeTest, NetbuffPoolZeroLengthReturnsNull)
{
	EXPECT_EQ(ngl::netbuff_pool::malloc(0), nullptr);
	ngl::netbuff_pool::free(nullptr);
}

TEST(SerializeTest, PackResetClearsAllocatedState)
{
	ngl::pack packet;
	ASSERT_TRUE(packet.malloc(32));
	ASSERT_NE(packet.m_buff, nullptr);

	packet.m_protocol = ngl::ENET_KCP;
	packet.m_id = 99;
	packet.m_pos = 12;
	packet.m_rate_accounted = true;
	packet.m_head->m_data[ngl::EPH_BYTES] = 7;

	packet.reset();

	EXPECT_EQ(packet.m_protocol, ngl::ENET_TCP);
	EXPECT_EQ(packet.m_id, 0);
	EXPECT_EQ(packet.m_buff, nullptr);
	EXPECT_EQ(packet.m_len, 0);
	EXPECT_EQ(packet.m_pos, 0);
	EXPECT_FALSE(packet.m_rate_accounted);
	EXPECT_EQ(packet.m_head.getvalue(ngl::EPH_BYTES), 0);
}

TEST(SerializeTest, PackSetActorSkipsShortBuffer)
{
	auto packet = ngl::pack::make_pack(nullptr, ngl::pack_head::size() - 1);
	ASSERT_NE(packet, nullptr);
	ASSERT_NE(packet->m_buff, nullptr);

	memset(packet->m_buff, 0x5A, static_cast<size_t>(packet->m_len));
	std::vector<char> original(packet->m_buff, packet->m_buff + packet->m_len);

	packet->set_actor(ngl::nguid::make(ngl::ACTOR_ROLE, 2, 11), ngl::nguid::make(ngl::ACTOR_GATEWAY, 3, 22));

	ASSERT_EQ(original.size(), static_cast<size_t>(packet->m_len));
	EXPECT_EQ(std::memcmp(packet->m_buff, original.data(), original.size()), 0);
}

TEST(SerializeTest, PackHeadRoundTripsActorIdsWithoutAliasCasts)
{
	const ngl::i64_actorid actor = ngl::nguid::make(ngl::ACTOR_ROLE, 2, 11);
	const ngl::i64_actorid request_actor = ngl::nguid::make(ngl::ACTOR_GATEWAY, 3, 22);

	ngl::pack_head head;
	head.set_actor(actor, request_actor);

	EXPECT_EQ(head.get_actor(), actor);
	EXPECT_EQ(head.get_request_actor(), request_actor);
	EXPECT_EQ(head.get_actortype(), ngl::ACTOR_ROLE);
	EXPECT_EQ(head.get_actorarea(), 2);
	EXPECT_EQ(head.get_actordataid(), 11);
	EXPECT_EQ(head.get_request_actortype(), ngl::ACTOR_GATEWAY);
}

TEST(SerializeTest, PackHeadReservebuffRejectsBufferSmallerThanHeader)
{
	ngl::pack_head head;
	std::array<char, 8> small = {};
	std::pair<char*, int32_t> window = { reinterpret_cast<char*>(1), 7 };

	head.reservebuff(small.data(), static_cast<int32_t>(small.size()), window);
	EXPECT_EQ(window.first, nullptr);
	EXPECT_EQ(window.second, 0);

	std::vector<char> exact(static_cast<size_t>(ngl::pack_head::size()), '\0');
	head.reservebuff(exact.data(), static_cast<int32_t>(exact.size()), window);
	EXPECT_EQ(window.first, exact.data() + ngl::pack_head::size());
	EXPECT_EQ(window.second, 0);
}

TEST(SerializeTest, StructbytesRejectsPositiveLengthWithoutBuffer)
{
	RegisterSerializeCustomTypes();

	auto packet = ngl::pack::make_pack(nullptr, 0);
	ASSERT_NE(packet, nullptr);
	packet->m_head.m_data[ngl::EPH_BYTES] = sizeof(int32_t);
	packet->m_pos = sizeof(int32_t);

	OverestimatedPayload payload;
	EXPECT_FALSE((ngl::structbytes<OverestimatedPayload>::tostruct(packet, payload)));
}

TEST(SerializeTest, StructbytesUsesActualSerializedLength)
{
	RegisterSerializeCustomTypes();

	OverestimatedPayload payload;
	payload.value = 12345;

	auto packet = ngl::pack::make_pack(nullptr, ngl::pack_head::size() + static_cast<int32_t>(sizeof(int32_t) * 2));
	ASSERT_NE(packet, nullptr);
	ASSERT_NE(packet->m_buff, nullptr);

	ASSERT_TRUE((ngl::structbytes<OverestimatedPayload>::tobytes(packet, payload, 101, 202)));
	EXPECT_EQ(packet->m_head.getvalue(ngl::EPH_BYTES), static_cast<int32_t>(sizeof(int32_t)));
	EXPECT_EQ(packet->m_len, ngl::pack_head::size() + static_cast<int32_t>(sizeof(int32_t)));
	EXPECT_EQ(packet->m_pos, packet->m_len);

	int32_t roundtrip = 0;
	ngl::ser::serialize_pop pop(packet->m_buff + ngl::pack_head::size(), packet->m_head.getvalue(ngl::EPH_BYTES));
	ASSERT_TRUE(ngl::ser::serialize_format<int32_t>::pop(&pop, roundtrip));
	EXPECT_EQ(roundtrip, payload.value);
}

TEST(SerializeTest, StructbytesRejectsBufferSmallerThanHeader)
{
	RegisterSerializeCustomTypes();

	OverestimatedPayload payload;
	payload.value = 12345;

	auto packet = ngl::pack::make_pack(nullptr, ngl::pack_head::size() - 1);
	ASSERT_NE(packet, nullptr);
	ASSERT_NE(packet->m_buff, nullptr);

	EXPECT_FALSE((ngl::structbytes<OverestimatedPayload>::tobytes(packet, payload, 101, 202)));
}

TEST(SerializeTest, SegpackRejectsNullBufferWhenLengthIsPositive)
{
	ngl::segpack parser;

	EXPECT_FALSE(parser.push(1001, nullptr, 1, false));
	EXPECT_TRUE(parser.push(1001, nullptr, 0, false));
}

TEST(SerializeTest, DbBuffBinaryUnserializeRejectsOversizedLength)
{
	ngl::db_buff buffer;
	int32_t value = 0;
	const std::array<char, sizeof(int32_t)> data = {};

	EXPECT_FALSE(buffer.unserialize(true, value, data.data(), static_cast<std::size_t>(std::numeric_limits<int32_t>::max()) + 1u));
}

} // namespace serialize_test_case
