import java.util.Properties
import java.io.InputStream

plugins {
    id("com.android.application") version "9.0.1"
}

val keystoreProperties = Properties()
val keystoreFile = rootProject.file("keystore.properties")
if (keystoreFile.exists()) {
    keystoreFile.inputStream().use { keystoreProperties.load(it) }
}

android {
    namespace = "xyz.ws3917.game"
    compileSdk = 36
    ndkVersion = "29.0.14206865"

    defaultConfig {
        applicationId = "xyz.ws3917.yourbattle"
        minSdk = 21
        targetSdk = 36
        versionCode = 1
        versionName = "1.0"

        externalNativeBuild {
            cmake {
                val vcpkgRoot = System.getenv("VCPKG_ROOT")
                arguments(
                    "-DANDROID_STL=c++_shared",
                    "-DCMAKE_TOOLCHAIN_FILE=$vcpkgRoot/scripts/buildsystems/vcpkg.cmake"
                )
                abiFilters("arm64-v8a")
            }
        }
    }
    externalNativeBuild {
        cmake {
            version = "4.1.2"
            path = file("../CMakeLists.txt")
        }
    }
    signingConfigs {
        create("release") {
            if (keystoreProperties.containsKey("STORE_FILE")) {
                storeFile = file(keystoreProperties.getProperty("STORE_FILE"))
                storePassword = keystoreProperties.getProperty("STORE_PASSWORD")
                keyAlias = keystoreProperties.getProperty("KEY_ALIAS")
                keyPassword = keystoreProperties.getProperty("KEY_PASSWORD")
            }
        }
    }
    buildTypes {
        getByName("debug") {
            externalNativeBuild {
                cmake {
                    arguments("--preset", "android-arm64-debug")
                }
            }
        }
        release {
            isMinifyEnabled = true
            isShrinkResources = true
            signingConfig = signingConfigs.getByName("release")
            proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt"), "proguard-rules.pro")
            externalNativeBuild {
                cmake {
                    arguments("--preset", "android-arm64-release")
                }
            }
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }
    sourceSets {
        getByName("main") {
            jniLibs.directories.add("libs")
            assets.directories.add("../assets")
        }
    }
}

dependencies {
    implementation(fileTree("libs") { include("*.jar") })
}