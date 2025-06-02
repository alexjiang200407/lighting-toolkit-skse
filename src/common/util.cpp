#include "common/util.h"

std::string GetEditorID(const RE::TESForm* form)
{
	assert(form);

	using _GetFormEditorID = const char* (*)(std::uint32_t);

	static auto tweaks = GetModuleHandleW(L"po3_Tweaks");
	static auto function =
		reinterpret_cast<_GetFormEditorID>(GetProcAddress(tweaks, "GetFormEditorID"));
	if (function)
	{
		std::string str = function(form->formID);
		if (!str.empty())
			return str;
	}
	return fmt::format("0x{:X}", form->formID);
}
