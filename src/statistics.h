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

#ifndef STATISTICS_H
#define STATISTICS_H

#include "curve.h"
#include "point.h"
#include <cassert>

struct Statistics {
    float mindist;
    float avgmindist;
    float orientorder;
    float effnyquist;
    float oscillations;
    
    Statistics() : mindist(0), avgmindist(0), orientorder(0),
                   effnyquist(0), oscillations(0) {};
    
    inline void Divide(const float f) {
        assert(f != 0.f);
        float inv = 1.f / f;
        mindist *= inv;
        avgmindist *= inv;
        orientorder *= inv;
        effnyquist *= inv;
        oscillations *= inv;
    }
};

void SpatialStatistics(const PointSet &points, int npoints, Statistics *stats);
void SpectralStatistics(const PointSet &points, int npoints, Statistics *stats);
void SpectralStatistics(std::vector<PointSet> &sets, int npoints, Statistics *stats);

#endif // STATISTICS_H

