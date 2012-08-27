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

#include "spectrum.h"
#include "util.h"
#include <cmath>
#include <string>

#ifdef _OPENMP
#include <omp.h>
#endif


Spectrum::Spectrum(int size) {
    this->size = size;
    this->ft = new float[size * size * 2];
    for (int i = 0; i < size * size * 2; ++i)
        ft[i] = 0;
}

Spectrum& Spectrum::operator= (const Spectrum &s) {
    this->size = s.size;
    if (this->ft) delete this->ft;
    this->ft = new float[size * size * 2];
    memcpy(this->ft, s.ft, size * size * 2 * sizeof(float));
    return *this;
}

void Spectrum::PointSetSpectrum(Spectrum *spectrum, const PointSet &points,
                                const int npoints)
{
    const int size2 = spectrum->size / 2;
#ifdef _OPENMP
#pragma omp parallel
#endif
{
#ifdef _OPENMP
#pragma omp for schedule(static)
#endif
    for (int x = 0; x < spectrum->size; ++x) {
        for (int y = 0; y < spectrum->size; ++y) {
            float fx = 0.f, fy = 0.f;
            float wx = x - size2;
            float wy = y - size2;
            for (int i = 0; i < npoints; ++i) {
                float exp = -TWOPI * (wx * points[i].x + wy * points[i].y);
                fx += cosf(exp);
                fy += sinf(exp);
            }
            spectrum->ft[2*(x + y*spectrum->size)  ] = fx;
            spectrum->ft[2*(x + y*spectrum->size)+1] = fy;
        }
    }
}
}

