#pragma once
#include "DeviceKeyMapping.h"

namespace Input
{
	class InputFilter
	{
	public:
		bool IsSuppressing(RE::IDEvent* evt) const;
		bool IsSuppressing(const DeviceKeyMapping& id) const;
		void SuppressInput(const std::vector<DeviceKeyMapping>& list);
		void AllowInput(const std::vector<DeviceKeyMapping>& list);
		bool IsBlockingChar() const;
		void SetBlockingChar(bool blockingChar);
		void SetBlockingMouseMove(bool blockingMouseMove);
		bool IsBlockingMouseMove() const;
		void Clear();

	private:
		std::set<DeviceKeyMapping> allowedInputIDs;
		bool                       blockCharEvents      = true;
		bool                       blockMouseMoveEvents = true;
	};
}
