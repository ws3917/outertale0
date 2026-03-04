package("libsdl3_mixer")
    set_homepage("https://github.com/libsdl-org/SDL_mixer")
    set_description("An audio mixer that supports various file formats for Simple Directmedia Layer.")
    set_license("zlib")

    add_urls("https://github.com/libsdl-org/SDL_mixer/releases/download/prerelease-$(version)/SDL3_mixer-$(version).zip", { alias = "archive" })
    add_urls("https://github.com/libsdl-org/SDL_mixer", { alias = "github" })

    add_deps("cmake")

    on_load(function (package)
        package:add("deps", "libsdl3", { configs = { shared = package:config("shared") }})
    end)

    on_install(function (package)
        local configs = {"-DSDLMIXER_SAMPLES=OFF", "-DSDLMIXER_TESTS=OFF", "-DSDLMIXER_VENDORED=OFF"}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)

    on_test(function (package)
        assert(package:has_cfuncs("MIX_Init", {includes = "SDL3_mixer/SDL_mixer.h"}))
    end)