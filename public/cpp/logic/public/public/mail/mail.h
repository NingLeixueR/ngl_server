#pragma once

#include "ndb_modular.h"
#include "manage_csv.h"
#include "localtime.h"

#include <string>
#include <map>

namespace ngl
{
	class actor_mail;
	using mail_db_modular = ndb_modular<
		EPROTOCOL_TYPE_PROTOCOLBUFF, 
		pbdb::ENUM_DB_MAIL,
		pbdb::db_mail,
		actor_mail>;
	class mails : public mail_db_modular
	{
		std::map<i64_actorid, int32_t> m_maxid;
	public:
		mails() :
			mail_db_modular()
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

		virtual void initdata()
		{
			LogLocalError("actor_mail###loaddb_finish")
			
			for (const auto& [_roleid, _mails] : data())
			{
				int32_t& lid = m_maxid[_roleid];
				for (const auto& [_id, _mail] : _mails.getconst().m_mail())
				{
					if (lid < _id)
						lid = _id;
					std::cout << "#####################roleid[" << _roleid << "]" << std::endl;
					std::cout << "id:" << _mail.m_id() << std::endl;
					std::cout << "tid:" << _mail.m_tid() << std::endl;
					std::cout << "draw:" << (_mail.m_draw() ? "yes" : "no") << std::endl;
					std::cout << "read:" << (_mail.m_read() ? "yes" : "no") << std::endl;
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

		void addmail(i64_actorid aroleid, int atid, std::map<int32_t, int32_t>& aitem, const std::string& aparm = "")
		{
			tab_mail* tab = allcsv::tab<tab_mail>(atid);
			if (tab == nullptr)
			{
				LogLocalError("addmail tab id[%] not find!!!", atid)
				return;
			}
			
			pbdb::mail lmail;
			int32_t& lid = maxid(aroleid);
			lmail.set_m_id(++lid);
			lmail.set_m_tid(atid);
			lmail.set_m_createutc(localtime::gettime());
			lmail.set_m_draw(false);
			lmail.set_m_read(false);
			lmail.set_m_prams(aparm);
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
				return;
			}
			lpmap->insert({ lid, lmail });
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

		// ## 一键操作
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

		bool delmail(i64_actorid aroleid, int64_t aid)
		{
			if (aid != -1)
			{
				pbdb::mail* ltemp = get_mail(aroleid, aid);
				if (ltemp == nullptr)
					return false;
				if (ltemp->m_draw() == false || ltemp->m_read() == false)
				{
					return false;
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
	////	void rollback_drawmail(int64_t aroleid, int amailid);
	////	// 领取单个邮件
	////	bool drawmail(int64_t aid, int64_t aroleid);
	////	// 一键领取邮件
	////	bool drawmail(int64_t aroleid);

	////	void getmailbyrole(int64_t aroleid, std::vector<DB_MAIL>& avec);
	};
}