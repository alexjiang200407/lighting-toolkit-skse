#pragma once
#include "Input/InputFilter.h"

namespace ImGui
{
	class ImGuiInputAdapter
	{
	public:
		typedef std::bitset<256> KeyboardSupressionMask;
		typedef std::bitset<256> MouseSupressionMask;
		typedef std::bitset<256> GamePadSupressionMask;

	private:
		struct SendInputEvent
		{
			static void                                    thunk(RE::BSTEventSource<RE::InputEvent*>* dispatcher, RE::InputEvent** ppEvents);
			static inline REL::Relocation<decltype(thunk)> func;
		};

	public:
		void                      Init();
		void                      EnableSupression(Input::InputFilter filter);
		void                      DisableSupression();
		static ImGuiInputAdapter* GetSingleton();
		bool                      IsSuppressingButtons();
		void                      DispatchImGuiInputEvents();
		void                      Adapt(RE::BSTEventSource<RE::InputEvent*>* dispatcher, RE::InputEvent** ppEvents);
		static bool               IsKeyPressed(const char* keyID, bool repeat);
		static bool               IsKeyDown(const char* keyID);

	private:
		ImGuiInputAdapter() = default;

		static ImGuiKey ToImGuiKey(RE::BSWin32KeyboardDevice::Key key);
		static ImGuiKey ToImGuiKey(RE::BSWin32GamepadDevice::Key key);
		static int      ToImGuiKey(RE::BSWin32MouseDevice::Key key);

		typedef std::pair<RE::InputEvent*, RE::InputEvent*> InputList;
		template <size_t S>
		static void UpdateInputList(std::bitset<S> keySet, InputList& list, InputList& removed, RE::ButtonEvent* const event)
		{
			if (!singleton.IsSuppressingButtons() || !keySet.test(event->GetIDCode()) || event->IsUp())
			{
				AddToInputList(list, event);
			}
			else
			{
				AddToInputList(removed, event);
			}
		}

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
		Input::InputFilter                       filter;
		static ImGuiInputAdapter                 singleton;
		std::list<std::function<void(ImGuiIO&)>> inputEvtCallbacks;
		std::mutex                               inputEvtCallbacksLock;
	};

}
