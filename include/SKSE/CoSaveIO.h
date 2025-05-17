#pragma once

namespace SKSE
{
	class CoSaveIO
	{
	public:
		CoSaveIO(SerializationInterface* a_intfc);

		template <typename T>
		void Read(T& buf)
		{
			serial->ReadRecordData(buf);
		}

		template <typename T>
		void Read(T* buf, uint32_t sz)
		{
			uint32_t exp = sz * sizeof(T);
			if (exp != serial->ReadRecordData(buf, exp))
				throw std::runtime_error("Invalid SKSE Save file");
		}

		template <typename T>
		void Write(const T& buf)
		{
			serial->WriteRecordData(buf);
		}

		template <typename T>
		void Write(T* buf, uint32_t sz)
		{
			serial->WriteRecordData(buf, sz * sizeof(T));
		}

	private:
		SerializationInterface* serial;
	};
}
