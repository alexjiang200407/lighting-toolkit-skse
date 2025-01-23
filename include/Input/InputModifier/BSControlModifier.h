#pragma once
#include "InputModifier.h"
namespace Input
{
	template <RE::INPUT_DEVICE DEVICE>
	class BSControlModifier :
		virtual public InputModifier
	{
	public:
		template <size_t SZ>
		BSControlModifier(const char* const (&inputIDs)[SZ], bool blockControls) :
			blockControls(blockControls)
		{
			for (size_t i = 0; i < SZ; i++)
			{
				this->inputIDs.push_back(DeviceKeyMapping(inputIDs[i], DEVICE));
			}
		}

		BSControlModifier(std::initializer_list<DeviceKeyMapping> inputIDs, bool blockControls) :
			blockControls(blockControls), inputIDs(inputIDs)
		{
		}

	public:
		void ApplyKeySuppressionModifier(InputFilter& filter) const override
		{
			if (blockControls)
				filter.SuppressInput(inputIDs);
			else
				filter.AllowInput(inputIDs);
		}

	private:
		bool                          blockControls;
		std::vector<DeviceKeyMapping> inputIDs;
	};
}
