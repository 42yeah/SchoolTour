//
//  main.cpp
//  SchoolTour
//
//  Created by apple on 2019/4/9.
//  Copyright © 2019 aiofwa. All rights reserved.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <chrono>
#include <cmath>
#include "Runtime.hpp"


int main(int argc, const char * argv[]) {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 16;
    sf::RenderWindow win(sf::VideoMode(1600, 1200), "A Tour Of DGUT", sf::Style::Default, settings);
    
    sf::Font font;
    font.loadFromFile("Assets/Noto.otf");
    
    sf::Color textColor(90, 90, 90);
    sf::Color primaryColor(0, 127, 255);
    sf::Color secondaryColor(155, 230, 255);
    
    sf::Text title(L"东莞理工学院的地图", font, 30);
    title.setFillColor(textColor);
    
    // init runtime
    Runtime runtime;
    runtime.init();
    
    // delta time
    float delta = 0.0f;
    sf::Clock clock;

    sf::Vector2u winSize = win.getSize();
    sf::Vector2f offset = { 0.0f, 0.0f };
    sf::Vector2f destination = { 0.0f, 0.0f };
    sf::View cam(sf::FloatRect(offset.x, offset.y, winSize.x, winSize.y));
    win.setView(cam);
    
    // construct polygon
    Polygon polygon;
    polygon.color = sf::Color::Blue;
    polygon.shape.setFillColor(polygon.color);
    
    sf::View hud(sf::FloatRect(0.0f, 0.0f, winSize.x, winSize.y));
    
    // selection point
    Point *inRange = nullptr;
    Point *selected = nullptr;
    
    // acc
    int accumulator = 0;
    
    while (win.isOpen()) {
        delta = clock.getElapsedTime().asSeconds();
        clock.restart();
        
        sf::Event e;
        sf::Vector2i mousePos = sf::Mouse::getPosition(win);
        mousePos.x -= winSize.x / 2.0f - offset.x;
        mousePos.y -= winSize.y / 2.0f - offset.y;
        
        while (win.pollEvent(e)) {
            // Placement mode
            if (e.type == sf::Event::MouseButtonPressed) {
                if (!inRange) {
                    Point p = { static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) };
                    
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                        std::string str = "USER" + std::to_string(accumulator);
                        p.name = str;
                        accumulator++;
                        runtime.getPoints().push_back(p);
                    } else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                        polygon.shape.setPointCount(polygon.shape.getPointCount() + 1);
                        polygon.shape.setPoint(polygon.shape.getPointCount() - 1, { p.x, p.y });
                    }
                } else if (selected) {
                    Vector edge;
                    edge.start = *selected;
                    edge.end = *inRange;
                    runtime.getEdges().push_back(edge);
                    selected = nullptr;
                } else if (!selected) {
                    selected = inRange;
                }
            }

            if (e.type == sf::Event::Closed) {
                win.close();
            }
            
            if (e.type == sf::Event::Resized) {
                winSize = win.getSize();
                hud.reset(sf::FloatRect(0.0f, 0.0f, winSize.x, winSize.y));
                cam.reset(sf::FloatRect(0.0f, 0.0f, winSize.x, winSize.y));
            }
            
            if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::Key::Enter) {
                    if (polygon.shape.getPointCount() != 0) {
                        runtime.getPolygons().push_back(polygon);
                        polygon = Polygon();
                        polygon.color = sf::Color::Blue;
                        polygon.shape.setFillColor(polygon.color);
                    }
                    selected = nullptr;
                }
                if (e.key.code == sf::Keyboard::Key::P) {
                    sf::Clock now;
                    
                    std::string nonsense = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
                    bool ok = runtime.dump(nonsense);
                    if (!ok) {
                        std::cout << "Dumping " << nonsense.c_str() << " failed." << std::endl;
                    }
                }
            }
        }
        
        
        using sf::Keyboard;
        float fastDelta = delta * 1000.0f;
        if (sf::Keyboard::isKeyPressed(Keyboard::W)) {
            destination.y -= 1.0f * fastDelta;
        }
        if (sf::Keyboard::isKeyPressed(Keyboard::A)) {
            destination.x -= 1.0f * fastDelta;
        }
        if (sf::Keyboard::isKeyPressed(Keyboard::S)) {
            destination.y += 1.0f * fastDelta;
        }
        if (sf::Keyboard::isKeyPressed(Keyboard::D)) {
            destination.x += 1.0f * fastDelta;
        }
        
        win.clear(sf::Color(255, 255, 255));
        title.setPosition(20.0f, 20.0f);
        
        sf::Vector2f deltaPos = destination - offset;
        offset += deltaPos * delta * 10.0f;
        cam.setCenter(offset);
        win.setView(cam);
        
        std::vector<Polygon> &polygons = runtime.getPolygons();
        win.draw(polygon.shape);
        for (int i = 0; i < polygons.size(); i++) {
            sf::ConvexShape shape = polygons[i].shape;
            shape.setFillColor(polygons[i].color);
            win.draw(shape);
        }
        
        std::vector<Point> &points = runtime.getPoints();
        inRange = nullptr;
        for (int i = 0; i < points.size(); i++) {
            Point &point = points[i];
            sf::CircleShape circle(15.0f, 30);
            sf::CircleShape radiant(point.backRadius, 30);
            point.backRadius += (50.0f - point.backRadius) * delta * 1.0f;
            if (point.backRadius >= 45.0f) {
                point.backRadius = 0.0f;
            }
            
            circle.setPosition(point.x - 15.0f, point.y - 15.0f);
            radiant.setPosition(point.x - radiant.getRadius(), point.y - radiant.getRadius());
            
            Point deltaPos;
            deltaPos.x = mousePos.x - point.x;
            deltaPos.y = mousePos.y - point.y;
            float distance = sqrt(deltaPos.x * deltaPos.x + deltaPos.y * deltaPos.y);
            sf::Color primary = primaryColor;
            sf::Color secondary = secondaryColor;
            // in selection range!
            if (selected == &point) {
                primary = sf::Color(255, 77, 77);
                secondary = sf::Color(255, 154, 154);
            } else if (distance <= 15.0f) {
                inRange = &point;
                primary = sf::Color(255, 166, 77);
                secondary = sf::Color(255, 204, 154);
            }
            
            circle.setFillColor(primary);
            sf::Color newColor = primary;
            float colors[3] = {
                secondary.r / 255.0f,
                secondary.g / 255.0f,
                secondary.b / 255.0f
            };
            for (int i = 0; i < 3; i++) {
                colors[i] *= (0.9f + (radiant.getRadius() - 15.0f) / 30.0f);
                if (colors[i] >= 1.0f) {
                    colors[i] = 1.0f;
                }
            }
            newColor.r = colors[0] * 255.0f;
            newColor.g = colors[1] * 255.0f;
            newColor.b = colors[2] * 255.0f;
            newColor.a = (0.9f + (radiant.getRadius() - 15.0f) / 30.0f);
            radiant.setFillColor(newColor);
            win.draw(radiant);
            win.draw(circle);
            
            sf::Text desc(point.name, font, 25);
            desc.setPosition(point.x, point.y + 18.0f);
            desc.setFillColor(textColor);
            win.draw(desc);
        }
        
        std::vector<Vector> &edges = runtime.getEdges();
        sf::VertexArray lines(sf::Lines, (edges.size() * 2));
        for (int i = 0; i < edges.size(); i += 1) {
            lines[i * 2].position.x = edges[i].start.x;
            lines[i * 2].position.y = edges[i].start.y;
            lines[i * 2 + 1].position.x = edges[i].end.x;
            lines[i * 2 + 1].position.y = edges[i].end.y;
            lines[i * 2].color = primaryColor;
            lines[i * 2 + 1].color = primaryColor;
        }
        win.draw(lines);
        
        win.setView(hud);
        win.draw(title);
        
        win.display();
    }
    return 0;
}
