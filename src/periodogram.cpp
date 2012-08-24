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

#include "periodogram.h"
#include "util.h"
#include <cassert>
#include <string>


Periodogram::Periodogram(int size) {
    this->size = size;
    this->periodogram = new float[size * size];
    for (int i = 0; i < size * size; ++i)
        periodogram[i] = 0;
}

Periodogram& Periodogram::operator= (const Periodogram &p) {
    this->size = p.size;
    this->periodogram = new float[size * size];
    memcpy(this->periodogram, p.periodogram, size * size * sizeof(float));
    return *this;
}

Periodogram::Periodogram(const Spectrum &s) {
    this->size = s.size;
    this->periodogram = new float[size * size];
    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            const float &u = s.ft[2*(x + y*size)  ];
            const float &v = s.ft[2*(x + y*size)+1];
            periodogram[x + y*size] = u*u + v*v;
        }
    }
}

void Periodogram::Accumulate(const Periodogram &p) {
    assert(this->size == p.size);
    for (int i = 0; i < size * size; ++i)
        this->periodogram[i] += p.periodogram[i];
}

void Periodogram::Divide(const float f) {
    assert(f > 0.f);
    float inv = 1.f / f;
    for (int i = 0; i < size * size; ++i)
        periodogram[i] *= inv;
}

void Periodogram::Anisotropy(Curve *ani) const {
    // Determine radial power curve first, using the same parameters as 'ani'
    Curve rp = *ani;
    this->RadialPower(&rp);
    // Now determine anisotropy curve
    const int size2 = size / 2;
    std::vector<unsigned long> Nr(ani->size());
    ani->SetZero();
    // Measure variance within each ring
    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            int cx = abs(x - size2);
            int cy = abs(y - size2);
            float r = sqrtf(cx*cx + cy*cy);
            int i = ani->ToIndex(r);
            assert(i < ani->size());
            (*ani)[i] += (periodogram[x + y*size] - rp[i]) *
                         (periodogram[x + y*size] - rp[i]);
            Nr[i]++;
        }
    }
    // Normalize and convert to decibel
    for (int i = 0; i < ani->size(); ++i) {
        if (Nr[i] > 1)
            (*ani)[i] /= Nr[i] - 1;
        float sqpow = rp[i] * rp[i];
        (*ani)[i] = (sqpow > 0) ? (*ani)[i] / sqpow : 1;
        (*ani)[i] = Decibel((*ani)[i]);
    }
}

void Periodogram::RadialPower(Curve *rp) const {
    const int size2 = size / 2;
    std::vector<unsigned long> Nr(rp->size());
    rp->SetZero();
    // Add each power component to the corresponding ring
    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            int cx = abs(x - size2);
            int cy = abs(y - size2);
            float r = sqrtf(cx*cx + cy*cy);
            int i = rp->ToIndex(r);
            assert(i < rp->size());
            (*rp)[i] += periodogram[x + y*size];
            Nr[i]++;
        }
    }
    // Normalize
    for (int i = 0; i < rp->size(); ++i)
        if (Nr[i] > 0)
            (*rp)[i] /= Nr[i];
}

void Periodogram::ToImage(Image *img) const {
    assert(img->width == size && img->height == size);
    for (int x = 0; x < size; ++x)
        for (int y = 0; y < size; ++y)
            img->SetPixel(x, y, periodogram[x + y*size]);
}

