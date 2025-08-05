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
			const pbdb::db_friends* lpfriendsconst = lfriends.getconst();
			if (lpfriendsconst == nullptr)
			{
				return 1;
			}
			if (lpfriendsconst->mfriends_size() >= ttab_specialid::instance().m_friendscount)
			{
				return 2;
			}
			if (lpfriendsconst->mapplyfriends_size() >= ttab_specialid::instance().m_friendsapplylistcount)
			{
				return 3;
			}
			auto& lfriendslist = lpfriendsconst->mfriends();
			auto& lapplyfriends = lpfriendsconst->mapplyfriends();
			if (check_friends(lfriendslist, lapplyfriends, afriends))
			{
				return 4;
			}
			if (check_friends(lfriendslist, lapplyfriends, aroleid))
			{
				return 5;
			}
			pbdb::db_friends* lpfriends = lfriends.get();
			if (lpfriends == nullptr)
			{
				return 6;
			}
			lpfriends->add_mapplyfriends(aroleid);
			return 0;
		}

		// 同意/拒绝好友申请
		int ratifyfriends(i64_actorid aroleid, i64_actorid afriends, bool aratify)
		{
			data_modified<pbdb::db_friends>& lfriends1 = get(aroleid);
			data_modified<pbdb::db_friends>& lfriends2 = get(afriends);
			const pbdb::db_friends* lpfriendsconst1 = lfriends1.getconst();
			const pbdb::db_friends* lpfriendsconst2 = lfriends2.getconst();
			if (lpfriendsconst1 == nullptr || lpfriendsconst2 == nullptr)
			{
				return 1;
			}
			if (lpfriendsconst1->mfriends_size() >= ttab_specialid::instance().m_friendscount)
			{
				return 3;
			}
			if (lpfriendsconst2->mfriends_size() >= ttab_specialid::instance().m_friendscount)
			{
				return 4;
			}

			pbdb::db_friends* lpfriends1 = lfriends1.get();
			pbdb::db_friends* lpfriends2 = lfriends2.get();
			if (lpfriends1 == nullptr || lpfriends2 == nullptr)
			{
				return 5;
			}

			auto lpapplyfriends = lpfriends1->mutable_mapplyfriends();
			auto itor = std::find(lpapplyfriends->begin(), lpapplyfriends->end(), afriends);
			if (itor == lpapplyfriends->end())
			{
				return 6;
			}
			lpapplyfriends->erase(itor);
			if (aratify)
			{
				lpfriends1->add_mfriends(afriends);
				lpfriends2->add_mfriends(afriends);
			}
			return 0;
		}

		// 删除好友
		int erasefriends(i64_actorid aroleid, i64_actorid afriends)
		{
			data_modified<pbdb::db_friends>& lfriends1 = get(aroleid);
			data_modified<pbdb::db_friends>& lfriends2 = get(afriends);
			pbdb::db_friends* lpfriend1 = lfriends1.get();
			pbdb::db_friends* lpfriend2 = lfriends2.get();
			if (lpfriend1 == nullptr || lpfriend2 == nullptr)
			{
				return 1;
			}

			auto lpfriends1 = lpfriend1->mutable_mfriends();
			auto itor = std::find(lpfriends1->begin(), lpfriends1->end(), afriends);
			if (itor == lpfriends1->end())
			{
				return 2;
			}
			lpfriends1->erase(itor);

			auto lpfriends2 = lpfriend2->mutable_mfriends();
			auto itor2 = std::find(lpfriends2->begin(), lpfriends2->end(), afriends);
			if (itor == lpfriends2->end())
			{
				return 3;
			}
			lpfriends2->erase(itor);
			return 0;
		}

		// 同步好友信息
		void syncfriends(i64_actorid aroleid)
		{
			auto pro = std::make_shared<pbnet::PROBUFF_NET_FRIEND_RESPONSE>();
			data_modified<pbdb::db_friends>& lfriends = get(aroleid);
			const pbdb::db_friends* lpfriendsconst = lfriends.getconst();
			if (lpfriendsconst == nullptr)
			{
				return;
			}

			for (i64_actorid afriends : lpfriendsconst->mfriends())
			{
				const pbdb::db_brief* lpbrief = tdb_brief::nsp_cli<actor_friends>::instance(
					get_actor()->id_guid()).getconst(afriends);
				if (lpbrief == nullptr)
				{
					return;
				}
				*pro->add_mfriends() = *lpbrief;
			}

			for (i64_actorid afriends : lpfriendsconst->mapplyfriends())
			{
				const pbdb::db_brief* lpbrief = tdb_brief::nsp_cli<actor_friends>::instance(
					get_actor()->id_guid()).getconst(afriends);
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
			const pbdb::db_friends* lpfriendsconst = lfriends.getconst();
			if (lpfriendsconst == nullptr)
			{
				return false;
			}
			for (i64_actorid aactorid : lpfriendsconst->mfriends())
			{
				afriends.push_back(aactorid);
			}
			return !afriends.empty();
		}
	};
}// namespace ngl

mk_formatter(pbdb::db_friends)

