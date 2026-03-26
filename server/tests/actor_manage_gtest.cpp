// File overview: Implements GoogleTest coverage for actor manage.

#include <gtest/gtest.h>

#include <algorithm>
#include <atomic>
#include <vector>

#include "actor/actor_base/core/actor_base.h"
#include "actor/actor_base/core/actor_manage.h"
#include "actor/protocol/nprotocol.h"

namespace actor_manage_test_case
{
constexpr ngl::ENUM_ACTOR kEphemeralActorType = ngl::ACTOR_EXAMPLE_GUESS_NUMBER;

class TestManagedActor final : public ngl::actor_base
{
public:
	TestManagedActor()
		: ngl::actor_base(make_param())
	{
	}

	ngl::actor_stat activity_stat() override
	{
		return m_stat;
	}

	void set_activity_stat(ngl::actor_stat astat) override
	{
		m_stat = astat;
	}

	bool list_empty() override
	{
		return true;
	}

	void actor_handle(ngl::i32_threadid) override
	{
		// Test double: scheduling behavior is validated elsewhere in these tests.
	}

	void push(ngl::handle_pram&) override
	{
		// Test double: no queueing is needed for the metadata-only assertions below.
	}

	void release() override
	{
		++m_release_count;
	}

	int release_count() const
	{
		return m_release_count;
	}

private:
	static ngl::actorparmbase make_param()
	{
		ngl::actorparmbase parm;
		parm.m_type = kEphemeralActorType;
		parm.m_area = 777;
		parm.m_id = next_id();
		return parm;
	}

	static ngl::i32_actordataid next_id()
	{
		static std::atomic<int32_t> next{ 1 };
		return next.fetch_add(1);
	}

	ngl::actor_stat m_stat = ngl::actor_stat_init;
	int m_release_count = 0;
};

bool contains_type(const std::vector<ngl::i16_actortype>& types, ngl::ENUM_ACTOR type)
{
	return std::find(types.begin(), types.end(), static_cast<ngl::i16_actortype>(type)) != types.end();
}

bool contains_actor_name(const ngl::msg_actor_stat& data, ngl::ENUM_ACTOR type)
{
	const char* expected = ngl::em<ngl::ENUM_ACTOR>::name(type);
	return std::any_of(data.m_vec.begin(), data.m_vec.end(), [expected](const ngl::msg_actor& item) {
		return item.m_actor_name == expected;
	});
}
TEST(ActorManageTest, EraseActorRemovesLastTypeFromIndex)
{
	ngl::actor_manage& manage = ngl::actor_manage::instance();
	std::vector<ngl::i16_actortype> before;
	manage.get_type(before);
	ASSERT_FALSE(contains_type(before, kEphemeralActorType));

	TestManagedActor actor;
	ASSERT_TRUE(manage.add_actor(&actor, nullptr));

	std::vector<ngl::i16_actortype> during;
	manage.get_type(during);
	EXPECT_TRUE(contains_type(during, kEphemeralActorType));

	manage.erase_actor(actor.guid());

	std::vector<ngl::i16_actortype> after;
	manage.get_type(after);
	EXPECT_FALSE(contains_type(after, kEphemeralActorType));
}

TEST(ActorManageTest, PushTaskTypeWithoutActorsDoesNotCreateGhostStats)
{
	ngl::actor_manage& manage = ngl::actor_manage::instance();
	ngl::msg_actor_stat before;
	manage.get_actor_stat(before);
	ASSERT_FALSE(contains_actor_name(before, kEphemeralActorType));

	ngl::handle_pram apram;
	apram.m_actor = ngl::nguid::make(kEphemeralActorType);
	apram.m_issend = false;
	manage.push_task_type(kEphemeralActorType, apram);

	ngl::msg_actor_stat after;
	manage.get_actor_stat(after);
	EXPECT_FALSE(contains_actor_name(after, kEphemeralActorType));
}

TEST(ActorManageTest, GetTypeOverwritesCallerBuffer)
{
	ngl::actor_manage& manage = ngl::actor_manage::instance();

	std::vector<ngl::i16_actortype> types = {
		static_cast<ngl::i16_actortype>(-32768),
	};
	manage.get_type(types);

	EXPECT_FALSE(std::find(types.begin(), types.end(), static_cast<ngl::i16_actortype>(-32768)) != types.end());
}

TEST(ActorManageTest, GetActorStatOverwritesCallerBuffer)
{
	ngl::actor_manage& manage = ngl::actor_manage::instance();

	ngl::msg_actor_stat stat;
	ngl::msg_actor stale;
	stale.m_actor_name = "stale";
	stat.m_vec.push_back(std::move(stale));

	manage.get_actor_stat(stat);

	EXPECT_FALSE(std::any_of(stat.m_vec.begin(), stat.m_vec.end(), [](const ngl::msg_actor& item) {
		return item.m_actor_name == "stale";
	}));
}

TEST(ActorManageTest, NthreadShutdownIsIdempotentWhenIdle)
{
	auto worker = std::make_shared<ngl::nthread>(123);
	ASSERT_NE(worker, nullptr);
	EXPECT_FALSE(worker->isactivity());

	EXPECT_NO_THROW(worker->shutdown());
	EXPECT_NO_THROW(worker->shutdown());
	EXPECT_FALSE(worker->isactivity());
}

} // namespace actor_manage_test_case
