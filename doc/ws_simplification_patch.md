# WebSocket 简化方案

## 问题分析

当前 WebSocket 实现存在以下问题：
1. 使用自定义的 `ws_send_node` 类，增加复杂度
2. 支持 text/binary/pack 三种类型，但实际上 WebSocket 协议本身就区分 text/binary
3. 与 TCP 实现不一致，TCP 使用 `node_pack` 和 `m_list`/`m_issend`

## 简化方案

### 1. 统一数据结构

**修改 `service_ws` (asio_base.h:189-191)**
```cpp
// 旧代码
bool m_ws_sending = false;
std::deque<ws_send_node> m_ws_send_list;

// 新代码
bool m_issend = false;
std::deque<node_pack> m_list;
```

### 2. 简化 queue_send 函数

**修改 `asio_ws.cpp:675-712`**
```cpp
bool asio_ws::queue_send(i32_sessionid asessionid, node_pack anode)
{
	if (!ngl::ws::is_alive(m_alive)) return false;

	const std::shared_ptr<service_ws> ws = get_ws(asessionid);
	if (ws == nullptr || ws->m_closing.load(std::memory_order_acquire)) return false;

	bool lstart = false;
	{
		std::lock_guard<std::mutex> llock(ws->m_mutex);
		if (ws->m_closing.load(std::memory_order_relaxed)) return false;
		ws->m_list.push_back(std::move(anode));
		if (!ws->m_issend)
		{
			ws->m_issend = true;
			lstart = true;
		}
	}
	if (lstart) do_send(ws);
	return true;
}
```

### 3. 简化 do_send 函数

**修改 `asio_ws.cpp:714-947`**
```cpp
void asio_ws::do_send(const std::shared_ptr<service_ws>& aservice)
{
	if (!ngl::ws::is_alive(m_alive) || aservice == nullptr) return;

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
			astream.async_write(bufs,
				[this, aservice, lpack](const basio_errorcode& ec, std::size_t)
				{
					handle_write(aservice, ec, lpack);
					if (!ec) do_send(aservice);
				}
			);
		}
		else
		{
			astream.async_write(basio::buffer(lpack->m_buff, lpack->m_pos),
				[this, aservice, lpack](const basio_errorcode& ec, std::size_t)
				{
					handle_write(aservice, ec, lpack);
					if (!ec) do_send(aservice);
				}
			);
		}
	});
}
```

### 4. 简化 handle_write 函数

**修改 `asio_ws.cpp:854-877`**
```cpp
void asio_ws::handle_write(const std::shared_ptr<service_ws>& aservice,
                           const basio_errorcode& error,
                           const std::shared_ptr<pack>& apack)
{
	if (!ngl::ws::is_alive(m_alive)) return;

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
```

### 5. 更新 send 函数

**修改 `asio_ws.cpp:1069-1091`**
```cpp
bool asio_ws::send(i32_sessionid asessionid, std::shared_ptr<pack>& apack)
{
	return queue_send(asessionid, node_pack(apack));
}

bool asio_ws::send(i32_sessionid asessionid, std::shared_ptr<void>& apack)
{
	return queue_send(asessionid, node_pack(apack));
}

// 删除 send_text 函数，WebSocket 的 text 模式通过 pack 发送
```

### 6. 更新头文件声明

**修改 `asio_ws.h:123-125`**
```cpp
bool queue_send(i32_sessionid asessionid, node_pack anode);
void do_send(const std::shared_ptr<service_ws>& aservice);
void handle_write(const std::shared_ptr<service_ws>& aservice,
                  const basio_errorcode& error,
                  const std::shared_ptr<pack>& apack);
```

## 优化效果

1. **代码量减少**: 从 ~230 行减少到 ~70 行
2. **逻辑统一**: 与 TCP 实现完全一致
3. **维护性提升**: 只需维护一套发送逻辑
4. **性能优化**: 使用两个 buffer 一次发送（header + body）

## 注意事项

1. WebSocket 的 text/binary 模式由 `astream.text(true)` 或 `astream.binary(true)` 控制
2. 当前简化方案统一使用 binary 模式，如需 text 模式可在 pack 中添加标志位
3. 删除了 `ws_send_node` 类，可以考虑从 `asio_base.h` 中移除其定义
