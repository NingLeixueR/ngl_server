/*
* Copyright (c) [2020-2025] NingLeixueR
*
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
*/

#include "tools/serialize/socket_buff_pool.h"
#include <gtest/gtest.h>
#include <thread>
#include <vector>

namespace ngl
{
	TEST(SocketPoolTest, BasicAllocation)
	{
		char* buff = optimized_socket_pool::malloc(256);
		ASSERT_NE(buff, nullptr);
		memset(buff, 0xAB, 256);
		optimized_socket_pool::free(buff);
	}

	TEST(SocketPoolTest, MultipleAllocations)
	{
		std::vector<char*> buffers;
		int sizes[] = {64, 128, 512, 1024, 4096};

		for (int size : sizes)
		{
			char* buff = optimized_socket_pool::malloc(size);
			ASSERT_NE(buff, nullptr);
			buffers.push_back(buff);
		}

		for (char* buff : buffers)
		{
			optimized_socket_pool::free(buff);
		}
	}

	TEST(SocketPoolTest, LargeAllocation)
	{
		char* buff = optimized_socket_pool::malloc(200000);
		ASSERT_NE(buff, nullptr);
		optimized_socket_pool::free(buff);
	}
