// File overview: Implements GoogleTest coverage for actor role logic branches.

#include <gtest/gtest.h>

#include <atomic>
#include <format>
#include <memory>
#include <string>

#include "actor/actor_logic/actor_role/actor_role.h"

namespace actor_role_test
{
namespace detail
{
ngl::i32_actordataid next_roleid()
{
	static std::atomic<int32_t> g_next{ 1000 };
	return g_next.fetch_add(1);
}

std::unique_ptr<ngl::actor_role> make_role()
{
	ngl::np_actorswitch_process_role lparm;
	lparm.m_gatewayid = 321;

	auto lrole = std::make_unique<ngl::actor_role>(ngl::tab_self_area, next_roleid(), &lparm);
	lrole->init();
	return lrole;
}

pbdb::brief_base* init_brief(ngl::actor_role& arole, const char* aname)
{
	auto& lwrite = ngl::tdb_brief::nsp_cwrite<ngl::actor_role>::instance(arole.id_guid());
	pbdb::db_brief* lbrief = lwrite.add(arole.id_guid());
	if (lbrief == nullptr)
	{
		return nullptr;
	}

	pbdb::brief_base* lbase = lbrief->mutable_mbase();
	if (lbase == nullptr)
	{
		return nullptr;
	}

	lbase->set_mname(aname);
	lbase->set_mlv(1);
	lbase->set_mvip(0);
	lbase->set_mmoneygold(0);
	lbase->set_mmoneysilver(0);
	lbase->set_mnotalkutc(0);
	return lbase;
}

void add_rechg(ngl::actor_role& arole, int32_t aid)
{
	pbdb::db_role* ldb = arole.m_info.get().get();
	ASSERT_NE(ldb, nullptr);

	pbdb::db_role_recharge* lrechg = ldb->add_mrecharge();
	ASSERT_NE(lrechg, nullptr);
	lrechg->set_mrechargeid(aid);
}

} // namespace detail

TEST(ActorRoleTest, ChatForwardTypeHonorsBanAndChannel)
{
	auto lrole = detail::make_role();
	ASSERT_NE(lrole, nullptr);

	pbdb::brief_base* lbase = detail::init_brief(*lrole, "chat_role");
	ASSERT_NE(lbase, nullptr);

	pbnet::PROBUFF_NET_CHAT lchat;
	lchat.set_mchannelid(pbnet::ENUM_CHAT_CHANNEL::ENUM_CHAT_CHANNEL_ORDINARY);

	lbase->set_mnotalkutc(static_cast<int32_t>(ngl::tools::time::gettime()) + 60);
	EXPECT_EQ(lrole->forward_type(lchat), ngl::ecross_none);

	lbase->set_mnotalkutc(0);
	EXPECT_EQ(lrole->forward_type(lchat), ngl::ecross_ordinary);

	lchat.set_mchannelid(pbnet::ENUM_CHAT_CHANNEL::ENUM_CHAT_CHANNEL_CROSS_ORDINARY);
	EXPECT_EQ(lrole->forward_type(lchat), ngl::ecross_cross_ordinary);

	lchat.set_mchannelid(pbnet::ENUM_CHAT_CHANNEL::ENUM_CHAT_CHANNEL_NONE);
	EXPECT_EQ(lrole->forward_type(lchat), ngl::ecross_none);
}

TEST(ActorRoleTest, RanklistForwardTypeTracksCrossFlag)
{
	auto lrole = detail::make_role();
	ASSERT_NE(lrole, nullptr);

	pbnet::PROBUFF_NET_RANKLIST lrank;
	lrank.set_miscross(false);
	EXPECT_EQ(lrole->forward_type(lrank), ngl::ecross_ordinary);

	lrank.set_miscross(true);
	EXPECT_EQ(lrole->forward_type(lrank), ngl::ecross_cross_ordinary);
}

TEST(ActorRoleTest, ExampleForwardTypeTracksStateAndCrossFlag)
{
	auto lrole = detail::make_role();
	ASSERT_NE(lrole, nullptr);

	pbexample::PROBUFF_EXAMPLE_PLAY_JOIN ljoin;
	ljoin.set_mcross(pbexample::ECROSS::ECROSS_ORDINARY);
	EXPECT_EQ(lrole->forward_type(ljoin), ngl::ecross_ordinary);

	ljoin.set_mcross(pbexample::ECROSS::ECROSS_CROSS_ORDINARY);
	EXPECT_EQ(lrole->forward_type(ljoin), ngl::ecross_cross_ordinary);

	lrole->m_example.second = ngl::nguid::make(ngl::ACTOR_EXAMPLE_GUESS_NUMBER, ngl::tab_self_area, 77);
	EXPECT_EQ(lrole->forward_type(ljoin), ngl::ecross_none);

	pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE lenter;
	lenter.set_mcross(pbexample::ECROSS::ECROSS_ORDINARY);
	EXPECT_EQ(lrole->forward_type(lenter), ngl::ecross_ordinary);

	lenter.set_mcross(pbexample::ECROSS::ECROSS_CROSS_ORDINARY);
	EXPECT_EQ(lrole->forward_type(lenter), ngl::ecross_cross_ordinary);

	pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER lguess;
	EXPECT_EQ(lrole->forward_type(lguess), ngl::ecross_cross_example);
}

TEST(ActorRoleTest, ForwardGuidSelectsTargetAreaOrExample)
{
	auto lrole = detail::make_role();
	ASSERT_NE(lrole, nullptr);

	ngl::nguid lguid;
	int32_t ldata = 0;

	EXPECT_TRUE(lrole->forward_guid(ngl::ACTOR_CHAT, ngl::ecross_ordinary, lguid, ldata));
	EXPECT_EQ(lguid.type(), ngl::ACTOR_CHAT);
	EXPECT_EQ(lguid.area(), ngl::tab_self_area);

	EXPECT_TRUE(lrole->forward_guid(ngl::ACTOR_CHAT, ngl::ecross_cross_ordinary, lguid, ldata));
	EXPECT_EQ(lguid.type(), ngl::ACTOR_CHAT);
	EXPECT_EQ(lguid.area(), ngl::tab_self_cros_area);

	lrole->m_example.second = ngl::nguid::make(ngl::ACTOR_EXAMPLE_GUESS_NUMBER, ngl::tab_self_area, 88);
	EXPECT_TRUE(lrole->forward_guid(ngl::ACTOR_CHAT, ngl::ecross_cross_example, lguid, ldata));
	EXPECT_EQ(lguid, ngl::nguid(lrole->m_example.second));

	EXPECT_FALSE(lrole->forward_guid(ngl::ACTOR_CHAT, ngl::ecross_none, lguid, ldata));
}

TEST(ActorRoleTest, IsFirstRechargeDetectsExistingRechargeIds)
{
	auto lrole = detail::make_role();
	ASSERT_NE(lrole, nullptr);

	EXPECT_TRUE(lrole->is_first_recharge(17));

	detail::add_rechg(*lrole, 17);

	EXPECT_FALSE(lrole->is_first_recharge(17));
	EXPECT_TRUE(lrole->is_first_recharge(18));
}

TEST(ActorRoleTest, CreateOrderEncodesRoleAndStaysUnique)
{
	auto lrole = detail::make_role();
	ASSERT_NE(lrole, nullptr);

	std::string lfirst;
	std::string lsecond;

	lrole->createorder(lfirst, 23);
	lrole->createorder(lsecond, 23);

	EXPECT_EQ(lfirst.size(), 37U);
	EXPECT_EQ(lsecond.size(), 37U);
	EXPECT_NE(lfirst, lsecond);
	EXPECT_EQ(lfirst.substr(0, 5), std::format("{:05d}", lrole->area()));
	EXPECT_EQ(lfirst.substr(5, 10), std::format("{:010d}", lrole->id()));
	EXPECT_EQ(lfirst.substr(15, 10), "0000000023");
}

} // namespace actor_role_test
