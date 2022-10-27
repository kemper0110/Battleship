#include "BattleshipGame.h"


Battleship::BattleshipGame::BattleshipGame() : window(sf::VideoMode(750, 550), "battleship", sf::Style::Titlebar | sf::Style::Close, sf::ContextSettings{ 0U, 0U, 8U }), pipe_stream(main_context) {
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

	if (player_id == 0) {	// attacking
		button_active = true;
	}
	else {					// defencing
		button_active = false;
		co_spawn(main_context, defence(), detached);
	}


	const auto main_context_guard = boost::asio::make_work_guard(main_context);

	main_thread = std::thread([this] {
		main_context.run();
		}
	);

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

asio::awaitable<void> Battleship::BattleshipGame::defence()
{
	std::cout << "defencing\n";

	std::string request(1024, 0);
	const auto& [receive_ec, received_n] = co_await pipe_stream.async_read_some(asio::buffer(request));
	if (receive_ec) {
		std::cout << "receive error: " << receive_ec.message() << '\n';
		co_return;
	}
	const auto pos = std::stoi(request);
	std::cout << "received request: " << pos << '\n';

	const auto event = [this, pos, &request] {
		switch (self_board[pos]) {
		case Cell::Empty:
			self_board[pos] = Cell::FiredEmpty;
			return Event::Missed;
		case Cell::FiredEmpty: case Cell::FiredShip:
			throw std::runtime_error("bad strategy: opponent fires already fired cell #" + request);
		}
		for (const auto dir : { 1, -1, 10, -10 }) {
			if (pos + dir < 0 or pos + dir >= self_board.size())
				continue;
			if (self_board[pos] == Cell::Ship) {
				self_board[pos] = Cell::FiredShip;
				return Event::Hitted;
			}
		}
		self_board[pos] = Cell::FiredShip;
		return Event::Destroyed;
	}();
	
	std::string response = std::to_string(event);
	const auto&[sended_ec, sended_n] = co_await pipe_stream.async_write_some(asio::buffer(response));
	assert(sended_n == response.length());
	if (sended_ec) {
		std::cout << "send error: " << sended_ec.message() << '\n';
		co_return;
	}

	// transitions

	std::cout << "sended response: " << event << '\n';
	switch (event) {
	case Event::Destroyed: case Event::Hitted: 
		co_spawn(main_context, defence(), detached);
		break;
	case Event::Missed:
		button_active = true;
		break;
	}
	render();
}

asio::awaitable<void> Battleship::BattleshipGame::onButtonClickAsync() {
	button_active = false;


	auto pos = board_pos(rnd);
	while (opponent_board[pos] != Cell::Empty)
		pos = board_pos(rnd);

	std::cout << "attacking " << pos << '\n';

	//send(std::to_string(pos));
	const auto message = std::to_string(pos);
	const auto& [send_ec, sended_n] = co_await pipe_stream.async_write_some(asio::buffer(message));
	assert(sended_n == message.size());
	if (send_ec) {
		std::cout << "send error: " << send_ec.message() << '\n';
		co_return;
	}
	std::cout << "sended\n";
	render();
	std::string response_str(1024, 0);
	const auto& [receive_ec, received_n] = co_await pipe_stream.async_read_some(asio::buffer(response_str));
	if (receive_ec) {
		std::cout << "receive error: " << receive_ec.message() << '\n';
		co_return;
	}

	// transitions
	const auto event = std::stoi(response_str);
	std::cout << "received response: " << event << '\n';
	switch (event) {
	case Event::Destroyed: case Event::Hitted:
		button_active = true;
		opponent_board[pos] = Cell::FiredShip;
		render();
		break;
	case Event::Missed:
		button_active = false;
		opponent_board[pos] = Cell::FiredEmpty;
		render();
		co_spawn(main_context, defence(), detached);
		break;
	default:
		throw std::runtime_error("unknown message: " + response_str);
	}
}

void Battleship::BattleshipGame::onButtonClick() {
	asio::co_spawn(main_context, onButtonClickAsync(), detached);
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
