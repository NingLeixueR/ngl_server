#include <gtest/gtest.h>

#include <array>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "actor/actor_base/nguid.h"
#include "actor/tab/ttab_servers.h"
#include "tools/curl/ncurl.h"
#include "tools/localtime.h"
#include "tools/operator_file.h"
#include "tools/tab/csv/csv.h"
#include "tools/tab/json/njson.h"
#include "tools/tab/xml/sysconfig.h"
#include "tools/tab/xml/xml.h"
#include "tools/tools.h"

namespace
{
int CountCurlHeaders(const curl_slist* headers)
{
	int count = 0;
	for (auto* node = headers; node != nullptr; node = node->next)
	{
		++count;
	}
	return count;
}

std::filesystem::path make_temp_test_dir(const std::string& test_name)
{
	const auto suffix = std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	const std::filesystem::path dir =
		std::filesystem::temp_directory_path() / ("ngl_tools_" + test_name + "_" + suffix);
	std::error_code ec;
	std::filesystem::remove_all(dir, ec);
	std::filesystem::create_directories(dir, ec);
	return dir;
}

struct CsvTestRow
{
	int32_t m_id = 0;
	std::string m_name;

	bool rcsv(ngl::csvpair& apair)
	{
		return ngl::rcsv::readcsv(apair, m_id, m_name);
	}
};

struct JsonCustomPayload
{
	int32_t m_value = 0;
	std::string m_name;

	bool json_pop(rapidjson::Value& ajson)
	{
		return ngl::njson::pop(ajson, { "m_value", "m_name" }, m_value, m_name);
	}

	bool json_push(rapidjson::Value& ajson, rapidjson::Document::AllocatorType* aallocator) const
	{
		return ngl::njson::push(ajson, aallocator, { "m_value", "m_name" }, m_value, m_name);
	}
};
}

TEST(ToolsTest, CurlHttpFactoryInitializesHandle)
{
	auto http = ngl::ncurl::http();
	ASSERT_NE(http, nullptr);
	EXPECT_NE(http->m_curl, nullptr);
}

TEST(ToolsTest, CurlSettersHandleNullAndNullUrl)
{
	std::shared_ptr<ngl::http_parm> http;
	EXPECT_NO_THROW(ngl::ncurl::set_mode(http, ngl::ENUM_MODE_HTTP));
	EXPECT_NO_THROW(ngl::ncurl::set_type(http, ngl::ENUM_TYPE_GET));
	EXPECT_NO_THROW(ngl::ncurl::set_url(http, static_cast<const char*>(nullptr)));
	EXPECT_NO_THROW(ngl::ncurl::set_param(http, "a=1"));
	EXPECT_NO_THROW(ngl::ncurl::set_headers(http, std::vector<std::string>{ "A: 1" }));
	EXPECT_NO_THROW(ngl::ncurl::set_callback(http, [](int, ngl::http_parm&) {}));

	auto http2 = ngl::ncurl::http();
	ASSERT_NE(http2, nullptr);
	ngl::ncurl::set_url(http2, "abc");
	ngl::ncurl::set_url(http2, static_cast<const char*>(nullptr));
	EXPECT_TRUE(http2->m_url.empty());
}

TEST(ToolsTest, CurlParamSkipsNullKeysAndBuildsQuery)
{
	std::string query;
	ngl::ncurl::param(query, nullptr, 1);
	ngl::ncurl::param(query, "", 2);
	ngl::ncurl::param(query, "a", 1);
	ngl::ncurl::param(query, "b", "x");

	EXPECT_EQ(query, "a=1&b=x");
}

TEST(ToolsTest, CurlHeadersReplaceExistingList)
{
	auto http = ngl::ncurl::http();
	ASSERT_NE(http, nullptr);

	const std::vector<std::string> first = {
		"A: 1",
		"B: 2",
	};
	http->headers(first);
	ASSERT_NE(http->m_headers, nullptr);
	EXPECT_EQ(CountCurlHeaders(http->m_headers), 2);

	const std::vector<std::string> second = {
		"C: 3",
	};
	http->headers(second);
	ASSERT_NE(http->m_headers, nullptr);
	EXPECT_EQ(CountCurlHeaders(http->m_headers), 1);
	EXPECT_STREQ(http->m_headers->data, "C: 3");
}

TEST(ToolsTest, CurlSendEmailHandlesNullMailParam)
{
	std::shared_ptr<ngl::mail_param> mail;
	EXPECT_NO_THROW(ngl::ncurl::sendemail(mail));
}

TEST(ToolsTest, Utf8ValidationRejectsBrokenSequences)
{
	const std::string truncated_utf8("\xE4\xB8", 2);
	const std::string invalid_continuation("\xE4\x41\x80", 3);

	EXPECT_FALSE(ngl::tools::isutf8(truncated_utf8));
	EXPECT_FALSE(ngl::tools::isutf8(invalid_continuation));
	EXPECT_FALSE(ngl::tools::isincludeutf8mb4(truncated_utf8));
}

TEST(ToolsTest, Utf8ValidationDetectsMb4Characters)
{
	const std::string plain_utf8("\xE6\xB5\x8B\xE8\xAF\x95", 6);
	const std::string emoji_utf8("\xE6\xB5\x8B\xE8\xAF\x95\xF0\x9F\x99\x82", 10);

	EXPECT_TRUE(ngl::tools::isutf8(plain_utf8));
	EXPECT_TRUE(ngl::tools::isutf8(emoji_utf8));
	EXPECT_FALSE(ngl::tools::isincludeutf8mb4(plain_utf8));
	EXPECT_TRUE(ngl::tools::isincludeutf8mb4(emoji_utf8));
}

TEST(ToolsTest, Utf8ConversionsHandleEmptyAndInvalidInput)
{
	std::string ascii;
	EXPECT_TRUE(ngl::tools::to_asscii("", ascii));
	EXPECT_TRUE(ascii.empty());

	std::wstring wide;
	EXPECT_TRUE(ngl::tools::utf82wasscii("", wide));
	EXPECT_TRUE(wide.empty());

	const std::string invalid_utf8("\xFF\xFE", 2);
	EXPECT_FALSE(ngl::tools::utf82wasscii(invalid_utf8, wide));
	EXPECT_TRUE(wide.empty());
}

TEST(ToolsTest, AsciiWideConversionsRoundTrip)
{
	std::wstring wide;
	ASSERT_TRUE(ngl::tools::asscii2wasscii("hello", wide));
	EXPECT_EQ(wide, L"hello");

	std::string ascii;
	ASSERT_TRUE(ngl::tools::wasscii2asscii(wide, ascii));
	EXPECT_EQ(ascii, "hello");
}

TEST(ToolsTest, Base64EmptyInputReturnsEmptyOutput)
{
	EXPECT_TRUE(ngl::tools::base64_encode("", 0).empty());
	EXPECT_TRUE(ngl::tools::base64_decode("", 0).empty());
	EXPECT_TRUE(ngl::tools::base64_encode(std::string()).empty());
	EXPECT_TRUE(ngl::tools::base64_decode(std::string()).empty());
}

TEST(ToolsTest, LexicalCastAcceptsValidFloatingPointFormats)
{
	EXPECT_FLOAT_EQ(ngl::tools::lexical_cast<float>(std::string(".5")), 0.5f);
	EXPECT_FLOAT_EQ(ngl::tools::lexical_cast<float>(std::string("-.5")), -0.5f);
	EXPECT_DOUBLE_EQ(ngl::tools::lexical_cast<double>(std::string("1.")), 1.0);
}

TEST(ToolsTest, LexicalCastRejectsInvalidNumericFormats)
{
	EXPECT_THROW(ngl::tools::lexical_cast<int32_t>(std::string("")), boost::bad_lexical_cast);
	EXPECT_THROW(ngl::tools::lexical_cast<int32_t>(std::string("+")), boost::bad_lexical_cast);
	EXPECT_THROW(ngl::tools::lexical_cast<uint32_t>(std::string("-1")), boost::bad_lexical_cast);
	EXPECT_THROW(ngl::tools::lexical_cast<float>(std::string(".")), boost::bad_lexical_cast);
	EXPECT_THROW(ngl::tools::lexical_cast<float>(std::string("1.2.3")), boost::bad_lexical_cast);
}

TEST(ToolsTest, LexicalCastRejectsIntegralOverflow)
{
	EXPECT_THROW(ngl::tools::lexical_cast<int8_t>(std::string("128")), boost::bad_lexical_cast);
	EXPECT_THROW(ngl::tools::lexical_cast<uint8_t>(std::string("256")), boost::bad_lexical_cast);
	EXPECT_THROW(ngl::tools::lexical_cast<int16_t>(std::string("40000")), boost::bad_lexical_cast);
}

TEST(ToolsTest, NguidMakeActordataidUpdatesOnlyDataId)
{
	const ngl::i64_actorid actor = ngl::nguid::make(ngl::ACTOR_ROLE, 2, 11);
	const ngl::nguid updated(ngl::nguid::make_actordataid(actor, 99));

	EXPECT_EQ(updated.type(), ngl::ACTOR_ROLE);
	EXPECT_EQ(updated.area(), 2);
	EXPECT_EQ(updated.actordataid(), 99);
}

TEST(ToolsTest, NodeIdPackingRoundTripsTidAndTcount)
{
	const auto node = ngl::nnodeid::nodeid(42, 7);
	EXPECT_EQ(ngl::nnodeid::tid(node), 42);
	EXPECT_EQ(ngl::nnodeid::tcount(node), 7);

	const auto invalid = ngl::nnodeid::nodeid(-1, -1);
	EXPECT_EQ(ngl::nnodeid::tid(invalid), -1);
	EXPECT_EQ(ngl::nnodeid::tcount(invalid), -1);
}

TEST(ToolsTest, MapSplicingAppendsFormatterOutput)
{
	const std::map<int, std::string> values = {
		{ 1, "one" },
		{ 2, "two" },
	};

	std::string text;
	const bool ok = ngl::tools::splicing<int, std::string>(
		values,
		"|",
		text,
		[](const int key, const std::string& value) {
			return std::to_string(key) + ":" + value;
		}
	);
	ASSERT_TRUE(ok);

	EXPECT_EQ(text, "1:one|2:two");
}

TEST(ToolsTest, SpliteSpecialStoresParsedPairsInOutputVector)
{
	std::vector<std::pair<std::string, std::string>> values;
	ASSERT_TRUE(ngl::tools::splite_special("[user1:test1][user2:test2]", "\\[", "]", values));

	ASSERT_EQ(values.size(), 2u);
	EXPECT_EQ(values[0].first, "user1");
	EXPECT_EQ(values[0].second, "test1");
	EXPECT_EQ(values[1].first, "user2");
	EXPECT_EQ(values[1].second, "test2");
}

TEST(ToolsTest, SpliteSpecialSupportsTypedPairs)
{
	std::vector<std::pair<int32_t, int32_t>> values;
	ASSERT_TRUE(ngl::tools::splite_special("[1:2][3:4]", "\\[", "]", values));

	ASSERT_EQ(values.size(), 2u);
	EXPECT_EQ(values[0].first, 1);
	EXPECT_EQ(values[0].second, 2);
	EXPECT_EQ(values[1].first, 3);
	EXPECT_EQ(values[1].second, 4);
}

TEST(ToolsTest, SpliteSpecialMapDoesNotMutateOutputOnParseFailure)
{
	std::map<int32_t, int32_t> values = {
		{ 99, 100 },
	};

	EXPECT_FALSE(ngl::tools::splite_special("[1:2][broken]", "\\[", "]", values));
	ASSERT_EQ(values.size(), 1u);
	EXPECT_EQ(values.at(99), 100);
}

TEST(ToolsTest, UrlDecodeHandlesIncompletePercentSequences)
{
	EXPECT_EQ(ngl::tools::url_decode("value%"), "value%");
	EXPECT_EQ(ngl::tools::url_decode("value%2"), "value%2");
	EXPECT_EQ(ngl::tools::url_decode("value%zz"), "value%zz");
}

TEST(ToolsTest, UrlCodecPreservesExistingSafeEscapesAndDecodesUnsafeBytes)
{
	EXPECT_EQ(ngl::tools::url_encode("A z+"), "A%20z%2B");
	EXPECT_EQ(ngl::tools::url_decode("%41%20%2F%22"), "%41 %2F\"");
	EXPECT_EQ(ngl::tools::char2hex(static_cast<char>(0xAF)), "AF");
}

TEST(ToolsTest, GetLineHonorsProvidedBufferLength)
{
	const std::string data = "alpha\r\nbeta\r\ngamma";
	const auto lines = ngl::tools::get_line(data.data(), std::string("alpha\r\nbe").size());

	ASSERT_EQ(lines.size(), 2u);
	EXPECT_EQ(lines[0], "alpha");
	EXPECT_EQ(lines[1], "be");
	EXPECT_TRUE(ngl::tools::get_line(nullptr, 5).empty());
}

TEST(ToolsTest, SpliteRejectsEmptyDelimiterAndHandlesLongerDelimiters)
{
	std::vector<std::string> values;
	EXPECT_FALSE(ngl::tools::splite("alpha", "", values));
	EXPECT_TRUE(values.empty());

	EXPECT_TRUE(ngl::tools::splite("alpha", "alphabet", values));
	ASSERT_EQ(values.size(), 1u);
	EXPECT_EQ(values[0], "alpha");
}

TEST(ToolsTest, SpliteKeepsIntermediateEmptyFieldsAndDropsTrailingDelimiterField)
{
	std::vector<std::string> values;
	ASSERT_TRUE(ngl::tools::splite(",alpha,,beta,", ",", values));

	const std::vector<std::string> expected = {
		"",
		"alpha",
		"",
		"beta",
	};
	EXPECT_EQ(values, expected);
}

TEST(ToolsTest, SpliteVariadicDoesNotMutateOutputsOnFailure)
{
	std::string first = "keep";
	int32_t second = 7;

	EXPECT_FALSE(ngl::tools::splite("only_one_value", ",", first, second));
	EXPECT_EQ(first, "keep");
	EXPECT_EQ(second, 7);
}

TEST(ToolsTest, SpliteContainersDoNotMutateOutputsOnFailure)
{
	std::vector<int32_t> numbers = {
		42,
	};
	std::set<int32_t> unique_numbers = {
		99,
	};

	EXPECT_THROW(ngl::tools::splite("1,two,3", ",", numbers), std::string);
	EXPECT_THROW(ngl::tools::splite("1,two,3", ",", unique_numbers), std::string);

	ASSERT_EQ(numbers.size(), 1u);
	EXPECT_EQ(numbers[0], 42);
	ASSERT_EQ(unique_numbers.size(), 1u);
	EXPECT_TRUE(unique_numbers.contains(99));
}

TEST(ToolsTest, DirectoryListingRecursesAndIgnoresMissingPaths)
{
	const std::filesystem::path root = make_temp_test_dir("dir");
	const std::filesystem::path nested = root / "nested" / "leaf";
	std::filesystem::create_directories(nested);

	{
		std::ofstream(root / "top.txt") << "top";
		std::ofstream(nested / "deep.txt") << "deep";
	}

	std::vector<std::string> files;
	EXPECT_NO_THROW(ngl::tools::dir(root.string(), files, true));

	ASSERT_EQ(files.size(), 2u);
	std::set<std::string> names;
	for (const std::string& file : files)
	{
		names.insert(std::filesystem::path(file).filename().string());
	}
	EXPECT_TRUE(names.contains("top.txt"));
	EXPECT_TRUE(names.contains("deep.txt"));

	std::vector<std::string> missing_files;
	EXPECT_NO_THROW(ngl::tools::dir((root / "missing").string(), missing_files, true));
	EXPECT_TRUE(missing_files.empty());

	std::error_code ec;
	std::filesystem::remove_all(root, ec);
}

TEST(ToolsTest, DirectoryHelpersDistinguishFilesAndDirectories)
{
	const std::filesystem::path root = make_temp_test_dir("path_helpers");
	const std::filesystem::path file = root / "config.txt";
	const std::filesystem::path nested = root / "nested" / "leaf";
	{
		std::ofstream(file) << "data";
	}

	EXPECT_TRUE(ngl::tools::directories_exists(root.string()));
	EXPECT_FALSE(ngl::tools::directories_exists(file.string()));
	EXPECT_TRUE(ngl::tools::file_exists(file.string()));
	EXPECT_FALSE(ngl::tools::file_exists(root.string()));
	EXPECT_TRUE(ngl::tools::create_dir(nested.string()));
	EXPECT_TRUE(ngl::tools::directories_exists(nested.string()));
	EXPECT_FALSE(ngl::tools::create_dir(file.string()));

	std::error_code ec;
	std::filesystem::remove_all(root, ec);
}

TEST(ToolsTest, SplitStrRejectsNullOrEmptyBuffers)
{
	EXPECT_TRUE(ngl::tools::split_str(nullptr, 0).empty());

	char empty_buffer[] = "";
	const auto values = ngl::tools::split_str(empty_buffer, 0);
	EXPECT_TRUE(values.empty());
}

TEST(ToolsTest, SplitStrArrayCopyDoesNotWritePastArrayBounds)
{
	char buffer[] = "one, two, three";
	std::array<const char*, 2> values = {};
	ngl::tools::split_str(buffer, static_cast<int32_t>(sizeof(buffer) - 1), values);

	ASSERT_NE(values[0], nullptr);
	ASSERT_NE(values[1], nullptr);
	EXPECT_STREQ(values[0], "one");
	EXPECT_STREQ(values[1], "two");
}

TEST(ToolsTest, CsvReadVariadicDoesNotMutateOutputsOnFailure)
{
	ngl::csvpair pair;
	pair.m_data = "broken,alice";

	int32_t id = 7;
	std::string name = "keep";

	EXPECT_FALSE(ngl::rcsv::readcsv(pair, id, name));
	EXPECT_EQ(id, 7);
	EXPECT_EQ(name, "keep");
}

TEST(ToolsTest, CsvReadListParsesDelimitedItems)
{
	ngl::csvpair pair;
	pair.m_data = "1*2*3";

	std::list<int32_t> values;
	ASSERT_TRUE((ngl::csv_read<std::list<int32_t>>::read(pair, values)));

	ASSERT_EQ(values.size(), 3u);
	EXPECT_EQ(values.front(), 1);
	EXPECT_EQ(values.back(), 3);
}

TEST(ToolsTest, CsvReaderSkipsCommentLinesWithoutPoisoningNextRow)
{
	const std::filesystem::path root = make_temp_test_dir("csv_reader");
	const std::filesystem::path file = root / "sample.csv";
	{
		std::ofstream(file) << "header1\nheader2\nheader3\n#comment\n1,alpha\n2,beta\n";
	}

	ngl::rcsv reader;
	std::string verify;
	ASSERT_TRUE(reader.read(file.string(), verify));

	std::map<int, CsvTestRow> rows;
	ASSERT_TRUE(reader.readcsv(rows));
	ASSERT_EQ(rows.size(), 2u);
	EXPECT_EQ(rows.at(1).m_name, "alpha");
	EXPECT_EQ(rows.at(2).m_name, "beta");

	std::error_code ec;
	std::filesystem::remove_all(root, ec);
}

TEST(ToolsTest, ReadFileGetMaxlinePreservesReadPosition)
{
	const std::filesystem::path root = make_temp_test_dir("readfile");
	const std::filesystem::path file = root / "data.txt";
	{
		std::ofstream(file) << "line1\nline2\n";
	}

	ngl::readfile reader(file.string());
	ASSERT_TRUE(reader.is_open());

	std::string line;
	ASSERT_TRUE(reader.readline(line));
	EXPECT_EQ(line, "line1");

	EXPECT_EQ(reader.get_maxline(), 2);

	ASSERT_TRUE(reader.readline(line));
	EXPECT_EQ(line, "line2");

	std::error_code ec;
	std::filesystem::remove_all(root, ec);
}

TEST(ToolsTest, ReadFileReadWorksAfterReachingEof)
{
	const std::filesystem::path root = make_temp_test_dir("read_after_eof");
	const std::filesystem::path file = root / "data.txt";
	{
		std::ofstream(file, std::ios::binary) << "line1\nline2\n";
	}

	ngl::readfile reader(file.string());
	ASSERT_TRUE(reader.is_open());

	std::string line;
	ASSERT_TRUE(reader.readline(line));
	ASSERT_TRUE(reader.readline(line));
	EXPECT_FALSE(reader.readline(line));

	std::string content;
	EXPECT_NO_THROW(reader.read(content));
	EXPECT_EQ(content, "line1\nline2\n");

	std::error_code ec;
	std::filesystem::remove_all(root, ec);
}

TEST(ToolsTest, NguidStringRejectsNullAndMalformedInput)
{
	EXPECT_EQ(ngl::tools::nguidstr2int64(nullptr), ngl::nguid::make());
	EXPECT_EQ(ngl::tools::nguidstr2int64("malformed"), ngl::nguid::make());
}

TEST(ToolsTest, JsonRoundTripsIntegralValues)
{
	ngl::ncjson json;
	ASSERT_TRUE((ngl::njson::push(json, { "value", "small", "uvalue" }, int32_t(123), int8_t(-5), uint32_t(456))));

	int32_t value = 0;
	int8_t small = 0;
	uint32_t uvalue = 0;
	ASSERT_TRUE((ngl::njson::pop(json, { "value", "small", "uvalue" }, value, small, uvalue)));
	EXPECT_EQ(value, 123);
	EXPECT_EQ(small, -5);
	EXPECT_EQ(uvalue, 456u);
}

TEST(ToolsTest, JsonPopAcceptsBoolAndDouble)
{
	ngl::ncjson json("{\"flag\":true,\"ratio\":1.25}");
	ASSERT_TRUE(json.parsecheck());

	bool flag = false;
	double ratio = 0.0;
	ASSERT_TRUE((ngl::njson::pop(json, { "flag", "ratio" }, flag, ratio)));
	EXPECT_TRUE(flag);
	EXPECT_DOUBLE_EQ(ratio, 1.25);
}

TEST(ToolsTest, JsonPushCopiesStringStorage)
{
	ngl::ncjson json;
	std::string name = "alpha";
	ASSERT_TRUE((ngl::njson::push(json, { "name" }, name)));
	name[0] = 'z';

	std::string roundtrip;
	ASSERT_TRUE((ngl::njson::pop(json, { "name" }, roundtrip)));
	EXPECT_EQ(roundtrip, "alpha");
}

TEST(ToolsTest, JsonSetPopDoesNotMutateOutputOnFailure)
{
	ngl::ncjson json("{\"values\":[1,1]}");
	ASSERT_TRUE(json.parsecheck());

	std::set<int32_t> values = {
		42,
	};
	EXPECT_FALSE((ngl::njson::pop(json, { "values" }, values)));
	ASSERT_EQ(values.size(), 1u);
	EXPECT_TRUE(values.contains(42));
}

TEST(ToolsTest, JsonCustomRoundTripUsesToolHelpers)
{
	JsonCustomPayload source;
	source.m_value = 77;
	source.m_name = "delta";

	std::string text;
	ASSERT_TRUE(ngl::tools::custom2json(source, text));

	JsonCustomPayload roundtrip;
	ASSERT_TRUE(ngl::tools::json2custom(text, roundtrip));
	EXPECT_EQ(roundtrip.m_value, 77);
	EXPECT_EQ(roundtrip.m_name, "delta");
}

TEST(ToolsTest, XargInfoFindRejectsNullKeys)
{
	ngl::xarg_info info;
	int32_t number = 7;
	std::string text = "keep";
	bool flag = true;

	EXPECT_FALSE(info.find(nullptr, number));
	EXPECT_FALSE(info.find(nullptr, text));
	EXPECT_FALSE(info.find(nullptr, flag));
	EXPECT_EQ(number, 7);
	EXPECT_EQ(text, "keep");
	EXPECT_TRUE(flag);
}

TEST(ToolsTest, SysconfigNodeCountRejectsNullName)
{
	EXPECT_EQ(ngl::sysconfig::node_count(nullptr), 1);
}

TEST(LocaltimeTest, GetMothdayValidatesCalendarDayAgainstTargetMonth)
{
	const time_t non_leap_february = ngl::localtime::str2time("2025-02-10 12:00:00");
	const auto invalid = ngl::localtime::getmothday(non_leap_february, 29, 8, 30, 0);
	EXPECT_FALSE(invalid.first);
	EXPECT_EQ(invalid.second, -1);

	const time_t leap_february = ngl::localtime::str2time("2024-02-10 12:00:00");
	const auto valid = ngl::localtime::getmothday(leap_february, 29, 8, 30, 0);
	ASSERT_TRUE(valid.first);
	EXPECT_EQ(ngl::localtime::time2str(valid.second, "%Y-%m-%d %H:%M:%S"), "2024-02-29 08:30:00");
}

TEST(XmlUtilsTest, NullElementApisFailGracefully)
{
	int number = 0;

	EXPECT_EQ(ngl::xml::get_child(nullptr, "node"), nullptr);
	EXPECT_EQ(ngl::xml::set_child(nullptr, "node"), nullptr);
	EXPECT_FALSE(ngl::xml::get(nullptr, "node", number));
	EXPECT_FALSE(ngl::xml::set(nullptr, "node", 42));
	EXPECT_FALSE(ngl::xml::get_xmlattr(nullptr, "attr", number));
	EXPECT_FALSE(ngl::xml::set_xmlattr(nullptr, "attr", 42));
	EXPECT_FALSE(ngl::xml::foreach(nullptr, "node", [](tinyxml2::XMLElement*) { return true; }));
	EXPECT_FALSE(ngl::xml::foreach(nullptr, [](tinyxml2::XMLElement*) { return true; }));
	EXPECT_FALSE(ngl::xml::foreach_xmlattr(nullptr, [](const char*, const char*) { return true; }));
}

TEST(XmlUtilsTest, NullPathApisFailGracefully)
{
	tinyxml2::XMLDocument document;
	EXPECT_FALSE(ngl::xml::readxml(nullptr, document));
	EXPECT_FALSE(ngl::xml::writexml(nullptr, document));
	EXPECT_EQ(ngl::xml::set_child(document, nullptr), nullptr);
}

TEST(XmlUtilsTest, XmlSetPopDoesNotMutateOutputOnFailure)
{
	tinyxml2::XMLDocument document;
	tinyxml2::XMLElement* root = ngl::xml::set_child(document, "root");
	ASSERT_NE(root, nullptr);

	tinyxml2::XMLElement* first = ngl::xml::set_child(root, "value");
	tinyxml2::XMLElement* second = ngl::xml::set_child(root, "value");
	ASSERT_NE(first, nullptr);
	ASSERT_NE(second, nullptr);
	ASSERT_TRUE(ngl::xml::set(first, 1));
	ASSERT_TRUE(ngl::xml::set(second, 1));

	std::set<int32_t> values = {
		42,
	};
	EXPECT_FALSE((ngl::xml_serialize<false, std::set<int32_t>>::pop(root, "value", values)));
	ASSERT_EQ(values.size(), 1u);
	EXPECT_TRUE(values.contains(42));
}
