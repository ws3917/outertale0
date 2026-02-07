import java.util.Properties
import java.io.InputStream

plugins {
    id("com.android.application") version "9.0.0"
}

val keystoreProperties = Properties()
val keystoreFile = rootProject.file("keystore.properties")
if (keystoreFile.exists()) {
    keystoreFile.inputStream().use { keystoreProperties.load(it) }
}

android {
    namespace = "xyz.ws3917.game"
    compileSdk = 36
    ndkVersion = "28.2.13676358"

    defaultConfig {
        applicationId = "xyz.ws3917.outertale0"
        minSdk = 21
        targetSdk = 36
        versionCode = 1
        versionName = "1.0"

        externalNativeBuild {
            cmake {
                arguments("-DANDROID_PLATFORM=android-21", "-DANDROID_STL=c++_shared")
                abiFilters("arm64-v8a", "armeabi-v7a", "x86", "x86_64")
            }
        }
    }
    externalNativeBuild {
        cmake {
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
        release {
            isMinifyEnabled = true
            signingConfig = signingConfigs.getByName("release")
            proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt"), "proguard-rules.pro")
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }
    buildFeatures {
        prefab = true
    }
    sourceSets {
        getByName("main") {
            jniLibs.srcDirs("libs")
            assets.srcDirs("../assets")
        }
    }
}

dependencies {
    implementation(fileTree(mapOf("dir" to "libs", "include" to listOf("*.jar"))))
    implementation(files("libs/SDL3-3.4.0.aar"))
    implementation(files("libs/SDL3_mixer-3.1.2.aar"))
}