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
#pragma once

#include "nactor_auto.h"
#include "ttab_mail.h"
#include "localtime.h"
#include "ncsv.h"

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
			auto itor = m_maxid.find(aroleid);
			if (itor == m_maxid.end())
			{
				m_maxid[aroleid] = 1;
				return m_maxid[aroleid];
			}
			return itor->second;
		}

		bool addmail(i64_actorid aroleid, int atid, const std::map<int32_t, int32_t>& aitem, const std::string& acontent, const std::string& aparm)
		{
			if (atid != -1)
			{
				tab_mail* tab = ttab_mail::instance().tab(atid);
				if (tab == nullptr)
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
			for (auto& [itemid, count] : aitem)
			{
				pbdb::mailitem* lpmailitem = lmail.add_mitems();
				lpmailitem->set_mitemtid(itemid);
				lpmailitem->set_mcount(count);
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

			auto itor = lpdbmail->mutable_mmail()->find((int32_t)aid);
			if (itor == lpdbmail->mutable_mmail()->end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		// # 一键操作
		void one_touch(i64_actorid aroleid, std::function<bool(const pbdb::mail&)> acheck, const std::function<void(int32_t)>& afun)
		{
			data_modified<pbdb::db_mail>& lpdb_mail = get(aroleid);
			std::vector<int32_t> ldellist;
			pbdb::db_mail* lpdbmail = lpdb_mail.get();
			if (lpdbmail == nullptr)
			{
				return;
			}
			for (auto& [_id, _mail] : *lpdbmail->mutable_mmail())
			{
				if (_id != -1 && acheck(_mail))
				{
					ldellist.push_back(_mail.mid());
				}
			}

			for (int32_t _id : ldellist)
			{
				afun(_id);
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
				lpdbmail->mutable_mmail()->erase((int32_t)aid);
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

		bool sync_mail(i64_actorid aroleid, i64_actorid amailid, pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE& apro)
		{
			data_modified<pbdb::db_mail>& lpdb_mail = get(aroleid);
			pbdb::db_mail* lpdbmail = lpdb_mail.get();
			if (lpdbmail == nullptr)
			{
				return false;
			}
			for (auto& [_mailid, _mails] : *lpdbmail->mutable_mmail())
			{
				apro.mutable_mmail()->insert({ _mailid, _mails });
			}
			return true;
		}
	};
}// namespace ngl

mk_formatter(pbdb::db_mail)