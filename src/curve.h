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

#ifndef CURVE_H
#define CURVE_H

#include <iostream>
#include <string>
#include <vector>

class Curve
{
public:
    std::vector<float> y;
    float x0, x1, dx;
    
    Curve() { x0 = x1 = dx = 0.f; };
    Curve(const Curve &c);
    Curve(int size, float x0, float x1);
    
    float  operator[] (int i) const { return y[i]; }
    float& operator[] (int i) { return y[i]; }
    
    int size() const { return (int) y.size(); }
    int ToIndex(float x) const { return static_cast<int>((x - x0) / dx); }
    float ToX(int index) const { return x0 + index * dx; }

    void Accumulate(const Curve &c);
    void Divide(float f);
    void FilterGauss(float sigma);
    void SetZero();
    
    static Curve Load(const std::string &fname);
    void SaveTXT(const std::string &fname);
    void SaveTEX(const std::string &fname, std::string labels[2],
                 float yrange[2], float refLvl, float xscale);
};

#endif  // CURVE_H
