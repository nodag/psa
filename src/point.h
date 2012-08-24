/**
 * This file is part of the point set analysis tool psa
 *
 * Copyright 2012
 * Thomas Schlömer, thomas.schloemer@uni-konstanz.de
 * Daniel Heck, daniel.heck@uni-konstanz.de
 *
 * psa is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef POINT_H
#define POINT_H

#include "curve.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>


struct Point
{
    // Data
    union {
        struct { float x, y; };
        struct { float e[2]; };
    };
    
    // Constructors
    Point() {};
    Point(float f) { x = y = f; }
    Point(float x, float y) { this->x = x; this->y = y; }
    Point(const float *f) { x = f[0]; y = f[1]; }
    
    // Operators
    float  operator[] (int i) const { return e[i]; }
    float& operator[] (int i) { return e[i]; }
    const Point& operator+ () const { return *this; }
    Point  operator- () const {
        Point p(*this); p.x = -p.x; p.y = -p.y; return p;
    }
    Point& operator= (const Point &p) {
        x = p.x; y = p.y; return *this;
    }
    Point& operator+= (const Point &p) {
        *this = *this + p; return *this;
    }
    Point& operator-= (const Point &p) {
        *this = *this - p; return *this;
    }
    Point& operator*= (float f) { *this = *this * f; return *this; }
    Point& operator/= (float f) { *this = *this / f; return *this; }
    
    // Friend operators
    friend bool operator== (const Point &p1, const Point &p2) {
        if (p1.x != p2.x) return false;
        if (p1.y != p2.y) return false;
        return true;
    }
    friend bool operator!= (const Point &p1, const Point &p2) {
        return !(p1 == p2);
    }
    friend Point operator* (float f, const Point &p) {
        Point r(p);
        r.x *= f;
        r.y *= f;
        return r;
    }
    friend Point operator* (const Point &p, float f) {
        return f * p;
    }
    friend Point operator/ (const Point &p, float f) {
        float f_ = 1.f / f;
        return f_ * p;
    }
    friend Point operator+ (const Point &p1, const Point &p2) {
        Point r;
        r.x = p1.x + p2.x;
        r.y = p1.y + p2.y;
        return r;
    }
    friend Point operator- (const Point &p1, const Point &p2) {
        Point r;
        r.x = p1.x - p2.x;
        r.y = p1.y - p2.y;
        return r;
    }
    friend std::istream& operator>> (std::istream &is, Point &p) {
        for (int i = 0; i < 2; ++i) {
            std::string s;
            is >> s;
            std::replace(s.begin(), s.end(), '(', ' ');
            std::replace(s.begin(), s.end(), ')', ' ');
            std::stringstream ss;
            ss << s;
            ss >> p.e[i];
        }
        return is;
    }
    friend std::ostream& operator<< (std::ostream &os, const Point &p) {
        os << p.x << " " << p.y;
        return os;
    }
    
    // Functions
    inline bool IsInUnitTorus() const {
        return (0.f <= x && x < 1.f) && (0.f <= y && y < 1.f);
    }
    inline void WrapUnitTorus() {
        x = (x <  0.f) ? x + 1.f : (x >= 1.f) ? x - 1.f : x;
        y = (y <  0.f) ? y + 1.f : (y >= 1.f) ? y - 1.f : y;
    }
    inline float SquaredDistUnitTorus(const Point &p) const {
        float x = fabsf(this->x - p.x);
        float y = fabsf(this->y - p.y);
        x = (x > .5f) ? 1.f - x : x;
        y = (y > .5f) ? 1.f - y : y;
        return x*x + y*y;
    }
    inline float DistUnitTorus(const Point &p) const {
        return sqrtf(SquaredDistUnitTorus(p));
    }
};


class PointSet
{
public:
    std::vector<Point> points;
    
    PointSet() {};
    PointSet(const PointSet &input) { points = input.points; };
    
    Point  operator[] (int i) const { return points[i]; }
    Point& operator[] (int i) { return points[i]; }
    
    int size() const { return (int) points.size(); }
    void RDF(Curve *rdf) const;
    
    static PointSet Load(const std::string &fname);
    void Save(const std::string &fname);
    void SaveEPS(const std::string &fname);
};

#endif    // POINT_H

