#pragma once

#include <format>
#include <print>


/// Checks whether OpenGL has reported any errors, and prints a message to the console if so.
void report_errors();

template <typename... Args> void print_error( std::format_string<Args...> string, Args &&... args ) {
    std::println( "\e[91m[ERROR]\e[0m {}", std::format( string, args... ) );
}
