#include "SKSE/SerializationControl.h"

SKSE::SerializationControl SKSE::SerializationControl::singleton;

void SKSE::SerializationControl::Init()
{
	auto* serial = SKSE::GetSerializationInterface();
	serial->SetUniqueID('Asz0');
	serial->SetLoadCallback(singleton.Load);
	serial->SetSaveCallback(singleton.Save);
	serial->SetRevertCallback(singleton.Revert);
}

void SKSE::SerializationControl::RegisterSerializer(SerializableCollection* serializer)
{
	serializables.insert({ serializer->GetKey(), serializer });
}

void SKSE::SerializationControl::UnregisterSerializer(SerializableCollection* serializer)
{
	serializables.erase(serializer->GetKey());
}

SKSE::SerializationControl* SKSE::SerializationControl::GetSingleton()
{
	return &singleton;
}

void SKSE::SerializationControl::Save(SerializationInterface* a_intfc)
{
	for (const auto& [_, serializable] : singleton.serializables)
	{
		if (!a_intfc->OpenRecord(serializable->GetKey(), 0))
			continue;
		serializable->SerializeItems(a_intfc);
	}
}

void SKSE::SerializationControl::Load(SerializationInterface* a_intfc)
{
	std::uint32_t type;
	std::uint32_t size;
	std::uint32_t version;

	while (a_intfc->GetNextRecordInfo(type, version, size))
	{
		if (!size)
			continue;
		if (const auto& it = singleton.serializables.find(type); it != singleton.serializables.end())
		{
			try
			{
				it->second->DeserializeItems(a_intfc);
			}
			catch (const std::exception& e)
			{
				logger::error("Failed to deserialize, reason {}", e.what());
			}
		}
	}
}

void SKSE::SerializationControl::Revert(SerializationInterface* a_intfc)
{
	for (const auto& [_, serializable] : singleton.serializables)
	{
		serializable->Revert(a_intfc);
	}
}
