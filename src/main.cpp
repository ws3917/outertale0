#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <optional>

int main() {
    // 1. 初始化窗口 (SFML 3.0 推荐使用 sf::VideoMode)
    sf::RenderWindow window(sf::VideoMode({800, 600}), "SFML 3.0 Modern C++ Demo");
    window.setFramerateLimit(60);

    // 2. 加载图像 (创建一个绿色的圆球代替外部贴图，方便直接运行)
    sf::CircleShape player(50.f);
    player.setFillColor(sf::Color::Green);
    player.setOrigin({50.f, 50.f}); // 设置中心点
    player.setPosition({400.f, 300.f});

    // 3. 加载声音 (注意：你需要准备一个 sound.wav 文件)
    sf::SoundBuffer buffer;
    sf::Sound bounceSound(buffer);
    if (buffer.loadFromFile("jump.wav")) {
        std::cout << "Sound loaded successfully!\n";
    }

    // 4. 游戏主循环
    while (window.isOpen()) {
        // --- A. 事件处理 (SFML 3.0 的新写法：使用 std::optional) ---
        while (const std::optional event = window.pollEvent()) {
            // 检查是否点击了关闭按钮
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            
            // 检查按键点击
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Space) {
                    bounceSound.play(); // 按空格响一声
                }
            }
        }

        // --- B. 逻辑更新 (简单的平滑移动) ---
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) player.move({0.f, -5.f});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) player.move({0.f, 5.f});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) player.move({-5.f, 0.f});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) player.move({5.f, 0.f});

        // --- C. 渲染绘制 ---
        window.clear(sf::Color(30, 30, 30)); // 深灰色背景
        
        window.draw(player); // 画玩家
        
        window.display();
    }

    return 0;
}