$input = "icon.png"

$sizes = @{
    "mdpi"    = 48
    "hdpi"    = 72
    "xhdpi"   = 96
    "xxhdpi"  = 144
    "xxxhdpi" = 192
}

foreach ($key in $sizes.Keys) {
    $size = $sizes[$key]
    $output = "src/main/res/mipmap-$key/ic_launcher.png"
    new-item -type directory "src/main/res/mipmap-$key"

    magick $input `
        -filter point `
        -resize ${size}x${size} `
        $output
}
