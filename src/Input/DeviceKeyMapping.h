#pragma once

namespace Input
{
	struct DeviceKeyMapping
	{
		RE::INPUT_DEVICE device;
		std::string      userEvt;
		uint32_t         idCode;

		bool operator<(const DeviceKeyMapping& rhs) const;

		DeviceKeyMapping(RE::IDEvent* idEvt);
		DeviceKeyMapping(const char* userEvt, RE::INPUT_DEVICE device);
		DeviceKeyMapping(const char* userEvt, RE::INPUT_DEVICE device, uint32_t idCode);
	};
}
