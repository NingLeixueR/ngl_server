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
			return nullptr;
		return &itor->second;
	}

	bool room::initmember(i64_actorid aroleid, pbnet::MATCHING_MEMBER& amember)
	{
		const pbdb::db_brief* lpbrief = actor_matching::nclient::get(aroleid);
		if (lpbrief == nullptr)
			return false;
		amember.set_m_id(aroleid);
		amember.set_m_begmatch(localtime::gettime());
		amember.set_m_name(lpbrief->m_name());
		amember.set_m_lv(lpbrief->m_lv());
		amember.set_m_isconfirm(false);
		return true;
	}

	bool room::try_addroom(i64_actorid aroleid)
	{
		if (is_roomfull())
			return false;
		auto itor = m_memberlist.find(aroleid);
		if (itor != m_memberlist.end())
			return false;
		pbnet::MATCHING_MEMBER& ldata = m_memberlist[aroleid];
		if (initmember(aroleid, ldata) == false)
		{
			return false;
		}

		auto pro = std::make_shared<pbnet::PROBUFF_NET_MATCHING_SYNC>();
		pro->set_m_type(m_type);
		pro->set_m_roomid(m_id);
		std::for_each(m_memberlist.begin(), m_memberlist.end(), [&pro](std::pair<const i64_actorid, pbnet::MATCHING_MEMBER>& pair)
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
		std::for_each(m_memberlist.begin(), m_memberlist.end(), [&pro](std::pair<const int64_t, pbnet::MATCHING_MEMBER>& amember)
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

		for (std::pair<const int64_t, pbnet::MATCHING_MEMBER>& item : m_memberlist)
		{
			if (item.second.m_isconfirm() == false)
			{
				return pbnet::ematching_confirm_success;
			}
		}
		// 都确认了 准备开始玩法
		auto pro = std::make_shared<pbnet::PROBUFF_NET_MATCHING_SUCCESS>();
		pro->set_m_type(m_type);
		pro->set_m_roomid(m_id);
		pro->set_m_dataid(0);
		for (std::pair<const int64_t, pbnet::MATCHING_MEMBER>& item : m_memberlist)
		{
			*pro->add_m_member() = item.second;
		}
		// 发送给[ACTOR_MANAGE_PLAYS]创建玩法
		actor::static_send_actor(nguid::make_self(ACTOR_MANAGE_PLAYS), nguid::make(), pro);
		// 发送给所有玩家
		send_client(pro);
		return pbnet::ematching_confirm_success;
	}

	void room::auto_cancel(pbnet::etype_matching_cancel atype, std::vector<i64_actorid>& avec)
	{
		auto pro = std::make_shared<pbnet::PROBUFF_NET_MEMBER_MATCHING_CANCEL>();
		pro->set_m_type(m_type);
		pro->set_m_canceltype(pbnet::etype_matching_cancel_timeout);
		std::for_each(avec.begin(), avec.end(), [&pro](i64_actorid amember)
			{
				pro->add_m_cancelmember(amember);
			});
		send_client(pro);
	}

	void room::check_timeout()
	{
		int lnow = localtime::gettime();
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
				std::for_each(m_memberlist.begin(), m_memberlist.end(), [&louttime](std::pair<const int64_t, pbnet::MATCHING_MEMBER>& apair)
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
				std::for_each(m_memberlist.begin(), m_memberlist.end(), [&louttime](std::pair<const int64_t, pbnet::MATCHING_MEMBER>& apair)
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
			return false;
		if (is_roomfull() == true)
			return false;
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
			// 匹配成功
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