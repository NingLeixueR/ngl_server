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

		void set_id()final
		{
			set_actorid(nguid::make());
		}

		data_modified<pbdb::db_friends>* get_friends(i64_actorid aroleid, bool achange = true)
		{
			auto itor = data().find(aroleid);
			if (itor == data().end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		void initdata()final
		{
			log_error()->print("friends###loaddb_finish {}", data());
		}

		bool check_friends(const google::protobuf::RepeatedField<i64_actorid>& avec1, const google::protobuf::RepeatedField<i64_actorid>& avec2, i64_actorid aroleid)
		{
			return std::find(avec1.begin(), avec1.end(), aroleid) != avec1.end() || std::find(avec2.begin(), avec2.end(), aroleid) != avec2.end();
		}

		// 请求添加好友
		int addfriends(i64_actorid aroleid, i64_actorid afriends)
		{
			data_modified<pbdb::db_friends>* lfriends = get_friends(aroleid);
			if (lfriends == nullptr)
			{
				return 1;
			}
			auto& lconstfriends = lfriends->getconst();
			if (lconstfriends.m_friends_size() >= ttab_specialid::m_friendscount)
			{
				return 2;
			}
			if (lconstfriends.m_applyfriends_size() >= ttab_specialid::m_friendsapplylistcount)
			{
				return 3;
			}
			auto& lfriendslist = lconstfriends.m_friends();
			auto& lapplyfriends = lconstfriends.m_applyfriends();
			if (check_friends(lfriendslist, lapplyfriends, afriends))
			{
				return 4;
			}
			if (check_friends(lfriendslist, lapplyfriends, aroleid))
			{
				return 5;
			}
			pbdb::db_friends& lpfriend = lfriends->get();
			lpfriend.add_m_applyfriends(aroleid);
			return 0;
		}

		// 同意/拒绝好友申请
		int ratifyfriends(i64_actorid aroleid, i64_actorid afriends, bool aratify)
		{
			data_modified<pbdb::db_friends>* lfriends1 = get_friends(aroleid);
			if (lfriends1 == nullptr)
			{
				return 1;
			}
			data_modified<pbdb::db_friends>* lfriends2 = get_friends(afriends);
			if (lfriends2 == nullptr)
			{
				return 2;
			}
			if (lfriends1->getconst().m_friends_size() >= ttab_specialid::m_friendscount)
			{
				return 3;
			}
			if (lfriends2->getconst().m_friends_size() >= ttab_specialid::m_friendscount)
			{
				return 4;
			}
			
			auto lpapplyfriends = lfriends1->get().mutable_m_applyfriends();
			auto itor = std::find(lpapplyfriends->begin(), lpapplyfriends->end(), afriends);
			if (itor == lpapplyfriends->end())
			{
				return 5;
			}
			lpapplyfriends->erase(itor);
			if (aratify)
			{
				lfriends1->get().add_m_friends(afriends);
				lfriends2->get().add_m_friends(afriends);
			}

			return 0;
		}

		// 删除好友
		int erasefriends(i64_actorid aroleid, i64_actorid afriends)
		{
			data_modified<pbdb::db_friends>* lfriends1 = get_friends(aroleid);
			if (lfriends1 == nullptr)
			{
				return 1;
			}
			data_modified<pbdb::db_friends>* lfriends2 = get_friends(afriends);
			if (lfriends2 == nullptr)
			{
				return 2;
			}
			auto lpfriends1 = lfriends1->get().mutable_m_friends();
			auto itor = std::find(lpfriends1->begin(), lpfriends1->end(), afriends);
			if (itor == lpfriends1->end())
			{
				return 3;
			}
			lpfriends1->erase(itor);

			auto lpfriends2 = lfriends2->get().mutable_m_friends();
			auto itor2 = std::find(lpfriends2->begin(), lpfriends2->end(), afriends);
			if (itor == lpfriends2->end())
			{
				return 4;
			}
			lpfriends2->erase(itor);
			return 0;
		}

		// 同步好友信息
		void syncfriends(i64_actorid aroleid)
		{
			auto pro = std::make_shared<pbnet::PROBUFF_NET_FRIEND_RESPONSE>();
			data_modified<pbdb::db_friends>* lfriends = get_friends(aroleid);
			if (lfriends != nullptr)
			{
				std::ranges::for_each(lfriends->getconst().m_friends(), [&pro, lfriends, this](i64_actorid afriends)
					{
						const pbdb::db_brief* lpbrief = tdb_brief::nsp_cli<actor_friends>::getInstance(get_actor()->id_guid()).getconst(afriends);
						*pro->add_m_friends() = *lpbrief;
					});

				std::ranges::for_each(lfriends->getconst().m_applyfriends(), [&pro, lfriends, this](i64_actorid afriends)
					{
						const pbdb::db_brief* lpbrief = tdb_brief::nsp_cli<actor_friends>::getInstance(get_actor()->id_guid()).getconst(afriends);
						*pro->add_m_applyfriends() = *lpbrief;
					});
			}
			actor::send_client(aroleid, pro);
		}

		// 获取好友
		bool get_friends(i64_actorid aroleid, std::vector<i64_actorid>& afriends)
		{
			data_modified<pbdb::db_friends>* lfriends = get_friends(aroleid);
			if (lfriends == nullptr)
			{
				return false;
			}
			std::ranges::for_each(lfriends->getconst().m_friends(), [&afriends](i64_actorid aactorid)
				{
					afriends.push_back(aactorid);
				});
			return !afriends.empty();
		}
	};
}// namespace ngl

mk_formatter(pbdb::db_friends)

