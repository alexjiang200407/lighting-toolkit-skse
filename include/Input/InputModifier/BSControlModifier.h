#pragma once
#include "InputModifier.h"
namespace Input
{
	class BSControlModifier :
		virtual public InputModifier
	{
	public:
		BSControlModifier(std::initializer_list<std::string> controlIDs, bool blockControls);

	public:
		KeyboardSupressionMask ApplyKeyModifier(KeyboardSupressionMask kbd) const override;
		GamePadSupressionMask  ApplyGamepadKeyModifier(GamePadSupressionMask gamepad) const override;

		template <typename T>
		T ApplyBSControlModifier(T mask, RE::INPUT_DEVICE deviceType) const
		{
			using INPUT_CONTEXT = RE::UserEvents::INPUT_CONTEXT_IDS;
			auto* controlMap    = RE::ControlMap::GetSingleton();

			for (const auto& controlID : controls)
			{
				if (blockControls)
				{
					mask.set(controlMap->GetMappedKey(controlID.c_str(), deviceType, INPUT_CONTEXT::kGameplay));
				}
				else
				{
					mask.reset(controlMap->GetMappedKey(controlID.c_str(), deviceType, INPUT_CONTEXT::kGameplay));
				}
			}
			return mask;
		}

	private:
		bool                           blockControls;
		const std::vector<std::string> controls;
	};
}
