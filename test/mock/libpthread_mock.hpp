/**
 * SPDX-License-Identifier: Apache-2.0
 */
#include <gmock/gmock.h>
#include <functional>

#include <signal.h>

/*
int pthread_sigmask(int how, const sigset_t *set, sigset_t *oldset);
*/
static std::function<int(int how, const sigset_t *set, sigset_t *oldset)> _pthread_sigmask;

/*
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);

static std::function<int(pthread_mutex_t *mutex)> _pthread_mutex_lock;
static std::function<int(pthread_mutex_t *mutex)> _pthread_mutex_trylock;
static std::function<int(pthread_mutex_t *mutex)> _pthread_mutex_unlock;
*/

class LibpthreadMocker {
public:
	LibpthreadMocker() {
		_pthread_sigmask = [this](int how, const sigset_t *set, sigset_t *oldset)
		{
			return pthread_sigmask(how, set, oldset);
		};

/*
		_pthread_mutex_lock = [this](pthread_mutex_t *mutex)
		{
			return pthread_mutex_lock(mutex);
		};
		_pthread_mutex_trylock = [this](pthread_mutex_t *mutex)
		{
			return pthread_mutex_trylock(mutex);
		};
		_pthread_mutex_unlock = [this](pthread_mutex_t *mutex)
		{
			return pthread_mutex_unlock(mutex);
		};
*/
    }

	~LibpthreadMocker() {
		_pthread_sigmask = {};

/*
		_pthread_mutex_lock = {};
		_pthread_mutex_trylock = {};
		_pthread_mutex_unlock = {};
*/
    }

	MOCK_CONST_METHOD3(pthread_sigmask, int(int how, const sigset_t *set, sigset_t *oldset));
/*
	MOCK_CONST_METHOD1(pthread_mutex_lock, int(pthread_mutex_t *mutex));
	MOCK_CONST_METHOD1(pthread_mutex_trylock, int(pthread_mutex_t *mutex));
	MOCK_CONST_METHOD1(pthread_mutex_unlock, int(pthread_mutex_t *mutex));
*/
};

class LibpthreadMockBase {
protected:
   LibpthreadMocker lpm;
};

#ifdef __cplusplus
extern "C" {
#endif

int pthread_sigmask(int how, const sigset_t *set, sigset_t *oldset)
{
    return _pthread_sigmask(how, set, oldset);
}

/*
static int pthread_mutex_lock(pthread_mutex_t *mutex)
{
	return _pthread_mutex_lock(mutex);
}
static int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
	return _pthread_mutex_trylock(mutex);
}
static int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	return _pthread_mutex_unlock(mutex);
}
*/
#ifdef __cplusplus
}
#endif
