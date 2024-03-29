#pragma once

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

#ifdef NDEBUG
#	include <spdlog/sinks/basic_file_sink.h>
#else
#	include <spdlog/sinks/msvc_sink.h>
#endif

using namespace std::literals;

namespace logger = SKSE::log;

namespace util
{
	using SKSE::stl::report_and_fail;
	using SKSE::stl::utf8_to_utf16;
	using SKSE::stl::utf16_to_utf8;
	using SKSE::stl::to_underlying;

	inline std::uintptr_t object_ptr(void* obj)
	{
		return reinterpret_cast<std::uintptr_t>(obj);
	}

	template <typename R, typename... Args>
	inline std::uintptr_t function_ptr(R(*fn)(Args...))
	{
		return reinterpret_cast<std::uintptr_t>(fn);
	}

	template <typename Class, typename R, typename... Args>
	inline std::uintptr_t function_ptr(R(Class::*fn)(Args...))
	{
		return reinterpret_cast<std::uintptr_t>((void*&)fn);
	}
}

#define DLLEXPORT __declspec(dllexport)

#include "Plugin.h"
