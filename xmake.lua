add_rules("mode.debug", "mode.release")

set_languages("c17")
set_warnings("all", "extra", "pedantic")

if is_mode("debug") then
    set_optimize("none")
    set_symbols("debug", "hidden")
    set_policy("build.sanitizer.address", true)
    set_policy("build.sanitizer.undefined", true)
else
    set_optimize("fastest")
    set_symbols("hidden")
    set_strip("all")
end

add_includedirs("include")

target("lsmat")
    set_kind("static")
    add_files("src/lsmat/*.c")
target_end()

target("lsmat_cli")
    set_kind("binary")
    add_files("src/cli/*.c")
    add_deps("lsmat")
target_end()
