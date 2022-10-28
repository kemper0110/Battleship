#define _SILENCE_ALL_CXX23_DEPRECATION_WARNINGS
#include "BattleshipGame.h"



Battleship::BattleshipGame::BattleshipGame() : window(sf::VideoMode(750, 550), "battleship", sf::Style::Titlebar | sf::Style::Close, sf::ContextSettings{ 0U, 0U, 8U }), pipe_stream(main_context), buttonClickEvent(main_context) {
	window.setActive(false);
	const auto font_loaded = font.loadFromFile("Segoe ui bold.ttf");
	assert(font_loaded);

	button_text.setFont(font);
	self_text.setFont(font);
	self_text.setString("Self board");
	opponent_text.setFont(font);
	opponent_text.setString("Opponent board");
}

int Battleship::BattleshipGame::run() {
	_init_connect_pipe();
	pipe_stream.assign(pipe_handle);

	std::cout << "i'm player #" << player_id << '\n';

	//if (player_id == 0) 	// attacking
	//	button_active = true;
	//else 			// defencing
	//	button_active = false;


	const auto main_context_guard = boost::asio::make_work_guard(main_context);

	main_thread = std::thread([this] {
		main_context.run();
		}
	);

	asio::co_spawn(main_context, play(), detached);

	asio::dispatch(main_context, [this] {
		this->render();
		}
	);

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

void Battleship::BattleshipGame::render() {
	window.clear(sf::Color(100, 100, 0));

	self_text.setPosition({ padding.x, 0.f });
	window.draw(self_text);
	draw_board(self_board, padding);

	opponent_text.setPosition({ 2 * padding.x + board_size, 0.f });
	window.draw(opponent_text);
	draw_board(opponent_board, padding + sf::Vector2f{ padding.x + board_size, 0.f });


	sf::RectangleShape button(button_size);
	button.setOutlineColor(sf::Color::Black);
	button.setOutlineThickness(2.f);
	button.setPosition(padding + sf::Vector2f{ 0.f, board_size } + sf::Vector2f{ 0.f, padding.y });
	if (button_active) {
		button_text.setString("fire");
		button.setFillColor(sf::Color::Green);
	}
	else {
		button_text.setString("waiting");
		button.setFillColor(sf::Color::Red);
	}
	window.draw(button);
	{
		const auto text_bounds = button_text.getLocalBounds();
		this->button_bounds = button.getGlobalBounds();
		const auto button_bounds = button.getSize();
		button_text.setPosition(button.getPosition() + button_bounds / 2.f - sf::Vector2f{ text_bounds.width / 2, text_bounds.height });
		window.draw(button_text);
	}

	window.display();
}


void Battleship::BattleshipGame::onButtonClick() {
	//asio::co_spawn(main_context, onButtonClickAsync(), detached);
	buttonClickEvent.notify_one();
}

void Battleship::BattleshipGame::onEvent(sf::Event event) {
	switch (event.type) {
	case sf::Event::Closed:
		window.close();
		break;
	case sf::Event::MouseButtonPressed:
		switch (event.mouseButton.button) {
		case sf::Mouse::Left:
			if (button_active and
				button_bounds.contains(sf::Vector2f((float)event.mouseButton.x, (float)event.mouseButton.y)))
				onButtonClick();
			break;
		}
		break;
	case sf::Event::KeyPressed:
		switch (event.key.code) {
		case sf::Keyboard::Space:
			render();
			//asio::co_spawn(main_context, send((std::ostringstream() << rand()).str()), asio::detached);
			break;
		}
		break;
	}
}

void Battleship::BattleshipGame::_init_connect_pipe() {
	pipe_handle = CreateNamedPipe(pipe_name, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 8192, 8192, 5000, NULL);
	const auto error = GetLastError();
	if (error == ERROR_ALREADY_EXISTS) {
		player_id = 1;
		const auto wait_status = WaitNamedPipe(pipe_name, NMPWAIT_WAIT_FOREVER);
		pipe_handle = CreateFile(pipe_name, GENERIC_ALL, NULL, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, NULL);
		if (pipe_handle == INVALID_HANDLE_VALUE)
			throw std::runtime_error("write error: " + std::to_string(GetLastError()));
	}
	else {
		player_id = 0;
		ConnectNamedPipe(pipe_handle, NULL);
	}
	if (pipe_handle == INVALID_HANDLE_VALUE)
		throw std::runtime_error("write error: " + std::to_string(error));
}




asio::any_io_executor Battleship::BattleshipGame::get_executor() {
	return main_context.get_executor();
}

awaitable<void> Battleship::BattleshipGame::send(const std::string& message)
{
	const auto& [send_ec, sended_n] = co_await pipe_stream.async_write_some(asio::buffer(message));
	assert(sended_n == message.size());
	if (send_ec) {
		std::cout << "send error: " << send_ec.message() << '\n';
		co_return;
	}
	std::cout << "sended\n";
}

awaitable<std::string> Battleship::BattleshipGame::receive()
{
	std::string response_str(1024, 0);
	const auto& [receive_ec, received_n] = co_await pipe_stream.async_read_some(asio::buffer(response_str));
	if (receive_ec) {
		std::cout << "receive error: " << receive_ec.message() << '\n';
		co_return "";
	}
	co_return response_str;
}

awaitable<void> Battleship::BattleshipGame::play() {
	// start from defencing if player is second
	if (player_id == 0)
		goto player0;
	else
		goto player1;

	for (;;) {
	player0:
		{
			button_active = true;
			std::cout << "attack state, button = " << button_active << '\n';
			render();
			Event attack_result;
			do attack_result = co_await co_spawn(main_context, attack_coro(), use_awaitable);
			while (attack_result != Event::Missed);
			if (attack_result == Event::Destroyed)
				break;
		}
	player1:
		{
			button_active = false;
			std::cout << "defence state, button = " << button_active << '\n';
			render();
			Event defence_result;
			do defence_result = co_await co_spawn(main_context, defence_coro(), use_awaitable);
			while (defence_result != Event::Missed);
			if (defence_result == Event::Destroyed)
				break;
		}
	}
}

awaitable<Battleship::BattleshipGame::Event>
Battleship::BattleshipGame::attack_coro()
{
	try {
		co_await buttonClickEvent.async_wait();
	}
	catch (...){}

	Event attack_result;
	switch (attack_state.state) {
	case AttackState::indefinite:
		attack_result = co_await co_spawn(main_context, attack_indefinite_coro(), use_awaitable);
		break;
	case AttackState::target_found:
		attack_result = co_await co_spawn(main_context, attack_target_found_coro(), use_awaitable);
		break;
	case AttackState::target_continue:
		attack_result = co_await co_spawn(main_context, attack_target_continue_coro(), use_awaitable);
		break;
	case AttackState::target_reverse:
		attack_result = co_await co_spawn(main_context, attack_target_reverse_coro(), use_awaitable);
		break;
	}
	render();
	co_return attack_result;
}


awaitable<Battleship::BattleshipGame::Event>
Battleship::BattleshipGame::defence_coro()
{
	const auto& message = co_await receive();
	const auto pos = std::stoi(message);

	const auto event = [this, pos, &message] {
		switch (self_board[pos]) {
		case Cell::Empty:
			self_board[pos] = Cell::FiredEmpty;
			return Event::Missed;
		case Cell::FiredEmpty: case Cell::FiredShip:
			throw std::runtime_error("bad strategy: opponent fires already fired cell #" + message);
		}
		self_board[pos] = Cell::FiredShip;
		// check is ship destroyed
		
		for (const auto dir : { 1, -1, 10, -10 }) {
			const auto newpos = pos + dir;
			if (newpos < 0 or newpos >= self_board.size())
				continue;
			if (self_board[newpos] == Cell::Ship) 
				return Event::Hitted;
			if (self_board[newpos] == Cell::FiredShip) 
				for (int i = newpos; i >= 0 and i < self_board.size() and self_board[i] != Cell::Empty; i += dir) 
					if (self_board[i] == Cell::Ship)
						return Event::Hitted;
		}
		return Event::Destroyed;
	}();
	co_await send(std::to_string(event));
	render();
	co_return event;
}

awaitable<Battleship::BattleshipGame::Event> Battleship::BattleshipGame::attack_indefinite_coro()
{
	const auto pos = [this] {
		auto pos = board_pos(rnd);
		while (opponent_board[pos] != Cell::Empty)
			pos = board_pos(rnd);
		return pos;
	}();

	attack_state.push(pos);

	std::cout << "attacking " << pos << '\n';

	co_await send(std::to_string(pos));
	const auto& response_str = co_await receive();

	// transitions
	const auto event = (Event)std::stoi(response_str);
	std::cout << "received response: " << event << '\n';
	switch (event) {
	case Event::Destroyed:
		attack_state.state = AttackState::indefinite;
		opponent_board[pos] = Cell::FiredShip;
		break;
	case Event::Hitted:
		attack_state.state = AttackState::target_found;
		opponent_board[pos] = Cell::FiredShip;
		break;
	case Event::Missed:
		attack_state.state = AttackState::indefinite;
		opponent_board[pos] = Cell::FiredEmpty;
		break;
	default:
		throw std::runtime_error("unknown message: " + response_str);
	}

	co_return event;
}

awaitable<Battleship::BattleshipGame::Event> Battleship::BattleshipGame::attack_target_found_coro()
{
	const auto step = [this] {
		auto const prev = attack_state.cur;
		const auto deltas = { -10, +10, -1, +1 }; // up, down, left, right
		for (const auto delta : deltas)
			if (auto idx = prev + delta;
				idx >= 0 and idx < opponent_board.size() and opponent_board[idx] == Cell::Empty)
				return idx;
		throw std::runtime_error("can't hit ship and then not found continuation");
	}();


	std::cout << "attacking " << step << '\n';

	co_await send(std::to_string(step));
	const auto& response_str = co_await receive();

	// transitions
	const auto event = (Event)std::stoi(response_str);
	std::cout << "received response: " << event << '\n';
	switch (event) {
	case Event::Destroyed:
		attack_state.push(step);
		attack_state.state = AttackState::indefinite;
		opponent_board[step] = Cell::FiredShip;
		break;
	case Event::Hitted:
		attack_state.state = AttackState::target_continue;
		// push only if HITTED
		attack_state.push(step);
		opponent_board[step] = Cell::FiredShip;
		break;
	case Event::Missed:
		attack_state.state = AttackState::target_found;
		opponent_board[step] = Cell::FiredEmpty;
		break;
	default:
		throw std::runtime_error("unknown message: " + response_str);
	}

	co_return event;
}

awaitable<Battleship::BattleshipGame::Event> Battleship::BattleshipGame::attack_target_continue_coro()
{
	const auto direction = attack_state.cur - attack_state.last;
	const auto step = attack_state.cur + direction;
	// delegate attacking
	// if step is out of bounds
	{
		const auto [step_y, step_x] = std::div(step, 10);
		const auto [cur_y, cur_x] = std::div(attack_state.cur, 10);
		const auto abs_dir = std::abs(direction);
		if (
			step < 0 or step >= opponent_board.size() or
			abs_dir == 1 and step_y != cur_y or			// new step is in same row
			abs_dir == 5 and step_x != cur_x			// new step is in same col
			) {
			attack_state.state = AttackState::target_reverse;
			co_return co_await co_spawn(main_context, attack_target_reverse_coro(), use_awaitable);
		}
	}


	std::cout << "attacking " << step << '\n';

	co_await send(std::to_string(step));
	const auto& response_str = co_await receive();

	// transitions
	const auto event = (Event)std::stoi(response_str);
	std::cout << "received response: " << event << '\n';
	switch (event) {
	case Event::Destroyed:
		attack_state.push(step);
		attack_state.state = AttackState::indefinite;
		opponent_board[step] = Cell::FiredShip;
		break;
	case Event::Hitted:
		attack_state.state = AttackState::target_continue;
		attack_state.push(step);
		opponent_board[step] = Cell::FiredShip;
		break;
	case Event::Missed:
		attack_state.push(step);
		attack_state.state = AttackState::target_reverse;
		opponent_board[step] = Cell::FiredEmpty;
		break;
	default:
		throw std::runtime_error("unknown message: " + response_str);
	}

	co_return event;
}

awaitable<Battleship::BattleshipGame::Event> Battleship::BattleshipGame::attack_target_reverse_coro()
{
	const auto direction = attack_state.cur - attack_state.last;
	const auto reverse_direction = -direction;
	const auto step = [this, reverse_direction] {
		for (int i = attack_state.cur; i >= 0 and i < opponent_board.size(); i += reverse_direction)
			if (opponent_board[i] == Cell::Empty)
				return i;
		throw std::runtime_error("lier");
	}();
	attack_state.push(step + direction);	// simulate 'prev' step
	attack_state.push(step);

	std::cout << "attacking " << step << '\n';

	co_await send(std::to_string(step));
	const auto& response_str = co_await receive();

	// transitions
	const auto event = (Event)std::stoi(response_str);
	std::cout << "received response: " << event << '\n';
	switch (event) {
	case Event::Destroyed:
		attack_state.state = AttackState::indefinite;
		opponent_board[step] = Cell::FiredShip;
		break;
	case Event::Hitted:
		attack_state.state = AttackState::target_continue;
		opponent_board[step] = Cell::FiredShip;
		break;
	case Event::Missed:
		throw std::runtime_error("lier: in" + std::to_string(step) + " ship or you lied previously");
		attack_state.state = AttackState::indefinite;
		opponent_board[step] = Cell::FiredEmpty;
		break;
	default:
		throw std::runtime_error("unknown message: " + response_str);
	}

	co_return event;
}

void Battleship::BattleshipGame::draw_board(std::array<Cell, 10 * 10>& board, const sf::Vector2f& start_pos) {

	sf::RectangleShape self_rect({ board_size, board_size });
	self_rect.setPosition(start_pos);
	self_rect.setFillColor(sf::Color::Blue);
	window.draw(self_rect);

	const auto step = board_size / 10;

	for (int i = 0; i < board.size(); ++i) {
		if (board[i] == Cell::Empty)
			continue;
		sf::Color color;
		switch (board[i]) {
		case Cell::FiredEmpty: color = sf::Color::Black; break;
		case Cell::FiredShip: color = sf::Color(200, 150, 0); break;
		case Cell::Ship: color = sf::Color::Green; break;
		}
		const auto [y, x] = std::div(i, 10);

		const auto pos = start_pos + sf::Vector2f{ x * step, y * step };
		sf::Vertex quad[4]{
			{ { pos }, color },
			{ { pos + sf::Vector2f{ step, 0.f } }, color },
			{ { pos + sf::Vector2f{ step, step } }, color },
			{ { pos + sf::Vector2f{ 0.f, step } }, color },
		};
		window.draw(quad, 4, sf::Quads);
	}

	{
		for (int i = 0; i < 11; ++i) {
			sf::Vertex vline[2]{
				{ { start_pos.x + step * i,   start_pos.y } },
				{ { start_pos.x + step * i,   start_pos.y + board_size } },
			};
			sf::Vertex hline[2]{
				{ { start_pos.x,              start_pos.y + step * i } },
				{ { start_pos.x + board_size, start_pos.y + step * i } },
			};
			window.draw(vline, 2, sf::Lines);
			window.draw(hline, 2, sf::Lines);
		}
	}
}




//coro<Battleship::BattleshipGame::Event> Battleship::BattleshipGame::attack_coro() {
//
//	//auto indefinite = attack_indefinite_coro(state);
//	//auto target_found = attack_target_found_coro(state);
//	//auto target_continue = attack_target_continue_coro(state);
//	//auto target_reverse = attack_target_reverse_coro(state);
//	//for (;;) {
//	//	switch (state.state) {
//	//	case AttackState::indefinite:
//	//		co_await indefinite.async_resume(use_coro);
//	//		break;
//	//	case AttackState::target_found:
//	//		co_await target_found.async_resume(use_coro);
//	//		break;
//	//	case AttackState::target_continue:
//	//		co_await target_continue.async_resume(use_coro);
//	//		break;
//	//	case AttackState::target_reverse:
//	//		co_await target_reverse.async_resume(use_coro);
//	//		break;
//	//	}
//	//	co_yield Event::Missed;
//	//}
//	//co_yield Event{};
//	co_return;
//}
