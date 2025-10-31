/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#include "actor_example_guess_number.h"
#include "actor_brief.h"
namespace ngl
{
	bool actor_example_guess_number::handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER>>& adata)
	{
		const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER* lpdata = adata.get_data()->data();
		i64_actorid roleid = adata.get_data()->identifier();

		if (isfinish())
		{//游戏已经结束
			bomb();
			return true;
		}
		if (roleid != next_guess_role())
		{
			send_error(roleid, pbexample::E_GUESS_NUMBER_ERROR_NOT_GUESS);
			return true;
		}
		int32_t lnumber = lpdata->mguessnumber();
		if (lnumber >= m_maxnumber || lnumber <= m_minnumber)
		{
			send_error(roleid, pbexample::E_GUESS_NUMBER_ERROR_VALUE);
			return true;
		}

		if (lnumber == m_bombvalues)
		{//炸了 游戏结束
			set_finish(roleid);
			bomb();
			return true;
		}

		if (lnumber > m_bombvalues)
		{
			m_maxnumber = lnumber;
		}
		else
		{
			m_minnumber = lnumber;
		}

		int32_t lsize = m_maxnumber - 1 - m_minnumber;
		if (lsize <= 1)
		{//游戏结束 下一个人被炸
			set_finish(next_guess_role());
			bomb();
			return true;
		}

		next_setp();
		return true;
	}
	bool actor_example_guess_number::handle(const message<np_example_entergame_ready>& adata)
	{
		next_setp();
		return true;
	}
}//namespace ngl
