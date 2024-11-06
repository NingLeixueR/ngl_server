#pragma once

#include "pack_head.h"
#include "type.h"

#include <functional>
#include <cstdint>
#include <atomic>

namespace ngl
{
	class bpool
	{
	public:
		bpool();
		char* malloc(int32_t alen);
		void free(char* ap, int32_t alen);
	};

	class segpack;

	struct pack
	{
		pack() {};

		void free();
	public:
		// # ����������Э������
		ENET_PROTOCOL	m_protocol		= ENET_TCP;
		i32_session		m_id			= 0;
		pack_head		m_head;
		segpack*		m_segpack		= nullptr;
		bpool*			m_bpool			= nullptr;
		/*[������������� Start]*/
		char*			m_buff			= nullptr;
		int32_t			m_len			= 0;
		int32_t			m_pos			= 0;
		/*[������������� Finish]*/

		~pack();

		// # �����ڴ��
		void set(bpool& apool);
		
		// # �������ݰ�
		void reset();

		// # �Ƿ�������
		bool isready();

		// # Ϊ���ݰ�����
		bool malloc(int32_t alen);

		// # �����ݰ�������actor
		void set_actor(i64_actorid aactor, i64_actorid arequestactorid);

		// # �����
		static std::shared_ptr<pack> make_pack(bpool* apool, int32_t alen);
	};
}// namespace ngl