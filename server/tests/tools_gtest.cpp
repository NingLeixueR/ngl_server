#include <gtest/gtest.h>

#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "actor/actor_base/nguid.h"
#include "tools/localtime.h"
#include "tools/operator_file.h"
#include "tools/tab/xml/xml.h"
#include "tools/tools.h"

namespace
{
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
	EXPECT_THROW(ngl::tools::lexical_cast<int32_t>(std::string("")), std::string);
	EXPECT_THROW(ngl::tools::lexical_cast<int32_t>(std::string("+")), std::string);
	EXPECT_THROW(ngl::tools::lexical_cast<uint32_t>(std::string("-1")), std::string);
	EXPECT_THROW(ngl::tools::lexical_cast<float>(std::string(".")), std::string);
	EXPECT_THROW(ngl::tools::lexical_cast<float>(std::string("1.2.3")), std::string);
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
