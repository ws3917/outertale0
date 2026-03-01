import java.util.Properties
import java.io.InputStream

plugins {
    id("com.android.application") version "9.0.1"
}


android {
    namespace = "xyz.ws3917.game"
    compileSdk = 36
    ndkVersion = "29.0.14206865"

    defaultConfig {
        applicationId = "xyz.ws3917.game"
        minSdk = 24
        targetSdk = 36
        versionCode = 1
        versionName = "1.0"

        externalNativeBuild {
            cmake {
                arguments(
                    "-DANDROID_STL=c++_shared",
                    "-DCMAKE_TOOLCHAIN_FILE=${project.rootDir}/vcpkg_android_wrapper.cmake"
                )
                abiFilters("arm64-v8a", "x86_64")
            }
        }
    }
    externalNativeBuild {
        cmake {
            version = "4.1.2"
            path = file("../CMakeLists.txt")
        }
    }
    buildTypes {
        release {
            isMinifyEnabled = true
            isShrinkResources = true
            proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt"), "proguard-rules.pro")
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }
    sourceSets {
        getByName("main") {
            jniLibs.directories.add("libs")
            assets.directories.add("../app/main.pak")
        }
    }
}

dependencies {
    implementation(fileTree("libs") { include("*.jar") })
}

tasks.register("clean", Delete::class) {
    delete(rootProject.layout.buildDirectory)
}
