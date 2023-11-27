#pragma once

#include <map>
#include <list>
#include <forward_list>
#include <cstdint>
#include "type.h"
#include "localtime.h"
#include "net.pb.h"
#include "csvtable.h"
#include "actor.h"

namespace ngl
{
	// 匹配房间的状态
	enum ematching_room
	{
		ematching_run,				// 匹配中
		ematching_waitconfirm,		// 等待确认(如果需要确认的话)
		ematching_create,			// 创建玩法
	};

#define ematching_create_time		(3*60)

	class room
	{
		int32_t m_id;							// 房间id
		pbnet::eplays m_type;				// 玩法类型
		std::map<int64_t, pbnet::MATCHING_MEMBER> m_memberlist;
		tab_matching* m_tab;
		ematching_room m_stat;
		int32_t m_wbegtm;					// 等待匹配的开始时间

		static std::array<room*, pbnet::eplays_count> m_typeroom;

		bool initmember(i64_actorid aroleid, pbnet::MATCHING_MEMBER& amember);
	public:
		room(int32_t aid, pbnet::eplays m_type, int32_t atid);

		room(pbnet::eplays m_type)
			: m_id(0)
			, m_type(m_type)
			, m_tab(nullptr)
			, m_stat(ematching_run)
		{
			m_typeroom[m_type] = this;
		}

		virtual room* make_create(int aid, int atid) = 0;

		
		static room* create(int aid, pbnet::eplays m_type, int atid)
		{
			room* lpdata = m_typeroom[m_type];
			if (lpdata == nullptr)
				return nullptr;
			return lpdata->make_create(aid, atid);
		}

		int id();

		pbnet::eplays type();

		ematching_room stat()
		{
			return m_stat;
		}

		void change_stat(ematching_room atype)
		{
			if (atype > m_stat)
			{
				return;
			}
			m_wbegtm = localtime::gettime();
			m_stat = atype;
		}

		bool empty()
		{
			return m_memberlist.empty();
		}

		pbnet::MATCHING_MEMBER* find(i64_actorid aroleid);

		bool try_addroom(i64_actorid aroleid);

		pbnet::ematching_cancel_stat try_cancel(i64_actorid aroleid);

		// 房间是否已经满了
		bool is_roomfull();

		// 是否可以加入房间
		virtual bool is_addroom(i64_actorid aroleid);

		template <typename T>
		void send_client(std::shared_ptr<T>& apro)
		{
			std::list<i64_actorid> lactorset;
			std::transform(m_memberlist.begin(), m_memberlist.end(), std::inserter(lactorset
				, lactorset.end())
				, [](std::pair<const i64_actorid, pbnet::MATCHING_MEMBER>& pair)
				{
					return pair.first;
				}
			);
			actor::send_client(lactorset, apro);
		}

		void sync_confirm();

		pbnet::ematching_confirm_stat confirm(i64_actorid aroleid);

		void auto_cancel(pbnet::etype_matching_cancel atype, std::vector<i64_actorid>& avec);

		void check_timeout();

		// # 检查是否开启玩法有足够的人
		bool check_playscount();
	};

	class room_go_undergroundpalace : public room
	{
		room_go_undergroundpalace(int aid, int atid)
			: room(aid, pbnet::eplays_go_undergroundpalace, atid)
		{}
	public:
		room_go_undergroundpalace()
			: room(pbnet::eplays_go_undergroundpalace)
		{}

		virtual room* make_create(int aid, int atid)
		{
			return new room_go_undergroundpalace(aid, atid);
		}
	};

	// ########################## roomid #################################
	// ##### uint16(玩法tid) ########################## int16(dataid) ####

	union roomid
	{
		int32_t m_id;
		int16_t m_value[2];			// m_value1[0] = type  m_value1[1]=actordataid

		roomid(int16_t atid, int16_t adataid)
		{
			m_value[0] = atid;
			m_value[1] = adataid;
		}

		roomid(int32_t aid)
		{
			m_id = aid;
		}

		static int32_t make(int16_t atid, int16_t adataid)
		{
			roomid ltemp(atid, adataid);
			return ltemp.m_id;
		}

		static int16_t tid(int32_t aid)
		{
			roomid ltemp(aid);
			return ltemp.m_value[0];
		}

		static int16_t dataid(int32_t aid)
		{
			roomid ltemp(aid);
			return ltemp.m_value[1];
		}
	};

	class matching
	{
	private:
		using prt_room = std::shared_ptr<room>;
		
		std::array<std::list<prt_room>, pbnet::eplays_count> m_room;
		std::array<int32_t, pbnet::eplays_count> m_currentroom;
		std::map<int32_t, room*> m_roombyid;							// key roomid
		std::map<i64_actorid, int32_t> m_roombyroleid;					// key roleid value roomid
	public:
		matching();

		pbnet::MATCHING_MEMBER* find(i64_actorid aid)
		{
			pbnet::MATCHING_MEMBER* lp = nullptr;
			auto itor = m_roombyroleid.find(aid);
			if (itor == m_roombyroleid.end())
				return nullptr;
			auto itor_room = m_roombyid.find(itor->second);
			if (itor_room == m_roombyid.end())
				return nullptr;
			return itor_room->second->find(aid);
		}

		room* findroombyrole(i64_actorid aroleid)
		{
			auto itor = m_roombyroleid.find(aroleid);
			if (itor == m_roombyroleid.end())
				return nullptr;
			auto itorroom = m_roombyid.find(itor->second);
			if (itorroom == m_roombyid.end())
				return nullptr;
			return itorroom->second;
		}

		bool add_member(pbnet::eplays atype, int8_t atid, i64_actorid aroleid)
		{
			if (find(aroleid) != nullptr)
				return false;

			room* lproom = nullptr;
			for (auto itor = m_room[atype].begin();itor != m_room[atype].end();++itor)
			{
				if ((*itor)->try_addroom(aroleid))
				{
					lproom = (*itor).get();
					break;
				}
			}
			if (lproom == nullptr)
			{
				// 新建房间
				
				int32_t lroomid = roomid::make(atid, ++m_currentroom[atype]);
				lproom = room::create(lroomid, atype, atype);
				std::shared_ptr<room> lroomptr;
				m_room[atype].push_back(lroomptr);
				m_roombyid.insert(std::make_pair(lroomid, lroomptr.get()));
				if (lproom->try_addroom(aroleid))
					return false;
				m_roombyroleid.insert(std::make_pair(aroleid, lroomid));
			}
			
			auto pro = std::make_shared<pbnet::PROBUFF_NET_MATCHING_SYNC>();
			pro->set_m_roomid(lproom->id());
			 

			return true;
		}

		pbnet::ematching_confirm_stat confirm(int32_t aroomid, i64_actorid aroleid)
		{
			auto itor = m_roombyid.find(aroomid);
			if (itor == m_roombyid.end())
			{
				return pbnet::ematching_confirm_roomnotfind;
			}
			return itor->second->confirm(aroleid);
		}

		pbnet::ematching_cancel_stat try_cancel(int32_t aroomid, i64_actorid aroleid)
		{
			auto itor = m_roombyid.find(aroomid);
			if (itor == m_roombyid.end())
			{
				return pbnet::ematching_cancel_roomnotfind;
			}
			return itor->second->try_cancel(aroleid);
		}

		void check_timeout()
		{
			for (std::pair<const int32_t, room*>& item : m_roombyid)
			{
				item.second->check_timeout();
			}
		}

		// # 检查是否开启玩法有足够的人
		void check_playscount()
		{
			for (std::pair<const int32_t, room*>& item : m_roombyid)
			{
				item.second->check_playscount();
			}
		}

		// ## 在固定时间进行检查[单位秒]
		void do_match()
		{
			// 到时间自动取消的
			check_timeout();
			// 够人数直接进入
			check_playscount();
		}
	};
}