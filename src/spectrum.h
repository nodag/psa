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

#ifndef SPECTRUM_H
#define SPECTRUM_H

#include "point.h"

class Spectrum
{
public:
    float *ft;
    int size;
    
    Spectrum() { ft = NULL; size = 0; }
    Spectrum(int size);
    Spectrum& operator= (const Spectrum &s);
    ~Spectrum() { if (ft) delete[] ft; }

    static void PointSetSpectrum(Spectrum *spectrum, const PointSet &points,
                                 const int npoints);
};

#endif  // SPECTRUM_H

