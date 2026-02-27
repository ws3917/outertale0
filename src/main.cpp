#include <SFML/Graphics.hpp>
int main() {
  constexpr float WIDTH = 320, HEIGHT = 240;
  constexpr int REAL_WIDTH = 1280, REAL_HEIGHT = 960;
  sf::RenderWindow window(sf::VideoMode({REAL_WIDTH, REAL_HEIGHT}), "",
                          sf::Style::Default, sf::State::Fullscreen);
  window.setFramerateLimit(60);
  sf::View view({WIDTH / 2, HEIGHT / 2}, {WIDTH, HEIGHT});
  view.setViewport(sf::FloatRect{{0, 0}, {1.0f, 1.0f}});
  window.setView(view);
  sf::RectangleShape rect;
  rect.setPosition({0, 0});
  rect.setFillColor(sf::Color::Red);
  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      } 
    }

    // 清空窗口
    window.clear();
    window.draw(rect);  // 绘制圆形
    window.display();   // 显示内容
  }
  return 0;
}