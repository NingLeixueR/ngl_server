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
		>(EPROTOCOL_TYPE_CUSTOM);
	}

	void reister_channel_db()
	{
		tprotocol::set_customs_index(110000000);
		_reister_channel_db<pbdb::dbcross_test>();
		tprotocol::set_customs_index(110000010);
		_reister_channel_db<pbdb::db_account>();
		tprotocol::set_customs_index(110000020);
		_reister_channel_db<pbdb::db_activity>();
		tprotocol::set_customs_index(110000030);
		_reister_channel_db<pbdb::db_activitytimes>();
		tprotocol::set_customs_index(110000040);
		_reister_channel_db<pbdb::db_bag>();
		tprotocol::set_customs_index(110000050);
		_reister_channel_db<pbdb::db_brief>();
		tprotocol::set_customs_index(110000060);
		_reister_channel_db<pbdb::db_family>();
		tprotocol::set_customs_index(110000070);
		_reister_channel_db<pbdb::db_familyer>();
		tprotocol::set_customs_index(110000080);
		_reister_channel_db<pbdb::db_friends>();
		tprotocol::set_customs_index(110000090);
		_reister_channel_db<pbdb::db_keyvalue>();
		tprotocol::set_customs_index(110000100);
		_reister_channel_db<pbdb::db_mail>();
		tprotocol::set_customs_index(110000110);
		_reister_channel_db<pbdb::db_notice>();
		tprotocol::set_customs_index(110000120);
		_reister_channel_db<pbdb::db_ranklist>();
		tprotocol::set_customs_index(110000130);
		_reister_channel_db<pbdb::db_role>();
		tprotocol::set_customs_index(110000140);
		_reister_channel_db<pbdb::db_rolekeyvalue>();
		tprotocol::set_customs_index(110000150);
		_reister_channel_db<pbdb::db_task>();
	}
}//namespace ngl
