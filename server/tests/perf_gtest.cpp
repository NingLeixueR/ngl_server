// File overview: Implements GoogleTest coverage for perf.

#include <gtest/gtest.h>

#include <chrono>
#include <cctype>
#include <cstdint>
#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "actor/tab/ttab_servers.h"
#include "tools/nfilterword.h"
#include "tools/tab/csv/csv.h"
#include "tools/tab/xml/xmlinfo.h"

namespace perf_test_case
{
template <typename TFUN>
long long benchmark_us(TFUN&& afun)
{
	const auto start = std::chrono::steady_clock::now();
	afun();
	const auto finish = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
}

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
	if (!ngl::nfilterword::utf8to32(atext, decoded))
	{
		return false;
	}

	std::u32string normalized;
	normalized.reserve(decoded.size());
	for (char32_t codepoint : decoded)
	{
		if (!ngl::nfilterword::is_emojispecial(codepoint))
		{
			normalized.push_back(codepoint);
		}
	}

	std::string filtered;
	if (!ngl::nfilterword::utf32to8(normalized, filtered))
	{
		return false;
	}

	int state = -1;
	std::vector<std::pair<int, int>> matches;
	for (std::string::size_type i = 0; i < filtered.size(); ++i)
	{
		if (ngl::nfilterword::match(filtered[i], state, static_cast<int>(i), matches))
		{
			return true;
		}
	}
	return false;
}

void load_perf_filter_words()
{
	ngl::nfilterword::clear();
	ngl::nfilterword::init();
	ngl::nfilterword::load("badword");
	ngl::nfilterword::load("unsafe");
	ngl::nfilterword::load("123456");
	ngl::nfilterword::build();
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
	constexpr int kIterations = 20000;
	volatile int sink = 0;

	const long long legacy_us = benchmark_us([&]() {
		for (int i = 0; i < kIterations; ++i)
		{
			const auto* row = legacy_find_server_by_name(*table, 1, target);
			if (row != nullptr)
			{
				sink += row->m_id;
			}
		}
	});

	const long long optimized_us = benchmark_us([&]() {
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
	std::cout << "[perf] ttab_servers_lookup legacy_us=" << legacy_us
		<< " optimized_us=" << optimized_us << std::endl;
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
	constexpr int kIterations = 200000;
	volatile long long sink = 0;

	const long long legacy_us = benchmark_us([&]() {
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

	const long long optimized_us = benchmark_us([&]() {
		for (int i = 0; i < kIterations; ++i)
		{
			const ngl::i16_area larea = static_cast<ngl::i16_area>((i % kAreaCount) + 1);
			sink += optimized_index[ngl::ttab_mergearea::merge_slot(larea)];
		}
	});

	EXPECT_EQ(optimized_index[ngl::ttab_mergearea::merge_slot(1)], kAreaCount);
	EXPECT_LT(optimized_us, legacy_us);
	std::cout << "[perf] ttab_mergearea_lookup legacy_us=" << legacy_us
		<< " optimized_us=" << optimized_us << std::endl;
}

TEST(XargInfoPerfTest, BoolLookupBenchmark)
{
	ngl::xarg_info info;
	info.data()["flag"] = std::string(96, ' ') + "TrUe" + std::string(96, ' ');
	constexpr int kIterations = 200000;
	volatile int sink = 0;

	const long long legacy_us = benchmark_us([&]() {
		for (int i = 0; i < kIterations; ++i)
		{
			bool value = false;
			if (legacy_parse_bool(info.data().at("flag"), value) && value)
			{
				++sink;
			}
		}
	});

	const long long optimized_us = benchmark_us([&]() {
		for (int i = 0; i < kIterations; ++i)
		{
			bool value = false;
			if (info.find("flag", value) && value)
			{
				++sink;
			}
		}
	});

	EXPECT_GT(sink, 0);
	EXPECT_LT(optimized_us, legacy_us);
	std::cout << "[perf] xarg_info_bool_lookup legacy_us=" << legacy_us
		<< " optimized_us=" << optimized_us << std::endl;
}

TEST(CsvPerfTest, StringFieldParsingBenchmark)
{
	const std::string payload = "\"" + std::string(256, 'x') + "\",tail";
	constexpr int kIterations = 100000;
	volatile int sink = 0;

	const long long legacy_us = benchmark_us([&]() {
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

	const long long optimized_us = benchmark_us([&]() {
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
	std::cout << "[perf] csv_string_parse legacy_us=" << legacy_us
		<< " optimized_us=" << optimized_us << std::endl;
}

TEST(NFilterWordPerfTest, IsFilterBenchmark)
{
	load_perf_filter_words();
	const std::string payload = std::string(2048, 'x') + "badword" + std::string(2048, 'y');
	constexpr int kIterations = 5000;
	volatile int sink = 0;

	const long long legacy_us = benchmark_us([&]() {
		for (int i = 0; i < kIterations; ++i)
		{
			if (legacy_is_filter(payload))
			{
				++sink;
			}
		}
	});

	const long long optimized_us = benchmark_us([&]() {
		for (int i = 0; i < kIterations; ++i)
		{
			if (ngl::nfilterword::is_filter(payload))
			{
				++sink;
			}
		}
	});

	EXPECT_GT(sink, 0);
	EXPECT_LT(optimized_us, legacy_us);
	std::cout << "[perf] nfilterword_is_filter legacy_us=" << legacy_us
		<< " optimized_us=" << optimized_us << std::endl;
}

} // namespace perf_test_case
