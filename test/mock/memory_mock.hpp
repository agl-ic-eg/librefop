#include <gmock/gmock.h>
#include <functional>

#include <stdlib.h>

/*
void *malloc(size_t size);
void free(void *ptr);
*/
static std::function<void*(size_t size)> _malloc;
static std::function<void(void *ptr)> _free;

class MemoryMocker {
public:
	MemoryMocker() {
		_malloc = [this](size_t size) {
			return malloc(size);
		};
		_free = [this](void *ptr) {
			return free(ptr);
		};
	}

	~MemoryMocker() {
		_malloc = {};
		_free = {};
	}

	MOCK_CONST_METHOD1(malloc, void*(size_t));
	MOCK_CONST_METHOD1(free, void(void *));
};

class MemoryMockBase {
protected:
	MemoryMocker memorym;
};

#ifdef __cplusplus
extern "C" {
#endif
static void* unittest_malloc(size_t size)
{
	return _malloc(size);
}
/*
static void free(void *ptr)
{
	_free(ptr);
}
*/
#ifdef __cplusplus
}
#endif
