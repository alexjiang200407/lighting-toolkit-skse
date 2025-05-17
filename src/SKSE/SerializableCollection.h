#pragma once
#include "CoSaveIO.h"

namespace SKSE
{
	class SerializableCollection
	{
	public:
		virtual void               SerializeItems(CoSaveIO io) const = 0;
		virtual void               DeserializeItems(CoSaveIO io)     = 0;
		virtual void               Revert(CoSaveIO io)               = 0;
		virtual constexpr uint32_t GetKey()                          = 0;
	};
}
