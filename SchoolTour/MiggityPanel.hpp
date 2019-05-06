//
//  MiggityPanel.hpp
//  SchoolTour
//
//  Created by apple on 2019/5/6.
//  Copyright © 2019 aiofwa. All rights reserved.
//

#ifndef MiggityPanel_hpp
#define MiggityPanel_hpp

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

class MiggityPanel {
public:
    void draw(sf::Shader *shader = nullptr);
    
    void resize();
    
    MiggityPanel(sf::RenderWindow *win, sf::Font *font);
    
    void update(float deltaTime, float mouseX, float mouseY);
    
    float max(float a, float b) {
        return a > b ? a : b;
    }
    
    bool event(sf::Event &e, float mouseX, float mouseY);
    
    sf::Text *text;
    sf::Font *font;
    
    void show(bool yep);
    
private:
    sf::RectangleShape panel;
    sf::RectangleShape shade;
    sf::RenderWindow *win;
    sf::Vector2f resolution;
    float transitionY;
    float target;
    float height;
    
    sf::RectangleShape ok;
    sf::Text okText;
    bool hoveringOK;
    bool showing;
};

#endif /* MiggityPanel_hpp */
