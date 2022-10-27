#pragma once
//#include <boost/statechart/state_machine.hpp>
//#include <boost/statechart/custom_reaction.hpp>
//#include <boost/statechart/simple_state.hpp>
//#include <boost/statechart/state.hpp>
//#include <boost/statechart/transition.hpp>
//
//#include <boost/msm/front/state_machine_def.hpp>
//#include <boost/msm/back/state_machine.hpp>
//#include <boost/mpl/list.hpp>
//#include <boost/mpl/vector.hpp>


#include <iostream>
#include <array>
#include <random>

#include "fwd.h"

//
//namespace sc = boost::statechart;
//namespace mpl = boost::mpl;
//namespace msm = boost::msm;




// events
//namespace event {
//	struct first_player : sc::event<first_player> {};
//	struct second_player : sc::event<second_player> {};
//
//	struct missed : sc::event<missed> {};
//	struct hitted : sc::event<hitted> {};
//
//	struct won : sc::event<won> {};
//	struct losed : sc::event<losed> {};
//}
//
//namespace state {
//	struct start;
//	struct end;
//	struct attack;
//	struct defence;
//}
//
//
//struct Fsm : sc::state_machine<Fsm, state::start> {
//	friend Battleship::BattleshipGame;
//	Battleship::BattleshipGame& game;
//	Fsm(Battleship::BattleshipGame& game);
//};
//
//// states
//struct state::start : sc::state<state::start, Fsm> {
//	using base = sc::state<::state::start, Fsm>;
//	using reactions = mpl::list<
//		sc::custom_reaction<event::first_player>,
//		sc::transition<event::second_player, ::state::defence>
//	>;
//	Battleship::BattleshipGame& game;
//	start(my_context ctx);
//	sc::result react(const event::first_player&);
//};
//struct state::end : sc::state<state::end, Fsm> {
//	using base = sc::state<::state::end, Fsm>;
//	Battleship::BattleshipGame& game;
//	end(my_context ctx);
//};
//
//struct state::attack : sc::state<state::attack, Fsm> {
//	using base = sc::state<::state::attack, Fsm>;
//	using reactions = mpl::list<
//		sc::transition<event::hitted, ::state::attack >,
//		sc::transition<event::missed, ::state::defence>
//	>;
//	Battleship::BattleshipGame& game;
//	attack(my_context ctx);
//};
//struct state::defence : sc::state<state::defence, Fsm> {
//	using base = sc::state<::state::defence, Fsm>;
//	using reactions = mpl::list<
//		sc::transition<event::hitted, ::state::defence>,
//		sc::transition<event::missed, ::state::attack>
//	>;
//	Battleship::BattleshipGame& game;
//	defence(my_context ctx);
//};


//namespace event {
//	struct hit : sc::event<hit> {};
//	struct destroyed : sc::event<destroyed> {};
//	struct missed : sc::event<destroyed> {};
//}
//namespace state {
//	struct indefinite;
//	struct targetFound;
//	struct continuationFound;
//	struct continuationReverse;
//}


//struct Data {
//	std::array<int, 5 * 5> map;
//	std::random_device rnd;
//	std::uniform_int_distribution<int> distr = std::uniform_int_distribution<int>(0, 5 * 5 - 1);
//	int last = 0, cur = 0;
//	void push(int step) {
//		last = cur;
//		cur = step;
//	}
//	int random_pos() {
//		return distr(rnd);
//	}
//} data{
//	std::array<int, 5 * 5>{
//		0, 0, 0, 0, 0,
//		0, 0, 0, 0, 0,
//		0, 0, 0, 0, 0,
//		0, 0, 0, 0, 0,
//		0, 0, 0, 0, 0
//	}
//};

/*enum Cell : int {
	Empty = 0, Ship = 1, FiredEmpty, FiredShip
};

struct hit {};
struct destroyed {};
struct missed {};

struct MSM_ : msm::front::state_machine_def<MSM_> {
	template <class Event, class FSM>
	void on_entry(Event const&, FSM&)
	{
		std::cout << "entering: Player" << std::endl;
	}
	template <class Event, class FSM>
	void on_exit(Event const&, FSM&)
	{
		std::cout << "leaving: Player" << std::endl;
	}
	struct indefinite : msm::front::state<> {
		template <class Event, class FSM>
		void on_entry(Event const&, FSM&) { std::cout << "entering: indefinite" << std::endl; }
		template <class Event, class FSM>
		void on_exit(Event const&, FSM&) { std::cout << "leaving: indefinite" << std::endl; }
	};
	struct targetFound : msm::front::state<> {};
	struct continuationFound : msm::front::state<> {};
	struct continuationReverse : msm::front::state<> {};

	using initial_state = indefinite;
};

using Fsm = msm::back::state_machine<MSM_>;*/

//
//struct hitMachine : sc::state_machine<hitMachine, state::indefinite> {
//};
//
//
//
//struct state::indefinite : sc::simple_state<state::indefinite, hitMachine> {
//	using reactions = mpl::list <
//		sc::custom_reaction<event::hit>,
//		sc::custom_reaction<event::missed>,
//		sc::custom_reaction<event::destroyed>
//	>;
//	indefinite() {
//		auto pos = data.random_pos();
//		while (data.map[pos] != Cell::Empty)
//			pos = data.random_pos();
//		data.push(pos);
//		std::cout << "firing " << pos << '\n';
//	}
//	sc::result react(const event::hit&) {
//		data.map[data.cur] = Cell::FiredShip;
//		return transit<state::targetFound>();
//	}
//	sc::result react(const event::destroyed&) {
//		data.map[data.cur] = Cell::FiredShip;
//		return transit<state::indefinite>();
//	}
//	sc::result react(const event::missed&) {
//		data.map[data.cur] = Cell::FiredEmpty;
//		return transit<state::indefinite>();
//	}
//};
//
//struct state::targetFound : sc::simple_state<state::targetFound, hitMachine> {
//	using reactions = mpl::list <
//		sc::custom_reaction<event::hit>,
//		sc::custom_reaction<event::missed>,
//		sc::custom_reaction<event::destroyed>
//	>;
//	targetFound() {
//		// round algo
//		const auto step = [this] {
//			auto const prev = data.cur;
//			const auto deltas = { -5, +5, -1, +1 }; // up, down, left, right
//			for (const auto delta : deltas)
//				if (auto idx = prev + delta;
//					idx >= 0 and idx < data.map.size() and data.map[idx] == Cell::Empty)
//					return idx;
//			throw std::runtime_error("can't hit ship and then not found continuation");
//		}();
//		data.push(step);
//		std::cout << "firing " << step << '\n';
//	}
//	sc::result react(const event::hit&) {
//		data.map[data.cur] = Cell::FiredShip;
//		return transit<state::continuationFound>();
//	}
//	sc::result react(const event::destroyed&) {
//		data.map[data.cur] = Cell::FiredShip;
//		return transit<state::indefinite>();
//	}
//	sc::result react(const event::missed&) {
//		data.map[data.cur] = Cell::FiredEmpty;
//		return transit<state::targetFound>();
//	}
//};
//
//struct state::continuationFound : sc::simple_state<state::continuationFound, hitMachine> {
//	using reactions = mpl::list <
//		sc::custom_reaction<event::hit>,
//		sc::custom_reaction<event::missed>,
//		sc::custom_reaction<event::destroyed>
//	>;
//	continuationFound() {
//		const auto direction = data.cur - data.last;
//		const auto step = data.cur + direction;
//		if (step < 0 or step >= data.map.size()) {
//			this->post_event(event::missed {});
//		} else {
//			data.push(step);
//			std::cout << "firing " << step << '\n';
//		}
//	}
//	sc::result react(const event::hit&) {
//		data.map[data.cur] = Cell::FiredShip;
//		return transit<state::continuationFound>();
//	}
//	sc::result react(const event::destroyed&) {
//		data.map[data.cur] = Cell::FiredShip;
//		return transit<state::indefinite>();
//	}
//	sc::result react(const event::missed&) {
//		data.map[data.cur] = Cell::FiredEmpty;
//		return transit<state::continuationReverse>();
//	}
//};
//
//struct state::continuationReverse : sc::simple_state<state::continuationReverse, hitMachine> {
//	using reactions = mpl::list <
//		sc::custom_reaction<event::hit>,
//		sc::custom_reaction<event::missed>,
//		sc::custom_reaction<event::destroyed>
//	>;
//	continuationReverse()
//	{
//		const auto direction = data.cur - data.last;
//		const auto reverse_direction = -direction;
//		const auto step = [this, reverse_direction] {
//			for (int i = data.cur; i >= 0 and i < data.map.size(); i += reverse_direction)
//				if (data.map[i] == Cell::Empty)
//					return i;
//			throw std::runtime_error("lier");
//		}();
//		data.push(step + direction);
//		data.push(step);
//	}
//	sc::result react(const event::hit&) {
//		data.map[data.cur] = Cell::FiredShip;
//		return transit<state::continuationFound>();
//	}
//	sc::result react(const event::destroyed&) {
//		data.map[data.cur] = Cell::FiredShip;
//		return transit<state::indefinite>();
//	}
//	sc::result react(const event::missed&) {
//		throw std::runtime_error("lier");
//		return transit<state::indefinite>();
//	}
//};




//
//int dela_main()
//{
//	try {
//		Fsm m;
//		m.start();
//
//		while (true) {
//			int result;
//			std::cin >> result;
//			switch (result) {
//			case 0: // missed
//				m.process_event(missed {});
//				break;
//			case 1: // hit
//				m.process_event(hit {});
//				break;
//			case 2: // destroyed
//				m.process_event(destroyed {});
//				break;
//			}
//		}
//	}
//	catch (const std::exception& ex) {
//		std::cout << ex.what() << '\n';
//	}
//	return 0;
//}