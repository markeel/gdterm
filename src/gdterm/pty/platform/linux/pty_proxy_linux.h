#ifndef PTY_PROXY_LINUX_H
#define PTY_PROXY_LINUX_H

#include "poller.h"
#include "pty_proxy.h"

class PtyProxyLinux : public PtyProxy, PollHandler {
public:
	PtyProxyLinux();
	PtyProxyLinux(const std::wstring &command);
	virtual ~PtyProxyLinux();

	int send_string(const char * data) override;
	int available_to_send() override;
	void resize_screen(int nrows, int ncols) override;

	virtual void handle(unsigned char * data, int data_len) override;
	virtual void apply_size() override;
	virtual void exited() override;


private:
	Poller * _poller;
	int _pty_fd;
    const std::wstring _command;

	void _init_pty();
};

#endif
