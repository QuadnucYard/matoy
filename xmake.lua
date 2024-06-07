add_rules("mode.debug", "mode.release")

toolchain("llvm-mingw")
    set_kind("standalone")
    set_toolset("cxx", "clang")
    set_toolset("ld", "clang++")
    set_sdkdir("D:/Program Files/llvm-mingw")
    set_bindir("D:/Program Files/llvm-mingw/bin")
toolchain_end()

set_config("target_os", "windows")
set_toolchains("llvm-mingw")

set_languages("c++latest")
add_cxxflags("-Wall", "-Wextra")
set_defaultmode("debug")

add_requires("magic_enum")

target("matoy-syntax")
    set_kind("static")
    add_includedirs("src")
    add_files("src/matoy/syntax/*.cpp")

target("matoy")
    set_kind("binary")
    add_deps("matoy-syntax")
    add_includedirs("src")
    add_files("src/*.cpp")
    add_packages("magic_enum")
