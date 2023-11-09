#include "pack_head.h"
#include "actor_guid.h"
#include "nlog.h"

namespace ngl
{
	pack_head::pack_head() :
		m_wpos(0)
	{
		memset(m_data, 0x0, size());
	}

	void pack_head::head_set_actor(
		int32_t* abuff,
		i64_actorid aactor, /* aenum == ACTOR_NONE 此值无效 */
		i64_actorid arequestactorid
	)
	{
		/* 此值为ACTOR_NONE,则送发送的进程上的所有注册ACTOR的都会被调用 (EPH::EPH_ACTORID_TYPE)*/
		/* aenum == ACTOR_NONE 此值无效 (EPH::EPH_ACTORID_ID) */
		*(i64_actorid*)(&abuff[EPH::EPH_ACTOR_TYPEAREA]) = aactor;
		*(i64_actorid*)(&abuff[EPH::EPH_REQUEST_ACTOR_TYPEAREA]) = arequestactorid;
	}

	void pack_head::set_actor(
		i64_actorid aactor, /* aenum == ACTOR_NONE 此值无效 */
		i64_actorid arequestactorid
	)
	{
		head_set_actor(m_data, aactor, arequestactorid);
	}

	void pack_head::set_actor(uint32_t* abuff, i64_actorid aactor)
	{
		serialize lserialize((char*)&abuff[EPH::EPH_ACTOR_TYPEAREA], sizeof(i64_actorid));
		lserialize.push(((int32_t*)(&aactor))[0], ((int32_t*)(&aactor))[1]);
	}

	void pack_head::set_time()
	{
		head_set_time(m_data);
	}

	void pack_head::head_set_time(int32_t* abuff)
	{
		abuff[EPH::EPH_TIME] = localtime::gettime();
	}

	i32_time pack_head::head_get_time(int32_t* abuff)
	{
		return abuff[EPH::EPH_TIME];
	}

	i32_time pack_head::get_time()
	{
		return head_get_time(m_data);
	}

	i64_actorid pack_head::get_actor()
	{
		return *(i64_actorid*)(&m_data[EPH::EPH_ACTOR_TYPEAREA]);
	}

	i64_actorid pack_head::get_request_actor()
	{
		return *(i64_actorid*)(&m_data[EPH::EPH_REQUEST_ACTOR_TYPEAREA]);
	}

	i16_actortype pack_head::get_actortype()
	{
		return actor_guid::type(get_actor());
	}

	i16_actortype pack_head::get_request_actortype()
	{
		return actor_guid::type(get_request_actor());
	}

	i16_area pack_head::get_actorearea()
	{
		return actor_guid::area(get_actor());
	}

	i32_actordataid pack_head::get_actordataid()
	{
		return actor_guid::actordataid(get_actor());
	}

	void pack_head::reset()
	{
		memset(&m_data, 0x0, EPH_SUM * sizeof(int));
		m_wpos = 0;
	}

	int pack_head::getvalue(EPH aeph)const
	{
		return m_data[aeph];
	}

	int pack_head::get_bytes()const
	{
		return m_data[EPH::EPH_BYTES];
	}

	int pack_head::size()
	{
		return EPH_SUM * sizeof(int);
	}

	bool pack_head::isok()
	{
		return m_wpos >= size();
	}

	i32_protocolnum pack_head::protocolnum()const
	{
		return m_data[EPH::EPH_PROTOCOLNUM];
	}

	///// 获取协议号
	i32_protocolnum pack_head::get_protocolnumber()const
	{
		return protocolnum();
	}

	void pack_head::set_protocol(i32_protocolnum aprotocolnum)
	{
		m_data[EPH::EPH_PROTOCOLNUM] = aprotocolnum;
	}

	EPROTOCOL_TYPE pack_head::get_protocoltype()const
	{
		return (EPROTOCOL_TYPE)m_data[EPH::EPH_PROTOCOLTYPE];
	}
	void pack_head::set_protocoltype(EPROTOCOL_TYPE atype)
	{
		m_data[EPH::EPH_PROTOCOLTYPE] = atype;
	}

	bool pack_head::push(const char*& abuff, int& alen)
	{
		if (isok())
			return true;
		int ltemp = size() - m_wpos;
		ltemp = (alen < ltemp) ? alen : ltemp;
		char* lp = (char*)m_data;
		memcpy(&lp[m_wpos], abuff, ltemp);
		alen -= ltemp;
		abuff += ltemp;
		m_wpos += ltemp;
		return isok();
	}

	bool pack_head::push(ngl::serialize& aflow)
	{
		return aflow.basetype(m_data, sizeof(int32_t) * EPH_SUM);
	}

	void pack_head::reservebuff(char* abuff, int abufflen, std::pair<char*, int>& apair)
	{
		apair.first = &(abuff[size()]);
		apair.second = abufflen - size();
	}

	bool pack_head::_log(logserialize& atstr, int anumber/* = EPH_BYTES*/)const
	{
		if (atstr(m_data[anumber])) return false;
		if (atstr("#"))				return false;
		if (EPH_SUM != anumber)		return _log(atstr, anumber + 1);
		return true;
	}

	bool pack_head::log(logserialize& atstr)const
	{
		return atstr("HEAD[") && _log(atstr) && atstr("]");
	}
}