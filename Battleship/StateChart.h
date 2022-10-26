#pragma once
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/mpl/list.hpp>

#include <iostream>
#include <array>
#include <random>

namespace sc = boost::statechart;
namespace mpl = boost::mpl;


//// events
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
//
//struct start;
//struct end;
//struct attack;
//struct defence;
//
//struct machine : sc::state_machine<machine, start> {};
//
//// states
//struct start : sc::simple_state<start, machine> {
//	using reactions = mpl::list<
//		sc::custom_reaction<event::first_player>,
//		//sc::transition<event::first_player, attack>,
//		sc::transition<event::second_player, defence>
//	>;
//	start() {
//		std::cout << "start\n";
//	}
//	sc::result react(const event::first_player&) {
//		std::cout << "ama first\n";
//		return transit<attack>();
//	}
//};
//struct end : sc::simple_state<end, machine> {
//};
//
//struct attack : sc::simple_state<attack, machine> {
//	using reactions = mpl::list<
//		sc::transition<event::hitted, attack >,
//		sc::transition<event::missed, defence>
//	>;
//	attack() {
//		std::cout << "attack\n";
//	}
//};
//struct defence : sc::simple_state<defence, machine> {
//	using reactions = mpl::list<
//		sc::transition<event::hitted, defence>,
//		sc::transition<event::missed, attack>
//	>;
//};


namespace event {
	struct hit : sc::event<hit> {};
	struct destroyed : sc::event<destroyed> {};
	struct missed : sc::event<destroyed> {};
}
namespace state {
	struct indefinite;
	struct targetFound;
	struct continuationFound;
	struct continuationReverse;
}

enum Cell : int {
	Empty = 0, Ship = 1, FiredEmpty, FiredShip
};

struct Data {
	std::array<int, 5 * 5> map;
	int last = 0, cur = 0;
	int push(int step) {
		last = cur;
		cur = step;
	}
	int random_pos() {
		static auto Rnd = [rnd = std::random_device(), distr = std::uniform_int_distribution<int>(0, map.size() - 1)]() mutable {
			return distr(rnd);
		};
		return Rnd();
	}
};


struct hitMachine : sc::state_machine<hitMachine, state::indefinite> {
	Data& data;
	hitMachine(Data& data) : data(data) {

	}
};



struct state::indefinite : sc::simple_state<state::indefinite, hitMachine> {
	using reactions = mpl::list <
		sc::custom_reaction<event::hit>,
		sc::custom_reaction<event::missed>,
		sc::custom_reaction<event::destroyed>
	>;
	Data& data;
	indefinite() : data(context<hitMachine>().data) {
		auto pos = data.random_pos();
		while (data.map[pos] != Cell::Empty)
			pos = data.random_pos();
		data.push(pos);
		std::cout << "firing " << pos << '\n';
	}
	sc::result react(const event::hit&) {
		data.map[data.cur] = Cell::FiredShip;
		return transit<state::targetFound>();
	}
	sc::result react(const event::destroyed&) {
		data.map[data.cur] = Cell::FiredShip;
		return transit<state::indefinite>();
	}
	sc::result react(const event::missed&) {
		data.map[data.cur] = Cell::FiredEmpty;
		return transit<state::indefinite>();
	}
};

struct state::targetFound : sc::simple_state<state::targetFound, hitMachine> {
	using reactions = mpl::list <
		sc::custom_reaction<event::hit>,
		sc::custom_reaction<event::missed>,
		sc::custom_reaction<event::destroyed>
	>;
	Data& data;
	targetFound() : data(context<hitMachine>().data) {
		// round algo
		const auto step = [this] {
			auto const prev = data.cur;
			const auto deltas = { -5, +5, -1, +1 }; // up, down, left, right
			for (const auto delta : deltas) {
				auto const idx = prev + delta;
				if (idx >= 0 and idx < data.map.size() and data.map[idx] == Cell::Empty)
					return idx;
			}
			throw std::runtime_error("can't hit ship and then not found continuation");
		}();
		data.push(step);
	}
	sc::result react(const event::hit&) {
		data.map[data.cur] = Cell::FiredShip;
		return transit<state::continuationFound>();
	}
	sc::result react(const event::destroyed&) {
		data.map[data.cur] = Cell::FiredShip;
		return transit<state::indefinite>();
	}
	sc::result react(const event::missed&) {
		data.map[data.cur] = Cell::FiredEmpty;
		return transit<state::targetFound>();
	}
};

struct state::continuationFound : sc::simple_state<state::continuationFound, hitMachine> {
	using reactions = mpl::list <
		sc::custom_reaction<event::hit>,
		sc::custom_reaction<event::missed>,
		sc::custom_reaction<event::destroyed>
	>;
	Data& data;
	continuationFound() : data(context<hitMachine>().data) {

	}
	sc::result react(const event::hit&) {
		return transit<state::continuationFound>();
	}
	sc::result react(const event::destroyed&) {
		return transit<state::indefinite>();
	}
	sc::result react(const event::missed&) {
		return transit<state::continuationReverse>();
	}
};

struct state::continuationReverse : sc::simple_state<state::continuationReverse, hitMachine> {
	using reactions = mpl::list <
		sc::custom_reaction<event::hit>,
		sc::custom_reaction<event::missed>,
		sc::custom_reaction<event::destroyed>
	>;
	Data& data;
	continuationReverse() : data(context<hitMachine>().data) {

	}
	sc::result react(const event::hit&) {
		return transit<state::continuationFound>();
	}
	sc::result react(const event::destroyed&) {
		return transit<state::indefinite>();
	}
	sc::result react(const event::missed&) {
		throw std::runtime_error("lier");
		return transit<state::indefinite>();
	}
};





int dela_main()
{
	Data data{
		std::array<int, 5 * 5>{
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0
	}
	};
	hitMachine machine(data);
	machine.initiate();

	while (true) {
		int result;
		std::cin >> result;
		switch (result) {
		case 0: // missed
			machine.process_event(event::missed {});
			break;
		case 1: // hit
			machine.process_event(event::hit {});
			break;
		case 2: // destroyed
			machine.process_event(event::destroyed {});
			break;
		}
	}
	return 0;
}