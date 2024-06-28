#pragma once
// https://en.cppreference.com/w/cpp/header
#include <cstdlib>
#include <csignal>
#include <csetjmp>
#include <cstdarg>
#include <typeinfo>
#include <typeindex>
#include <type_traits>
#include <bitset>
#include <functional>
#include <utility>
#include <time.h>
#include <chrono>
#include <cstddef>
#include <initializer_list>
#include <tuple>
#include <any>
#include <optional>
#include <variant>
#include <new>
#include <memory>
#include <scoped_allocator>
#include <memory_resource>
#include <climits>
#include <cfloat>
#include <cstdint>
#include <cinttypes>
#include <limits>
#include <exception>
#include <stdexcept>
#include <cassert>
#include <system_error>
#include <cerrno>
#include <cctype>
#include <cwctype>
#include <cstring>
#include <cwchar>
#include <cuchar>
#include <string>
#include <string_view>
#include <charconv>
#include <array>
#include <vector>
#include <deque>
#include <list>
#include <forward_list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <iterator>
#include <algorithm>
#include <execution>
#include <cmath>
#include <complex>
#include <valarray>
#include <random>
#include <numeric>
#include <ratio>
#include <cfenv>
#include <locale>
#include <clocale>
#include <codecvt>
#include <iosfwd>
#include <ios>
#include <istream>
#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <streambuf>
#include <cstdio>
#include <filesystem>
#include <regex>
#include <atomic>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <future>
#include <condition_variable>

#if __cplusplus > 201703L
#include <concepts> // cpp20
#include <coroutine> // cpp20
#include <compare>// cpp20
#include <version> // cpp20
#include <source_location> // cpp20
#include <format>// cpp20
#include <span> // cpp20
#include <ranges> // cpp20
#include <bit> // cpp20
#include <numbers> // cpp20
#include <syncstream> // cpp20
#include <stop_token> // cpp20
#include <semaphore> // cpp20
#include <latch> // cpp20
#include <barrier> // cpp20
#endif


#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING  1
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS 1

#undef min
#undef max
