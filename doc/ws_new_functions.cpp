// 替换 asio_ws.cpp 中的 do_send 和 handle_write 函数

void asio_ws::do_send(const std::shared_ptr<service_ws>& aservice)
{
	if (!ngl::ws::is_alive(m_alive))
	{
		return;
	}
	if (aservice == nullptr)
	{
		return;
	}
	if (aservice->m_closing.load(std::memory_order_acquire))
	{
		std::lock_guard<std::mutex> llock(aservice->m_mutex);
		aservice->m_list.clear();
		aservice->m_issend = false;
		return;
	}

	node_pack litem;
	{
		std::lock_guard<std::mutex> llock(aservice->m_mutex);
		if (aservice->m_list.empty())
		{
			aservice->m_issend = false;
			return;
		}
		litem = aservice->m_list.front();
		aservice->m_list.pop_front();
	}

	std::shared_ptr<pack> lpack = litem.get_pack();
	if (lpack == nullptr)
	{
		do_send(aservice);
		return;
	}

	lpack->m_protocol = ENET_WS;

	aservice->visit_stream([this, aservice, lpack](auto& astream)
		{
			astream.binary(true);

			if (lpack->m_head != nullptr)
			{
				std::array<basio::const_buffer, 2> bufs{
					basio::buffer(lpack->m_head->m_data, sizeof(lpack->m_head->m_data)),
					basio::buffer(lpack->m_buff, lpack->m_pos)
				};
				astream.async_write(
					bufs,
					[this, aservice, lpack](const basio_errorcode& ec, std::size_t)
					{
						handle_write(aservice, ec, lpack);
						if (ec) return;
						do_send(aservice);
					}
				);
			}
			else
			{
				astream.async_write(
					basio::buffer(lpack->m_buff, lpack->m_pos),
					[this, aservice, lpack](const basio_errorcode& ec, std::size_t)
					{
						handle_write(aservice, ec, lpack);
						if (ec) return;
						do_send(aservice);
					}
				);
			}
		}
	);
}

void asio_ws::handle_write(const std::shared_ptr<service_ws>& aservice, const basio_errorcode& error, const std::shared_ptr<pack>& apack)
{
	if (!ngl::ws::is_alive(m_alive))
	{
		return;
	}
	if (error)
	{
		if (!aservice->m_closing.load(std::memory_order_acquire))
		{
			log_error()->print("asio_ws::handle_write [{}]", error.message());
		}
		close(aservice.get());
	}
	if (m_sendfinishfun != nullptr)
	{
		m_sendfinishfun(aservice->m_sessionid, error ? true : false, apack.get());
	}
}
