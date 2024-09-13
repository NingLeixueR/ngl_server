#pragma once

#include "ttab_specialid.h"
#include "nactor_auto.h"
#include "manage_csv.h"
#include "localtime.h"

#include <string>
#include <map>

namespace ngl
{
	class friends : public tdb_friends::db_modular
	{
		friends(const friends&) = delete;
		friends& operator=(const friends&) = delete;

	public:
		friends() = default;

		void set_id()final
		{
			m_id = -1;
		}

		const pbdb::db_friends* get_constfriends(i64_actorid aroleid)
		{
			auto itor = data().find(aroleid);
			if (itor == data().end())
				return nullptr;
			return &itor->second.getconst();
		}

		pbdb::db_friends* get_friends(i64_actorid aroleid, bool achange = true)
		{
			auto itor = data().find(aroleid);
			if (itor == data().end())
				return nullptr;
			return &itor->second.get(achange);
		}

		void initdata()final
		{
			auto lstream = log_error();
			(*lstream) << "friends###loaddb_finish" << std::endl;
			for (const auto& [_roleid, _data] : data())
			{
				(*lstream) << _roleid << "[";
				for (const auto& lfriend : _data.getconst().m_friends())
				{
					(*lstream) << lfriend << ",";
				}
				(*lstream) << "]" << std::endl;
			}
			lstream->print("");
		}

		// 请求添加好友
		int addfriends(i64_actorid aroleid, i64_actorid afriends)
		{
			const pbdb::db_friends* lrolefriendconst = get_constfriends(aroleid);
			if (lrolefriendconst == nullptr)
				return 1;
			const pbdb::db_friends* lfriendconst = get_constfriends(afriends);
			if (lfriendconst == nullptr)
				return 2;
			if (lrolefriendconst->m_friends_size() >= ttab_specialid::m_friendscount)
				return 3;
			if (lfriendconst->m_applyfriends_size() >= ttab_specialid::m_friendsapplylistcount)
				return 4;
			auto itorrole = std::find(lrolefriendconst->m_friends().begin(), lrolefriendconst->m_friends().end(), afriends);
			if (itorrole != lrolefriendconst->m_friends().end())
				return 5;
			auto itorrole2 = std::find(lrolefriendconst->m_applyfriends().begin(), lrolefriendconst->m_applyfriends().end(), afriends);
			if (itorrole2 != lrolefriendconst->m_applyfriends().end())
				return 6;
			auto itorfriend = std::find(lfriendconst->m_friends().begin(), lfriendconst->m_friends().end(), aroleid);
			if (itorrole != lfriendconst->m_friends().end())
				return 7;
			auto itorfriend2 = std::find(lfriendconst->m_applyfriends().begin(), lfriendconst->m_applyfriends().end(), aroleid);
			if (itorfriend2 != lfriendconst->m_applyfriends().end())
				return 8;
			pbdb::db_friends* lfriend = get_friends(afriends);
			if (lfriend == nullptr)
				return 9;
			lfriend->add_m_applyfriends(aroleid);
			return 0;
		}

		// 同意/拒绝好友申请
		int ratifyfriends(i64_actorid aroleid, i64_actorid afriends, bool aratify)
		{
			const pbdb::db_friends* lrolefriendconst = get_constfriends(aroleid);
			if (lrolefriendconst == nullptr)
				return 1;
			const pbdb::db_friends* lfriendconst = get_constfriends(afriends);
			if (lfriendconst == nullptr)
				return 2;
			if (lrolefriendconst->m_friends_size() >= ttab_specialid::m_friendscount)
				return 3;
			if (lfriendconst->m_friends_size() >= ttab_specialid::m_friendscount)
				return 4;
			pbdb::db_friends* lrolefriend = get_friends(aroleid);
			if (lrolefriend == nullptr)
				return 5;
			pbdb::db_friends* lfriend = get_friends(afriends);
			if (lfriend == nullptr)
				return 6;
			auto itor = std::find(lrolefriend->mutable_m_applyfriends()->begin(), lrolefriend->mutable_m_applyfriends()->end(), afriends);
			if (itor == lrolefriend->mutable_m_applyfriends()->end())
				return 7;
			lrolefriend->mutable_m_applyfriends()->erase(itor);
			lrolefriend->add_m_friends(afriends);
			lfriend->add_m_friends(afriends);

			return 0;
		}

		// 删除好友
		int erasefriends(i64_actorid aroleid, i64_actorid afriends)
		{
			pbdb::db_friends* lrolefriend = get_friends(aroleid);
			if (lrolefriend == nullptr)
				return 1;
			pbdb::db_friends* lfriend = get_friends(afriends);
			if (lfriend == nullptr)
				return 2;
			auto itor = std::find(lrolefriend->mutable_m_friends()->begin(), lrolefriend->mutable_m_friends()->end(), afriends);
			if (itor == lrolefriend->mutable_m_friends()->end())
				return 3;
			lrolefriend->mutable_m_friends()->erase(itor);

			auto itor2 = std::find(lfriend->mutable_m_friends()->begin(), lfriend->mutable_m_friends()->end(), afriends);
			if (itor == lrolefriend->mutable_m_friends()->end())
				return 4;
			lrolefriend->mutable_m_friends()->erase(itor);
			return 0;
		}

		// 同步好友信息
		void syncfriends(i64_actorid aroleid)
		{
			auto pro = std::make_shared<pbnet::PROBUFF_NET_FRIEND_RESPONSE>();
			const pbdb::db_friends* lfriendconst = get_constfriends(aroleid);
			if (lfriendconst != nullptr)
			{
				std::ranges::for_each(lfriendconst->m_friends(), [&pro, lfriendconst](i64_actorid afriends)
					{
						const pbdb::db_brief* lpbrief = tdb_brief::nsp_cli<actor_ranklist>::getconst(afriends);
						*pro->add_m_friends() = *lpbrief;
					});

				std::ranges::for_each(lfriendconst->m_applyfriends(), [&pro, lfriendconst](i64_actorid afriends)
					{
						const pbdb::db_brief* lpbrief = tdb_brief::nsp_cli<actor_ranklist>::getconst(afriends);
						*pro->add_m_applyfriends() = *lpbrief;
					});
			}
			actor::send_client(aroleid, pro);
		}
	};
}// namespace ngl