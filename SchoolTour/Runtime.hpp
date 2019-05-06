//
//  Runtime.hpp
//  SchoolTour
//
//  Created by apple on 2019/4/16.
//  Copyright © 2019 aiofwa. All rights reserved.
//

#ifndef Runtime_hpp
#define Runtime_hpp

#include <vector>
#include <string>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>


struct Point {
    float x, y;
    float backRadius;
    sf::String name;
    Point *prev;
};

struct Vector {
    Point start;
    Point end;
    bool selected;
};

struct Polygon {
    sf::ConvexShape shape;
    sf::Color color;
};

class Runtime {
public:
    void init();
    
    std::vector<Point> &getPoints();
    std::vector<Vector> &getEdges();
    std::vector<Polygon> &getPolygons();
    std::vector<Vector *> getNeighboringEdges(Point p);
    std::vector<Point *> getNeighbors(Point p);
    bool dump(std::string name);
    bool load(std::string name);
    bool pathfind(Point *from, Point *to);
    
    std::vector<Vector> route;
    
private:
    std::vector<Point> points;
    std::vector<Vector> edges;
    std::vector<Polygon> polygons;
};

#endif /* Runtime_hpp */

// trash code
// sf::RectangleShape miggityPanel(sf::Vector2f{ static_cast<float>(resolution.x), max(100.0f, resolution.y / 3.0f) });
// miggityPanel.setPosition(0.0f, resolution.y - resolution.y / 3.0f);
