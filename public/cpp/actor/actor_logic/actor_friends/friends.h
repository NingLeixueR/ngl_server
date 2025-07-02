#pragma once

#include "ttab_specialid.h"
#include "nactor_auto.h"
#include "manage_csv.h"
#include "localtime.h"

#include <string>
#include <map>

namespace ngl
{
	class friends : 
		public tdb_friends::db_modular
	{
		friends(const friends&) = delete;
		friends& operator=(const friends&) = delete;

	public:
		friends() = default;

		void related_actorid()final
		{
			set_actorid(nguid::make());
		}

		void initdata()final
		{
			log_error()->print("friends###loaddb_finish {}", data());
		}

		bool check_friends(
			const google::protobuf::RepeatedField<i64_actorid>& avec1, 
			const google::protobuf::RepeatedField<i64_actorid>& avec2, 
			i64_actorid aroleid
		)
		{
			return std::find(avec1.begin(), avec1.end(), aroleid) != avec1.end() || std::find(avec2.begin(), avec2.end(), aroleid) != avec2.end();
		}

		// 请求添加好友
		int addfriends(i64_actorid aroleid, i64_actorid afriends)
		{
			data_modified<pbdb::db_friends>& lfriends = get(aroleid);
			const pbdb::db_friends& lconstfriends = lfriends.getconst();
			if (lconstfriends.mfriends_size() >= ttab_specialid::instance().m_friendscount)
			{
				return 1;
			}
			if (lconstfriends.mapplyfriends_size() >= ttab_specialid::instance().m_friendsapplylistcount)
			{
				return 2;
			}
			auto& lfriendslist = lconstfriends.mfriends();
			auto& lapplyfriends = lconstfriends.mapplyfriends();
			if (check_friends(lfriendslist, lapplyfriends, afriends))
			{
				return 3;
			}
			if (check_friends(lfriendslist, lapplyfriends, aroleid))
			{
				return 4;
			}
			lfriends.get().add_mapplyfriends(aroleid);
			return 5;
		}

		// 同意/拒绝好友申请
		int ratifyfriends(i64_actorid aroleid, i64_actorid afriends, bool aratify)
		{
			data_modified<pbdb::db_friends>& lfriends1 = get(aroleid);
			data_modified<pbdb::db_friends>& lfriends2 = get(afriends);
			if (lfriends1.getconst().mfriends_size() >= ttab_specialid::instance().m_friendscount)
			{
				return 1;
			}
			if (lfriends2.getconst().mfriends_size() >= ttab_specialid::instance().m_friendscount)
			{
				return 2;
			}
			
			auto lpapplyfriends = lfriends1.get().mutable_mapplyfriends();
			auto itor = std::find(lpapplyfriends->begin(), lpapplyfriends->end(), afriends);
			if (itor == lpapplyfriends->end())
			{
				return 3;
			}
			lpapplyfriends->erase(itor);
			if (aratify)
			{
				lfriends1.get().add_mfriends(afriends);
				lfriends2.get().add_mfriends(afriends);
			}
			return 0;
		}

		// 删除好友
		int erasefriends(i64_actorid aroleid, i64_actorid afriends)
		{
			data_modified<pbdb::db_friends>& lfriends1 = get(aroleid);
			data_modified<pbdb::db_friends>& lfriends2 = get(afriends);
			auto lpfriends1 = lfriends1.get().mutable_mfriends();
			auto itor = std::find(lpfriends1->begin(), lpfriends1->end(), afriends);
			if (itor == lpfriends1->end())
			{
				return 1;
			}
			lpfriends1->erase(itor);

			auto lpfriends2 = lfriends2.get().mutable_mfriends();
			auto itor2 = std::find(lpfriends2->begin(), lpfriends2->end(), afriends);
			if (itor == lpfriends2->end())
			{
				return 2;
			}
			lpfriends2->erase(itor);
			return 0;
		}

		// 同步好友信息
		void syncfriends(i64_actorid aroleid)
		{
			auto pro = std::make_shared<pbnet::PROBUFF_NET_FRIEND_RESPONSE>();
			data_modified<pbdb::db_friends>& lfriends = get(aroleid);
			for (i64_actorid afriends : lfriends.getconst().mfriends())
			{
				const pbdb::db_brief* lpbrief = tdb_brief::nsp_cli<actor_friends>::instance(get_actor()->id_guid()).getconst(afriends);
				if (lpbrief == nullptr)
				{
					return;
				}
				*pro->add_mfriends() = *lpbrief;
			}

			for (i64_actorid afriends : lfriends.getconst().mapplyfriends())
			{
				const pbdb::db_brief* lpbrief = tdb_brief::nsp_cli<actor_friends>::instance(get_actor()->id_guid()).getconst(afriends);
				if (lpbrief == nullptr)
				{
					return;
				}
				*pro->add_mapplyfriends() = *lpbrief;
			}
			actor::send_client(aroleid, pro);
		}

		// 获取好友
		bool get_friends(i64_actorid aroleid, std::vector<i64_actorid>& afriends)
		{
			data_modified<pbdb::db_friends>& lfriends = get(aroleid);
			for (i64_actorid aactorid : lfriends.getconst().mfriends())
			{
				afriends.push_back(aactorid);
			}
			return !afriends.empty();
		}
	};
}// namespace ngl

mk_formatter(pbdb::db_friends)

