#define _SILENCE_ALL_CXX23_DEPRECATION_WARNINGS



#include "BattleshipGame.h"
//#include <sdkddkver.h>
//#include <boost/asio.hpp>
//#include "StateChart.h"
#include <iostream>





int main() {
	std::system("chcp 1251 && cls");
	//dela_main();



	try {
		return Battleship::BattleshipGame().run();
	}
	catch (const std::exception& ex) {
		std::cout << "exception inside battleship: " << ex.what() << '\n';
	}
	return 0;
}
