#include "fwd.h"

class AsyncEvent {
public:
	AsyncEvent(boost::asio::io_context& ioc) : timer(ioc) {
		timer.expires_at(boost::posix_time::pos_infin);
	}
	template<typename WaitToken>
	auto async_wait(WaitToken&& handler) {
		return timer.async_wait(std::forward<WaitToken>(handler));
	}
	auto async_wait() {
		return timer.async_wait();
	}
	void notify_one() {
		timer.cancel_one();
	}
	void notify_all() {
		timer.cancel();
	}
private:
	deadline_timer timer;
};

