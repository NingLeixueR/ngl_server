#include "actor_matching.h"
#include "matching.h"

namespace ngl
{
	std::array<room*, pbnet::eplays_count> room::m_typeroom;

	room::room(int32_t aid, pbnet::eplays m_type, int32_t atid)
		: m_id(aid)
		, m_type(m_type)
		, m_tab(nullptr)
		, m_stat(ematching_run)
	{
		m_tab = allcsv::tab<tab_matching>(atid);
		if (m_tab == nullptr)
		{
			log_error()->print("allcsv::tab<tab_matching>({}) == nullptr", atid);
			return;
		}
	}

	int room::id()
	{
		return m_id;
	}

	pbnet::eplays room::type()
	{
		return m_type;
	}

	pbnet::MATCHING_MEMBER* room::find(i64_actorid aroleid)
	{
		auto itor = m_memberlist.find(aroleid);
		if (itor == m_memberlist.end())
		{
			return nullptr;
		}
		return &itor->second;
	}

	bool room::initmember(i64_actorid aroleid, pbnet::MATCHING_MEMBER& amember)
	{
		const pbdb::db_brief* lpbrief = actor_matching::nclient::get(aroleid);
		if (lpbrief == nullptr)
		{
			return false;
		}
		amember.set_m_id(aroleid);
		amember.set_m_begmatch((int32_t)localtime::gettime());
		amember.set_m_name(lpbrief->m_name());
		amember.set_m_lv(lpbrief->m_lv());
		amember.set_m_isconfirm(false);
		return true;
	}

	bool room::try_addroom(i64_actorid aroleid)
	{
		if (is_roomfull())
		{
			return false;
		}
		auto itor = m_memberlist.find(aroleid);
		if (m_memberlist.contains(aroleid))
		{
			return false;
		}
		pbnet::MATCHING_MEMBER& ldata = m_memberlist[aroleid];
		if (initmember(aroleid, ldata) == false)
		{
			return false;
		}

		auto pro = std::make_shared<pbnet::PROBUFF_NET_MATCHING_SYNC>();
		pro->set_m_type(m_type);
		pro->set_m_roomid(m_id);
		std::ranges::for_each(m_memberlist, [&pro](const auto& pair)
			{
				*pro->add_m_member() = pair.second;				
			});
		send_client(pro);

		check_playscount();
		return true;
	}

	pbnet::ematching_cancel_stat room::try_cancel(i64_actorid aroleid)
	{
		if (m_stat != ematching_run)
		{
			return pbnet::ematching_cancel_stage;
		}
		if (m_memberlist.find(aroleid) == m_memberlist.end())
		{
			return pbnet::ematching_cancel_roommembernotfind;
		}
		m_memberlist.erase(aroleid);
		
		return pbnet::ematching_cancel_success;
	}

	bool room::is_roomfull()
	{
		return m_memberlist.size() >= m_tab->m_count;
	}

	bool room::is_addroom(i64_actorid aroleid)
	{
		if (is_roomfull())
			return false;
		return true;
	}

	void room::sync_confirm()
	{
		auto pro = std::make_shared<pbnet::PROBUFF_NET_SYNC_MATCHING_CONFIRM>();
		pro->set_m_type(m_type);
		pro->set_m_roomid(m_id);
		std::ranges::for_each(m_memberlist, [&pro](const auto& amember)
			{
				pro->add_m_roleid(amember.first);
			});
		send_client(pro);
	}

	pbnet::ematching_confirm_stat room::confirm(i64_actorid aroleid)
	{
		if (m_stat != ematching_waitconfirm)
		{
			return pbnet::ematching_confirm_stage;
		}
		auto itor = m_memberlist.find(aroleid);
		if (itor == m_memberlist.end())
			return pbnet::ematching_confirm_roommembernotfind;
		itor->second.set_m_isconfirm(true);

		sync_confirm();

		auto itorfind = std::ranges::find_if(m_memberlist, [](const auto& item)
			{
				if (item.second.m_isconfirm())
				{
					return true;
				}
				return false;
			});
		if (itorfind != m_memberlist.end())
		{
			return pbnet::ematching_confirm_success;
		}
		// ��ȷ���� ׼����ʼ�淨
		auto pro = std::make_shared<pbnet::PROBUFF_NET_MATCHING_SUCCESS>();
		pro->set_m_type(m_type);
		pro->set_m_roomid(m_id);
		pro->set_m_dataid(0);
		std::ranges::for_each(m_memberlist, [&pro](const auto& item)
			{
				*pro->add_m_member() = item.second;
			});
		// ���͸�[ACTOR_MANAGE_PLAYS]�����淨
		actor::static_send_actor(nguid::make_self(ACTOR_MANAGE_PLAYS), nguid::make(), pro);
		// ���͸��������
		send_client(pro);
		return pbnet::ematching_confirm_success;
	}

	void room::auto_cancel(pbnet::etype_matching_cancel atype, std::vector<i64_actorid>& avec)
	{
		auto pro = std::make_shared<pbnet::PROBUFF_NET_MEMBER_MATCHING_CANCEL>();
		pro->set_m_type(m_type);
		pro->set_m_canceltype(pbnet::etype_matching_cancel_timeout);
		std::ranges::for_each(avec, [&pro](i64_actorid amember)
			{
				pro->add_m_cancelmember(amember);
			});
		send_client(pro);
	}

	void room::check_timeout()
	{
		auto lnow = (int32_t)localtime::gettime();
		if (m_stat == ematching_run)
		{
			std::vector<i64_actorid> louttime;
			int ltm = m_tab->m_time;
			for (auto itor = m_memberlist.begin(); itor != m_memberlist.end();)
			{
				if (lnow <= ltm + m_tab->m_time)
				{
					louttime.push_back(itor->first);
					m_memberlist.erase(itor);
				}
				else
				{
					++itor;
				}
			}
			if (louttime.empty() == false)
			{
				auto_cancel(pbnet::etype_matching_cancel_timeout, louttime);
			}
		}
		else if (m_stat == ematching_waitconfirm)
		{
			if (lnow <= m_wbegtm + m_tab->m_waitconfirmtime)
			{
				std::vector<i64_actorid> louttime;
				std::ranges::for_each(m_memberlist, [&louttime](const auto& apair)
					{
						louttime.push_back(apair.first);
					});
				
				auto_cancel(pbnet::etype_matching_waitconfirm_timeout, louttime);
			}
		}
		else if (m_stat == ematching_create)
		{
			if (lnow <= m_wbegtm + m_tab->m_waitconfirmtime)
			{
				std::vector<i64_actorid> louttime;
				std::ranges::for_each(m_memberlist, [&louttime](const auto& apair)
					{
						louttime.push_back(apair.first);
					});
				auto_cancel(pbnet::etype_matching_waitcreate_timeout, louttime);
			}
		}
	}

	bool room::check_playscount()
	{
		if (m_stat != ematching_run)
		{
			return false;
		}
		if (is_roomfull() == true)
		{
			return false;
		}
		if (m_tab->m_iswaitconfirm)
		{
			change_stat(ematching_waitconfirm);
			auto pro = std::make_shared<pbnet::PROBUFF_NET_MATCHING_WAITCONFIRM>();
			pro->set_m_type(m_type);
			pro->set_m_roomid(m_id);
			send_client(pro);
		}
		else
		{
			change_stat(ematching_create);
			// ƥ��ɹ�
			auto pro = std::make_shared<pbnet::PROBUFF_NET_MATCHING_SUCCESS>();
			pro->set_m_type(m_type);
			pro->set_m_roomid(m_id);
			pro->set_m_dataid(m_id);
			for (std::pair<const int64_t, pbnet::MATCHING_MEMBER>& item : m_memberlist)
			{
				*pro->add_m_member() = item.second;
			}
			actor::static_send_actor(nguid::make_self(ACTOR_MANAGE_PLAYS), nguid::make(), pro);
		}
		return true;
	}

	matching::matching() :
		m_currentroom({ 0 })
	{
	}
}// namespace ngl