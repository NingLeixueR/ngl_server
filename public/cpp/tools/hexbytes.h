#pragma once

namespace ngl
{
	class hexbytes
	{
	public:
		static int	to_hex(void* apso, int alen, void* apto);
		static bool to_bytes(void* apso, int alen, void* apto, int& apotlen);
	};
}// namespace ngl