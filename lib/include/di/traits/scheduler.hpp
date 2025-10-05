#ifndef INCLUDE_DI_TRAITS_SCHEDULER_HPP
#define INCLUDE_DI_TRAITS_SCHEDULER_HPP

#include "di/empty_types.hpp"
#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <exception>
#include <string>
#endif

#include "di/traits/scheduler.hxx"

namespace di {

DI_MODULE_EXPORT
struct TerminateSchedulerThreadException : std::exception
{
    explicit TerminateSchedulerThreadException(std::string msg) : msg(std::move(msg)) {}
    char const* what() const noexcept override { return msg.c_str(); }

private:
    std::string msg;
};

DI_MODULE_EXPORT
struct StopSchedulerException : std::exception
{
    explicit StopSchedulerException(std::string msg) : msg(std::move(msg)) {}
    char const* what() const noexcept override { return msg.c_str(); }

private:
    std::string msg;
};

} // namespace di

#endif // INCLUDE_DI_TRAITS_SCHEDULER_HPP
