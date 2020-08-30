/*******************************************************************************
 * Project Ips4o Benchmark Suite
 *
 * src/generator/generator.hpp
 *
 * Extract name of a data type.
 *
 * Copyright (C) 2020 Michael Axtmann <michael.axtmann@gmail.com>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see
 * <https://www.gnu.org/licenses/>.
 ******************************************************************************/

#pragma once

#include <cxxabi.h>
#include <iostream>
#include <memory>
#include <type_traits>
#include <string>

template <class T>
std::string type_name() {
  using Type = typename std::remove_reference<T>::type;

  // Create cxa name from Type.
  std::unique_ptr<char, decltype(&std::free)> cxa_name(
    abi::__cxa_demangle(typeid(Type).name(), nullptr, nullptr, nullptr), std::free);
  // 'name' becomes cxa name or typeid-name if cxa name is not available.
  std::string name = cxa_name ? cxa_name.get() : typeid(Type).name();

  // Add const and volatile if 'Type' is const and volatile.
  if (std::is_const<Type>::value) {
    name += " const";
  }
  if (std::is_volatile<Type>::value) {
    name += " volatile";
  }

  // Add references from 'T'
  if (std::is_lvalue_reference<T>::value) {
    name += "&";
  } else if (std::is_rvalue_reference<T>::value) {
    name += "&&";
  }
    
  return name;
}

template <class T>
void print_type_name() {
  std::cout << type_name<T>() << '\n';
} 
