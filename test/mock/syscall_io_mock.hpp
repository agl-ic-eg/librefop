#include <gmock/gmock.h>
#include <functional>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>

/*
int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
*/
static std::function<int(const char *pathname, int flags)> _open;
/*
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
int close(int fd);
*/
static std::function<ssize_t(int fd, void *buf, size_t count)> _read;
static std::function<ssize_t(int fd, const void *buf, size_t count)> _write;
static std::function<int(int)> _close;

/*
int fsync(int fd);
*/
static std::function<int(int)> _fsync;

/*
int unlink(const char *pathname);
int stat(const char *pathname, struct stat *buf);
*/
static std::function<int(const char *)> _unlink;
static std::function<int(const char *pathname, struct stat *buf)> _stat;


/*
int socket(int socket_family, int socket_type, int protocol);
int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
int listen(int sockfd, int backlog);
int accept4(int sockfd, struct sockaddr *addr,
			socklen_t *addrlen, int flags);
*/
static std::function<int(int socket_family, int socket_type, int protocol)> _socket;
static std::function<int(int sockfd, const struct sockaddr *addr,socklen_t addrlen)> _bind;
static std::function<int(int sockfd, int backlog)> _listen;
static std::function<int(int sockfd, struct sockaddr *addr,
			socklen_t *addrlen, int flags)> _accept4;

class SyscallIOMocker {
public:
	SyscallIOMocker() {
		_open = [this](const char *pathname, int flags) {
			return open(pathname, flags);
		};

		_read = [this](int fd, void *buf, size_t count) {
			return read(fd, buf, count);
		};
		_write = [this](int fd, const void *buf, size_t count) {
			return write(fd, buf, count);
		};
		_close = [this](int fd){
			return close(fd);
		};

		_fsync = [this](int fd){
			return fsync(fd);
		};

		_unlink = [this](const char *pathname){
			return unlink(pathname);
		};
		_stat = [this](const char *pathname, struct stat *buf) {
			return stat(pathname, buf);
		};

		_socket = [this](int socket_family, int socket_type, int protocol){
			return socket(socket_family, socket_type, protocol);
		};
		_bind = [this](int sockfd, const struct sockaddr *addr,socklen_t addrlen){
			return bind(sockfd, addr,addrlen);
		};
		_listen = [this](int sockfd, int backlog){
			return listen(sockfd, backlog);
		};
		_accept4
			= [this](int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags) {
			return accept4(sockfd, addr, addrlen, flags);
		};
	}

	~SyscallIOMocker() {
		_open = {};

		_read = {};
		_write = {};
		_close = {};

		_fsync = {};

		_unlink = {};
		_stat = {};

		_socket = {};
		_bind = {};
		_listen = {};
		_accept4 = {};
	}

	MOCK_CONST_METHOD2(open, int(const char *pathname, int flags));

	MOCK_CONST_METHOD3(read, ssize_t(int fd, void *buf, size_t count));
	MOCK_CONST_METHOD3(write, ssize_t(int fd, const void *buf, size_t count));
	MOCK_CONST_METHOD1(close, int(int));

	MOCK_CONST_METHOD1(fsync, int(int));

	MOCK_CONST_METHOD1(unlink, int(const char *));
	MOCK_CONST_METHOD2(stat, int(const char *pathname, struct stat *buf));

	MOCK_CONST_METHOD3(socket, int(int socket_family, int socket_type, int protocol));
	MOCK_CONST_METHOD3(bind, int(int sockfd, const struct sockaddr *addr,socklen_t addrlen));
	MOCK_CONST_METHOD2(listen, int(int sockfd, int backlog));
	MOCK_CONST_METHOD4(accept4, int(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags));
};

class SyscallIOMockBase {
protected:
	SyscallIOMocker sysiom;
};

#ifdef __cplusplus
extern "C" {
#endif
static int open(const char *pathname, int flags, ...)
{
	return _open(pathname, flags);
}

static ssize_t read(int fd, void *buf, size_t count)
{
    return _read(fd, buf, count);
}

static ssize_t write(int fd, const void *buf, size_t count)
{
    return _write(fd, buf, count);
}

static int close(int fd)
{
    return _close(fd);
}

static  int fsync(int fd)
{
	return _fsync(fd);
}

static int unlink(const char *pathname)
{
	return _unlink(pathname);
}

static int stat(const char *pathname, struct stat *buf)
{
	return _stat(pathname, buf);
}

static int socket(int socket_family, int socket_type, int protocol)
{
	return _socket(socket_family, socket_type, protocol);
}

static int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen)
{
	return _bind(sockfd, addr, addrlen);
}

static int listen(int sockfd, int backlog)
{
	return _listen(sockfd, backlog);
}

static int accept4(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags)
{
	return _accept4(sockfd, addr, addrlen, flags);
}

#ifdef __cplusplus
}
#endif
