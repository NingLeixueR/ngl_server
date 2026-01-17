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
			, pbdb::db_account
		>();
		/*_reister_channel_db<pbdb::dbcross_test>();
		_reister_channel_db<pbdb::db_account>();
		_reister_channel_db<pbdb::db_activity>();
		_reister_channel_db<pbdb::db_activitytimes>();
		_reister_channel_db<pbdb::db_bag>();
		_reister_channel_db<pbdb::db_brief>();
		_reister_channel_db<pbdb::db_family>();
		_reister_channel_db<pbdb::db_familyer>();
		_reister_channel_db<pbdb::db_friends>();
		_reister_channel_db<pbdb::db_keyvalue>();
		_reister_channel_db<pbdb::db_mail>();
		_reister_channel_db<pbdb::db_notice>();
		_reister_channel_db<pbdb::db_ranklist>();
		_reister_channel_db<pbdb::db_role>();
		_reister_channel_db<pbdb::db_rolekeyvalue>();
		_reister_channel_db<pbdb::db_task>();
		_reister_channel_db<pbdb::db_testlua>();*/
	}
}//namespace ngl
