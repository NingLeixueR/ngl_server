/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements logic for net.

#include "net/asio_base.h"

#include <algorithm>
#include <type_traits>

namespace ngl
{
	namespace
	{
		template <typename T>
		using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;
	}

	basio_ioservice* serviceio_info::get_ioservice(i32_threadid athreadid)
	{
		if (m_ioservices.empty())
		{
			return nullptr;
		}
		if (athreadid < 0 || static_cast<size_t>(athreadid) >= m_ioservices.size())
		{
			return std::get<0>(m_ioservices[0]).get();
		}
		return std::get<0>(m_ioservices[athreadid]).get();
	}

	basio_ioservicework* serviceio_info::get_ioservice_work(i32_threadid athreadid)
	{
		if (m_ioservices.empty())
		{
			return nullptr;
		}
		if (athreadid < 0 || static_cast<size_t>(athreadid) >= m_ioservices.size())
		{
			return std::get<1>(m_ioservices[0]).get();
		}
		return std::get<1>(m_ioservices[athreadid]).get();
	}

	serviceio_info::serviceio_info(i32_threadid athread, int32_t abuffmaxsize) :
		m_buffmaxsize(abuffmaxsize),
		m_recvthreadsize(std::max<int32_t>(1, athread)),
		m_next_index(0)
	{
		// Ensure at least one io_service exists. create exactly m_recvthreadsize services.
		for (int32_t i = 0; i < m_recvthreadsize; ++i)
		{
			auto lpioservice = std::make_shared<basio_ioservice>();
			auto lpwork = std::make_shared<basio_ioservicework>(basio::make_work_guard(*lpioservice));
			m_ioservices.push_back(
				std::make_tuple(lpioservice, lpwork, std::make_shared<std::thread>([lpioservice]() {lpioservice->run(); }))
			);
		}
	}

	void serviceio_info::shutdown()
	{
		if (m_ioservices.empty())
		{
			return;
		}

		for (auto& item : m_ioservices)
		{
			std::get<1>(item).reset();
		}

		for (auto& item : m_ioservices)
		{
			if (auto& service = std::get<0>(item); service != nullptr)
			{
				service->stop();
			}
		}

		for (auto& item : m_ioservices)
		{
			auto& thread = std::get<2>(item);
			if (thread == nullptr || !thread->joinable())
			{
				continue;
			}

			if (thread->get_id() == std::this_thread::get_id())
			{
				thread->detach();
			}
			else
			{
				thread->join();
			}
		}

		m_ioservices.clear();
		m_next_index = 0;
		m_recvthreadsize = 0;
	}

	serviceio_info::~serviceio_info()
	{
		shutdown();
	}

	service_io::service_io(serviceio_info& amsi, i32_session asessionid) :
		m_threadid(amsi.m_next_index++ % amsi.m_recvthreadsize)
		, m_ioservice(*(amsi.get_ioservice(m_threadid)))
		, m_buff1(std::make_unique<char[]>(amsi.m_buffmaxsize))
		, m_buff2(std::make_unique<char[]>(amsi.m_buffmaxsize))
		, m_pbuff1(m_buff1.get())
		, m_pbuff2(m_buff2.get())
		, m_sessionid(asessionid)
		, m_is_lanip(false)
		, m_issend(false)
	{
	}

	char* service_io::buff()
	{
		std::swap(m_pbuff1, m_pbuff2);
		return m_pbuff1;
	}

	service_tcp::service_tcp(serviceio_info& amsi, i32_session asessionid) :
		service_io(amsi, asessionid),
		m_socket(m_ioservice)
	{
	}

	service_ws::service_ws(serviceio_info& amsi, i32_session asessionid) :
		service_io(amsi, asessionid),
		m_stream(std::in_place_type<basio_websocket>, m_ioservice),
		m_use_tls(false)
	{
		visit_stream([](auto& astream)
			{
				astream.binary(true);
			}
		);
	}

	service_ws::service_ws(serviceio_info& amsi, i32_session asessionid, basio_sslcontext& acontext) :
		service_io(amsi, asessionid),
		m_stream(std::in_place_type<basio_websocket_tls>, m_ioservice, acontext),
		m_use_tls(true)
	{
		visit_stream([](auto& astream)
			{
				astream.binary(true);
			}
		);
	}

	basio_iptcpsocket& service_ws::socket()
	{
		return visit_stream([](auto& astream) -> basio_iptcpsocket&
			{
				using stream_type = remove_cvref_t<decltype(astream)>;
				if constexpr (std::is_same_v<stream_type, basio_websocket>)
				{
					return astream.next_layer().socket();
				}
				else
				{
					return astream.next_layer().next_layer().socket();
				}
			}
		);
	}

	const basio_iptcpsocket& service_ws::socket() const
	{
		return visit_stream([](const auto& astream) -> const basio_iptcpsocket&
			{
				using stream_type = remove_cvref_t<decltype(astream)>;
				if constexpr (std::is_same_v<stream_type, basio_websocket>)
				{
					return astream.next_layer().socket();
				}
				else
				{
					return astream.next_layer().next_layer().socket();
				}
			}
		);
	}

	bool service_ws::using_tls() const
	{
		return m_use_tls;
	}

	bool service_ws::message_is_text() const
	{
		return m_message_is_text;
	}

	void service_ws::set_message_is_text(bool avalue)
	{
		m_message_is_text = avalue;
	}

	beast::flat_buffer& service_ws::read_buffer()
	{
		return m_read_buffer;
	}

	void service_ws::consume_read_buffer(std::size_t asize)
	{
		m_read_buffer.consume(std::min(asize, m_read_buffer.size()));
	}
}// namespace ngl