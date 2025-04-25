#pragma once

#include "nactor_auto.h"
#include "remakes.h"
#include "db.pb.h"
#include "itme.h"

namespace ngl
{
	struct autoitem;

	class bag :
		public tdb_bag::db_modular
	{
		std::map<int, pbdb::item*> m_stackitems;				// key tid �ɶѵ�����Ʒ
		std::map<int, pbdb::item*> m_nostackitems;				// key id  ���ɶѵ�����Ʒ
		std::unique_ptr<autoitem> m_autoitem;
	public:
		bag();

		pbdb::db_bag& get_bag();

		const pbdb::db_bag& get_constbag();

		virtual void initdata();

		pbdb::item* add(pbdb::item& aitem);
	
		bool add_item(int32_t atid, int32_t acount);
		bool add_item(const std::map<int32_t, int32_t>& amap);
		bool add_item(std::vector<pbdb::item>& avec);

		// ֻ��ɾ���ɶѵ���Ʒ
		bool dec_item(int32_t atid, int32_t acount);
		// ɾ�����ɶѵ���Ʒ
		bool dec_item(int32_t aid);
		
		bool checkbytid(int32_t atid, int32_t acount);
		bool checkbyid(int32_t aid);

		void sync_client();
	};
}// namespace ngl