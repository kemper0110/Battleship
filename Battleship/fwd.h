#pragma once

#define _SILENCE_ALL_CXX23_DEPRECATION_WARNINGS
#include <sdkddkver.h>
#define BOOST_ASIO_HAS_IOCP
#include <boost/asio.hpp>
#include <boost/asio/experimental/as_single.hpp>
#include <boost/asio/experimental/coro.hpp>



namespace asio = boost::asio;
namespace proc = asio::ip;
using tcp = proc::tcp;
using asio::co_spawn;
using asio::detached;
using asio::awaitable;
using asio::experimental::use_coro;
using asio::experimental::coro;
using asio::use_awaitable;
namespace this_coro = asio::this_coro;
using boost::system::error_code;
namespace errc = boost::system::errc;
using asio::any_io_executor;


using use_awaitable_ec_t = asio::experimental::as_single_t<asio::use_awaitable_t<>>;

//using use_coro_ec_t = asio::experimental::as_single_t<asio::experimental::use_coro_t<>>;

using deadline_timer = use_awaitable_ec_t::as_default_on_t<asio::deadline_timer>;
using stream_handle = use_awaitable_ec_t::as_default_on_t<asio::windows::stream_handle>;


//struct Fsm;

namespace Battleship {
	class BattleshipGame;
}