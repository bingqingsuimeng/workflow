set_project("workflow")
set_version("0.10.5")

option("workflow_inc",  {description = "workflow inc", default = "$(projectdir)/_include"})
option("workflow_lib",  {description = "workflow lib", default = "$(projectdir)/_lib"})
option("kafka",         {description = "build kafka component", default = false})
option("consul",        {description = "build consul component", default = true})
option("mysql",         {description = "build mysql component", default = true})
option("redis",         {description = "build redis component", default = true})
option("upstream",      {description = "build upstream component", default = true})

add_rules("mode.release", "mode.debug")
set_languages("c90", "c++11")
set_warnings("all")
set_exceptions("no-cxx")

add_syslinks("pthread")

if has_config("kafka") then
    add_requires("snappy", "lz4", "zstd", "zlib")
end

add_includedirs(get_config("workflow_inc"))
add_includedirs(path.join(get_config("workflow_inc"), "workflow"))

set_config("buildir", "build.xmake")

add_cflags("-fPIC", "-pipe")
add_cxxflags("-fPIC", "-pipe", "-Wno-invalid-offsetof")

includes("src", "test", "benchmark", "tutorial")

