// File overview: Implements GoogleTest coverage for perf.

#include <gtest/gtest.h>

#include <cctype>
#include <cstdint>
#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "actor/actor_logic/actor_role/actor_role.h"
#include "actor/tab/ttab_random.h"
#include "actor/tab/ttab_servers.h"
#include "actor/tab/ttab_task.h"
#include "test_support.h"
#include "tools/tools/tools_filterword.h"
#include "tools/tab/csv/csv.h"
#include "tools/tab/xml/xmlinfo.h"

namespace perf_test_case
{
struct csv_miss_row
{
	int32_t m_id = 0;

	bool rcsv(ngl::csvpair&)
	{
		return false;
	}
};

struct csv_miss_tab :
	public ngl::csv<csv_miss_row>
{
};

const ngl::tab_servers* legacy_find_server_by_name(const ngl::ttab_servers& atable, int area, const std::string& aname)
{
	for (const auto& [_, row] : atable.m_csv)
	{
		if (row.m_area == area && row.m_name == aname)
		{
			return &row;
		}
	}
	return nullptr;
}

bool legacy_parse_bool(std::string_view avalue, bool& aout)
{
	std::string normalized;
	normalized.reserve(avalue.size());
	for (unsigned char ch : avalue)
	{
		if (std::isspace(ch) == 0)
		{
			normalized.push_back(static_cast<char>(std::tolower(ch)));
		}
	}

	if (normalized == "true" || normalized == "1" || normalized == "yes" || normalized == "on")
	{
		aout = true;
		return true;
	}
	if (normalized == "false" || normalized == "0" || normalized == "no" || normalized == "off")
	{
		aout = false;
		return true;
	}
	return false;
}

bool legacy_csv_read_string(ngl::csvpair& apair, std::string& adata)
{
	std::string& text = apair.m_data;
	int& pos = apair.m_pos;
	const int size = static_cast<int>(text.size());
	std::string value;
	for (; pos < size; ++pos)
	{
		if (!apair.m_doublequotationmarks)
		{
			if (text[pos] == apair.m_fg || text[pos] == '\n')
			{
				++pos;
				break;
			}
			if (text[pos] == '\r')
			{
				continue;
			}
		}
		if (text[pos] == '"')
		{
			apair.m_doublequotationmarks = apair.m_doublequotationmarks ? false : true;
			continue;
		}
		value += text[pos];
	}
	if (apair.m_doublequotationmarks)
	{
		return false;
	}
	adata = std::move(value);
	return true;
}

bool legacy_is_filter(const std::string& atext)
{
	if (atext.empty())
	{
		return false;
	}

	std::u32string decoded;
	if (!ngl::tools::filterword::utf8to32(atext, decoded))
	{
		return false;
	}

	std::u32string normalized;
	normalized.reserve(decoded.size());
	for (char32_t codepoint : decoded)
	{
		if (!ngl::tools::filterword::is_emojispecial(codepoint))
		{
			normalized.push_back(codepoint);
		}
	}

	std::string filtered;
	if (!ngl::tools::filterword::utf32to8(normalized, filtered))
	{
		return false;
	}

	int state = -1;
	std::vector<std::pair<int, int>> matches;
	for (std::string::size_type i = 0; i < filtered.size(); ++i)
	{
		if (ngl::tools::filterword::match(filtered[i], state, static_cast<int>(i), matches))
		{
			return true;
		}
	}
	return false;
}

void load_perf_filter_words()
{
	ngl::tools::filterword::clear();
	ngl::tools::filterword::init();
	ngl::tools::filterword::load("badword");
	ngl::tools::filterword::load("unsafe");
	ngl::tools::filterword::load("123456");
	ngl::tools::filterword::build();
}

struct legacy_mergearea_indexes
{
	std::map<ngl::i16_area, ngl::i16_area> m_merge1;
	std::map<ngl::i16_area, std::set<ngl::i16_area>> m_merge2;
};

legacy_mergearea_indexes legacy_reload_mergeareas(const std::map<ngl::i16_area, ngl::i16_area>& adirect)
{
	legacy_mergearea_indexes lindexes;
	auto resolve_mergeid = [&adirect](ngl::i16_area aarea)
	{
		std::set<ngl::i16_area> lvisited;
		ngl::i16_area lcurrent = aarea;
		while (true)
		{
			if (!lvisited.insert(lcurrent).second)
			{
				return *std::min_element(lvisited.begin(), lvisited.end());
			}

			auto it = adirect.find(lcurrent);
			if (it == adirect.end() || it->second == lcurrent)
			{
				break;
			}
			lcurrent = it->second;
		}
		return lcurrent;
	};

	for (const auto& [area, _mergeid] : adirect)
	{
		const ngl::i16_area lroot = resolve_mergeid(area);
		lindexes.m_merge1.emplace(area, lroot);
		lindexes.m_merge2[lroot].insert(lroot);
		lindexes.m_merge2[lroot].insert(area);
	}
	return lindexes;
}

std::unique_ptr<ngl::ttab_mergearea> make_mergearea_table(int count)
{
	auto table = std::make_unique<ngl::ttab_mergearea>();
	for (int i = 1; i <= count; ++i)
	{
		ngl::tab_mergearea row{};
		row.m_id = i;
		row.m_name = std::format("area_{}", i);
		row.m_mergeid = (i == count) ? i : (i + 1);
		table->m_csv.emplace(row.m_id, std::move(row));
	}
	return table;
}

std::unique_ptr<ngl::ttab_servers> make_server_table(int count)
{
	auto table = std::make_unique<ngl::ttab_servers>();
	for (int i = 1; i <= count; ++i)
	{
		ngl::tab_servers row;
		row.m_id = i;
		row.m_name = std::format("server_{}", i);
		row.m_area = 1;
		row.m_type = (i % 2 == 0) ? ngl::GAME : ngl::LOGIN;
		row.m_tcount = 1;

		ngl::net_works tcp;
		tcp.m_type = ngl::ENET_TCP;
		tcp.m_ip = "127.0.0.1";
		tcp.m_port = static_cast<uint16_t>(8000 + (i % 1000));
		row.m_net.push_back(tcp);

		table->m_csv.emplace(row.m_id, std::move(row));
	}
	table->reload();
	return table;
}
TEST(TTabServersTest, ReloadClearsStaleIndexesAndCachedNetworks)
{
	ngl::ttab_servers table;

	ngl::tab_servers game;
	game.m_id = 1;
	game.m_name = "game";
	game.m_area = 1;
	game.m_type = ngl::GAME;
	game.m_tcount = 1;
	ngl::net_works tcp;
	tcp.m_type = ngl::ENET_TCP;
	tcp.m_ip = "127.0.0.1";
	tcp.m_port = 9000;
	game.m_net.push_back(tcp);
	table.m_csv.emplace(game.m_id, game);
	table.reload();

	ASSERT_NE(table.const_tab("game", 1), nullptr);
	ASSERT_NE(table.const_tab(ngl::GAME, 1), nullptr);
	ngl::net_works first_network;
	ASSERT_TRUE(table.get_nworks(table.const_tab("game", 1), ngl::ENET_TCP, 1, first_network));
	EXPECT_EQ(first_network.m_port, 9000);

	table.m_csv.clear();
	ngl::tab_servers login;
	login.m_id = 2;
	login.m_name = "login";
	login.m_area = 1;
	login.m_type = ngl::LOGIN;
	login.m_tcount = 1;
	ngl::net_works ws;
	ws.m_type = ngl::ENET_WS;
	ws.m_ip = "127.0.0.1";
	ws.m_port = 9100;
	login.m_net.push_back(ws);
	table.m_csv.emplace(login.m_id, login);
	table.reload();

	EXPECT_EQ(table.const_tab("game", 1), nullptr);
	EXPECT_EQ(table.const_tab(ngl::GAME, 1), nullptr);
	ASSERT_NE(table.const_tab("login", 1), nullptr);

	ngl::net_works second_network;
	EXPECT_FALSE(table.get_nworks(table.const_tab("login", 1), ngl::ENET_TCP, 1, second_network));
	ASSERT_TRUE(table.get_nworks(table.const_tab("login", 1), ngl::ENET_WS, 1, second_network));
	EXPECT_EQ(second_network.m_port, 9100);
}

TEST(TTabServersTest, FindFirstKeepsLowestMatchingServerId)
{
	const auto table = make_server_table(12);
	const ngl::tab_servers* first_game = table->find_first(ngl::GAME, 1, [](const ngl::tab_servers* row) {
		return row->m_tcount == 1;
	});

	ASSERT_NE(first_game, nullptr);
	EXPECT_EQ(first_game->m_id, 2);
}

TEST(TTabServersTest, GetNetworkByServerIdReusesTidAndTcount)
{
	ngl::ttab_servers table;

	ngl::tab_servers game;
	game.m_id = 7;
	game.m_name = "game";
	game.m_area = 1;
	game.m_type = ngl::GAME;
	game.m_tcount = 4;
	ngl::net_works tcp;
	tcp.m_type = ngl::ENET_TCP;
	tcp.m_ip = "127.0.0.1";
	tcp.m_port = 9200;
	game.m_net.push_back(tcp);
	table.m_csv.emplace(game.m_id, game);
	table.reload();

	const ngl::i32_serverid serverid = ngl::nnodeid::nodeid(static_cast<int16_t>(game.m_id), 3);
	ngl::net_works network;
	ASSERT_TRUE(table.get_nworks(serverid, 1, ngl::ENET_TCP, network));
	EXPECT_EQ(network.m_ip, "127.0.0.1");
	EXPECT_EQ(network.m_port, 9202);
}

TEST(NcsvEdgeTest, GetMissingTabReturnsNullptr)
{
	EXPECT_EQ(ngl::ncsv::get<csv_miss_tab>(), nullptr);
}

TEST(TTabServersTest, ConnectRejectsInvalidTargetsWithoutCrash)
{
	const int lold_tid = nconfig.tid();
	const int lold_cnt = nconfig.tcount();

	ngl::ttab_servers ltable;

	ngl::tab_servers llocal;
	llocal.m_id = 5;
	llocal.m_name = "login";
	llocal.m_area = 1;
	llocal.m_type = ngl::LOGIN;
	llocal.m_tcount = 1;
	ngl::net_works llocal_tcp;
	llocal_tcp.m_type = ngl::ENET_TCP;
	llocal_tcp.m_ip = "127.0.0.1";
	llocal_tcp.m_port = 9300;
	llocal.m_net.push_back(llocal_tcp);

	ngl::tab_servers lother;
	lother.m_id = 7;
	lother.m_name = "game";
	lother.m_area = 1;
	lother.m_type = ngl::GAME;
	lother.m_tcount = 1;
	ngl::net_works lother_ws;
	lother_ws.m_type = ngl::ENET_WS;
	lother_ws.m_ip = "127.0.0.1";
	lother_ws.m_port = 9400;
	lother.m_net.push_back(lother_ws);

	ltable.m_csv.emplace(llocal.m_id, llocal);
	ltable.m_csv.emplace(lother.m_id, lother);
	ltable.reload();

	ngl::net_works lnet;
	
	nconfig.set_nodeid(static_cast<int16_t>(llocal.m_id), 1);
	EXPECT_FALSE(ltable.connect(ngl::nnodeid::nodeid(static_cast<int16_t>(llocal.m_id), 1), lnet));
	EXPECT_FALSE(ltable.connect(ngl::nnodeid::nodeid(8, 1), lnet));
	EXPECT_FALSE(ltable.connect(ngl::nnodeid::nodeid(static_cast<int16_t>(lother.m_id), 1), lnet));

	nconfig.set_nodeid(9, 1);
	EXPECT_FALSE(ltable.connect(ngl::nnodeid::nodeid(static_cast<int16_t>(lother.m_id), 1), lnet));

	nconfig.set_nodeid(lold_tid, lold_cnt);
}

TEST(TTabServersTest, NodeTypeMissingTidReturnsFail)
{
	ngl::ttab_servers ltable;

	EXPECT_EQ(ltable.node_type(77), ngl::FAIL);
}

TEST(TTabTaskTest, EqualCompleteConditionIndexesCompleteSet)
{
	ngl::ttab_task ltable;

	ngl::task_condition lcond{};
	lcond.m_type = ngl::ETaskTaskId;
	lcond.m_condition = ngl::ETaskConditionEqual;
	lcond.m_parmint = 42;

	ltable.set_data(9, lcond, ltable.m_map[lcond.m_type], false);

	const std::set<ngl::i32_taskid>* lrecv = ltable.check(lcond.m_type, lcond.m_parmint, true);
	const std::set<ngl::i32_taskid>* ldone = ltable.check(lcond.m_type, lcond.m_parmint, false);

	ASSERT_NE(lrecv, nullptr);
	EXPECT_TRUE(lrecv->empty());
	ASSERT_NE(ldone, nullptr);
	EXPECT_TRUE(ldone->contains(9));
}

TEST(TTabTaskTest, UnsupportedMoreTypeIsIgnoredWithoutCrash)
{
	ngl::ttab_task ltable;

	ngl::task_condition lcond{};
	lcond.m_type = static_cast<ngl::ETask>(ngl::ETaskCount + 1);
	lcond.m_condition = ngl::ETaskConditionMore;
	lcond.m_parmint = 3;

	std::map<int32_t, ngl::ttab_task::receive_complete> lrc;
	ltable.set_data(11, lcond, lrc, true);

	EXPECT_TRUE(lrc.empty());
	EXPECT_EQ(ltable.check(lcond.m_type, lcond.m_parmint, true), nullptr);
}

TEST(TTabRandomTest, ReloadAcceptsAcyclicGraph)
{
	ngl::ttab_random ltable;

	ngl::tab_random lroot{};
	lroot.m_id = 1;
	lroot.m_name = "root";
	lroot.m_childrandomids.push_back(2);

	ngl::tab_random lchild{};
	lchild.m_id = 2;
	lchild.m_name = "child";

	ltable.m_csv.emplace(lroot.m_id, lroot);
	ltable.m_csv.emplace(lchild.m_id, lchild);

	std::set<int32_t> lseen;
	EXPECT_TRUE(ltable.is_loop(lroot.m_id, lseen));

	ltable.reload();
}

TEST(TTabRandomTest, ReloadRejectsCycleWithoutCrash)
{
	ngl::ttab_random ltable;

	ngl::tab_random lroot{};
	lroot.m_id = 1;
	lroot.m_name = "root";
	lroot.m_childrandomids.push_back(2);

	ngl::tab_random lchild{};
	lchild.m_id = 2;
	lchild.m_name = "child";
	lchild.m_childrandomids.push_back(1);

	ltable.m_csv.emplace(lroot.m_id, lroot);
	ltable.m_csv.emplace(lchild.m_id, lchild);

	std::set<int32_t> lseen;
	EXPECT_FALSE(ltable.is_loop(lroot.m_id, lseen));

	ltable.reload();
}

TEST(TTabMergeAreaTest, ReloadResolvesLongChains)
{
	const auto table = make_mergearea_table(6);
	table->reload();

	EXPECT_EQ(table->mergeid(1), 6);
	EXPECT_EQ(table->mergeid(5), 6);
	auto* merged = table->mergelist(6);
	ASSERT_NE(merged, nullptr);
	EXPECT_EQ(merged->size(), 6U);
	EXPECT_TRUE(merged->contains(1));
	EXPECT_TRUE(merged->contains(6));
}

TEST(TTabServersPerfTest, IndexedLookupBenchmark)
{
	const auto table = make_server_table(2000);
	const std::string target = "server_2000";
	const int kIterations = ngl_test_support::scaled_iterations(20000);
	int sink = 0;

	const long long legacy_us = ngl_test_support::benchmark_us([&table, &target, kIterations, &sink]() {
		for (int i = 0; i < kIterations; ++i)
		{
			const auto* row = legacy_find_server_by_name(*table, 1, target);
			if (row != nullptr)
			{
				sink += row->m_id;
			}
		}
	});

	const long long optimized_us = ngl_test_support::benchmark_us([&table, &target, kIterations, &sink]() {
		for (int i = 0; i < kIterations; ++i)
		{
			const auto* row = table->const_tab(target, 1);
			if (row != nullptr)
			{
				sink += row->m_id;
			}
		}
	});

	EXPECT_GT(sink, 0);
	EXPECT_LT(optimized_us, legacy_us);
	ngl_test_support::print_perf_result("ttab_servers_lookup", legacy_us, optimized_us);
}

TEST(TTabMergeAreaPerfTest, MergeLookupBenchmark)
{
	constexpr int kAreaCount = 6000;
	std::vector<ngl::i16_area> optimized_index(ngl::ttab_mergearea::m_merge_slot_count, ngl::nguid::none_area());
	legacy_mergearea_indexes legacy;
	for (int i = 1; i <= kAreaCount; ++i)
	{
		const ngl::i16_area area = static_cast<ngl::i16_area>(i);
		legacy.m_merge1.emplace(area, static_cast<ngl::i16_area>(kAreaCount));
		optimized_index[ngl::ttab_mergearea::merge_slot(area)] = static_cast<ngl::i16_area>(kAreaCount);
	}
	const int kIterations = ngl_test_support::scaled_iterations(200000);
	long long sink = 0;

	const long long legacy_us = ngl_test_support::benchmark_us([&legacy, kIterations, kAreaCount, &sink]() {
		for (int i = 0; i < kIterations; ++i)
		{
			const ngl::i16_area larea = static_cast<ngl::i16_area>((i % kAreaCount) + 1);
			auto it = legacy.m_merge1.find(larea);
			if (it != legacy.m_merge1.end())
			{
				sink += it->second;
			}
		}
	});

	const long long optimized_us = ngl_test_support::benchmark_us([&optimized_index, kIterations, kAreaCount, &sink]() {
		for (int i = 0; i < kIterations; ++i)
		{
			const ngl::i16_area larea = static_cast<ngl::i16_area>((i % kAreaCount) + 1);
			sink += optimized_index[ngl::ttab_mergearea::merge_slot(larea)];
		}
	});

	EXPECT_EQ(optimized_index[ngl::ttab_mergearea::merge_slot(1)], kAreaCount);
	EXPECT_LT(optimized_us, legacy_us);
	ngl_test_support::print_perf_result("ttab_mergearea_lookup", legacy_us, optimized_us);
}

TEST(CsvPerfTest, StringFieldParsingBenchmark)
{
	const std::string payload = "\"" + std::string(256, 'x') + "\",tail";
	const int kIterations = ngl_test_support::scaled_iterations(100000);
	int sink = 0;

	const long long legacy_us = ngl_test_support::benchmark_us([&payload, kIterations, &sink]() {
		for (int i = 0; i < kIterations; ++i)
		{
			ngl::csvpair pair;
			pair.m_data = payload;
			std::string value;
			if (legacy_csv_read_string(pair, value))
			{
				sink += static_cast<int>(value.size());
			}
		}
	});

	const long long optimized_us = ngl_test_support::benchmark_us([&payload, kIterations, &sink]() {
		for (int i = 0; i < kIterations; ++i)
		{
			ngl::csvpair pair;
			pair.m_data = payload;
			std::string value;
			if (ngl::csv_read<std::string>::read(pair, value))
			{
				sink += static_cast<int>(value.size());
			}
		}
	});

	EXPECT_GT(sink, 0);
	EXPECT_LT(optimized_us, legacy_us);
	ngl_test_support::print_perf_result("csv_string_parse", legacy_us, optimized_us);
}

TEST(NFilterWordPerfTest, IsFilterBenchmark)
{
	load_perf_filter_words();
	const std::string payload = std::string(2048, 'x') + "badword" + std::string(2048, 'y');
	const int kIterations = ngl_test_support::scaled_iterations(5000);
	int sink = 0;

	const long long legacy_us = ngl_test_support::benchmark_us([&payload, kIterations, &sink]() {
		for (int i = 0; i < kIterations; ++i)
		{
			if (legacy_is_filter(payload))
			{
				++sink;
			}
		}
	});

	const long long optimized_us = ngl_test_support::benchmark_us([&payload, kIterations, &sink]() {
		for (int i = 0; i < kIterations; ++i)
		{
			if (ngl::tools::filterword::is_filter(payload))
			{
				++sink;
			}
		}
	});

	EXPECT_GT(sink, 0);
	EXPECT_LT(optimized_us, legacy_us);
	ngl_test_support::print_perf_result("nfilterword_is_filter", legacy_us, optimized_us);
}

} // namespace perf_test_case
