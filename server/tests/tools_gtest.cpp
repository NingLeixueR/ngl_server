#include <gtest/gtest.h>

#include <map>
#include <string>

#include "actor/actor_base/nguid.h"
#include "tools/tab/xml/xml.h"
#include "tools/tools.h"

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
