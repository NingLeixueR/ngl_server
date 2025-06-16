#pragma once

#include "nactor_auto.h"
#include "manage_csv.h"
#include "localtime.h"

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

		virtual void set_id()
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
				tab_mail* tab = allcsv::tab<tab_mail>(atid);
				if (tab == nullptr)
				{
					log_error()->print("addmail tab id[{}] not find!!!", atid);
					return false;
				}
			}
			
			pbdb::mail lmail;
			int32_t& lid = maxid(aroleid);
			lmail.set_m_id(++lid);
			lmail.set_m_tid(atid);
			lmail.set_m_createutc((int32_t)localtime::gettime());
			lmail.set_m_draw(false);
			lmail.set_m_read(false);
			lmail.set_m_prams(aparm);
			lmail.set_m_content(acontent);
			for (const auto& [itemid, count] : aitem)
			{
				pbdb::mailitem* lpmailitem = lmail.add_m_items();
				lpmailitem->set_m_itemtid(itemid);
				lpmailitem->set_m_count(count);
			}
			data_modified<pbdb::db_mail>* lpdb_mail = get(aroleid);
			if (lpdb_mail == nullptr)
			{
				return false;
			}
			lpdb_mail->get().mutable_m_mail()->insert({ lid, lmail });
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
			auto itor = lpdb_mail->get().mutable_m_mail()->find((int32_t)aid);
			if (itor == lpdb_mail->get().mutable_m_mail()->end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		// # Ò»¼ü²Ù×÷
		void one_touch(i64_actorid aroleid, std::function<bool(const pbdb::mail&)> acheck, const std::function<void(int32_t)>& afun)
		{
			data_modified<pbdb::db_mail>* lpdb_mail = get(aroleid);
			if (lpdb_mail == nullptr)
			{
				return;
			}

			std::vector<int32_t> ldellist;
			for (const auto& [_id, _mail] : *lpdb_mail->get().mutable_m_mail())
			{
				if (_id != -1 && acheck(_mail))
				{
					ldellist.push_back(_mail.m_id());
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
				ltemp->set_m_read(true);
			}
			else
			{
				one_touch(aroleid, [](const pbdb::mail& amail)
					{
						return amail.m_read() == false;
					}, 
					[aroleid, this](int32_t aid)
					{
						readmail(aroleid, aid);
					});
			}			
			return true;
		}

		bool drawmail(int64_t aroleid, int64_t aid);

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
					if (ltemp->m_draw() == false || ltemp->m_read() == false)
					{
						return false;
					}
				}
				
				data_modified<pbdb::db_mail>* lpdb_mail = get(aroleid);
				if (lpdb_mail == nullptr)
				{
					return false;
				}

				lpdb_mail->get().mutable_m_mail()->erase((int32_t)aid);
			}
			else
			{
				one_touch(aroleid, [](const pbdb::mail& amail)
					{
						return amail.m_read() && amail.m_draw();
					}, 
					[aroleid, this](int32_t aid)
					{
						delmail(aroleid, aid);
					});
			}
			return true;
		}

		std::shared_ptr<pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE> sync_mail(i64_actorid aroleid, i64_actorid amailid = -1)
		{
			data_modified<pbdb::db_mail>* lpdb_mail = get(aroleid);
			if (lpdb_mail == nullptr)
			{
				return nullptr;
			}
			auto pro = std::make_shared<pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE>();
			for (const auto& [_mailid, _mails] : *lpdb_mail->get().mutable_m_mail())
			{
				pro->mutable_m_mail()->insert({ _mailid, _mails });
			}
			return pro;
		}
	};
}// namespace ngl

mk_formatter(pbdb::db_mail)