//
// ip/impl/address_v4.ipp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_IP_IMPL_ADDRESS_V4_IPP
#define ASIO_IP_IMPL_ADDRESS_V4_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "asio/detail/config.hpp"
#include <climits>
#include <stdexcept>
#include "asio/error.hpp"
#include "asio/detail/socket_ops.hpp"
#include "asio/detail/throw_error.hpp"
#include "asio/detail/throw_exception.hpp"
#include "asio/ip/address_v4.hpp"

#include "asio/detail/push_options.hpp"

namespace asio {
namespace ip {

address_v4::address_v4(const address_v4::bytes_type& bytes)
{
#if UCHAR_MAX > 0xFF
  if (bytes[0] > 0xFF || bytes[1] > 0xFF
      || bytes[2] > 0xFF || bytes[3] > 0xFF)
  {
    std::out_of_range ex("address_v4 from bytes_type");
    asio::detail::throw_exception(ex);
  }
#endif // UCHAR_MAX > 0xFF

  using namespace std; // For memcpy.
  memcpy(&addr_.s_addr, bytes.data(), 4);
}

address_v4::address_v4(unsigned long addr)
{
#if ULONG_MAX > 0xFFFFFFFF
  if (addr > 0xFFFFFFFF)
  {
    std::out_of_range ex("address_v4 from unsigned long");
    asio::detail::throw_exception(ex);
  }
#endif // ULONG_MAX > 0xFFFFFFFF

  addr_.s_addr = asio::detail::socket_ops::host_to_network_long(
      static_cast<asio::detail::u_long_type>(addr));
}

address_v4::bytes_type address_v4::to_bytes() const
{
  using namespace std; // For memcpy.
  bytes_type bytes;
#if defined(ASIO_HAS_STD_ARRAY)
  memcpy(bytes.data(), &addr_.s_addr, 4);
#else // defined(ASIO_HAS_STD_ARRAY)
  memcpy(bytes.elems, &addr_.s_addr, 4);
#endif // defined(ASIO_HAS_STD_ARRAY)
  return bytes;
}

unsigned long address_v4::to_ulong() const
{
  return asio::detail::socket_ops::network_to_host_long(addr_.s_addr);
}

std::string address_v4::to_string() const
{
  asio::error_code ec;
  std::string addr = to_string(ec);
  asio::detail::throw_error(ec);
  return addr;
}

std::string address_v4::to_string(asio::error_code& ec) const
{
  char addr_str[asio::detail::max_addr_v4_str_len];
  const char* addr =
    asio::detail::socket_ops::inet_ntop(
        ASIO_OS_DEF(AF_INET), &addr_, addr_str,
        asio::detail::max_addr_v4_str_len, 0, ec);
  if (addr == 0)
    return std::string();
  return addr;
}

address_v4 address_v4::from_string(const char* str)
{
  asio::error_code ec;
  address_v4 addr = from_string(str, ec);
  asio::detail::throw_error(ec);
  return addr;
}

address_v4 address_v4::from_string(
    const char* str, asio::error_code& ec)
{
  address_v4 tmp;
  if (asio::detail::socket_ops::inet_pton(
        ASIO_OS_DEF(AF_INET), str, &tmp.addr_, 0, ec) <= 0)
    return address_v4();
  return tmp;
}

address_v4 address_v4::from_string(const std::string& str)
{
  return from_string(str.c_str());
}

address_v4 address_v4::from_string(
    const std::string& str, asio::error_code& ec)
{
  return from_string(str.c_str(), ec);
}

bool address_v4::is_loopback() const
{
  return (to_ulong() & 0xFF000000) == 0x7F000000;
}

bool address_v4::is_unspecified() const
{
  return to_ulong() == 0;
}

bool address_v4::is_class_a() const
{
  return (to_ulong() & 0x80000000) == 0;
}

bool address_v4::is_class_b() const
{
  return (to_ulong() & 0xC0000000) == 0x80000000;
}

bool address_v4::is_class_c() const
{
  return (to_ulong() & 0xE0000000) == 0xC0000000;
}

bool address_v4::is_multicast() const
{
  return (to_ulong() & 0xF0000000) == 0xE0000000;
}

address_v4 address_v4::broadcast(const address_v4& addr, const address_v4& mask)
{
  return address_v4(addr.to_ulong() | (mask.to_ulong() ^ 0xFFFFFFFF));
}

address_v4 address_v4::netmask(const address_v4& addr)
{
  if (addr.is_class_a())
    return address_v4(0xFF000000);
  if (addr.is_class_b())
    return address_v4(0xFFFF0000);
  if (addr.is_class_c())
    return address_v4(0xFFFFFF00);
  return address_v4(0xFFFFFFFF);
}

} // namespace ip
} // namespace asio

#include "asio/detail/pop_options.hpp"

#endif // ASIO_IP_IMPL_ADDRESS_V4_IPP
