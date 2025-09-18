#include "udp_cmd.h"
#include "asio_kcp.h"

namespace ngl
{
	bool udp_cmd::run_cmd(asio_kcp* ap, ptr_se& apstruct, const char* abuf, int32_t alen)
	{
		if (alen < ecmd_minlen)
		{
			return false;
		}
		if (memcmp(abuf, "ecmd*", ecmd_minlen) != 0)
		{
			return false;
		}
		try
		{
			std::string lecmd;
			int32_t lnum = 0;
			std::string ljson;
			if (tools::splite(abuf, "*", lecmd, lnum, ljson) == false)
			{
				return false;
			}
			handle_cmd::function((ecmd)lnum, ap, apstruct, ljson);
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	void udp_cmd::register_fun(ecmd anum, const ecmd_callback& afun)
	{
		handle_cmd::add(anum) = afun;
	}

	bool udp_cmd::sendcmd(asio_kcp* akcp, i32_sessionid asession, udp_cmd::ecmd acmd, const std::string& ajson)
	{
		std::string lbuff = std::format("ecmd*{}*{}", (int)acmd, ajson.c_str());
		if (lbuff.empty())
		{
			log_error()->print("udp_cmd::sendcmd fail [{}][{}]", (int)acmd, ajson);
			return false;
		}
		akcp->send(asession, lbuff.c_str(), (int)lbuff.size());
		return true;
	}

}//namespace ngl