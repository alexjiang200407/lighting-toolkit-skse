#pragma once
#include "SerializableCollection.h"

namespace SKSE
{
	class SerializationControl
	{
	public:
		void                         Init();
		void                         RegisterSerializer(SerializableCollection* serializer);
		void                         UnregisterSerializer(SerializableCollection* serializer);
		static SerializationControl* GetSingleton();

	private:
		SerializationControl() = default;

		static void Save(SerializationInterface* a_intfc);
		static void Load(SerializationInterface* a_intfc);
		static void Revert(SerializationInterface* a_intfc);

	private:
		std::map<uint32_t, SerializableCollection*> serializables;
		static SerializationControl                 singleton;
	};
}
