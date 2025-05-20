#pragma once
#include "BSControlModifier.h"

namespace Input
{
	class BSKeyboardGamepadModifier :
		public BSControlModifier<RE::INPUT_DEVICE::kKeyboard>,
		public BSControlModifier<RE::INPUT_DEVICE::kGamepad>
	{
	public:
		template <size_t SZ>
		BSKeyboardGamepadModifier(const char* const (&inputIDs)[SZ], bool blockControls) :
			BSControlModifier<RE::INPUT_DEVICE::kKeyboard>(inputIDs, blockControls),
			BSControlModifier<RE::INPUT_DEVICE::kGamepad>(inputIDs, blockControls)
		{}

		BSKeyboardGamepadModifier(
			std::initializer_list<DeviceKeyMapping> inputIDs,
			bool                                    blockControls) :
			BSControlModifier<RE::INPUT_DEVICE::kKeyboard>(inputIDs, blockControls),
			BSControlModifier<RE::INPUT_DEVICE::kGamepad>(inputIDs, blockControls)
		{}

	public:
		void ApplyKeySuppressionModifier(InputFilter& filter) const override;
	};
}
