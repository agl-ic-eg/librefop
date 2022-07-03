#include <gmock/gmock.h>
#include <functional>

#include <systemd/sd-event.h>


/*
int sd_event_add_signal(sd_event *event, sd_event_source **source,
			int signal, sd_event_signal_handler_t handler, void *userdata);
*/
static std::function<int(sd_event *event, sd_event_source **source,
			int signal, sd_event_signal_handler_t handler, void *userdata)> _sd_event_add_signal;

/*
int sd_event_add_io(sd_event *event, sd_event_source **source, int fd,
			uint32_t events, sd_event_io_handler_t handler, void *userdata);
int sd_event_source_get_io_fd(sd_event_source *source);
int sd_event_source_set_io_fd_own(sd_event_source *source, int b);
*/
static std::function<int(sd_event *event, sd_event_source **source, int fd,
			uint32_t events, sd_event_io_handler_t handler, void *userdata)>
			_sd_event_add_io;
static std::function<int(sd_event_source *source)> _sd_event_source_get_io_fd;
static std::function<int(sd_event_source *source, int b)> _sd_event_source_set_io_fd_own;


/*
int sd_event_source_set_time(sd_event_source *source, uint64_t usec);
int sd_event_now(sd_event *event, clockid_t clock, uint64_t *usec);
int sd_event_add_time(	sd_event *event,
 	sd_event_source **source,
 	clockid_t clock,
 	uint64_t usec,
 	uint64_t accuracy,
 	sd_event_time_handler_t handler,
 	void *userdata);
int sd_event_source_set_enabled(sd_event_source *source, int enabled);
*/
static std::function<int(sd_event_source *source, uint64_t usec)> _sd_event_source_set_time;
static std::function<int(sd_event *event, clockid_t clock, uint64_t *usec)> _sd_event_now;
static std::function<int(sd_event *event, sd_event_source **source, clockid_t clock,
 	uint64_t usec, uint64_t accuracy, sd_event_time_handler_t handler, void *userdata)> _sd_event_add_time;
static std::function<int(sd_event_source *source, int enabled)> _sd_event_source_set_enabled;


/*
sd_event_source* sd_event_source_unref(sd_event_source *source);
sd_event_source* sd_event_source_ref(sd_event_source *source);
sd_event_source* sd_event_source_disable_unref(sd_event_source *source);
*/
static std::function<sd_event_source*(sd_event_source *source)> _sd_event_source_unref;
static std::function<sd_event_source*(sd_event_source *source)> _sd_event_source_ref;
static std::function<sd_event_source*(sd_event_source *source)> _sd_event_source_disable_unref;


class LibsystemdMocker {
public:
	LibsystemdMocker() {
		_sd_event_add_signal
			= [this](sd_event *event, sd_event_source **source, int signal,
						sd_event_signal_handler_t handler, void *userdata) {
			return sd_event_add_signal(event, source, signal, handler, userdata);
		};

		_sd_event_add_io
			= [this](sd_event *event, sd_event_source **source, int fd,
						uint32_t events, sd_event_io_handler_t handler, void *userdata) {
			return sd_event_add_io(event, source, fd, events, handler, userdata);
		};
		_sd_event_source_get_io_fd = [this](sd_event_source *source) {
			return sd_event_source_get_io_fd(source);
		};
		_sd_event_source_set_io_fd_own = [this](sd_event_source *source, int b) {
			return sd_event_source_set_io_fd_own(source, b);
		};

		_sd_event_source_set_time = [this](sd_event_source *source, uint64_t usec) {
			return sd_event_source_set_time(source, usec);
		};
		_sd_event_now = [this](sd_event *event, clockid_t clock, uint64_t *usec) {
			return sd_event_now(event, clock, usec);
		};
		_sd_event_add_time
			= [this](sd_event *event, sd_event_source **source, clockid_t clock, uint64_t usec,
					uint64_t accuracy, sd_event_time_handler_t handler, void *userdata) {
			return sd_event_add_time(event, source, clock, usec, accuracy, handler, userdata);
		};
		_sd_event_source_set_enabled = [this](sd_event_source *source, int enabled) {
			return sd_event_source_set_enabled(source, enabled);
		};

		_sd_event_source_unref = [this](sd_event_source *source) {
			return sd_event_source_unref(source);
		};
		_sd_event_source_ref = [this](sd_event_source *source) {
			return sd_event_source_ref(source);
		};
		_sd_event_source_disable_unref = [this](sd_event_source *source) {
			return sd_event_source_disable_unref(source);
		};
	}

	~LibsystemdMocker() {
		_sd_event_add_signal = {};

		_sd_event_add_io = {};
		_sd_event_source_get_io_fd = {};
		_sd_event_source_set_io_fd_own = {};

		_sd_event_source_set_time = {};
		_sd_event_now = {};
		_sd_event_add_time = {};
		_sd_event_source_set_enabled = {};

		_sd_event_source_unref = {};
		_sd_event_source_ref = {};
		_sd_event_source_disable_unref = {};
	}

	MOCK_CONST_METHOD5(sd_event_add_signal, int(sd_event *event, sd_event_source **source, int signal, sd_event_signal_handler_t handler, void *userdata));

	MOCK_CONST_METHOD6(sd_event_add_io, int(sd_event *event, sd_event_source **source, int fd, uint32_t events, sd_event_io_handler_t handler, void *userdata));
	MOCK_CONST_METHOD1(sd_event_source_get_io_fd, int(sd_event_source *source));
	MOCK_CONST_METHOD2(sd_event_source_set_io_fd_own, int(sd_event_source *source, int b));

	MOCK_CONST_METHOD2(sd_event_source_set_time, int(sd_event_source *source, uint64_t usec));
	MOCK_CONST_METHOD3(sd_event_now, int(sd_event *event, clockid_t clock, uint64_t *usec));
	MOCK_CONST_METHOD7(sd_event_add_time, int(sd_event *event,sd_event_source **source, clockid_t clock, uint64_t usec, uint64_t accuracy, sd_event_time_handler_t handler, void *userdata));
	MOCK_CONST_METHOD2(sd_event_source_set_enabled, int(sd_event_source *source, int enabled));

	MOCK_CONST_METHOD1(sd_event_source_unref, sd_event_source*(sd_event_source *source));
	MOCK_CONST_METHOD1(sd_event_source_ref, sd_event_source*(sd_event_source *source));
	MOCK_CONST_METHOD1(sd_event_source_disable_unref, sd_event_source*(sd_event_source *source));
};

class LibsystemdMockBase {
protected:
   LibsystemdMocker lsm;
};

#ifdef __cplusplus
extern "C" {
#endif

int sd_event_add_signal(	sd_event *event,
 	sd_event_source **source,
 	int signal,
 	sd_event_signal_handler_t handler,
 	void *userdata)
{
    return _sd_event_add_signal(event, source, signal, handler, userdata);
}

int sd_event_add_io(sd_event *event, sd_event_source **source, int fd,
			uint32_t events, sd_event_io_handler_t handler, void *userdata)
{
	return _sd_event_add_io(event, source, fd, events, handler, userdata);
}

int sd_event_source_get_io_fd(	sd_event_source *source)
{
	return _sd_event_source_get_io_fd(source);
}

int sd_event_source_set_io_fd_own(sd_event_source *source, int b)
{
	return _sd_event_source_set_io_fd_own(source, b);
}

int sd_event_source_set_time(sd_event_source *source, uint64_t usec)
{
	return _sd_event_source_set_time(source, usec);
}

int sd_event_now(sd_event *event, clockid_t clock, uint64_t *usec)
{
	return _sd_event_now(event, clock, usec);
}

int sd_event_add_time(sd_event *event, sd_event_source **source, clockid_t clock, uint64_t usec,
			uint64_t accuracy, sd_event_time_handler_t handler, void *userdata)
{
	return _sd_event_add_time(event, source, clock, usec, accuracy, handler, userdata);
}

int sd_event_source_set_enabled(sd_event_source *source, int enabled)
{
	return _sd_event_source_set_enabled(source, enabled);
}

sd_event_source* sd_event_source_unref(sd_event_source *source)
{
	return _sd_event_source_unref(source);
}

sd_event_source* sd_event_source_ref(sd_event_source *source)
{
	return _sd_event_source_ref(source);
}

sd_event_source* sd_event_source_disable_unref(sd_event_source *source)
{
	return _sd_event_source_disable_unref(source);
}

#ifdef __cplusplus
}
#endif
