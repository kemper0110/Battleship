#include "fwd.h"

class AsyncEvent {
public:
	AsyncEvent(boost::asio::io_context& ioc) : timer(ioc) {
		timer.expires_at(boost::posix_time::pos_infin);
	}
	template<typename WaitHandler>
	void async_wait(WaitHandler&& handler) {
		timer.async_wait(std::forward(handler));
	}
	void notify_one() {
		timer.cancel_one();
	}
	void notify_all() {
		timer.cancel();
	}
private:
	boost::asio::deadline_timer timer;
};

