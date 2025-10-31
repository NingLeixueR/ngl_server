/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
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

		void related_actorid()final
		{
			set_actorid(nguid::make());
		}

		void initdata()final
		{
			log_error()->print("{}", data());
		}

		// ����Ƿ���Դ�������
		bool check_createfamily(i64_actorid aroleid)
		{
			data_modified<pbdb::db_familyer>& lpdbfamilyer = get(aroleid);
			auto lnow = (int32_t)localtime::gettime();
			const pbdb::db_familyer* lpdbfamilyerconst = lpdbfamilyer.getconst();
			if (lpdbfamilyerconst == nullptr)
			{
				return false;
			}
			if (lnow - lpdbfamilyerconst->mlastleaveutc() < ttab_specialid::instance().m_familjoininterval)
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

		int32_t									m_maxid = 0;
		std::map<int64_t, int64_t>				m_rolefamily;	// key:roleid value:familyid
		std::map<int64_t, std::set<int64_t>>	m_applylist;	// key:roleid value:std::set<familyid>
		std::set<std::string>					m_familyname;	// ���������������Ƿ��ظ�
	public:
		family():
			m_maxid(0)
		{
		}

		void related_actorid()final
		{
			set_actorid(nguid::make());
		}

		void initdata()final
		{
			log_error()->print("{}", data());
			for (std::pair<const nguid, data_modified<pbdb::db_family>>& lpair : data())
			{
				const pbdb::db_family* lpdbfamilyconst = lpair.second.getconst();
				if (lpdbfamilyconst == nullptr)
				{
					continue;
				}
				m_maxid = std::max(m_maxid, (int32_t)lpair.first.id());
				
				std::string lmember;
				for (i64_actorid roleid : lpdbfamilyconst->mmember())
				{
					m_rolefamily[roleid] = lpair.first;
				}

				m_familyname.insert(lpdbfamilyconst->mname());

				for (i64_actorid roleid : lpdbfamilyconst->mapplylist())
				{
					m_applylist[roleid].insert(lpair.first);
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

		void sync_familylist(i64_actorid aroleid, i64_actorid afamilyid);

		void sync_family(i64_actorid aroleid);

		// # ��ȡ����������
		bool get_familyers(i64_actorid aroleid, std::vector<i64_actorid>& afamilyers);
	};
}// namespace ngl

mk_formatter(pbdb::db_familyer)
mk_formatter(pbdb::db_family)