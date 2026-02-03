/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/

#pragma once

#include "tools.h"

#include <unordered_map>
#include <iostream>
#include <utility>
#include <string>
#include <vector>
#include <locale>
#include <queue>

namespace ngl
{

    // 修正AC节点结构：用哈希表存储子节点（适配中文等任意字符）
    template <typename TCHAR>
    struct nacnode
    {
        std::unordered_map<TCHAR, int> m_children;  // 宽字符 -> 子节点索引
        int m_fail = -1;
        int len = 0;  // 模式串长度，0表示非结尾
    };

    template <typename TSTRING>
    class nfilterword
    {
    private:
        using TCHAR = typename TSTRING::value_type;

        std::vector<nacnode<TCHAR>> m_nodes;
        int m_root = 0;

        // 创建新节点
        int newnode()
        {
            m_nodes.emplace_back();
            return m_nodes.size() - 1;
        }

        nfilterword() = default;
    public:       
        static nfilterword<TSTRING>& instance()
        {
            static nfilterword<TSTRING> ltemp;
            return ltemp;
        }

        void init()
        {
            m_root = newnode();
        }

        void clear()
        {
            m_nodes.clear();
            m_root = 0;
        }

        // 插入模式串（支持任意宽字符：中文/英文/符号）
        void load(const TSTRING& apattern)
        {
            if (apattern.empty())
            {
                return;
            }
            int cur = m_root;

            for (auto c : apattern)
            {  // 遍历宽字符（中文每个字是一个wchar_t）
                // 若当前字符的子节点不存在，则创建
                auto& ltemp = m_nodes[cur].m_children;
                if (!ltemp.contains(c))
                {
                    m_nodes[cur].m_children[c] = newnode();
                }
                cur = m_nodes[cur].m_children[c];
            }
            m_nodes[cur].len = apattern.size();  // 标记模式串结束，记录长度
        }

        // 构建失败指针（BFS，逻辑适配哈希表）
        void build()
        {
            std::queue<int> q;
            // 初始化根节点的子节点：失败指针指向根
            for (auto& pair : m_nodes[m_root].m_children)
            {
                int child = pair.second;
                m_nodes[child].m_fail = m_root;
                q.push(child);
            }

            // BFS遍历所有节点
            while (!q.empty())
            {
                int p = q.front();
                q.pop();

                // 遍历当前节点的所有子节点
                for (auto& pair : m_nodes[p].m_children)
                {
                    auto c = pair.first;   // 当前字符
                    int u = pair.second;      // 当前子节点索引
                    int fail_p = m_nodes[p].m_fail;  // 父节点的失败指针

                    // 回溯失败指针：直到找到包含当前字符的节点，或根节点
                    while (fail_p != -1 && !m_nodes[fail_p].m_children.contains(c))
                    {
                        fail_p = m_nodes[fail_p].m_fail;
                    }

                    // 设置当前节点的失败指针
                    m_nodes[u].m_fail = (fail_p == -1) ? m_root : m_nodes[fail_p].m_children[c];
                    q.push(u);
                }
            }
        }

        bool match(TCHAR c, int& cur, int i, std::vector<std::pair<int, int>>& res)
        {
            // 失配回退：沿失败指针找匹配的节点
            while (cur != m_root && !m_nodes[cur].m_children.contains(c))
            {
                cur = m_nodes[cur].m_fail;
            }

            // 匹配成功则移动到子节点，否则留在根节点
            if (m_nodes[cur].m_children.contains(c))
            {
                cur = m_nodes[cur].m_children[c];
            }

            // 收集所有匹配的模式串（遍历失败链，处理嵌套匹配）
            int temp = cur;
            while (temp != m_root)
            {
                if (m_nodes[temp].len > 0)
                {
                    int start = i - m_nodes[temp].len + 1;  // 计算起始位置
                    res.emplace_back(start, m_nodes[temp].len);
                }
                temp = m_nodes[temp].m_fail;
            }
            return !res.empty();
        }

        // 匹配文本，返回所有匹配的<起始位置, 模式串长度>
        std::vector<std::pair<int, int>> match(const TSTRING& text)
        {
            std::vector<std::pair<int, int>> res;
            int cur = m_root;

            // 遍历宽字符文本（中文每个字是一个元素）
            for (int i = 0; i < text.size(); ++i) 
            {
                match(text[i], cur, i, res);
            }
            return res;
        }

        // 辅助函数：过滤文本（将匹配的字符替换为*）
        TSTRING filter(const TSTRING& text)
        {
            TSTRING result = text;
            std::vector<std::pair<int, int>> matches = match(text);
            // 替换所有匹配的字符为*
            for (auto& [start, len] : matches)
            {
                for (int i = start; i < start + len; ++i)
                {
                    result[i] = '*';
                }
            }
            return result;
        }

        // 是否存在屏蔽字
        bool isfilter(const TSTRING& text)
        {
            int cur = m_root;
            std::vector<std::pair<int, int>> res;
            for (int i = 0; i < text.size(); ++i)
            {
                if (match(text[i], cur, i, res))
                {
                    return true;
                }
            }
            return false;
        }
    };

    // 测试示例
    void test_nfilterword();
}//namespace ngl