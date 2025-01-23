#pragma once
#include "BSControlModifier.h"

namespace Input
{
	class FreeCameraZTranslate :
		public BSControlModifier<RE::INPUT_DEVICE::kMouse>
	{
	public:
		FreeCameraZTranslate(bool blockMouseButtons);

	public:
		static inline constexpr const char* inputIDs[] = { "WorldZUp", "WorldZDown" };
	};
}
