/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "nactor_auto.h"
#include "manage_csv.h"
#include "db.pb.h"
#include "net.pb.h"

namespace ngl
{
	class roleinfo : 
		public tdb_role::db_modular
	{
		roleinfo(const roleinfo&) = delete;
		roleinfo& operator=(const roleinfo&) = delete;
	public:
		roleinfo() 
		{}

		~roleinfo(){}

		virtual void initdata();

		const i64_actorid roleid();

		pbdb::db_brief* get_brief();
		const pbdb::db_brief* get_constbrief();

		void change_event(eevents_logic type, int abegvalue, int32_t anowvalue);

		// # �ȼ�
		int32_t lv();
		void change_lv(int avalues);

		// # vip
		int32_t vip();
		void change_vip(int avalues);

		// # ����
		const char* name();
		void rename(const char* aname);

		// # ���
		int32_t gold();
		void change_gold(int avalues);

		// # ����
		int32_t silver();
		void change_silver(int avalues);

		// # ����
		int32_t notalkutc();
		void change_notalkutc(int avalues);
		bool bantalk();
	};
}//namespace ngl

mk_formatter(pbdb::db_role)