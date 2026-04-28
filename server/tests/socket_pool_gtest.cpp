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
}
