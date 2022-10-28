#pragma once
#define _SILENCE_ALL_CXX23_DEPRECATION_WARNINGS


#include <SFML/Graphics.hpp>


#include <iostream>
#include <array>
#include <random>

#include "AsyncEvent.h"

#include "fwd.h"

//#include "StateChart.h"


namespace Battleship
{

	class BattleshipGame {
	private:
		enum Cell : int {
			Empty = 0, Ship = 1, FiredShip = 2, FiredEmpty = 3
		};
		enum Event : int {
			Missed = 0, Hitted, Destroyed
		};
		struct AttackState {
			enum : int {
				indefinite = 0, target_found, target_continue, target_reverse
			};
			void push(int step) {
				last = cur;
				cur = step;
			}
			int last = 0, cur = 0;
			int state = indefinite;
		} attack_state;
	public:
		BattleshipGame();
		int run();
		//private:

		void render();
		void onButtonClick();
		void onEvent(sf::Event event);
		void draw_board(std::array<Cell, 10 * 10>& board, const sf::Vector2f& start_pos);
		void _init_connect_pipe();

		void fill_destroyed_ship(std::array<Cell, 10 * 10>& board, int pos) {
			for (const auto dir : { -10, 10, 1, -1 }) {
				const auto newpos = pos + dir;
				if (newpos < 0 or newpos >= board.size())
					continue;
				if (board[newpos] == Cell::FiredShip) {
					
					for (int i = newpos; i >= 0 and i < board.size() and board[i] != Cell::Empty; i += dir) {

					}

					break;
				}
			}
		}

		asio::any_io_executor get_executor();

		awaitable<void> send(const std::string& message);
		awaitable<std::string> receive();

		awaitable<void> play();

		awaitable<Event> attack_coro();
		awaitable<Event> defence_coro();

		awaitable<Event> attack_indefinite_coro();
		awaitable<Event> attack_target_found_coro();
		awaitable<Event> attack_target_continue_coro();
		awaitable<Event> attack_target_reverse_coro(); 

	public:
		constexpr static wchar_t pipe_name[] = LR"(\\.\pipe\AbobaPipe)";

		sf::RenderWindow window;

		//friend Fsm;
		//Fsm fsm;

		asio::io_context main_context{ 1 };
		std::thread main_thread;
		stream_handle pipe_stream;
		HANDLE pipe_handle = nullptr;
		int player_id = 0;

		AsyncEvent buttonClickEvent;

		/*
			Ships
				4x1
				3x2
				2x3
				1x4
		*/
		constexpr static unsigned SHIP_COUNT = 4 * 1 + 3 * 2 + 2 * 3 + 1 * 4;

		std::array<Cell, 10 * 10> self_board{
			Cell::Ship,  Cell::Empty, Cell::Empty, Cell::Empty,  Cell::Empty, Cell::Empty, Cell::Empty, Cell::Empty, Cell::Ship,  Cell::Empty,	// 0
			Cell::Ship,  Cell::Empty, Cell::Empty, Cell::Empty,  Cell::Empty, Cell::Ship,  Cell::Ship,  Cell::Empty, Cell::Empty, Cell::Empty,	// 1
			Cell::Ship,  Cell::Empty, Cell::Empty, Cell::Empty,  Cell::Empty, Cell::Empty, Cell::Empty, Cell::Empty, Cell::Empty, Cell::Empty,	// 2
			Cell::Ship,  Cell::Empty, Cell::Empty, Cell::Ship,   Cell::Ship,  Cell::Ship,  Cell::Empty, Cell::Ship,  Cell::Empty, Cell::Empty,	// 3
			Cell::Empty, Cell::Empty, Cell::Empty, Cell::Empty,  Cell::Empty, Cell::Empty, Cell::Empty, Cell::Empty, Cell::Empty, Cell::Empty,	// 4
			Cell::Empty, Cell::Ship,  Cell::Ship,  Cell::Ship,   Cell::Empty, Cell::Empty, Cell::Ship,  Cell::Empty, Cell::Empty, Cell::Empty,	// 5
			Cell::Empty, Cell::Empty, Cell::Empty, Cell::Empty,  Cell::Empty, Cell::Empty, Cell::Ship,  Cell::Empty, Cell::Empty, Cell::Ship,	// 6
			Cell::Ship,  Cell::Ship,  Cell::Empty, Cell::Empty,  Cell::Empty, Cell::Empty, Cell::Empty, Cell::Empty, Cell::Empty, Cell::Empty,	// 7
			Cell::Empty, Cell::Empty, Cell::Empty, Cell::Empty,  Cell::Empty, Cell::Empty, Cell::Empty, Cell::Empty, Cell::Empty, Cell::Ship,	// 9
		};
		std::array<Cell, 10 * 10> opponent_board{};

		constexpr static float board_size = 300.f;
		inline const static sf::Vector2f padding = sf::Vector2f{ 50.f, 50.f };
		inline const static sf::Vector2f button_size = sf::Vector2f{ 200.f, 100.f };

		sf::Font font;
		sf::Text button_text, self_text, opponent_text;
		sf::FloatRect button_bounds;
		bool button_active;

		std::random_device rnd;
		std::uniform_int_distribution<int> board_pos = std::uniform_int_distribution<int>(0, 10 * 10 - 1);

	};
}

