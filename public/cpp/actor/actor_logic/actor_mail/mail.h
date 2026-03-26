/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Declares interfaces for actor mail.

#pragma once

#include "actor/generated/auto/nactor_auto.h"
#include "actor/tab/ttab_mail.h"
#include "tools/tab/csv/ncsv.h"
#include "tools/localtime.h"

#include <string>
#include <map>

namespace ngl
{
	class mails : 
		public tdb_mail::db_modular
	{
		mails(const mails&) = delete;
		mails& operator=(const mails&) = delete;

		std::map<i64_actorid, int32_t> m_maxid;
	public:
		mails() = default;

		void related_actorid() final
		{
			set_actorid(nguid::make());
		}

		virtual void initdata()
		{
			log_error()->print("actor_mail###loaddb_finish ", data());
		}

		int32_t& maxid(i64_actorid aroleid)
		{
			auto [litor, _] = m_maxid.try_emplace(aroleid, 1);
			return litor->second;
		}

		bool addmail(i64_actorid aroleid, int atid, const std::map<int32_t, int32_t>& aitem, const std::string& acontent, const std::string& aparm)
		{
			if (atid != -1)
			{
				const tab_mail* ltab = ttab_mail::instance().tab(atid);
				if (ltab == nullptr)
				{
					log_error()->print("addmail tab id[{}] not find!!!", atid);
					return false;
				}
			}
			
			pbdb::mail lmail;
			int32_t& lid = maxid(aroleid);
			lmail.set_mid(++lid);
			lmail.set_mtid(atid);
			lmail.set_mcreateutc((int32_t)localtime::gettime());
			lmail.set_mdraw(false);
			lmail.set_mread(false);
			lmail.set_mprams(aparm);
			lmail.set_mcontent(acontent);
			for (const auto& [litemid, lcount] : aitem)
			{
				pbdb::mailitem* lpmailitem = lmail.add_mitems();
				lpmailitem->set_mitemtid(litemid);
				lpmailitem->set_mcount(lcount);
			}
			data_modified<pbdb::db_mail>& lpdb_mail = get(aroleid);
			pbdb::db_mail* lpdbmail = lpdb_mail.get();
			if (lpdbmail == nullptr)
			{
				return false;
			}
			lpdbmail->mutable_mmail()->insert({ lid, lmail });
			return true;
		}

		bool addmail(i64_actorid aroleid, int atid,const std::map<int32_t, int32_t>& aitem, const std::string& aparm = "")
		{
			return addmail(aroleid, atid, aitem, "", aparm);
		}


		bool addmail(i64_actorid aroleid, const std::map<int32_t, int32_t>& aitem, const std::string& acontent)
		{
			return addmail(aroleid, -1, aitem, acontent, "");
		}

		pbdb::mail* get_mail(i64_actorid aroleid, int64_t aid)
		{
			data_modified<pbdb::db_mail>* lpdb_mail = find(aroleid);
			if (lpdb_mail == nullptr)
			{
				return nullptr;
			}
			pbdb::db_mail* lpdbmail = lpdb_mail->get();
			if (lpdbmail == nullptr)
			{
				return nullptr;
			}

			auto* lmailmap = lpdbmail->mutable_mmail();
			auto lit = lmailmap->find((int32_t)aid);
			if (lit == lmailmap->end())
			{
				return nullptr;
			}
			return &lit->second;
		}

		// Translated comment.
		void one_touch(i64_actorid aroleid, std::function<bool(const pbdb::mail&)> acheck, const std::function<void(int32_t)>& afun)
		{
			data_modified<pbdb::db_mail>& lpdb_mail = get(aroleid);
			pbdb::db_mail* lpdbmail = lpdb_mail.get();
			if (lpdbmail == nullptr)
			{
				return;
			}
			const auto& lmailmap = lpdbmail->mmail();
			std::vector<int32_t> ldellist;
			ldellist.reserve(lmailmap.size());
			for (const auto& [lmailid, lmail] : lmailmap)
			{
				if (lmailid != -1 && acheck(lmail))
				{
					ldellist.emplace_back(lmail.mid());
				}
			}

			for (const int32_t lid : ldellist)
			{
				afun(lid);
			}
		}

		bool readmail(i64_actorid aroleid, int64_t aid)
		{
			if (aid != -1)
			{
				pbdb::mail* ltemp = get_mail(aroleid, aid);
				if (ltemp == nullptr)
				{
					return false;
				}
				ltemp->set_mread(true);
			}
			else
			{
				one_touch(aroleid, [](const pbdb::mail& amail)
					{
						return amail.mread() == false;
					}, 
					[aroleid, this](int32_t aid)
					{
						readmail(aroleid, aid);
					});
			}			
			return true;
		}

		bool drawmail(i64_actorid aroleid, int64_t aid);

		bool delmail(i64_actorid aroleid, int64_t aid, bool acheckdrawread = true)
		{
			if (aid != -1)
			{
				pbdb::mail* ltemp = get_mail(aroleid, aid);
				if (ltemp == nullptr)
				{
					return false;
				}
				if (acheckdrawread)
				{
					if (ltemp->mdraw() == false || ltemp->mread() == false)
					{
						return false;
					}
				}
				
				data_modified<pbdb::db_mail>& lpdb_mail = get(aroleid);
				pbdb::db_mail* lpdbmail = lpdb_mail.get();
				if (lpdbmail == nullptr)
				{
					return false;
				}
				auto* lmailmap = lpdbmail->mutable_mmail();
				lmailmap->erase((int32_t)aid);
			}
			else
			{
				one_touch(aroleid, [](const pbdb::mail& amail)
					{
						return amail.mread() && amail.mdraw();
					}, 
					[aroleid, this](int32_t aid)
					{
						delmail(aroleid, aid);
					});
			}
			return true;
		}

		bool sync_mail(i64_actorid aroleid, [[maybe_unused]] i64_actorid amailid, pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE& apro)
		{
			data_modified<pbdb::db_mail>& lpdb_mail = get(aroleid);
			pbdb::db_mail* lpdbmail = lpdb_mail.get();
			if (lpdbmail == nullptr)
			{
				return false;
			}
			const auto& lmailmap = lpdbmail->mmail();
			auto* lrespmap = apro.mutable_mmail();
			for (const auto& [lmailid, lmails] : lmailmap)
			{
				lrespmap->insert({ lmailid, lmails });
			}
			return true;
		}
	};
}// namespace ngl

mk_formatter(pbdb::db_mail)
