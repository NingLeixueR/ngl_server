/*
* Copyright (c) [2020-2025] NingLeixueR
*
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
*/
// File overview: Implements GoogleTest coverage for socket_pool allocation and free behavior.

#include "tools/serialize/socket_pool.h"
#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>
#include <numeric>

namespace ngl
{
	TEST(SocketPoolTest, BasicAllocation)
	{
		char* buff = socket_pool::malloc(256);
		ASSERT_NE(buff, nullptr);
		memset(buff, 0xAB, 256);
		socket_pool::free(buff);
	}

	TEST(SocketPoolTest, MultipleAllocations)
	{
		std::vector<char*> buffers;
		int sizes[] = { 64, 128, 512, 1024, 4096 };

		for (int size : sizes)
		{
			char* buff = socket_pool::malloc(size);
			ASSERT_NE(buff, nullptr);
			buffers.push_back(buff);
		}

		for (char* buff : buffers)
		{
			socket_pool::free(buff);
		}
	}

	TEST(SocketPoolTest, LargeAllocation)
	{
		char* buff = socket_pool::malloc(200000);
		ASSERT_NE(buff, nullptr);
		socket_pool::free(buff);
	}

	TEST(SocketPoolTest, CrossThreadFree)
	{
		char* buff = socket_pool::malloc(128);
		ASSERT_NE(buff, nullptr);
		memset(buff, 0xCD, 128);

		std::thread t([buff]()
		{
			socket_pool::free(buff);
		});
		t.join();

		// The freed buffer should be reclaimed by the allocating thread
		// on the next malloc via pending queue drain.
		char* buff2 = socket_pool::malloc(128);
		ASSERT_NE(buff2, nullptr);
		socket_pool::free(buff2);
	}

	TEST(SocketPoolTest, CrossThreadBatch)
	{
		constexpr int count = 32;
		std::vector<char*> buffers(count);
		for (int i = 0; i < count; ++i)
		{
			buffers[i] = socket_pool::malloc(256);
			ASSERT_NE(buffers[i], nullptr);
		}

		// Free all from a different thread.
		std::thread t([&buffers]()
		{
			for (char* p : buffers)
				socket_pool::free(p);
		});
		t.join();

		// Allocating thread drains pending on next malloc.
		char* check = socket_pool::malloc(256);
		ASSERT_NE(check, nullptr);
		socket_pool::free(check);
	}

	TEST(SocketPoolTest, CrossThreadContentPreserved)
	{
		constexpr int size = 512;
		char* buff = socket_pool::malloc(size);
		ASSERT_NE(buff, nullptr);

		for (int i = 0; i < size; ++i)
			buff[i] = static_cast<char>(i & 0xFF);

		std::atomic<bool> verified{false};
		std::thread t([buff, &verified]()
		{
			for (int i = 0; i < size; ++i)
			{
				if (buff[i] != static_cast<char>(i & 0xFF))
				{
					verified.store(false, std::memory_order_relaxed);
					socket_pool::free(buff);
					return;
				}
			}
			verified.store(true, std::memory_order_relaxed);
			socket_pool::free(buff);
		});
		t.join();
		EXPECT_TRUE(verified.load());
	}

	TEST(SocketPoolTest, SlotOverflow)
	{
		constexpr int thread_count = 140;
		std::atomic<int> success_count{0};
		std::vector<std::thread> threads;
		threads.reserve(thread_count);

		for (int i = 0; i < thread_count; ++i)
		{
			threads.emplace_back([&success_count]()
			{
				char* p = socket_pool::malloc(128);
				if (p != nullptr)
				{
					memset(p, 0xAA, 128);
					socket_pool::free(p);
					success_count.fetch_add(1, std::memory_order_relaxed);
				}
			});
		}
		for (auto& t : threads)
			t.join();

		EXPECT_EQ(success_count.load(), thread_count);
	}

	TEST(SocketPoolTest, BucketTrim)
	{
		constexpr int alloc_count = 250;
		std::vector<char*> buffers(alloc_count);
		for (int i = 0; i < alloc_count; ++i)
		{
			buffers[i] = socket_pool::malloc(64);
			ASSERT_NE(buffers[i], nullptr);
		}

		// Free all from another thread -> pushed to owner's pending queue.
		std::thread t([&buffers]()
		{
			for (char* p : buffers)
				socket_pool::free(p);
		});
		t.join();

		// Next malloc triggers drain_pending + trim (bucket 0 limit = 100 * 2 = 200).
		// 250 returned, 200 kept, 50 deleted. No crash = trim works.
		char* check = socket_pool::malloc(64);
		ASSERT_NE(check, nullptr);
		socket_pool::free(check);
	}

	TEST(SocketPoolTest, ConcurrentStress)
	{
		constexpr int thread_count = 8;
		constexpr int ops_per_thread = 1000;
		const int sizes[] = {32, 64, 128, 256, 512, 1024, 4096};

		auto& stats_before = socket_pool::get_stats();
		uint64_t alloc_before = stats_before.m_alloc_count.load(std::memory_order_relaxed);
		uint64_t free_before = stats_before.m_free_count.load(std::memory_order_relaxed);

		std::vector<std::thread> threads;
		threads.reserve(thread_count);
		for (int t = 0; t < thread_count; ++t)
		{
			threads.emplace_back([&sizes, t]()
			{
				std::vector<char*> held;
				held.reserve(32);
				for (int i = 0; i < ops_per_thread; ++i)
				{
					int size = sizes[(t * 7 + i) % std::size(sizes)];
					char* p = socket_pool::malloc(size);
					ASSERT_NE(p, nullptr);
					memset(p, 0xBB, size);
					held.push_back(p);

					if (held.size() >= 16)
					{
						for (char* buf : held)
							socket_pool::free(buf);
						held.clear();
					}
				}
				for (char* buf : held)
					socket_pool::free(buf);
			});
		}
		for (auto& th : threads)
			th.join();

		auto& stats_after = socket_pool::get_stats();
		uint64_t total_allocs = stats_after.m_alloc_count.load(std::memory_order_relaxed) - alloc_before;
		uint64_t total_frees = stats_after.m_free_count.load(std::memory_order_relaxed) - free_before;
		EXPECT_EQ(total_allocs, static_cast<uint64_t>(thread_count) * ops_per_thread);
		EXPECT_EQ(total_allocs, total_frees);
	}
}
