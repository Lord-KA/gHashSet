cmake_minimum_required(VERSION 3.14)
project(gHashSet)

set(CMAKE_CXX_STANDART 20)
set(CMAKE_CXX_STANDART_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "Comment" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -D NDEBUG" CACHE STRING "Comment" FORCE)
set(CMAKE_CXX_FLAGS_SANITIZER "${CMAKE_CXX_FLAGS} -Wpedantic -Wall -Wextra -Wformat=2 -fsanitize=address,undefined -g -D __SANITIZE_ADDRESS__ -D EXTRA_VERBOSE" CACHE STRING "Comment" FORCE)
set(CMAKE_CXX_FLAGS_FULL_DEBUG "${CMAKE_CXX_FLAGS} -D FULL_DEBUG -Wpedantic -Wall -Wextra -Wformat=2 -fsanitize=address,undefined -g" CACHE STRING "Comment" FORCE)
set(CMAKE_CXX_FLAGS_CHEAP_DEBUG "${CMAKE_CXX_FLAGS} -D CHEAP_DEBUG -Wpedantic -Wall -Wextra -Wformat=2 -fsanitize=address,undefined -g" CACHE STRING "Comment" FORCE)
set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS} -D NDEBUG -fprofile-instr-generate -fcoverage-mapping" CACHE STRING "Comment" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -D FULL_DEBUG -g -Werror -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++0x-compat -Wc++11-compat -Wc++14-compat -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlarger-than=8192 -Wlogical-op -Wmissing-declarations -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstack-usage=8192 -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -fcheck-new -fsized-deallocation -fstack-check -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -fPIE -fsanitize=address -fsanitize=alignment -fsanitize=bool -fsanitize=bounds -fsanitize=enum -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero -fsanitize=integer-divide-by-zero -fsanitize=leak -fsanitize=nonnull-attribute -fsanitize=null -fsanitize=object-size -fsanitize=return -fsanitize=returns-nonnull-attribute -fsanitize=shift -fsanitize=signed-integer-overflow -fsanitize=undefined -fsanitize=unreachable -fsanitize=vla-bound -fsanitize=vptr -lm -pie")

include(FetchContent)

FetchContent_Declare(
  gutils
  GIT_REPOSITORY https://github.com/lord-ka/gutils.git
  GIT_TAG        release-1.0.4
)

FetchContent_GetProperties(gutils)
if(NOT gutils_POPULATED)
  FetchContent_Populate(gutils)
  include_directories(${gutils_SOURCE_DIR})
endif()

FetchContent_Declare(
  gobjpool
  GIT_REPOSITORY https://github.com/Lord-KA/gObjPool.git
  GIT_TAG        release-1.0.2
)
if(NOT gobjpool_POPULATED)
  FetchContent_Populate(gobjpool)
  include_directories(${gobjpool_SOURCE_DIR})
endif()

FetchContent_Declare(
  glist
  GIT_REPOSITORY https://github.com/Lord-KA/gList.git
  # GIT_TAG        release-1.0.2
)
if(NOT glist_POPULATED)
  FetchContent_Populate(glist)
  include_directories(${glist_SOURCE_DIR})
endif()



add_executable(gHashSet test-ghashset.cpp ghashset.h)
