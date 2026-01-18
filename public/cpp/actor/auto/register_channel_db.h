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

namespace ngl
{
	struct reg_channel_db
	{
		template <typename T>
		static void func()
		{
			tprotocol::tp_customs<
				np_channel_data<T>
				, np_channel_register<T>
				, np_channel_register_reply<T>
				, np_channel_exit<T>
				, np_channel_dataid_sync<T>
			>(-1, 1);

			tprotocol::tp_customs<
				np_channel_check<T>
			>(-1, 0);
		}

		template <typename ...TAGS>
		static void funcs()
		{
			(func<TAGS>(), ...);
		}
	};

	void register_channel_db()
	{

		tprotocol::set_customs_index(110000000);
	reg_channel_db::funcs<
          pbdb::dbcross_test
          ,pbdb::db_account
          ,pbdb::db_activity
          ,pbdb::db_activitytimes
          ,pbdb::db_bag
          ,pbdb::db_brief
          ,pbdb::db_family
          ,pbdb::db_familyer
          ,pbdb::db_friends
          ,pbdb::db_keyvalue
          ,pbdb::db_mail
          ,pbdb::db_notice
          ,pbdb::db_ranklist
          ,pbdb::db_role
          ,pbdb::db_rolekeyvalue
          ,pbdb::db_task
          ,pbdb::db_testlua
		>();
	}
}//namespace ngl
