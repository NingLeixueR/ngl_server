#include "pack_head.h"
#include "nguid.h"
#include "nlog.h"

namespace ngl
{
	int32_t pack_head::m_version = 0;

	pack_head::pack_head() :
		m_wpos(0)
	{
		if (m_version == 0)
		{
			ngl::xmlinfo* lpublicxml = ngl::xmlnode::get_publicconfig();
			assert(lpublicxml->find("head_version", m_version));
		}
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

	void pack_head::set_requestactor(i64_actorid arequestactor)
	{
		*(i64_actorid*)(&m_data[EPH::EPH_REQUEST_ACTOR_TYPEAREA]) = arequestactor;
	}

	void pack_head::set_time()
	{
		head_set_time(m_data);
	}

	void pack_head::head_set_time(int32_t* abuff)
	{
		abuff[EPH::EPH_TIME] = localtime::gettime();
	}

	i32_time pack_head::head_get_time(const int32_t* abuff)
	{
		return abuff[EPH::EPH_TIME];
	}

	i32_time pack_head::get_time()const
	{
		return head_get_time(m_data);
	}

	void pack_head::head_set_version(int32_t* abuff)
	{
		abuff[EPH::EPH_VERSION] = m_version;
	}

	int32_t pack_head::head_get_version(const int32_t* abuff)
	{
		return abuff[EPH::EPH_VERSION];
	}

	void pack_head::set_version()
	{
		pack_head::head_set_version(m_data);
	}

	int32_t pack_head::get_version()const
	{
		return pack_head::head_get_version(m_data);
	}

	i64_actorid pack_head::get_actor()const
	{
		return *(i64_actorid*)(&m_data[EPH::EPH_ACTOR_TYPEAREA]);
	}

	i64_actorid pack_head::get_request_actor()const
	{
		return *(i64_actorid*)(&m_data[EPH::EPH_REQUEST_ACTOR_TYPEAREA]);
	}

	i16_actortype pack_head::get_actortype()const
	{
		return nguid::type(get_actor());
	}

	i16_actortype pack_head::get_request_actortype()const
	{
		return nguid::type(get_request_actor());
	}

	i16_area pack_head::get_actorearea()const
	{
		return nguid::area(get_actor());
	}

	i32_actordataid pack_head::get_actordataid()const
	{
		return nguid::actordataid(get_actor());
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

	EPH_HEAD_VAL pack_head::isready()const
	{
		EPH_HEAD_VAL lval = isversion();
		if (lval != EPH_HEAD_VERSION_SUCCESS)
			return lval;
		return m_wpos >= size() ? EPH_HEAD_SUCCESS : EPH_HEAD_FOLLOW;
	}

	EPH_HEAD_VAL pack_head::isversion()const
	{
		static int32_t lversionpos = (EPH::EPH_VERSION + 1) * sizeof(int32_t);
		if (m_wpos < lversionpos)
		{
			return EPH_HEAD_VERSION_FOLLOW;
		}
		return get_version() == m_version ? EPH_HEAD_VERSION_SUCCESS : EPH_HEAD_VERSION_FAIL;
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

	EPH_HEAD_VAL pack_head::push(const char*& abuff, int& alen)
	{
		EPH_HEAD_VAL lval = isready();
		if (lval == EPH_HEAD_VERSION_FOLLOW || lval == EPH_HEAD_FOLLOW)
		{
			int ltemp = size() - m_wpos;
			ltemp = (alen < ltemp) ? alen : ltemp;
			char* lp = (char*)m_data;
			memcpy(&lp[m_wpos], abuff, ltemp);
			alen -= ltemp;
			abuff += ltemp;
			m_wpos += ltemp;
			return isready();
		}
		return lval;		
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