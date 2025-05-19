#pragma once

#include "ttab_specialid.h"
#include "nactor_auto.h"
#include "manage_csv.h"
#include "localtime.h"

#include <string>
#include <map>

namespace ngl
{
	class familyer : 
		public tdb_familyer::db_modular
	{
		familyer(const familyer&) = delete;
		familyer& operator=(const familyer&) = delete;

	public:
		familyer() = default;

		void set_id()final
		{
			m_id = -1;
		}

		data_modified<pbdb::db_familyer>* get_familyer(i64_actorid aroleid)
		{
			auto itor = data().find(aroleid);
			if (itor == data().end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		pbdb::db_familyer* add_familyer(i64_actorid aroleid)
		{
			data_modified<pbdb::db_familyer>* lpfamilyer = get_familyer(aroleid);
			if (lpfamilyer != nullptr)
			{
				return &lpfamilyer->get();
			}
			pbdb::db_familyer lfamilyer;
			lfamilyer.set_m_id(aroleid);
			lfamilyer.set_m_joinutc(0);
			lfamilyer.set_m_lastleaveutc(0);
			lfamilyer.set_m_lastsignutc(0);
			lfamilyer.set_m_position(pbdb::db_familyer_eposition_none);
			lpfamilyer = add(aroleid, lfamilyer);
			if (lpfamilyer == nullptr)
			{
				return nullptr;
			}
			return &lpfamilyer->get();
		}

		void initdata()final
		{
			log_error()->print("{}", data());
		}

		// ����Ƿ���Դ�������
		bool check_createfamily(i64_actorid aroleid)
		{
			pbdb::db_familyer* lpfamilyer = add_familyer(aroleid);
			if (lpfamilyer->m_position() != pbdb::db_familyer_eposition_none)
			{
				return false;
			}
			auto lnow = (int32_t)localtime::gettime();
			if (lnow - lpfamilyer->m_lastleaveutc() < ttab_specialid::m_familjoininterval)
			{
				return false;
			}
			return true;
		}
	};

	class family : 
		public tdb_family::db_modular
	{
		family(const family&) = delete;
		family& operator=(const family&) = delete;

		int64_t m_maxid;
		std::map<int64_t, int64_t>				m_rolefamily;	// key:roleid value:familyid
		std::map<int64_t, std::set<int64_t>>	m_applylist;	// key:roleid value:std::set<familyid>
		std::set<std::string>					m_familyname;	// ���������������Ƿ��ظ�
	public:
		family():
			m_maxid(0)
		{
		}

		void set_id()final
		{
			m_id = -1;
		}

		ngl::data_modified<pbdb::db_family>* get_family(i64_actorid afamilyid)
		{
			auto itor = data().find(afamilyid);
			if (itor == data().end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		void initdata()final
		{
			log_error()->print("{}", data());
			for (const auto& [_familyid, _family] : data())
			{
				const pbdb::db_family& lbdfamily = _family.getconst();
				m_maxid = std::max(m_maxid, _familyid.id());
				
				std::string lmember;
				for (i64_actorid roleid : lbdfamily.m_member())
				{
					m_rolefamily[roleid] = _familyid;
				}

				m_familyname.insert(lbdfamily.m_name());

				for (i64_actorid roleid : lbdfamily.m_applylist())
				{
					m_applylist[roleid].insert(_familyid);
				}
			}
		}

		// # ��������
		int32_t create_family(i64_actorid aroleid, const std::string& aname);

		// # ����������
		int32_t join_family(i64_actorid aroleid, i64_actorid afamilyid);

		// # �Ӿ���[afamilyid]�е�applylist���Ƴ�[aroleid]
		int32_t erase_applylist(i64_actorid aroleid, i64_actorid afamilyid);

		// # ȡ���������
		int32_t cancel_join_family(i64_actorid aroleid, i64_actorid afamilyid);

		// # ���ų�ͬ��/��ͬ����Ҽ������
		int32_t ratify_join_family(i64_actorid aroleid, i64_actorid ajoinroleid, bool aratify);

		// # ���ų�ת�þ���
		int32_t cede_family(i64_actorid aroleid, i64_actorid acederoleid);

		// # �˳�����
		int32_t leave_family(i64_actorid aroleid, i64_actorid afamilyid);

		// # �޸ľ�������
		int32_t change_familyname(i64_actorid aroleid, i64_actorid afamilyid, const std::string& afamilyname);

		// # ����ǩ��
		int32_t sign_family(i64_actorid aroleid, i64_actorid afamilyid);

		std::shared_ptr<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE> get_familylist(i64_actorid afamilyid);

		void sync_family(i64_actorid aroleid, i64_actorid afamilyid);

		// # ��ȡ����������
		bool get_familyers(i64_actorid aroleid, std::vector<i64_actorid>& afamilyers);
	};
}// namespace ngl

mk_formatter(pbdb::db_familyer)
mk_formatter(pbdb::db_family)