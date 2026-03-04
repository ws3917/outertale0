import java.util.Properties
import java.io.InputStream

plugins {
    id("com.android.application") version "9.0.1"
    id("org.tboox.gradle-xmake-plugin") version "1.2.3"
}

val keystoreProperties = Properties()
val keystoreFile = rootProject.file("keystore.properties")
if (keystoreFile.exists()) {
    keystoreFile.inputStream().use { keystoreProperties.load(it) }
}

android {
    namespace = "xyz.ws3917.spacey.spacetime0"
    compileSdk = 36
    ndkVersion = "29.0.14206865"

    defaultConfig {
        applicationId = android.namespace
        minSdk = 21
        targetSdk = 36
        versionCode = 1
        versionName = "1.0"

        xmake {
            path = "../xmake.lua"
            abiFilters.add("arm64-v8a")
//            arguments("-v", "-D") 
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
            isShrinkResources = true
            signingConfig = signingConfigs.getByName("release")
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
            assets.directories.add("../assets")
        }
    }
}

dependencies {
    implementation(fileTree("libs") { include("*.jar") })
}