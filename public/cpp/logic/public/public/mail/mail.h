#pragma once

#include "nactor_auto.h"
#include "manage_csv.h"
#include "localtime.h"

#include <string>
#include <map>

namespace ngl
{
	class mails : public typedb_mail::db_modular
	{
		std::map<i64_actorid, int32_t> m_maxid;
	public:
		mails()
		{
		}

		virtual void set_id()
		{
			m_id = -1;
		}

		const google::protobuf::Map<int32_t, pbdb::mail>* get_constmails(i64_actorid aroleid)
		{
			auto itor = data().find(aroleid);
			if (itor == data().end())
				return nullptr;
			return &itor->second.getconst().m_mail();
		}

		google::protobuf::Map<int32_t, pbdb::mail>* get_mails(i64_actorid aroleid, bool achange = true)
		{
			auto itor = data().find(aroleid);
			if (itor == data().end())
				return nullptr;
			return itor->second.get(achange).mutable_m_mail();
		}

		const pbdb::db_mail* get_db_mail(i64_actorid aroleid)
		{
			auto itor = data().find(aroleid);
			if (itor == data().end())
				return nullptr;
			return &itor->second.getconst();
		}

		virtual void initdata()
		{
			LogLocalStreamError(lstream);
			lstream << "actor_mail###loaddb_finish" << std::endl;
			for (const auto& [_roleid, _mails] : data())
			{
				int32_t& lid = m_maxid[_roleid];
				for (const auto& [_id, _mail] : _mails.getconst().m_mail())
				{
					if (lid < _id)
						lid = _id;
					lstream << "#####################roleid[" << _roleid << "]" << std::endl;
					lstream << "id:" << _mail.m_id() << std::endl;
					lstream << "tid:" << _mail.m_tid() << std::endl;
					lstream << "draw:" << (_mail.m_draw() ? "yes" : "no") << std::endl;
					lstream << "read:" << (_mail.m_read() ? "yes" : "no") << std::endl;
				}				
			}
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

		bool addmail(i64_actorid aroleid, int atid, std::map<int32_t, int32_t>& aitem, const std::string& acontent, const std::string& aparm)
		{
			if (atid != -1)
			{
				tab_mail* tab = allcsv::tab<tab_mail>(atid);
				if (tab == nullptr)
				{
					LogLocalError("addmail tab id[%] not find!!!", atid);
					return false;
				}
			}
			
			pbdb::mail lmail;
			int32_t& lid = maxid(aroleid);
			lmail.set_m_id(++lid);
			lmail.set_m_tid(atid);
			lmail.set_m_createutc(localtime::gettime());
			lmail.set_m_draw(false);
			lmail.set_m_read(false);
			lmail.set_m_prams(aparm);
			lmail.set_m_content(acontent);
			for (auto [itemid, count] : aitem)
			{
				pbdb::mailitem* lpmailitem = lmail.add_m_items();
				lpmailitem->set_m_itemtid(itemid);
				lpmailitem->set_m_count(count);
			}
			google::protobuf::Map<int32_t, pbdb::mail>* lpmap = get_mails(aroleid);
			if (lpmap == nullptr)
			{
				pbdb::db_mail ldbmail;
				ldbmail.set_m_id(aroleid);
				ldbmail.mutable_m_mail()->insert({ lid, lmail });
				add(aroleid, ldbmail);
				return true;
			}
			lpmap->insert({ lid, lmail });
			return true;
		}

		bool addmail(i64_actorid aroleid, int atid, std::map<int32_t, int32_t>& aitem, const std::string& aparm = "")
		{
			return addmail(aroleid, atid, aitem, "", aparm);
		}


		bool addmail(i64_actorid aroleid, std::map<int32_t, int32_t>& aitem, const std::string& acontent)
		{
			return addmail(aroleid, -1, aitem, acontent, "");
		}

		pbdb::mail* get_mail(i64_actorid aroleid, int64_t aid)
		{
			google::protobuf::Map<int32_t, pbdb::mail>* lpmap = get_mails(aroleid);
			if (lpmap == nullptr)
			{
				return nullptr;
			}
			auto itor = lpmap->find(aid);
			if (itor == lpmap->end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		// ## Ò»¼ü²Ù×÷
		void one_touch(i64_actorid aroleid, const std::function<void(int32_t)>& afun)
		{
			google::protobuf::Map<int32_t, pbdb::mail>* lmap = get_mails(aroleid);
			if (lmap == nullptr)
				return;

			std::vector<int32_t> ldellist;
			for (const auto& [_id, _mail] : *lmap)
			{
				if (_mail.m_draw() && _mail.m_read())
				{
					ldellist.push_back(_mail.m_id());
				}
			}

			for (int32_t _id : ldellist)
			{
				if (_id != -1)
					afun(_id);
			}
		}

		bool readmail(i64_actorid aroleid, int64_t aid)
		{
			if (aid != -1)
			{
				pbdb::mail* ltemp = get_mail(aroleid, aid);
				if (ltemp == nullptr)
					return false;
				ltemp->set_m_read(true);
			}
			else
			{
				one_touch(aroleid, [aroleid, this](int32_t aid)
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
					return false;
				if (acheckdrawread)
				{
					if (ltemp->m_draw() == false || ltemp->m_read() == false)
					{
						return false;
					}
				}
				
				google::protobuf::Map<int32_t, pbdb::mail>* lmap = get_mails(aroleid);
				if (lmap == nullptr)
					return false;

				lmap->erase(aid);
			}
			else
			{
				one_touch(aroleid, [aroleid, this](int32_t aid)
					{
						delmail(aroleid, aid);
					});
			}
			return true;
		}

		std::shared_ptr<pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE> sync_mail(i64_actorid aroleid, i64_actorid amailid = -1)
		{
			google::protobuf::Map<int32_t, pbdb::mail>* lpmap = get_mails(aroleid); 
			if (lpmap == nullptr)
			{
				return nullptr;
			}
			auto pro = std::make_shared<pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE>();
			for (const auto& [_mailid, _mails] : *lpmap)
			{
				pro->mutable_m_mail()->insert({ _mailid, _mails });
			}
			return pro;
		}
	};
}// namespace ngl