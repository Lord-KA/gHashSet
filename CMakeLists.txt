cmake_minimum_required(VERSION 3.14)
project(gHashSet)

# set(CMAKE_CXX_COMPILER "/usr/bin/clang++" CACHE string "clang compiler" FORCE) #TODO check if clang is present when COVERAGE needed

set(CMAKE_CXX_STANDART 20)
set(CMAKE_CXX_STANDART_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -O2 -g -msse4.2" CACHE STRING "Comment" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -D NDEBUG -D NLOGS" CACHE STRING "Comment" FORCE)
set(CMAKE_CXX_FLAGS_SANITIZER "${CMAKE_CXX_FLAGS} -Wpedantic -Wall -Wextra -Wformat=2 -fsanitize=address,undefined -g -D __SANITIZE_ADDRESS__ -D EXTRA_VERBOSE" CACHE STRING "Comment" FORCE)
set(CMAKE_CXX_FLAGS_FULL_DEBUG "${CMAKE_CXX_FLAGS} -D FULL_DEBUG -Wpedantic -Wall -Wextra -Wformat=2 -fsanitize=address,undefined -g" CACHE STRING "Comment" FORCE)
set(CMAKE_CXX_FLAGS_CHEAP_DEBUG "${CMAKE_CXX_FLAGS} -D CHEAP_DEBUG -Wpedantic -Wall -Wextra -Wformat=2 -fsanitize=address,undefined -g" CACHE STRING "Comment" FORCE)
set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS} -D NDEBUG -fprofile-instr-generate -fcoverage-mapping" CACHE STRING "Comment" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -D FULL_DEBUG -g -Werror -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++0x-compat -Wc++11-compat -Wc++14-compat -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlarger-than=8192 -Wlogical-op -Wmissing-declarations -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstack-usage=8192 -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -fcheck-new -fsized-deallocation -fstack-check -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -fPIE -fsanitize=address -fsanitize=alignment -fsanitize=bool -fsanitize=bounds -fsanitize=enum -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero -fsanitize=integer-divide-by-zero -fsanitize=leak -fsanitize=nonnull-attribute -fsanitize=null -fsanitize=object-size -fsanitize=return -fsanitize=returns-nonnull-attribute -fsanitize=shift -fsanitize=signed-integer-overflow -fsanitize=undefined -fsanitize=unreachable -fsanitize=vla-bound -fsanitize=vptr -lm -pie")

# for benchmarking in Github Actions
set(CMAKE_CXX_FLAGS_ROL     "${CMAKE_CXX_FLAGS} -D NDEBUG -D NLOGS -D ROL"     CACHE STRING "Comment" FORCE)
set(CMAKE_CXX_FLAGS_ROL_ASM "${CMAKE_CXX_FLAGS} -D NDEBUG -D NLOGS -D ROL_ASM" CACHE STRING "Comment" FORCE)
set(CMAKE_CXX_FLAGS_CRC32   "${CMAKE_CXX_FLAGS} -D NDEBUG -D NLOGS -D CRC32"   CACHE STRING "Comment" FORCE)
set(CMAKE_CXX_FLAGS_CRC32_AVX "${CMAKE_CXX_FLAGS} -mavx512vl -mavx512f -D NDEBUG -D NLOGS -D CRC32 -D __AVX512__"   CACHE STRING "Comment" FORCE)


include(FetchContent)


FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.8.0
)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

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
  GIT_TAG        release-1.1.0
)
if(NOT gobjpool_POPULATED)
  FetchContent_Populate(gobjpool)
  include_directories(${gobjpool_SOURCE_DIR})
endif()

FetchContent_Declare(
  glist
  GIT_REPOSITORY https://github.com/Lord-KA/gList.git
  GIT_TAG        release-1.1.0
)
if(NOT glist_POPULATED)
  FetchContent_Populate(glist)
  include_directories(${glist_SOURCE_DIR})
endif()

set_target_properties(gtest PROPERTIES FOLDER extern)
set_target_properties(gtest_main PROPERTIES FOLDER extern)
set_target_properties(gmock PROPERTIES FOLDER extern)
set_target_properties(gmock_main PROPERTIES FOLDER extern)

enable_testing()

add_executable(gHashSet-test test-ghashset.cpp ghashset.h)
add_executable(hash-stat test-hashes.cpp ghashset.h)
add_executable(benchmark benchmark.cpp ghashset.h)

target_link_libraries(
    gHashSet-test
    gtest_main
)

include(GoogleTest)

message("                                                                                                                                                                   ")
message("                                                                                                                                                                 ")
message("                                                                                                  ------                                                         ")
message("                                                                                              -========@-                                                        ")
message("                                                                                      ---  --=@@@@@@@@@@@=                                                       ")
message("                                                                 ---------------------=-=--================                                                      ")
message("                                                ----------=--------===-----------------------=--=-==========--                                                   ")
message("                                            -----==----------------------------------------------======@@@@@=-=-                                                 ")
message("                                          ----------------------------------------------------=======-=====@@=-==--                                              ")
message("                                     ----------------------------------------------------------==@@@=======@@@======-                                            ")
message("                              -----=-------------------------=----===---------------------------=@@=========@@@@======-                                          ")
message("                        ------------------------------===========-========-----------------------=@@@@=@=====@@@=-======-                                        ")
message("                    ---==----------------------------=======----------======---------------------=@@@=@@-====@@@===--=====-                                      ")
message("                -----------------------------------========-=-----------=====--------------------=@@@@=============-=--====-                                     ")
message("             --===--------------------------------======@@@@@@@@@==@====-===---------------------==@@@=@@@@@==========---===-                                    ")
message("          --====-----------------------------------=====@@@@@@@@@@@@==-=====----------------------====@@@@@=@================-                                   ")
message("       -========-----------------------------------=================-=====------------------------=-========================-=-                                  ")
message("     -=========----------------=---------------------===-------=-========---------------------------------===================-==-                                ")
message("    =============----------------------------------------=========----------------------------------------=========--====--=====--                               ")
message("   ======@=@=====--------------------------------------------------------------------------------------------===---==-=====--====--                              ")
message("  -===============--=------------------------===----------------------------------------------------------------------======--==-==-                             ")
message("  -====@=========-====---------------------------===----------------------------------------------------------------=----=====--====-                            ")
message("  -===-==-=======---=------------------------------------------------------------------------------------------------------------====-                           ")
message("  =@@==@@=========--=----====-====--===---------------------------------------------------=-------------------------------=------=====-                          ")
message("  -=@@@@@@=====-==--==-====================================-----====---------------------------------------------------=---==-==-===@==-                         ")
message("   ==@@@@@=====-=======-==========================================-===---------------------------------------------------==--=====@@@@=-                         ")
message("    =@@@@=====================================================================--------=-----------------------------------=======@@@=--@@                        ")
message("    -=@@@========================================================================------===------------------------=----======@@@@@=- =@@@@-                      ")
message("    -==@@====================================--=====-=============================-------=----=----------------======-=====@@@@@=- -@@@@@@@-                     ")
message("      =@@====================-=========-=======---==----===========================--=-=-----------==----==-=--===-====@@@@@@==- -=@@@@@@@@@-                    ")
message("      -=@@===============----=--======-----=====================--============--==------------------===---====--=@==@@@@@@@=-  -=@@@@@@@@@@@@-                   ")
message("       -=======================================================================------------=====--===-===========@@@@@@@==-  -=@@@@@@@@@@@@@@@-                  ")
message("        --======================================================================-=----=============-===-======@@@@@@@===-  -@@@@@@@@@@@@@@@@@@@=-                ")
message("           --=============================================================================================@=@@@@@@===-- -=@@@@@@@@@@@@@@@@@@@@@@@=               ")
message("                --=============@@@@=@@@@@@@@=@@@@@@=======================================================@@@@@@===-  -=@@@@@@@@@@@@@@@@@@@@@@@@@@@-             ")
message("                   -========@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=@@@@=================================-=====@@@@@@===-  -=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=            ")
message("                      ---------------=====@@@@@@@@@@@@@@@@@@@@@@@@@@@@===============================@@@@@@===--  -=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@-          ")
message("                                                 -=--======@@@@@@@@@@@@@=@@=======================@@@@@@====-  -=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@-        ")
message("                                                            --=@@@@@@@@@@@@@===================@@@@@@@@==--  -=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@-      ")
message("                                                               =@@@@@@@@@@@@@===============@@@@@@@@===-   -=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@-    ")
message("                                                               -=@@@@@@@@@@@@@============@@@@@@@@==--   =@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=-  ")
message("                                                                -@@@@@@@@@@@@@===========@@@@@===--   -=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@- ")
message("                                                              ---@@@@@@@@@@@@@@=======@@@@@@==--    -=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@= ")
message("                                                            -========@@@@@@@==@@@@@@@@@@====-    -=@@@@@@@@@@=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@= ")
message("                                                         -=====--==========@@@@@@@@@@===--     -=@@@@@@@@@@@@=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@= ")
message("                                                     -==@======-=========@@@@@@@====---     -=@@@@@@@@@@@@@@=@=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@- ")
message("                                                 -==@@@================@@@@@@==----       -=@@@@@@@@@@@@@@@@@@=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  ")
message("                                               -=@@@=-===============@@@@@@=----        -=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=  ")
message("                                             =@@@@=  -@====-========@@@@@---         -=@@@@@@@@@@@@@@@@@@@@@@=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@-  ")
message("                                            =@@@-   -@====-========@@@@@-          -=@@@@@@@@@@@@@@@@@@@@@@@@=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@   ")
message("                                           -@@=     =@===-========@@@@=          -=@@@@@@@@@@@@@@@@@@@@@@@@@@=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@-   ")
message("                                          -@@=     -===---=======@@=-          -=@@@@@@@@@@@@@@@@@@@@@@@@@@@@=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=    ")
message("                                         =@@-  -=- ====-========@=           -=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=-      ")
message("                                       -@@=  ---===@========@@@@@ ---      -=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=-         ")
message("                                      -@@-  ---==@@==========@@@=------  -=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=-            ")
message("                                     -@@-   --===============@@@-     -==@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=-             ")
message("                                    =@@-   -================@@=      -=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=-               ")
message("                                   =@@-  -================@@@@    -=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=-                  ")
message("                                   @@@- -================@@@@-  -=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=-                     ")
message("                                  -@@@ -===============@@@@@= -=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=-                        ")
message("                                  -@@@ -==============@@@@@@==@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=-                           ")
message("                                  -@@@ -=============@=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@==-                              ")
message("                                  -@@@  ==============@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@==--                                 ")
message("                                   @@@- ==============@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=@@@@@@@@@@@@@@@@@@@@@@@@@===--                                     ")
message("                                   =@@= -============@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=================@@@@@@@@@@@@@@=--                                           ")
message("                                   -@@= ============@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=================@=@@@@@@@@@@=-                                               ")
message("                                    @@=-@=========@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@========@@======@@@@@@@@@@==-                                                  ")
message("                                    -@@@=========@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=@@@@=--                                                     ")
message("                                     =@@=======@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@==-                                                         ")
message("                                      -@@====@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@==-                                                             ")
message("                                        @===@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@======--                                                                 ")
message("                                         =@@=@@@@@@@@@@@@@@@@@@@@@@@@@==@@@@@@@===---                                                                            ")
message("                                          ===@@@==================---   -----                                                                                    ")
message("                                                                                                                                                                 ")
message("                                                                                                                                                                   ")
message("\t\t\t\t\t\t\t  ----------------------------------")
message("\t\t\t\t\t\t\t  |  Thx for checking out my code! |")
message("\t\t\t\t\t\t\t  ----------------------------------")
