#include "error.hpp"

#include <glad/gl.h>

#include <print>


void report_errors() {
    auto const error_code { glGetError() };
    switch ( error_code ) {
    case GL_NO_ERROR:                                                                   break;
    case GL_INVALID_ENUM:                   std::println( "invalid enum" );             break;
    case GL_INVALID_VALUE:                  std::println( "invalid value" );            break;
    case GL_INVALID_OPERATION:              std::println( "invalid operation");         break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:  std::println( "invalid fb operation" );     break;
    case GL_OUT_OF_MEMORY:                  std::println( "out of memory" );            break;
    case GL_STACK_UNDERFLOW:                std::println( "stack underflow" );          break;
    case GL_STACK_OVERFLOW:                 std::println( "stack overflow" );           break;
    default:                                std::println( "Some other error");          break;
    }
}


