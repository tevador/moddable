#include <string.h>

#ifdef _MSC_VER

#include <intrin.h>
#include <Windows.h>

#else // _MSC_VER
#include <sys/mman.h>

__attribute__((always_inline)) inline void _BitScanReverse64(unsigned long* index, unsigned long long mask)
{
	*index = (unsigned long)(63 - __builtin_clzll(mask));
}

#endif // _MSC_VER

#define INDEX_COUNT 29ULL
#define INDEX_SHIFT 29

struct SChunk
{
	struct SChunk* next;
};

static struct SChunk* chunk_list_head[INDEX_COUNT];
static char* next_global_mem_chunk[INDEX_COUNT];
static char* global_mem = NULL;

#define GET_INDEX(ptr) (((char*)(ptr) - global_mem) >> INDEX_SHIFT)

#ifdef _MSC_VER
__declspec(noinline)
#else // _MSC_VER
__attribute__ ((noinline))
#endif // _MSC_VER
struct SChunk* AllocateNewChunks(unsigned long index)
{
	if (!global_mem)
	{
		const size_t global_mem_size = (INDEX_COUNT - 3) << INDEX_SHIFT;

#ifdef _MSC_VER
		global_mem = (char*) VirtualAlloc(0, global_mem_size, MEM_RESERVE, PAGE_READWRITE);
#else // _MSC_VER
		global_mem = (char*) mmap(0, global_mem_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif // _MSC_VER

		global_mem -= 3ULL << INDEX_SHIFT;
		for (size_t i = 3; i < INDEX_COUNT; ++i)
		{
			next_global_mem_chunk[i] = global_mem + (i << INDEX_SHIFT);
		}
	}

	const size_t alloc_size = 1 << ((index < 18) ? 18 : index);
	const size_t chunk_size = 1 << index;

#ifdef _MSC_VER
	// VirtualAlloc returns zero-filled memory
	char* ptr = (char*) VirtualAlloc(next_global_mem_chunk[index], alloc_size, MEM_COMMIT, PAGE_READWRITE);
#else // _MSC_VER
	char* ptr = next_global_mem_chunk[index];
#endif // _MSC_VER

	next_global_mem_chunk[index] += alloc_size;

	// Fill in non-zero "next" pointers
	for (size_t i = 0, n = alloc_size - chunk_size; i < n; i += chunk_size)
	{
		((struct SChunk*)(ptr + i))->next = (struct SChunk*)(ptr + i + chunk_size);
	}

	return (struct SChunk*)(ptr);
}

void* my_malloc(size_t size)
{
	if (size < sizeof(struct SChunk))
	{
		size = sizeof(struct SChunk);
	}

	unsigned long index;
	_BitScanReverse64(&index, size - 1);
	++index;

	struct SChunk* chunk = chunk_list_head[index];
	if (!chunk)
	{
		chunk = AllocateNewChunks(index);
	}
	chunk_list_head[index] = chunk->next;

	return chunk;
}

void my_free(void* ptr)
{
	if (ptr)
	{
		struct SChunk* chunk = (struct SChunk*)(ptr);
		const int index = GET_INDEX(ptr);

		chunk->next = chunk_list_head[index];
		chunk_list_head[index] = chunk;
	}
}

void *my_realloc(void *ptr, size_t new_size)
{
	struct SChunk* chunk;
	size_t index, max_old_size;
	if (ptr)
	{
		chunk = (struct SChunk*)(ptr);
		index = GET_INDEX(ptr);
		max_old_size = 1 << index;
		if (new_size <= max_old_size)
		{
			return ptr;
		}
	}

	void* new_ptr = my_malloc(new_size);

	if (ptr)
	{
		memcpy(new_ptr, ptr, max_old_size);
		chunk->next = chunk_list_head[index];
		chunk_list_head[index] = chunk;
	}

	return new_ptr;
}

void* my_calloc(size_t num, size_t size)
{
	size *= num;
	void* ptr = my_malloc(size);
	memset(ptr, 0, size);

	return ptr;
}
