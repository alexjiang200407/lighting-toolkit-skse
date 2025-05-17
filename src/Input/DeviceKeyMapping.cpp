#include "Input/DeviceKeyMapping.h"

bool Input::DeviceKeyMapping::operator<(const DeviceKeyMapping& rhs) const
{
	return device < rhs.device || (rhs.device == device && idCode < rhs.idCode);
}

Input::DeviceKeyMapping::DeviceKeyMapping(RE::IDEvent* idEvt) :
	DeviceKeyMapping(idEvt->userEvent.c_str(), idEvt->GetDevice(), idEvt->idCode)
{}

Input::DeviceKeyMapping::DeviceKeyMapping(const char* userEvt, RE::INPUT_DEVICE device) :
	userEvt(userEvt), device(device)
{
	idCode = RE::ControlMap::GetSingleton()->GetMappedKey(userEvt, device);
}

Input::DeviceKeyMapping::DeviceKeyMapping(
	const char*      userEvt,
	RE::INPUT_DEVICE device,
	uint32_t         idCode) :
	userEvt(userEvt),
	device(device), idCode(idCode)
{}
