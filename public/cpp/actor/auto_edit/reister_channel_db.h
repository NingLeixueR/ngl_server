namespace ngl
{
	template <typename T>
	void _reister_channel_db()
	{
		tprotocol::tp_customs::template func <
			np_channel_register<T>
			, np_channel_register_reply<T>
			, np_channel_data<T>
			, np_channel_exit<T>
			, np_channel_check<T>
			, np_channel_dataid_sync<T>
		>();
	}

	void reister_channel_db()
	{
		tprotocol::set_customs_index(110000000);
		_reister_channel_db<pbdb::dbcross_test>();
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
	}
}//namespace ngl
