#include "server_main.h"
#include "dumper.h"
#include "type.h"

#include <iostream>
#include <format>

Dumper lDumper;
//
//#include <iostream>
//#include <string>
//#include <vector>
//#include <queue>
//#include <cstring>
//#include <utility>
//using namespace std;
//#include "tools.h"
//
//#include <iostream>
//#include <string>
//#include <vector>
//#include <queue>
//#include <unordered_map>
//#include <locale>  // 用于设置本地化编码
//#include <utility>
//using namespace std;
//
//// 修正AC节点结构：用哈希表存储子节点（适配中文等任意字符）
//struct nacnode 
//{
//    unordered_map<wchar_t, int> m_children;  // 宽字符 -> 子节点索引
//    int m_fail = -1;
//    int len = 0;  // 模式串长度，0表示非结尾
//};
//
//class nfilterword 
//{
//private:
//    vector<nacnode> nodes;
//    int root;
//
//    // 创建新节点
//    int newNode() 
//    {
//        nodes.emplace_back();
//        return nodes.size() - 1;
//    }
//public:
//    nfilterword() 
//    {
//        root = newNode();
//    }
//
//    // 插入模式串（支持任意宽字符：中文/英文/符号）
//    void load(const wstring& pattern) 
//    {
//        if (pattern.empty()) return;
//        int cur = root;
//        for (wchar_t c : pattern) 
//        {  // 遍历宽字符（中文每个字是一个wchar_t）
//            // 若当前字符的子节点不存在，则创建
//            if (nodes[cur].m_children.find(c) == nodes[cur].m_children.end()) 
//            {
//                nodes[cur].m_children[c] = newNode();
//            }
//            cur = nodes[cur].m_children[c];
//        }
//        nodes[cur].len = pattern.size();  // 标记模式串结束，记录长度
//    }
//
//    // 构建失败指针（BFS，逻辑适配哈希表）
//    void build() 
//    {
//        queue<int> q;
//        // 初始化根节点的子节点：失败指针指向根
//        for (auto& pair : nodes[root].m_children) 
//        {
//            int child = pair.second;
//            nodes[child].m_fail = root;
//            q.push(child);
//        }
//
//        // BFS遍历所有节点
//        while (!q.empty()) 
//        {
//            int p = q.front();
//            q.pop();
//
//            // 遍历当前节点的所有子节点
//            for (auto& pair : nodes[p].m_children) 
//            {
//                wchar_t c = pair.first;   // 当前字符
//                int u = pair.second;      // 当前子节点索引
//                int fail_p = nodes[p].m_fail;  // 父节点的失败指针
//
//                // 回溯失败指针：直到找到包含当前字符的节点，或根节点
//                while (fail_p != -1 && nodes[fail_p].m_children.find(c) == nodes[fail_p].m_children.end()) 
//                {
//                    fail_p = nodes[fail_p].m_fail;
//                }
//
//                // 设置当前节点的失败指针
//                nodes[u].m_fail = (fail_p == -1) ? root : nodes[fail_p].m_children[c];
//                q.push(u);
//            }
//        }
//    }
//
//    // 匹配文本，返回所有匹配的<起始位置, 模式串长度>
//    vector<pair<int, int>> match(const wstring& text) 
//    {
//        vector<pair<int, int>> res;
//        int cur = root;
//
//        // 遍历宽字符文本（中文每个字是一个元素）
//        for (int i = 0; i < text.size(); ++i) {
//            wchar_t c = text[i];
//            // 失配回退：沿失败指针找匹配的节点
//            while (cur != root && nodes[cur].m_children.find(c) == nodes[cur].m_children.end()) {
//                cur = nodes[cur].m_fail;
//            }
//
//            // 匹配成功则移动到子节点，否则留在根节点
//            if (nodes[cur].m_children.find(c) != nodes[cur].m_children.end()) {
//                cur = nodes[cur].m_children[c];
//            }
//
//            // 收集所有匹配的模式串（遍历失败链，处理嵌套匹配）
//            int temp = cur;
//            while (temp != root) 
//            {
//                if (nodes[temp].len > 0) 
//                {
//                    int start = i - nodes[temp].len + 1;  // 计算起始位置
//                    res.emplace_back(start, nodes[temp].len);
//                }
//                temp = nodes[temp].m_fail;
//            }
//        }
//        return res;
//    }
//
//    // 辅助函数：过滤文本（将匹配的字符替换为*）
//    wstring filter(const wstring& text) 
//    {
//        wstring result = text;
//        vector<pair<int, int>> matches = match(text);
//        // 替换所有匹配的字符为*
//        for (auto& [start, len] : matches) 
//        {
//            for (int i = start; i < start + len; ++i) 
//            {
//                result[i] = L'*';
//            }
//        }
//        return result;
//    }
//};

int main(int argc, char** argv) 
{

	if (argc >= 3)
	{
		Dumper::m_excname = std::format("node_{}", argv[1]);
		lDumper.setDumperHandler(dump_logic("core dump"));
		return ngl_main(argc, argv);
	}
	std::cout << "参数错误:EXE name areaid tab_servers::tcount" << std::endl;
	return 0;
}