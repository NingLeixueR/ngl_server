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
 // ע�⡾makeproto ���������ļ�����Ҫ�ֶ��޸ġ�
 // ����ʱ�䡾2025-10-31 17:14:08��
#include "auto_actor_enum.h"
#include "nprotocol_auto.h"
#include "nactor_auto.h"

namespace ngl
{
	void tdb::tdb_init(bool ainstance)
	{
		tdb_account::init(ainstance);
		tdb_activity::init(ainstance);
		tdb_activitytimes::init(ainstance);
		tdb_bag::init(ainstance);
		tdb_brief::init(ainstance);
		tdb_family::init(ainstance);
		tdb_familyer::init(ainstance);
		tdb_friends::init(ainstance);
		tdb_keyvalue::init(ainstance);
		tdb_mail::init(ainstance);
		tdb_notice::init(ainstance);
		tdb_ranklist::init(ainstance);
		tdb_role::init(ainstance);
		tdb_rolekv::init(ainstance);
		tdb_task::init(ainstance);
		tdb_testlua::init(ainstance);
    }
}//namespace ngl