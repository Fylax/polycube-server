cmake_minimum_required(VERSION 3.12)
project(parser)

set(CMAKE_CXX_STANDARD 14)


add_executable(parser
        main.cpp Validators/Validator.h Validators/PatternValidator.cpp Validators/PatternValidator.h Validators/Validators.h Validators/Validators.cpp Validators/EnumValidator.cpp Validators/EnumValidator.h)
target_link_libraries(parser yang)
