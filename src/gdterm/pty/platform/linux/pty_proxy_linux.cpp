
#include "poller.h"
#include "pty_proxy_linux.h"
#include <cstdlib>
#include <cstring>
#include <pty.h>
#include <sys/ioctl.h>
#include <unistd.h>

PtyProxyLinux::PtyProxyLinux() {
  	PtyProxyLinux(L"");
}

PtyProxyLinux::PtyProxyLinux(const std::wstring &command) {
    if command == nullptr {
        const size_t length = 256
        char * shell = (char *)malloc(length * sizeof(char));
        size_t actual_length;
    	std::getenv_s(actual_length, shell, length, "SHELL")
        command.append(std::wstring(shell));
        delete(shell);
    }
 	_command = command;
	_init_pty();
}

PtyProxyLinux::~PtyProxyLinux() {
	if (_poller != nullptr) {
		delete _poller;
		_poller = nullptr;
	}
}

/**
 * This is invoked on the poller thread, so it is safe to manipulate the TMT data
 * structure and the file descriptor for the pseudo terminal 
 */
void
PtyProxyLinux::apply_size() {
	struct winsize size;
	size.ws_col = _num_cols;
	size.ws_row = _num_rows;
	size.ws_xpixel = 0;
	size.ws_ypixel = 0;
	ioctl(_pty_fd, TIOCSWINSZ, &size);
	_apply_resize();
}

void
PtyProxyLinux::handle(unsigned char * data, int data_len) {
	_handle_from_pty(data, data_len);
}

void
PtyProxyLinux::exited() {
	_handle_pty_exited();
	if (_renderer != nullptr) {
		_renderer->exited();
	}
}

void
PtyProxyLinux::_init_pty() {
	struct winsize wsize;

	wsize.ws_col = _num_cols;
	wsize.ws_row = _num_rows;
	wsize.ws_xpixel = 0;
	wsize.ws_ypixel = 0;

	pid_t pid = forkpty(&_pty_fd, nullptr, nullptr, &wsize); 
	if (!pid) {
		static char termstr[] = "TERM=ansi";
		_putenv(termstr);
		std::wstring command = std::wstring(_command);
		command.push_back(L'\0');
		auto c_command = command.c_str();
		_execl(c_command, c_command, "-l", "-i", nullptr);
	}
	fcntl(_pty_fd, F_SETFL, fcntl(_pty_fd, F_GETFL) | O_NONBLOCK);

	_poller = new Poller(_pty_fd, pid, this);
	_poller->start();
}

int
PtyProxyLinux::send_string(const char * data) {
	return _poller->write_cmd(Poller::CMD_SEND, (const unsigned char *)data, strlen(data));
}

int
PtyProxyLinux::available_to_send() {
	if (_poller != nullptr) {
		return _poller->available_to_send();
	}
	return 0;
}

void
PtyProxyLinux::resize_screen(int nrows, int ncols) {
	_num_rows = nrows;
	_num_cols = ncols;
	if (_poller != nullptr) {
		_poller->write_cmd(Poller::CMD_RESIZE, nullptr, 0);
	}
}

