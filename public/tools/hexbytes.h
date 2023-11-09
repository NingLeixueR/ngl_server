#pragma once

namespace ngl
{
	class hexbytes
	{
	public:
		static int to_hex(void* apso, int alen, void* apto);
		static int to_bytes(void* apso, int alen, void* apto);
	};
}