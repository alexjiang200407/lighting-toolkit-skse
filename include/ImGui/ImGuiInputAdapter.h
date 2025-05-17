#pragma once
#include "Input/DeviceKeyMapping.h"
#include "Input/InputContext.h"
#include "Input/InputFilter.h"

namespace ImGui
{
	class ImGuiInputAdapter
	{
	private:
		struct SendInputEvent
		{
			static void
				thunk(RE::BSTEventSource<RE::InputEvent*>* dispatcher, RE::InputEvent** ppEvents);
			static inline REL::Relocation<decltype(thunk)> func;
		};

	public:
		void                      Init();
		void                      EnableSupression(const Input::InputContext& inputCtx);
		void                      DisableSupression();
		static ImGuiInputAdapter* GetSingleton();
		void                      DispatchImGuiInputEvents();
		void Adapt(RE::BSTEventSource<RE::InputEvent*>* dispatcher, RE::InputEvent** ppEvents);
		static bool IsKeyPressed(const char* keyID, bool repeat);
		static bool IsKeyDown(const char* keyID);

	private:
		ImGuiInputAdapter() = default;

		static ImGuiKey ToImGuiKey(RE::BSWin32KeyboardDevice::Key key);
		static ImGuiKey ToImGuiKey(RE::BSWin32GamepadDevice::Key key);
		static int      ToImGuiKey(RE::BSWin32MouseDevice::Key key);

		typedef std::pair<RE::InputEvent*, RE::InputEvent*> InputList;
		bool AddButtonEvent(InputList& list, InputList& removed, RE::ButtonEvent* const event);
		bool AddIDEvent(InputList& list, InputList& removed, RE::IDEvent* const event);
		bool AddInputEvent(
			bool                  cond,
			InputList&            list,
			InputList&            removed,
			RE::InputEvent* const event);
		static void AddToInputList(InputList& list, RE::InputEvent* const event)
		{
			list.second       = list.first ? list.second->next = event : list.first = event;
			list.second->next = nullptr;
		}

		void HandleButtonEvent(RE::ButtonEvent* const event, InputList& list, InputList& removed);
		void HandleKeyboardButtonEvent(uint32_t key, bool isPressed);
		void HandleMouseButtonEvent(uint32_t key, float value, bool isPressed);
		void HandleGamepadButtonEvent(uint32_t key, bool isPressed);
		void HandleCharEvent(RE::CharEvent* const charEvt);

	private:
		bool                                     isSuppressing = false;
		std::vector<Input::DeviceKeyMapping>     injectedInputEvts;
		Input::InputFilter                       filter;
		static ImGuiInputAdapter                 singleton;
		std::list<std::function<void(ImGuiIO&)>> inputEvtCallbacks;
		std::mutex                               inputEvtCallbacksLock;
	};

}
