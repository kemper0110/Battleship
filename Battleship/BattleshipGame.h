#pragma once

#include <sdkddkver.h>
#include <boost/asio.hpp>
#include <boost/process/async_pipe.hpp>
#include <iostream>

#include <SFML/Graphics.hpp>

namespace Battleship
{
	namespace asio = boost::asio;
	namespace process = boost::process;

	class BattleshipGame {
	private:
		sf::RenderWindow window;

		asio::io_context main_context{ 1 };
		std::thread main_thread;
		asio::ip::tcp::socket socket;

	public:
		BattleshipGame() : window(sf::VideoMode(640, 480), "battleship"), socket(main_context) {
			window.setActive(false);

			std::cout << "trying connect\n";
			boost::system::error_code ec;
			socket.connect({ asio::ip::address::from_string("127.0.0.1"), 1234 }, ec);
			if (ec) {
				std::cout << "error: " << ec.message() << '\n';
				//socket.bind({ asio::ip::address_v4{}, 55'555 }, ec);
				std::cout << "accepting\n";
				asio::ip::tcp::acceptor acceptor(main_context, { asio::ip::tcp::v4(), 1234 });
				socket.close();
				acceptor.accept(socket, ec);
				if (ec) {
					std::cout << "error: " << ec.message() << '\n';
					std::exit(1);
				}
			}


		}
		int run() {
			const auto main_context_guard = boost::asio::make_work_guard(main_context);

			main_thread = std::thread([this] {
				main_context.run();
				}
			);
			/*
				run coroutines here
			*/
			asio::co_spawn(main_context, onMessage(), asio::detached);


			while (window.isOpen()) {
				sf::Event event;
				const auto status = window.waitEvent(event);
				if (not status) {
					window.close();
					break;
				}
				asio::dispatch(main_context, [this, event = sf::Event(event)] {
					this->onEvent(event);
					}
				);
			}
			return 0;
		}
	private:

		void render() {
			window.clear();
			window.display();
		}

		asio::awaitable<void> onMessage() {
			try {
				boost::asio::streambuf receive_buffer;
				std::cout << "receiving started\n";
				for (;;) {
					const auto n = co_await asio::async_read(socket, receive_buffer, asio::use_awaitable);
					std::cout << "received: " << n << '\n';
				}
			}
			catch (std::exception& ex) {
				std::cout << "received error: " << ex.what() << '\n';
			}
		}
		asio::awaitable<void> send(std::string message) {
			std::cout << "sending\n";
			const auto n = co_await asio::async_write(socket, asio::buffer(message), asio::use_awaitable);
			std::cout << "sended: " << n << '\n';
		}

		void onEvent(sf::Event event) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
				case sf::Keyboard::Space:
					render();
					asio::co_spawn(main_context, send((std::ostringstream() << rand()).str()), asio::detached);
					break;
				}
				break;
			}
		}

	};
}

