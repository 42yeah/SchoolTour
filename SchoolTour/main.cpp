//
//  main.cpp
//  SchoolTour
//
//  Created by apple on 2019/4/9.
//  Copyright © 2019 aiofwa. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <chrono>
#include <cmath>
#include "Runtime.hpp"
#include "MiggityPanel.hpp"
#define MAPVER "Maps/1557145975236"

int main(int argc, const char * argv[]) {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 16;
    sf::RenderWindow win(sf::VideoMode(1600, 1200), "A Tour Of DGUT", sf::Style::Default, settings);
    bool dev = false;
    
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
    
    float rotate = 0.0f;
    float destRotate = 0.0f;
    sf::View cam(sf::FloatRect(offset.x, offset.y, winSize.x, winSize.y));
    cam.setRotation(rotate);
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
    
    std::cout << "Welcome to this pretty SchoolTour, made by Hal in DGUT, and build version [" << __DATE__ << " " << __TIME__ << "]" << std::endl;
    
    sf::Shader vignette, glass;
    sf::Texture fbo;
    sf::Vector2u resolution = { win.getSize().x, win.getSize().y };
    fbo.create(resolution.x, resolution.y);
    bool shaderable = true;
    if ((shaderable = sf::Shader::isAvailable())) {
        vignette.loadFromFile("Assets/vignette.glsl", sf::Shader::Fragment);
        glass.loadFromFile("Assets/glass.glsl", sf::Shader::Fragment);
    }
    
    sf::Clock sinceEpoch;
    MiggityPanel panel(&win, &font);
    
    sf::Text miggity(L"", font, 30);
    miggity.setFillColor(textColor);
    panel.text = &miggity;
    
    std::string activeMap = MAPVER;
    std::ifstream configReader("Assets/shooltour.config");
    char in[128] = { 0 };
    if (configReader.good()) {
        configReader >> in;
        activeMap = "Maps/" + std::string(in);
    } else {
        sf::String err;
        err = L"找不到配置文件。默认加载 ";
        err += std::string(MAPVER);
        err += L"。\n如果你想加载特定份的地图，创建 Assets/schooltour.config，\n并写入地图名。";
        panel.text->setString(err);
        panel.show(true);
    }
    
    runtime.load(activeMap);
    
    while (win.isOpen()) {
        resolution = { win.getSize().x, win.getSize().y };
        if (shaderable) {
            vignette.setUniform("resolution", sf::Glsl::Vec2{ static_cast<float>(resolution.x), static_cast<float>(resolution.y) });
            vignette.setUniform("time", sinceEpoch.getElapsedTime().asSeconds());
        }
        delta = clock.getElapsedTime().asSeconds();
        clock.restart();
        
        sf::Event e;
        sf::Vector2i mousePos = sf::Mouse::getPosition(win);
        sf::Vector2i absoluteMousePos = mousePos;
        mousePos.x -= winSize.x / 2.0f - offset.x;
        mousePos.y -= winSize.y / 2.0f - offset.y;
        
        panel.update(delta, absoluteMousePos.x, absoluteMousePos.y);
        
        while (win.pollEvent(e)) {
            // Placement mode
            if (e.type == sf::Event::MouseButtonPressed && !panel.event(e, absoluteMousePos.x, absoluteMousePos.y)) {
                if (!inRange) {
                    if (dev) {
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
                    } else {
                        panel.show(false);
                        selected = nullptr;
                        runtime.route.clear();
                    }
                } else if (selected) {
                    if (dev) {
                        Vector edge;
                        edge.start = *selected;
                        edge.end = *inRange;
                        runtime.getEdges().push_back(edge);
                        selected = nullptr;
                    } else {
                        // GPS
                        destination = { (float) mousePos.x, (float) mousePos.y };
                        bool result = runtime.pathfind(selected, inRange);
                        std::cout << "Pathfinding result: " << result << std::endl;
                        sf::String info;
                        if (!result) {
                            info = L"导航不成功。\n从 " + selected->name + L" 到 " + inRange->name + L"\n找不到有效的路。";
                        } else {
                            info = L"导航\n从 " + selected->name + L" 到 " + inRange->name + L"\n经历 " + std::to_string(runtime.route.size() + 1) + L" 个节点。";
                        }
                        panel.text->setString(info);
                        panel.show(true);
                        selected = nullptr;
                    }
                } else if (!selected) {
                    destination = { (float) mousePos.x, (float) mousePos.y };
                    selected = inRange;
                    runtime.route.clear();
                    // TODO: miggity panel
                    sf::String info = selected->name + L"\n选择另外一个地点，然后就可以进行导航了。";
                    if (dev) {
                        info += L"\n点位于 (" + sf::String(std::to_string(selected->x)) + ", " + sf::String(std::to_string(selected->y)) + ")";
                    }
                    panel.text->setString(info);
                    panel.show(true);
                }
            }

            if (e.type == sf::Event::Closed) {
                win.close();
            }
            
            if (e.type == sf::Event::Resized) {
                winSize = win.getSize();
                hud.reset(sf::FloatRect(0.0f, 0.0f, winSize.x, winSize.y));
                cam.reset(sf::FloatRect(0.0f, 0.0f, winSize.x, winSize.y));
                fbo.create(winSize.x, winSize.y);
                
                panel.resize();
            }
            
            if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::Key::T) {
                    dev = !dev;
                    if (dev) {
                        panel.text->setString(L"你正在进入编辑模式。\n在编辑模式里头，左键可以增添新的节点。\n右键可以画出新的多边形（地形）。按下回车来增添。\nP 可以把改动保存到文件里，R 可以重载地图。\n注意: 编辑模式只能生成几何信息。其他的信息要到文档里编辑。");
                    } else {
                        panel.text->setString(L"你正在退出编辑模式。");
                    }
                    panel.show(true);
                }
                
                if (e.key.code == sf::Keyboard::Key::Enter) {
                    if (dev && polygon.shape.getPointCount() != 0) {
                        runtime.getPolygons().push_back(polygon);
                        polygon = Polygon();
                        polygon.color = sf::Color::Blue;
                        polygon.shape.setFillColor(polygon.color);
                    }
                    
                    selected = nullptr;
                }
                if (dev) {
                    if (e.key.code == sf::Keyboard::Key::P) {
                        sf::Clock now;
                        
                        std::string nonsense = "Maps/" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
                        bool ok = runtime.dump(nonsense);
                        sf::String msg;
                        if (!ok) {
                            std::cout << "Dumping " << nonsense.c_str() << " failed." << std::endl;
                            msg = L"保存\n文件保存失败。";
                        } else {
                            msg = L"保存\n文件保存为 " + sf::String(nonsense.c_str());
                        }
                        panel.text->setString(msg);
                        panel.show(true);
                    }
                    if (e.key.code == sf::Keyboard::Key::R) {
                        runtime.getPoints().clear();
                        runtime.getEdges().clear();
                        runtime.getPolygons().clear();
                        bool ok = runtime.load(activeMap);
                        if (!ok) {
                            std::cout << "Loading " << " failed." << std::endl;
                        }
                        panel.text->setString(L"加载\n文件已经重载。");
                        panel.show(true);
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
        if (sf::Keyboard::isKeyPressed(Keyboard::Q)) {
            destRotate += 0.5f * fastDelta;
        }
        if (sf::Keyboard::isKeyPressed(Keyboard::E)) {
            destRotate -= 0.5f * fastDelta;
        }
        
        win.clear(sf::Color(255, 255, 255));
        title.setPosition(20.0f, 20.0f);
        
        sf::Vector2f deltaPos = destination - offset;
        offset += deltaPos * delta * 10.0f;
        float deltaDeg = destRotate - rotate;
        rotate += deltaDeg * delta * 10.0f;
//        cam.setRotation(rotate); nah, not gonna do it 
        cam.setCenter(offset);
        win.setView(cam);
        
        std::vector<Polygon> &polygons = runtime.getPolygons();
        win.draw(polygon.shape);
        for (int i = 0; i < polygons.size(); i++) {
            sf::ConvexShape shape = polygons[i].shape;
            shape.setFillColor(polygons[i].color);
            win.draw(shape);
        }
        
        for (int i = 0; i < runtime.route.size(); i++) {
            Vector &line = runtime.route[i];
            sf::CircleShape dot(10.0f, 30);
            dot.setFillColor(sf::Color(255, 166, 77));
            
            // calculate the dist
            float dist = sqrt(pow(line.end.x - line.start.x, 2) + pow(line.end.y - line.start.y, 2));
            float dy = line.end.y - line.start.y;
            float dx = line.end.x - line.start.x;
            // and how much dots does it take
            int dots = dist / 30.0f;
            // and for this much dots,
            for (int j = 0; j < dots; j++) {
                // we draw them.
                float percent = ((float) j / dots) + sinceEpoch.getElapsedTime().asSeconds() * 0.1f;
                while (percent > 1.0f) { percent -= 1.0f; }
                dot.setPosition(line.start.x + dx * percent - 10.0f, line.start.y + dy * percent - 10.0f);
                win.draw(dot);
            }
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
                
                std::vector<Vector *> edges = runtime.getNeighboringEdges(point);
                for (int j = 0; j < edges.size(); j++) {
                    edges[j]->selected = true;
                }
            }
            
            circle.setFillColor(primary);
            sf::Color newColor = primary;
            newColor.a = (((45.0f - radiant.getRadius()) / 45.0f)) * 255.0f;
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
        for (int i = 0; i < edges.size(); i++) {
            lines[i * 2].position.x = edges[i].start.x;
            lines[i * 2].position.y = edges[i].start.y;
            lines[i * 2 + 1].position.x = edges[i].end.x;
            lines[i * 2 + 1].position.y = edges[i].end.y;
            sf::Color lineColor = primaryColor;
            if (edges[i].selected) { lineColor = sf::Color(255, 77, 77); }
            lines[i * 2].color = lineColor;
            lines[i * 2 + 1].color = lineColor;
            edges[i].selected = false;
        }
        win.draw(lines);
        
        win.setView(hud);
        
        if (shaderable) {
            fbo.update(win);
            sf::Sprite sprite(fbo);
            win.clear();
            win.draw(sprite, &vignette);
            title.setFillColor(sf::Color::White);
            
            fbo.update(win);
            glass.setUniform("back", fbo);
        }
        
        panel.draw(&glass);
        win.draw(title);
        
        win.display();
    }
    return 0;
}
