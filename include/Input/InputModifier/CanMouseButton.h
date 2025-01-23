#pragma once
#include "BSControlModifier.h"

namespace Input
{
	class CanMouseButton :
		public BSControlModifier<RE::INPUT_DEVICE::kMouse>
	{
	public:
		CanMouseButton(bool blockMouseButtons);

	public:
		static inline constexpr const char* inputIDs[] = { "WorldZUp", "WorldZDown" };
	};
}
