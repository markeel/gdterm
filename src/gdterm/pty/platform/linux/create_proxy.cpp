
#include "pty_proxy_linux.h"

PtyProxy *
create_proxy(TermRenderer * renderer, const std::wstring & command) {
	PtyProxy * proxy = new PtyProxyLinux(command);
	proxy->set_renderer(renderer);
	return proxy;
}
