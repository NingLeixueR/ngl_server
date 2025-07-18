#pragma once

#include "db.h"

namespace ngl
{
	class db_pool
	{
		db_pool(const db_pool&) = delete;
		db_pool& operator=(const db_pool&) = delete;

		std::vector<db*> m_vec;

		db_pool()
		{}
	public:
		static db_pool& instance()
		{
			static db_pool ltemp;
			return ltemp;
		}
		// # ��ʼ��db���ӳ�:Ŀǰ���ӳ� ֻ֧�ֵ����ã�
		// # ����ζ�ŵ�������ֻ����һ�����ݿ���������
		void init(const dbarg& adbarg);

		// # �����߳�id��ȡ���ж�Ӧ������
		db* get(int32_t aindex);
	};
}// namespace ngl
