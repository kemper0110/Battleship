#pragma once

#include <sdkddkver.h>
#define BOOST_ASIO_HAS_IOCP
#include <boost/asio.hpp>
#include <boost/asio/experimental/as_single.hpp>



namespace asio = boost::asio;
namespace proc = asio::ip;
using tcp = proc::tcp;
using asio::co_spawn;
using asio::detached;
using asio::awaitable;
using asio::use_awaitable;
namespace this_coro = asio::this_coro;
using boost::system::error_code;
namespace errc = boost::system::errc;


using token = asio::experimental::as_single_t<asio::use_awaitable_t<>>;

using deadline_timer = token::as_default_on_t<asio::deadline_timer>;
using stream_handle = token::as_default_on_t<asio::windows::stream_handle>;


struct Fsm;

namespace Battleship {
	class BattleshipGame;
}