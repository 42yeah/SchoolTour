//
//  MiggityPanel.cpp
//  SchoolTour
//
//  Created by apple on 2019/5/6.
//  Copyright © 2019 aiofwa. All rights reserved.
//

#include "MiggityPanel.hpp"
#include <iostream>

MiggityPanel::MiggityPanel(sf::RenderWindow *win, sf::Font *font) {
    this->win = win;
    this->font = font;
    resolution = sf::Vector2f{ static_cast<float>(win->getSize().x), static_cast<float>(win->getSize().y) };
    panel = sf::RectangleShape(sf::Vector2f{ resolution.x, max(300.0f, resolution.y / 3.0f) });
    panel.setPosition(0.0f, resolution.y);
    shade = sf::RectangleShape(sf::Vector2f{ resolution.x, 10 });
    shade.setPosition(0.0f, panel.getPosition().y - 10.0f);
    
    ok = sf::RectangleShape(sf::Vector2f{ 100.0f, 60.0f });
    ok.setFillColor(sf::Color(255, 165, 0));
    okText = sf::Text(L"好", *font, 30);
    okText.setFillColor(sf::Color::White);
    
    shade.setFillColor(sf::Color(242, 242, 242));
    transitionY = resolution.y;
    showing = false;
    text = nullptr;
}

void MiggityPanel::draw(sf::Shader *shader) {
    if (!shader) {
        panel.setPosition(0.0f, transitionY);
        shade.setPosition(0.0f, panel.getPosition().y - 10.0f);
        if (text) {
            text->setPosition(50.0f, panel.getPosition().y + 30.0f);
        }
        ok.setPosition(50.0f, target + height - 100.0f);
        okText.setPosition(ok.getPosition().x + 30.0f, ok.getPosition().y + 18.0f);
        
        win->draw(shade);
        win->draw(panel);
        win->draw(ok);
        win->draw(okText);
        if (text) { win->draw(*text); }
    } else {
        sf::RenderTexture texture;
        texture.create(resolution.x, resolution.y);
        texture.clear();
        
        shade.setPosition(shade.getPosition().x, 0.0f);
        panel.setPosition(panel.getPosition().x, 10.0f);
        ok.setPosition(50.0f, height - 100.0f);
        okText.setPosition(ok.getPosition().x + 30.0f, ok.getPosition().y + 10.0f);
        
        texture.draw(shade);
        texture.draw(panel);
        if (text) {
            text->setPosition(50.0f, panel.getPosition().y + 30.0f);
            texture.draw(*text);
        }
        texture.draw(ok);
        texture.draw(okText);
        texture.display();
        
        sf::Sprite sprite(texture.getTexture());
        sprite.setPosition(0.0f, transitionY);
        shader->setUniform("yOffset", (target) / resolution.y);
        win->draw(sprite, shader);
    }
}

void MiggityPanel::resize() {
    resolution = sf::Vector2f{ static_cast<float>(win->getSize().x), static_cast<float>(win->getSize().y) };
    update(0.0f, 0.0f, 0.0f);
    panel.setSize(sf::Vector2f{ resolution.x, height });
//    panel.setPosition(0.0f, resolution.y - max(300.0f, resolution.y / 3.0f));
    shade.setSize(sf::Vector2f{ resolution.x, 10 });
    shade.setPosition(0.0f, panel.getPosition().y - 10.0f);
}

void MiggityPanel::update(float deltaTime, float mouseX, float mouseY) {
    height = max(300.0f, resolution.y / 3.0f);
    target = showing ? resolution.y - height : resolution.y + 10.0f;
    float deltaPos = target - transitionY;
    transitionY += deltaPos * deltaTime * 5.0f;
    
    mouseY -= target;
    sf::Rect<float> buttonCollisionArea;
    ok.setPosition(50.0f, height - 100.0f);
    buttonCollisionArea.left = ok.getPosition().x;
    buttonCollisionArea.width = ok.getSize().x;
    buttonCollisionArea.top = ok.getPosition().y;
    buttonCollisionArea.height = ok.getSize().y;
    ok.setFillColor(sf::Color(255, 165, 0));
    hoveringOK = false;
    if (!(mouseX < buttonCollisionArea.left ||
          buttonCollisionArea.left + buttonCollisionArea.width < mouseX ||
          mouseY < buttonCollisionArea.top ||
          buttonCollisionArea.top + buttonCollisionArea.height < mouseY
          )) {
        ok.setFillColor(sf::Color(179, 116, 0));
        hoveringOK = true;
    }
}

bool MiggityPanel::event(sf::Event &e, float mouseX, float mouseY) { 
    // check mouse collision
    // we dont care X though. So easy!
    if ((mouseY < target ||
         mouseY > target + height)) {
        return false;
    }
    
    mouseY -= target;
    if (hoveringOK) {
        showing = false;
    }
    
    return true;
}

void MiggityPanel::show(bool yep) { 
    showing = yep;
    if (yep == true) {
        transitionY = resolution.y;
    }
}


