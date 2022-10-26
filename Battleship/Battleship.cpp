//#include "BattleshipGame.h"
//#include <sdkddkver.h>
//#include <boost/asio.hpp>
#include "StateChart.h"
#include <iostream>

//namespace asio = boost::asio;
//namespace proc = asio::ip;
//using tcp = proc::tcp;
//using asio::co_spawn;
//using asio::detached;
//using asio::awaitable;
//using asio::use_awaitable;
//namespace this_coro = asio::this_coro;
//
//awaitable<void> sender(tcp::socket& socket) {
//	try {
//		for (;;) {
//			std::string str = "abobus";
//			str.resize(4096);
//			asio::deadline_timer timer(socket.get_executor(), boost::posix_time::seconds(2));
//			co_await timer.async_wait(use_awaitable);
//			co_await asio::async_write(socket, asio::buffer(str), use_awaitable);
//		}
//	}
//	catch (std::exception e) {
//		std::cout << e.what() << '\n';
//	}
//}
//awaitable<void> receiver(tcp::socket& socket) {
//	try {
//		for (;;) {
//			std::string v;
//			v.resize(4096);
//			auto n = co_await asio::async_read(socket, asio::buffer(v), use_awaitable);
//			std::cout << "received: " << v << '\n';
//		}
//	}
//	catch (std::exception e) {
//		std::cout << e.what() << '\n';
//	}
//}

int main() {
	std::system("chcp 1251 && cls");
	dela_main();
	//asio::io_context ioc{ 1 };

	//try {
	//	tcp::socket socket(ioc);
	//	socket.connect({ proc::address::from_string("127.0.0.1"), 3000 });

	//	co_spawn(ioc, receiver(socket), detached);
	//	co_spawn(ioc, sender(socket), detached);
	//}
	//catch (std::exception e) {
	//	std::cout << e.what() << '\n';

	//	tcp::acceptor acceptor(ioc, { proc::make_address_v4("127.0.0.1"), 3000 });
	//	auto socket = acceptor.accept();

	//	co_spawn(ioc, receiver(socket), detached);
	//	co_spawn(ioc, sender(socket), detached);
	//}

	//ioc.run();




	//return Battleship::BattleshipGame().run();
}

//template <typename Callback>
//void read_input(const std::string& prompt, Callback cb)
//{
//	std::thread(
//		[prompt, cb = std::move(cb)]() mutable
//		{
//			while (true) {
//				std::cout << prompt << ": " << std::endl;
//				std::string line;
//				std::getline(std::cin, line);
//				(cb)(std::move(line));
//			}
//		}).detach();
//}
//
//
//template <boost::asio::completion_token_for<void(std::string)> CompletionToken>
//auto async_read_input(const std::string& prompt, CompletionToken&& token)
//{
//	auto init = [](boost::asio::completion_handler_for<void(std::string)> auto handler, const std::string& prompt)
//	{
//		auto work = boost::asio::make_work_guard(handler);
//
//		read_input(prompt,
//			[handler = std::move(handler), work = std::move(work)
//			](std::string result) mutable
//			{
//				auto alloc = boost::asio::get_associated_allocator(
//					handler, boost::asio::recycling_allocator<void>());
//
//				boost::asio::dispatch(work.get_executor(),
//					boost::asio::bind_allocator(alloc,
//						[handler = std::move(handler), result = std::string(result)]() mutable
//						{
//							std::move(handler)(result);
//						}));
//			});
//	};
//
//	return boost::asio::async_initiate<CompletionToken, void(std::string)>(init, token, prompt);
//}


//
//
//boost::asio::awaitable<void> handleMessages(boost::process::async_pipe pipe) {
//
//	std::vector<char> buf(4096);
//	for (;;) {
//		const auto received = co_await boost::asio::async_read(pipe, boost::asio::buffer(buf), boost::asio::use_awaitable);
//		const auto sended = co_await boost::asio::async_write(pipe, boost::asio::buffer(buf), boost::asio::use_awaitable);
//	}
//}
//
//int main()
//{
//	boost::asio::io_context main_ctx;
//
//	{
//		boost::process::async_pipe pipe(main_ctx, R"!!(\\.\pipe\BattleshipPipe)!!");
//		boost::asio::co_spawn(main_ctx, handleMessages(std::move(pipe)), boost::asio::detached);
//	}
//
//
//	window.setActive(false);
//
//	//async_wait_event(boost::asio::bind_executor(io_context, handleEvent));
//	//io_context.run();
//	//async_read_input("Enter your name", boost::asio::bind_executor(io_context, [](const std::string& result)
//	//	{
//	//		std::cout << "Hello " << result << "\n";
//	//	}));
//	//boost::asio::co_spawn(io_context, )
//
//	{
//
//		std::thread([&main_ctx] {
//			std::cout << "main ctx begin\n";
//			window.setActive();
//			main_ctx.run();
//			std::cout << "main ctx end\n";
//			}).detach();
//
//			while (true) {
//				sf::Event event;
//				const auto status = window.waitEvent(event);
//				if (not status) break;
//
//				boost::asio::dispatch(main_ctx, [event = sf::Event(event)] {
//					handleEvent(event);
//					});
//			}
//	}
//
//	main_ctx.stop();
//}
