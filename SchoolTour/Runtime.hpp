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
};

struct Vector {
    Point start;
    Point end;
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
    bool dump(std::string name);
    
private:
    std::vector<Point> points;
    std::vector<Vector> edges;
    std::vector<Polygon> polygons;
};

#endif /* Runtime_hpp */
