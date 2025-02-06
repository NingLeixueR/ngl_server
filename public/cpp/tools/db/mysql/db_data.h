#pragma once

#include "serialize.h"
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

		// # ���س�id ��ֹ�ڴ洩͸
		static std::set<int64_t>			m_idindex;
	public:
		// # ���ݵ���������
		static const std::set<int64_t>& id_index()
		{
			return m_idindex;
		}

		static void set_index(int64_t aid)
		{
			m_idindex.insert(aid);
		}

		static void set_index(std::set<int64_t>& asetid)
		{
			m_idindex = asetid;
		}

		static void erase_index(int64_t aid)
		{
			m_idindex.erase(aid);
		}

		// # ��������
		static bool set(i64_actorid aid, const T& adata)
		{
			m_data[aid] = adata;
			set_index(aid);
			return true;		
		}

		// # �Ƴ�����
		static void remove(i64_actorid aid)
		{
			m_data.erase(aid);
			erase_index(aid);
		}

		// # �Ƴ�һ������
		static void remove(const std::set<i64_actorid>& aid)
		{
			for (i64_actorid id : aid)
			{
				m_data.erase(id);
				erase_index(id);
			}				
		}

		// # �Ƴ�һ������
		static void remove(const std::vector<i64_actorid>& aid)
		{
			for (i64_actorid id : aid)
			{
				m_data.erase(id);
				erase_index(id);
			}
		}

		// # �Ƿ����ĳ������
		enum edbdata
		{
			edbdata_notload,	// û�м���
			edbdata_load,		// �Ѽ���
			edbdata_notdata,	// û������
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

		static T* find(i64_actorid aid)
		{
			auto itor = m_data.find(aid);
			if (itor == m_data.end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		static bool get(i64_actorid aid, T& adata)
		{
			auto itor = m_data.find(aid);
			if (itor == m_data.end())
			{
				return false;
			}
			adata = itor->second;
			return true;
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


