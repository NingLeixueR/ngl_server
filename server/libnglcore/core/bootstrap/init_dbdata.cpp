// File overview: Implements helpers that seed demo and test data into bootstrap database tables.
#include "actor/actor_base/core/ndbclient.h"
#include "actor/actor_base/core/actor_db.h"
#include "init_dbdata.h"

#include <map>

enum
{
	def_count = 2000
};

template <pbdb::ENUM_DB tdb, typename trec>
void save_seed(const trec& arec)
{
	// Keep the seed helpers backend-agnostic so tests and bootstrap scripts work with
	// either MySQL or PostgreSQL.
	if (nconfig.dbedb() == ngl::xarg_db::edb_mysql)
	{
		ngl::ndbtab<tdb, trec, ngl::nmysql_manage, ngl::nmysql_pool>::save(0, arec);
	}
	else if (nconfig.dbedb() == ngl::xarg_db::edb_postgresql)
	{
		ngl::ndbtab<tdb, trec, ngl::npostgresql_manage, ngl::npostgresql_pool>::save(0, arec);
	}
}

void init_db_acc(const char* aname, int abeg)
{
	for (int li = abeg; li < abeg + def_count; ++li)
	{
		pbdb::db_account ltemp;
		ltemp.set_maccount(std::string(aname) + ngl::tools::lexical_cast<std::string>(li % def_count));
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, li));
		ltemp.set_mpassworld("123456");
		ltemp.set_mroleid(ltemp.mid());
		ltemp.set_marea(tab_self_area);

		save_seed<pbdb::ENUM_DB_ACCOUNT>(ltemp);
	}
}

void init_db_acc()
{
	init_db_acc("libo", 1);
	init_db_acc("wac", def_count + 1);
}

// Seed demo role records.
void init_db_role(const char* aname, int abeg)
{
	for (int li = abeg; li < abeg + def_count; ++li)
	{
		const int32_t lnow = static_cast<int32_t>(ngl::tools::time::gettime());
		pbdb::db_role ltemp;
		ngl::i64_actorid lrole_id = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, li);
		ltemp.set_mid(lrole_id);
		pbdb::db_brief lrolebase;
		lrolebase.set_mid(ngl::nguid::make(ngl::ACTOR_BRIEF, tab_self_area, li));
		auto* lbase = lrolebase.mutable_mbase();
		lbase->set_mname(std::string(aname) + ngl::tools::lexical_cast<std::string>(li % def_count));
		lbase->set_mlv(li);
		lbase->set_mmoneygold(0);
		lbase->set_mmoneysilver(0);
		lbase->set_mcreateutc(lnow);

		save_seed<pbdb::ENUM_DB_BRIEF>(lrolebase);
		save_seed<pbdb::ENUM_DB_ROLE>(ltemp);
	}
}

void init_db_role()
{
	std::cout << "#########init_db_role()#############" << std::endl;
	std::string lname = std::format("{}_zone{}_", "libo", nconfig.area());
	init_db_role(lname.c_str(), 1);
	lname = std::format("{}_zone{}_", "wac", nconfig.area());
	init_db_role(lname.c_str(), def_count + 1);
}

// Seed demo bag records.
void init_db_bag(const char* aname, int abeg)
{
	(void)aname;
	for (int li = abeg; li < abeg + def_count; ++li)
	{
		pbdb::db_bag ltemp;
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, li));
		ltemp.set_mmaxid(1);

		save_seed<pbdb::ENUM_DB_BAG>(ltemp);
	}
}

void init_db_bag()
{
	init_db_bag("libo", 1);
	init_db_bag("wac", def_count + 1);
}

void init_db_task(const char* aname, int abeg)
{
	(void)aname;
	for (int li = abeg; li < abeg + def_count; ++li)
	{
		pbdb::db_task ltemp;
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, li));
		auto lrundata = ltemp.mutable_mrundatas();
		pbdb::db_task::data& ldata = (*lrundata)[1];
		ldata.set_mtaskid(1);
		ldata.set_mreceiveutc(0);
		ldata.set_mfinshutc(0);
		auto* lscheds = ldata.mutable_mschedules();
		auto* lnode = lscheds->Add();
		lnode->set_mvalue(1);
		lnode->set_msumint(10);

		save_seed<pbdb::ENUM_DB_TASK>(ltemp);
	}
}

void init_db_task()
{
	init_db_task("libo", 1);
	init_db_task("wac", def_count + 1);
}

// Seed demo mailbox records.
void init_db_mail(int abeg)
{
	for (int li = abeg; li < abeg + def_count; ++li)
	{
		pbdb::db_mail ltemp;
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, li));

		save_seed<pbdb::ENUM_DB_MAIL>(ltemp);
	}
}

void init_db_mail()
{
	init_db_mail(1);
	init_db_mail(def_count + 1);
}

void init_db_rkv(int abeg)
{
	for (int li = abeg; li < abeg + def_count; ++li)
	{
		pbdb::db_rolekeyvalue ltemp;
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, li));
		auto* ldata = ltemp.mutable_mdata();
		(*ldata)["test1"] = "1";
		(*ldata)["test2"] = "2";
		(*ldata)["test3"] = "3";

		save_seed<pbdb::ENUM_DB_ROLEKEYVALUE>(ltemp);
	}
}

void init_db_rkv()
{
	init_db_rkv(1);
	init_db_rkv(def_count + 1);
}

void init_db_note()
{
	const int32_t larea = ngl::ttab_servers::instance().const_tab()->m_area * 100;
	std::vector<std::string> lvec =
	{
		"note_1", // 1
		"note_2", // 2
		"note_3", // 3
		"note_4", // 4
		"note_5", // 5
		"note_6", // 6
		"note_7", // 7
		"note_8", // 8
		"note_9", // 9
		"note_10", // 10
	};
	for (int li = 1; li < 10; ++li)
	{
		constexpr int32_t lkeep_sec = 36000;
		pbdb::db_notice ltemp;
		ltemp.set_mid(larea + li);
		if (!ngl::tools::to_utf8(lvec[li], lvec[li]))
		{
			continue;
		}
		ltemp.set_mnotice(lvec[li]);
		const int32_t lnow = static_cast<int32_t>(time(nullptr));
		ltemp.set_mstarttime(lnow);
		ltemp.set_mfinishtime(lnow + lkeep_sec);
		save_seed<pbdb::ENUM_DB_NOTICE>(ltemp);
	}
}

void init_db_kv()
{
	pbdb::db_keyvalue ltemp;
	ltemp.set_mid(pbdb::db_keyvalue_ekv_none);
	const int32_t lnow = (int32_t)ngl::tools::time::gettime();
	std::string lval = std::format("{}*{}", lnow, ngl::tools::time::time2str(lnow, "%y/%m/%d %H:%M:%S"));
	ltemp.set_mvalue(lval);

	save_seed<pbdb::ENUM_DB_KEYVALUE>(ltemp);
}

void init_db_fam()
{
	for (int li = 1; li < 100; ++li)
	{
		const int32_t lnow = static_cast<int32_t>(ngl::tools::time::gettime());
		pbdb::db_family ltemp;
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_FAMILY, tab_self_area, li));
		ltemp.set_mcreateutc(lnow);
		ltemp.set_mexp(100);
		ltemp.set_mlv(1);
		ltemp.set_mname(std::format("FLIBO{}", li));
		ltemp.set_mleader(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, li));
		auto* lmembers = ltemp.mutable_mmember();
		*lmembers->Add() = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, li);

		save_seed<pbdb::ENUM_DB_FAMILY>(ltemp);

		pbdb::db_familyer ltempfamilyer;
		ltempfamilyer.set_mjoinutc(lnow);
		ltempfamilyer.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, li));
		ltempfamilyer.set_mposition(pbdb::db_familyer_eposition_leader);
		ltempfamilyer.set_mlastsignutc(lnow);

		save_seed<pbdb::ENUM_DB_FAMILYER>(ltempfamilyer);
	}
}

void init_db_rank()
{
	/*for (int li = 1; li < 100; ++li)
	{
		pbdb::db_ranklist ltemp;
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, li));
		pbdb::rankitem lrankitem;
		lrankitem.set_mtime((int32_t)ngl::tools::time::gettime());
		lrankitem.set_mvalue(li);
		(*ltemp.mutable_mitems())[(int)pbdb::eranklist::lv] = lrankitem;

			save_seed<pbdb::ENUM_DB_RANKLIST>(ltemp);
	}*/
}

void seed_frd(pbdb::db_friends& afriends, int abeg, int aend, int aidx)
{
	for (int lidx = abeg; lidx < aend; ++lidx)
	{
		if (aidx == lidx)
		{
			continue;
		}
		ngl::i64_actorid lactor2 = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, lidx);
		ngl::i64_actorid lactor3 = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, lidx + 10);
		afriends.add_mfriends(lactor2);
		afriends.add_mapplyfriends(lactor3);
	}
}

void init_db_frd()
{
	std::map<ngl::i64_actorid, pbdb::db_friends> lmap;
	for (int li = 0; li < 10; ++li)
	{
		const int lbeg = li * 10;
		const int lend = lbeg + 10;
		for (int lj1 = lbeg; lj1 < lend; ++lj1)
		{
			ngl::i64_actorid lactor1 = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, lj1);
			pbdb::db_friends& lfriends = lmap[lactor1];
			lfriends.set_mid(lactor1);
			seed_frd(lfriends, lbeg, lend, lj1);
		}
	}

	for (auto& apair : lmap)
	{
		save_seed<pbdb::ENUM_DB_FRIENDS>(apair.second);
	}
}

void init_db_tlua()
{
	std::map<ngl::i64_actorid, pbdb::db_testlua> lmap;
	for (int li = 0; li < 10; ++li)
	{
		ngl::i64_actorid lactor1 = ngl::nguid::make(ngl::ACTOR_TESTLUA, tab_self_area, li);
		pbdb::db_testlua& ltestlua = lmap[lactor1];
		ltestlua.set_mid(lactor1);
		for (int lj = 0; lj < 10; ++lj)
		{
			ltestlua.add_mvalue(li * 1000 + lj);
		}
		pbdb::db_testlua::luadata ltemp;
		auto* ldata = ltestlua.mutable_mdatas();
		for (int lidx = 0; lidx < 5; ++lidx)
		{
			ltemp.set_mkey(std::format("key_{}", lidx).c_str());
			ltemp.set_mval(std::format("val_{}", lidx).c_str());
			(*ldata)[lidx] = ltemp;
		}
	}

	for (auto& apair : lmap)
	{
		save_seed<pbdb::ENUM_DB_TESTLUA>(apair.second);
	}
}
