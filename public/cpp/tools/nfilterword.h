/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Declares interfaces for tools.


#pragma once

#include "utf8cpp/utf8/cpp11.h"
#include "utf8cpp/utf8.h"
#include "tools/tools.h"

#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <utility>
#include <string>
#include <vector>
#include <locale>
#include <queue>

namespace ngl
{
    // One trie node in the Aho-Corasick automaton used for sensitive-word matching.
    struct nacnode
    {
        std::unordered_map<char, int> m_children;  // Wide characters -> nodeindex
        int m_fail = -1;
        int len = 0;  // Match length ending at this node; 0 means "no terminal word here".
        bool m_has_match = false; // True when this node or one of its output links terminates a word.
    };

    class nfilterword
    {
    private:
        static std::vector<nacnode> m_nodes;
        static int m_root;
        static void ensure_initialized();
        static void advance_state(char c, int& cur);
        static bool append_matches(int cur, int i, std::vector<std::pair<int, int>>& res);
        static bool match_exists(char c, int& cur);

        // Create node
        static int newnode()
        {
            m_nodes.emplace_back();
            return static_cast<int>(m_nodes.size() - 1);
        }

        nfilterword() = delete;
        nfilterword(const nfilterword&) = delete;
        nfilterword(const nfilterword&&) = delete;
    public:
        // Build/reset the automaton, load patterns, and query/filter text.
        static void init();

        // Cleanup.
        static void clear();

        // Insert one UTF-8 pattern into the trie.
        static void load(const std::string& apattern);

        // Build fail links and output links after all patterns are loaded.
        static void build();

        static bool match(char c, int& cur, int i, std::vector<std::pair<int, int>>& res);

        // Return every match as <start, length>.
        static std::vector<std::pair<int, int>> match(const std::string& text);

        // Replace each matched byte range with '*'.
        static std::string filter(const std::string& text);

        // Function: 8convert 32
        static bool utf8to32(const std::string& text1, std::u32string& text2);

        // Function: 32convert 8
        static bool utf32to8(const std::u32string& text1, std::string& text2);

        // Whether
        static bool is_filter(const std::string& text);

        // ASCII,
        static int32_t charcount(const std::u32string& astr);

        enum enfilter
        {
            enfilter_success,               // Successful
            enfilter_invalid_utf8,          // :UTF-8
            enfilter_emojispecial,          // : Special
            enfilter_charcount,             // Translated comment.
            enfilter_filter,                // Translated comment.
        };

        static const char* enfilter_message(enfilter astat);

        // Validate a user-facing name against UTF-8, special-character, length, and filter rules.
        static enfilter check_name(const std::string& astr, int32_t amincount, int32_t amaxcount);

        // Function: check whether 「 」
        static bool is_language_char(uint32_t codepoint);

        // Function: check whether 「 」
        static bool is_emojispecial(char32_t codepoint);
       
        // Whetherpack special
        static bool is_emojispecial(const std::u32string& astr);
    };

    void test_nfilterword();
}//namespace ngl
