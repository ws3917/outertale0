set_project("spacetime0")
set_version("1.0.0")

-- 语言
set_languages("cxx20")
add_rules("mode.debug", "mode.release")
add_requireconfs("*", {configs = {shared = true}})

add_requires("physfs", "yyjson", "libcurl", "libsdl3 3.4.0")
add_repositories("myrepo local-repo")
add_requires("libsdl3_mixer 3.1.2")

includes("plugin/gamejolt-api")

target("spacetime0")
    -- 生成文件类型
    if is_plat("android") then
        set_kind("shared")
        add_syslinks("android", "log")
        set_basename("main")
    else
        set_kind("binary")
    end

    -- 源码
    add_files("src/**.cpp")
    if is_plat("windows") then
        add_files("app/res.rc")
    end

    -- 3. 链接依赖
    add_packages("physfs", "yyjson", "libsdl3", "libsdl3_mixer")
    add_deps("gjapi")

    -- windows 设置
    if is_plat("windows") then
        add_cxflags("/utf-8", "/MP", {platforms = "windows"})
    end

    -- 性能/体积优化
    if is_mode("release") then
        set_optimize("fastest")           -- /O2 或 -O2
        set_symbols("hidden")             -- 剥离符号
        set_policy("build.optimization.lto", true) -- 开启 IPO/LTO

        -- 删除符号
        if is_plat("windows") then
            add_cxflags("/Gy", "/Gw")
        else
            add_cxflags("-ffunction-sections", "-fdata-sections")
            add_ldflags("-Wl,--gc-sections")
        end
    end

    -- 打包
    if not is_plat("android") then
        after_build(function (target)
            import("utils.archive")
            local assets_dir = path.join(os.projectdir(), "assets")
            local output_zip = path.join(target:targetdir(), "main.zip")
            local output_pak = path.join(target:targetdir(), "main.pak")
            archive.archive(output_zip, assets_dir)
            os.mv(output_zip, output_pak)
        end)

        set_installdir("$(projectdir)/install")
        after_install(function (target)
            local pak_path = path.join(target:targetdir(), "main.pak")
            local install_path = path.join(target:installdir(), "bin")
            os.cp(pak_path, install_path)
        end)
    end