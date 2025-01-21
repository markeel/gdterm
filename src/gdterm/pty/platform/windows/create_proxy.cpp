#include "pty_proxy_win.h"
#include <stdlib.h>

PtyProxy *
create_proxy(TermRenderer * renderer, const std::wstring &command) {
	PtyProxy * proxy = new PtyProxyWin(command);
	proxy->set_renderer(renderer);
	return proxy;
}
