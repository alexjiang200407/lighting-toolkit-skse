#include "Input/InputFilter.h"

bool Input::InputFilter::IsSuppressing(RE::IDEvent* evt) const
{
	return IsSuppressing(DeviceKeyMapping(evt->userEvent.data(), evt->GetDevice()));
}

bool Input::InputFilter::IsSuppressing(const DeviceKeyMapping& idName) const
{
	return allowedInputIDs.find(idName) == allowedInputIDs.end();
}

void Input::InputFilter::SuppressInput(const std::vector<DeviceKeyMapping>& list)
{
	for (const auto& str : list)
	{
		allowedInputIDs.erase(str);
	}
}

void Input::InputFilter::AllowInput(const std::vector<DeviceKeyMapping>& list)
{
	for (const auto& str : list)
	{
		allowedInputIDs.insert(str);
	}
}

bool Input::InputFilter::IsBlockingChar() const
{
	return blockCharEvents;
}

void Input::InputFilter::SetBlockingChar(bool blockingChar)
{
	blockCharEvents = blockingChar;
}

void Input::InputFilter::SetBlockingMouseMove(bool blockingMouseMove)
{
	blockMouseMoveEvents = blockingMouseMove;
}

bool Input::InputFilter::IsBlockingMouseMove() const
{
	return blockMouseMoveEvents;
}

void Input::InputFilter::Clear()
{
	blockCharEvents = false;
	blockMouseMoveEvents = false;
	allowedInputIDs.clear();
}
