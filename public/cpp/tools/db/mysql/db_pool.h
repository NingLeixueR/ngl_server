#pragma once

#include "db.h"

namespace ngl
{
	class db_pool
	{
		db_pool() = delete;
		db_pool(const db_pool&) = delete;
		db_pool& operator=(const db_pool&) = delete;

		static std::vector<db*> m_vec;
	public:
		// # ��ʼ��db���ӳ�:Ŀǰ���ӳ� ֻ֧�ֵ����ã�
		// # ����ζ�ŵ�������ֻ����һ�����ݿ���������
		static void init(const dbarg& adbarg);

		// # �����߳�id��ȡ���ж�Ӧ������
		static db* get(int32_t aindex);
	};
}// namespace ngl
