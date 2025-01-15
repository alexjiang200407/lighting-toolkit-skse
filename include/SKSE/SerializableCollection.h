#pragma once

namespace SKSE
{
	class SerializableCollection
	{
	public:
		virtual void               SerializeItems(SerializationInterface* a_intfc) const = 0;
		virtual void               DeserializeItems(SerializationInterface* a_intfc)     = 0;
		virtual void               Revert(SerializationInterface* a_intfc)               = 0;
		virtual constexpr uint32_t GetKey()                                              = 0;
	};
}
