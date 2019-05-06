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

bool Runtime::load(std::string path) {
    std::ifstream reader(path.c_str());
    if (!reader.good()) {
        return false;
    }
    
    char in[512];
    
    int numPoints;
    reader >> numPoints;
    for (int i = 0; i < numPoints; i++) {
        float x, y;
        reader >> in >> x >> y;
        sf::String name = sf::String::fromUtf8(&in[0], in + strlen(in));
        Point newPoint{x, y, 0.0f, name};
        getPoints().push_back(newPoint);
    }
    
    int numEdges;
    reader >> numEdges;
    for (int i = 0; i < numEdges; i++) {
        float fromX, fromY, toX, toY;
        reader >> fromX >> fromY >> toX >> toY;
        Point start{fromX, fromY, 0.0f, ""};
        Point end{toX, toY, 0.0f, ""};
        Vector edge{start, end};
        getEdges().push_back(edge);
    }
    
    int numPolygons;
    reader >> numPolygons;
    for (int i = 0; i < numPolygons; i++) {
        uint32_t hexaColor;
        int numPoints;
        reader >> hexaColor >> numPoints;
        sf::Color color(hexaColor);
        Polygon polygon;
        polygon.color = color;
        polygon.shape.setFillColor(color);
        polygon.shape.setPointCount(numPoints);
        for (int j = 0; j < numPoints; j++) {
            float x, y;
            reader >> x >> y;
            polygon.shape.setPoint(j, { x, y });
        }
        getPolygons().push_back(polygon);
    }
    
    return true;
}

std::vector<Vector *> Runtime::getNeighboringEdges(Point p) {
    std::vector<Vector *> neighbors;
    for (int i = 0; i < getEdges().size(); i++) {
        Vector &edge = getEdges()[i];
        if ((edge.start.x == p.x && edge.start.y == p.y) ||
            (edge.end.x == p.x && edge.end.y == p.y)) {
            neighbors.push_back(&edge);
        }
    }
    return neighbors;
}

std::vector<Point *> Runtime::getNeighbors(Point p) {
    std::vector<Vector *> neighboringEdges = getNeighboringEdges(p);
    std::vector<Point *> neighbors;
    for (int i = 0; i < neighboringEdges.size(); i++) {
        Vector *edge = neighboringEdges[i];
        bool pIsStart = (p.x == edge->start.x && p.y == edge->start.y);
        
        if (pIsStart && (p.x == edge->end.x && p.y == edge->end.y)) {
            // what the fuck, a self pointing edge!
            continue;
        }
        for (int j = 0; j < getPoints().size(); j++) {
            Point &p1 = getPoints()[j];
            if (!pIsStart && p1.x == edge->start.x && p1.y == edge->start.y) {
                neighbors.push_back(&p1);
            } else if (pIsStart && p1.x == edge->end.x && p1.y == edge->end.y) {
                neighbors.push_back(&p1);
            }
        }
    }
    return neighbors;
}

bool Runtime::pathfind(Point *from, Point *to) {
    std::vector<Point *> frontier = getNeighbors(*from);
    std::vector<Point *> explored;
    Point *now = from;
    now->prev = nullptr;
    for (int i = 0; i < frontier.size(); i++) {
        frontier[i]->prev = now;
    }
    
    bool found = false;
    
    while (!frontier.empty()) {
        Point *incoming = frontier.front();
        explored.push_back(now);
        now = incoming;
        frontier.erase(frontier.begin(), frontier.begin() + 1);
        
        if (now == to) {
            found = true;
            break;
        }
        // otherwise, get its neighbors
        std::vector<Point *> neighbors = getNeighbors(*now);
        
        for (int i = 0; i < neighbors.size(); i++) {
            Point *neighbor = neighbors[i];
            bool hasExplored = false;
            for (int j = 0; j < explored.size(); j++) {
                if (neighbor->x == explored[j]->x &&
                    neighbor->y == explored[j]->y) {
                    // explored
                    hasExplored = true;
                    break;
                }
            }
            
            if (!hasExplored) {
                neighbor->prev = now;
                frontier.push_back(neighbor);
            }
        }
    }
    
    if (!found) {
        return false;
    }
    
    // found, now color the roads
    while (to != from) {
        std::vector<Vector *> edges = getNeighboringEdges(*to);
        Point *prev = to->prev;

        Vector fact;
        // make sure dest is dest and src is src
        fact.start = *prev;
        fact.end = *to;
        route.push_back(fact);
        to = prev;
    }
    
    return true;
}

