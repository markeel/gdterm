
#ifndef PTY_PROXY_WIN_H
#define PTY_PROXY_WIN_H

#define NTDDI_VERSION 0x0A000006

#include "pty_proxy.h"
#include <windows.h>
#include <wincon.h>
#include <mutex>
#include <condition_variable>
#ifdef __MINGW64__
#include <cstring>
#endif

static const int TO_BUFFER_MAX_SIZE = 1024;

class PtyProxyWin : public PtyProxy {
public:
	PtyProxyWin();
	PtyProxyWin(const std::wstring &command);
	virtual ~PtyProxyWin();

	virtual int send_string(const char * data);
	virtual int available_to_send();
	virtual void resize_screen(int nrows, int ncols);

	// Called from internal threads
	void _run_to_thread();
	void _run_from_thread();

	static void _print_last_error(const char * prefix);

private:
	HPCON _pty_console;
	HANDLE _to_pty;
	HANDLE _from_pty;
	STARTUPINFOEX _startup_info;
	PROCESS_INFORMATION _client;

	std::mutex    _state_mutex;
	int           _state;

	std::mutex              _to_mutex;
	std::condition_variable _to_ready;
	HANDLE                  _to_thread;
	unsigned char           _to_buffer[TO_BUFFER_MAX_SIZE];
	int                     _to_buffer_pos;
	std::wstring            _command;

	HANDLE                  _from_thread;

	void _create_pipes_and_pty();
	void _create_process();
	void _init_startup_info();
	void _create_to_pty_thread();
	void _create_from_pty_thread();
	bool _can_write();
	bool _can_read();
	void _exit_pty();
	void _set_state(int p_state);
	void _wake_to_thread();
	void _wake_from_thread();
	void _join_threads();
	void _terminate_process();
	void _close_pty();
};

#endif
