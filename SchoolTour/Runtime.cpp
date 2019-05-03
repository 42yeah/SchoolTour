//
//  Runtime.cpp
//  SchoolTour
//
//  Created by apple on 2019/4/16.
//  Copyright © 2019 aiofwa. All rights reserved.
//

#include "Runtime.hpp"
#include <iostream>
#include <fstream>

void Runtime::init() {
    points.clear();
//    Point newPoint = {
//        100.0f, 180.0f
//    };
//    newPoint.name = L"正门";
//    
//    Point another = {
//        300.0f, 300.0f
//    };
//    another.name = L"第一饭堂";
//    
//    points.push_back(newPoint);
//    points.push_back(another);
//    edges.push_back({ newPoint, another });
}

std::vector<Point> &Runtime::getPoints() {
    return points;
}

std::vector<Vector> &Runtime::getEdges() {
    return edges;
}

std::vector<Polygon> &Runtime::getPolygons() { 
    return polygons;
}

/**
 * dump dumps the whole runtime.
 */
bool Runtime::dump(std::string name) {
    std::ofstream writer(name.c_str());
    
    if (!writer.good()) {
        return false;
    }
    
    // Dump is done in three sectors. The first one is points, the second edges, and the third, polygons.
    // I am not saving it in binary format, as this will make me hard to modify the contents within.
    std::vector<Point> &points = getPoints();
    writer << points.size() << std::endl;
    for (int i = 0; i < points.size(); i++) {
        Point point = points[i];
        writer << "    " << point.name.toUtf8().c_str() << " " << point.x << " " << point.y << std::endl;
    }
    
    std::vector<Vector> &edges = getEdges();
    writer << edges.size() << std::endl;
    for (int i = 0; i < edges.size(); i++) {
        Vector edge = edges[i];
        writer << "    " << edge.start.x << " " << edge.start.y << " " << edge.end.x << " " << edge.end.y << std::endl;
    }
    
    std::vector<Polygon> &polygons = getPolygons();
    writer << polygons.size() << std::endl;
    for (int i = 0; i < polygons.size(); i++) {
        Polygon polygon = polygons[i];
        writer << "    " << polygon.color.toInteger() << " " << polygon.shape.getPointCount() << std::endl;
        for (int j = 0; j < polygon.shape.getPointCount(); j++) {
            sf::Vector2f point = polygon.shape.getPoint(j);
            writer << "        " << point.x << " " << point.y << std::endl;
        }
    }
    writer.flush();
    writer.close();
    
    return true;
}
