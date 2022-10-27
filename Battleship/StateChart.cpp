//#include "StateChart.h"
//
//
////
////state::start::start(my_context ctx) : base(ctx), game(context<Fsm>().game) {}
////
////sc::result state::start::react(const event::first_player&) {
////	std::cout << "ama first\n";
////	return transit<::state::attack>();
////}
////
////
////Fsm::Fsm(Battleship::BattleshipGame& game) : game(game) {
////
////}
////
////state::end::end(my_context ctx) : base(ctx), game(context<Fsm>().game) {
////
////}
////
////state::attack::attack(my_context ctx) : base(ctx), game(context<Fsm>().game) {
////
////}
////
////state::defence::defence(my_context ctx) : base(ctx), game(context<Fsm>().game) {
////
////}
