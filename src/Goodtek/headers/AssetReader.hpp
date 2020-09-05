#pragma once

using namespace std;

struct dumpListEntry
{
	void* start;
	int entryCount;
	int entrySize;
};

enum DUMP_TYPE
{
	DUMP_TYPE_INT = 0,
	DUMP_TYPE_STRING = 1,
	DUMP_TYPE_ASSET = 2,
	DUMP_TYPE_ARRAY = 3,
	DUMP_TYPE_OFFSET = 4,
	DUMP_TYPE_FLOAT = 5,
};

const char DUMP_EXISTING = 1;
const char DUMP_NONEXISTING = 0;

class AssetReader
{
protected:
	std::vector<dumpListEntry> list;
	FILE* fp_ = nullptr;
	ZoneMemory* memory_;

	template <typename T>
	T read_internal()
	{
		T value;

		if (fp_)
		{
			fread(&value, sizeof T, 1, fp_);
		}

		return value;
	}

	char* read_string_internal()
	{
		char tempBuf[1024];
		char ch = 0;
		int i = 0;

		if (fp_)
		{
			do
			{
				fread(&ch, 1, 1, fp_);

				tempBuf[i++] = ch;

				if (i >= sizeof(tempBuf))
				{
					throw std::exception("this is wrong");
				}
			} while (ch);
		}

		char* retval = memory_->Alloc<char>(i); // new char[i];
		strcpy(retval, tempBuf);

		return retval;
	}

public:
	AssetReader(ZoneMemory* mem)
	{
		memory_ = mem;

		list.clear();
	}

	~AssetReader()
	{
		// close file
		close();
	}

	bool open(const std::string& filename, bool preferLocal = false)
	{
		close();

		fp_ = fopen(filename.c_str(), "rb");

		return fp_ != nullptr;
	}

	void close()
	{
		if (fp_)
		{
			list.clear();
			fclose(fp_);
		}

		fp_ = nullptr;
	}

	float read_float()
	{
		if (fp_)
		{
			char type = this->read_internal<char>();
			if (type != DUMP_TYPE_FLOAT)
			{
				printf("Reader error: Type not DUMP_TYPE_FLOAT but %i", type);
				throw;
				return 0;
			}
			return this->read_internal<float>();
		}
		return 0;
	}

	int read_int()
	{
		if (fp_)
		{
			const auto type = this->read_internal<char>();
			if (type != DUMP_TYPE_INT)
			{
				printf("Reader error: Type not DUMP_TYPE_INT but %i", type);
				throw;
				return 0;
			}
			return this->read_internal<std::int32_t>();
		}
		return 0;
	}

	unsigned int read_uint()
	{
		if (fp_)
		{
			const auto type = this->read_internal<char>();
			if (type != DUMP_TYPE_INT)
			{
				printf("Reader error: Type not DUMP_TYPE_INT but %i", type);
				throw;
				return 0;
			}
			return this->read_internal<std::uint32_t>();
		}
		return 0;
	}

	char* read_string()
	{
		if (fp_)
		{
			const auto type = this->read_internal<char>();

			if (type == DUMP_TYPE_STRING)
			{
				const auto existing = this->read_internal<char>();
				if (existing == DUMP_NONEXISTING)
				{
					return nullptr;
				}
				const auto output = this->read_string_internal(); // freadstr(fp_);

				dumpListEntry entry;
				entry.entryCount = 1;
				entry.entrySize = 1;
				entry.start = static_cast<void*>(output);
				list.push_back(entry);

				return output;
			}
			if (type == DUMP_TYPE_OFFSET)
			{
				const auto listIndex = this->read_internal<int>(); // freadint(fp_);
				const auto arrayIndex = this->read_internal<int>();

				return reinterpret_cast<char*>(reinterpret_cast<int>(list[listIndex].start) + list[listIndex].entrySize * arrayIndex);
			}
			printf("Reader error: Type not DUMP_TYPE_STRING or DUMP_TYPE_OFFSET but %i", type);
			throw;
			return nullptr;
		}
		return nullptr;
	}

	template <typename T>
	T* read_asset()
	{
		if (fp_)
		{
			const auto type = this->read_internal<char>();

			if (type == DUMP_TYPE_ASSET)
			{
				const auto existing = this->read_internal<char>();
				if (existing == DUMP_NONEXISTING)
				{
					return nullptr;
				}
				const char* name = this->read_string_internal();
				// T* asset = new T;

				auto asset = memory_->Alloc<T>();

				memset(asset, 0, sizeof(T));
				asset->name = const_cast<char*>(name);

				dumpListEntry entry;
				entry.entryCount = 1;
				entry.entrySize = sizeof(T);
				entry.start = static_cast<void*>(asset);
				list.push_back(entry);

				return asset;
			}
			if (type == DUMP_TYPE_OFFSET)
			{
				const auto listIndex = this->read_internal<int>();
				const auto arrayIndex = this->read_internal<int>();

				return reinterpret_cast<T*>(reinterpret_cast<int>(list[listIndex].start) + list[listIndex].entrySize * arrayIndex);
			}

			printf("Reader error: Type not DUMP_TYPE_ASSET or DUMP_TYPE_OFFSET but %i", type);
			throw;
		}

		return nullptr;
	}

	template <typename T>
	T* read_array()
	{
		if (fp_)
		{
			const auto type = this->read_internal<char>();

			if (type == DUMP_TYPE_ARRAY)
			{
				const auto arraySize = this->read_internal<std::uint32_t>();

				if (arraySize <= 0)
				{
					return nullptr;
				}

				auto nArray = memory_->Alloc<T>(arraySize);
				fread(static_cast<void*>(nArray), sizeof(T), arraySize, fp_);

				dumpListEntry entry;
				entry.entryCount = arraySize;
				entry.entrySize = sizeof(T);
				entry.start = static_cast<void*>(nArray);
				list.push_back(entry);

				return nArray;
			}
			if (type == DUMP_TYPE_OFFSET)
			{
				const auto listIndex = this->read_internal<int>();
				const auto arrayIndex = this->read_internal<int>();

				return reinterpret_cast<T*>(reinterpret_cast<int>(list[listIndex].start) + list[listIndex].entrySize * arrayIndex);
			}

			printf("Reader error: Type not DUMP_TYPE_ARRAY or DUMP_TYPE_OFFSET but %i\n", type);
			throw;
		}

		return nullptr;
	}

	template <typename T> T* read_single()
	{
		return this->read_array<T>();
	}
};