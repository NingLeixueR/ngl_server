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

#include "nserialize.h"
#include "type.h"
#include "db.h"

#include <set>

namespace ngl
{
	template <typename T>
	struct db_data
	{
		db_data() = delete;
		db_data(const db_data&) = delete;
		db_data& operator=(const db_data&) = delete;
	private:
		static std::map<i64_actorid, T>		m_data;
		// # 加载出id 防止内存穿透
		static std::set<int64_t>			m_idindex;
	public:
		// # 获取数据的所有索引
		static const std::set<int64_t>& const_id_index()
		{
			return m_idindex;
		}

		// # 获取数据的所有索引
		static std::set<int64_t>& id_index()
		{
			return m_idindex;
		}

		// # 设置数据索引
		static void set_index(int64_t aid)
		{
			m_idindex.insert(aid);
		}

		// # 删除数据索引
		static void erase_index(int64_t aid)
		{
			m_idindex.erase(aid);
		}

		// # 设置数据
		static bool set(i64_actorid aid, const T& adata)
		{
			m_data[aid] = adata;
			set_index(aid);
			return true;		
		}

		// # 移除数据
		static void remove(i64_actorid aid)
		{
			m_data.erase(aid);
			erase_index(aid);
		}

		// # 移除一组数据
		static void remove(const std::set<i64_actorid>& aid)
		{
			for (i64_actorid id : aid)
			{
				m_data.erase(id);
				erase_index(id);
			}				
		}

		// # 移除一组数据
		static void remove(const std::vector<i64_actorid>& aid)
		{
			for (i64_actorid id : aid)
			{
				m_data.erase(id);
				erase_index(id);
			}
		}

		// # 是否加载某个数据
		enum edbdata
		{
			edbdata_notload,	// 没有加载
			edbdata_load,		// 已加载
			edbdata_notdata,	// 没有数据
		};
		static edbdata data_stat(i64_actorid aid)
		{
			auto itor = m_idindex.find(aid);
			if (itor == m_idindex.end())
			{
				return edbdata_notdata;
			}
			if (m_data.find(aid) == m_data.end())
			{
				return edbdata_notload;
			}
			return edbdata_load;
		}

		// # 查找指定数据
		static T* find(i64_actorid aid)
		{
			auto itor = m_data.find(aid);
			if (itor == m_data.end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		static bool add(i64_actorid aid, T& adata)
		{
			auto itor = m_data.find(aid);
			if (itor != m_data.end())
			{
				return false;
			}
			T& ldata = m_data[aid];
			ldata.set_mid(aid);
			ldata = adata;
			set_index(aid);
			return &ldata;
		}

		static void foreach(const std::function<void(T&)>& afun)
		{
			for (std::pair<const i64_actorid, T>& item : m_data)
			{
				afun(item.second);
			}
		}

		static void foreach_index(const std::function<void(int, T&)>& afun)
		{
			int lindex = 0;
			for (std::pair<const i64_actorid, T>& lpair : m_data)
			{
				afun(++lindex, lpair.second);
			}				
		}

		static void foreach_index(int32_t aindexbeg, int32_t aindexend, const std::function<void(int32_t, T&)>& afun)
		{
			int lindex = 0;
			for (std::pair<const i64_actorid, T>& lpair : m_data)
			{
				++lindex;
				if (lindex >= aindexbeg && lindex < aindexend)
				{
					afun(lindex, lpair.second);
				}
				if (lindex > aindexend)
				{
					return;
				}
			}
		}
	};

	template <typename T>
	std::map<i64_actorid, T> db_data<T>::m_data;

	template <typename T>
	std::set<int64_t> db_data<T>::m_idindex;
}// namespace ngl


