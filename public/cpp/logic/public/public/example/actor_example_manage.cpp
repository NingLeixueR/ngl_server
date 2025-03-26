#include "actor_example_manage.h"



namespace ngl
{
	bool actor_example_manage::handle(const message<np_create_example>& adata)
	{
		const np_create_example* lprecv = adata.get_data();
		if (lprecv == nullptr)
		{
			return true;
		}
		lprecv->m_type;
		bool lcreate = false;
		switch (lprecv->m_type)
		{
		case pbexample::EPLAY_GUESS_NUMBER:
		{
			lcreate = actor_base::create(
				ACTOR_EXAMPLE_GUESS_NUMBER, 
				tab_self_area, 
				++m_exampleindex[pbexample::EPLAY_GUESS_NUMBER],
				(void*)&lprecv->m_roleids
			) != nullptr;
		}
		break;
		}

		pbexample::PROBUFF_EXAMPLE_PLAY_CREATE pro;

		
		return true;
	}
	

}//namespace ngl