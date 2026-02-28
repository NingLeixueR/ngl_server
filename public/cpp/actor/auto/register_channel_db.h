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
