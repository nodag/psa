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

#include "statistics.h"
#include "util.h"
#ifdef PSA_HAS_CGAL
#include "delaunay.h"
#endif


static float Integrate(const Curve &c, float x0, float x1) {
    bool negate = false;
    if (x0 > x1) {
        std::swap(x0, x1);
        negate = true;
    }
    
    int i0 = c.ToIndex(x0);
    int i1 = c.ToIndex(x1 + c.dx);
    i0 = std::min(std::max(i0, 0), c.size() - 1);
    i1 = std::min(std::max(i1, 0), c.size() - 1);
    
    float T = 0.5 * (c[i0] + c[i1]);
    float M = 0;
    for (int i = i0+2; i < i1; i += 2) {
        T += c[i];
        M += c[i-1];
    }
    float a = c.dx * (T + M);
    return negate ? -a : a;
}

static inline float Integrate(const Curve &c) {
    return Integrate(c, c.x0, c.x1);
}

static inline float RingArea(float x0, float x1) {
    return PI * (x1*x1 - x0*x0);
}

static float EffectiveNyquist(const Curve &rp, int npoints) {
    Curve cumpower(rp);
    for (int i = 0; i < cumpower.size(); ++i)
        cumpower[i] *= cumpower.dx * 2 * cumpower.ToX(i);
    for (int i = 1; i < cumpower.size(); ++i)
        cumpower[i] += cumpower[i-1];
    for (int i = 1; i < cumpower.size(); ++i) {
        float f = cumpower.ToX(i);
        cumpower[i] /= f * f;
    }
    
    const float thresh1 = 0.1f;
    int i0 = cumpower.ToIndex(sqrtf(npoints) / 2);
    for (int i = i0; i < cumpower.size(); ++i)
        if (cumpower[i] > 0.5f)
            for (int j = i; j > 0; j--)
                if (cumpower[j] < thresh1)
                    return cumpower.ToX(j) / 2;
    return 0;
}

static float OscillationsMetric(const Curve &rp, int npoints) {
    // nuosci: lowest frequency v for which P(v) ~ 1
    float nuosci = 0.0, maxp = 0.0;
    for (int i = 0; i < rp.size() && maxp < 0.98; ++i) {
        float p = rp[i];
        if (p > maxp) {
            maxp = p;
            nuosci = rp.ToX(i);
        }
    }
    
    float npeaks = 10.f;
    float maxfreq = sqrtf(npoints) / 2;
    float x0 = nuosci;
    float x1 = std::min(x0 + npeaks * maxfreq, rp.x1);
    
    Curve osci(rp);
    for (int i = 0; i < osci.size(); ++i) {
        float nu = osci.ToX(i);
        osci[i] = nu < x0 ? 0.f : (osci[i] - 1.f) * (osci[i] - 1.f) * nu;
    }
    
    return 10.f * sqrtf(TWOPI * Integrate(osci, x0, x1) / RingArea(x0, x1));
}

static inline float BlackmanWindow(float x, float xlim) {
    return (x > xlim) ? 0.f : 0.43f + 0.5f * cosf(PI*x / xlim) + 0.08f * cosf(TWOPI*x / xlim);
}

static void RDFtoRP(const Curve &rdf, int npoints, Curve *rp) {
    const float wstep = 1.f / sqrtf(npoints);
    Curve tmp(rdf);
    for (int i = 0; i < rp->size(); ++i) {
        const float u0 = rp->ToX(i);
        const float u = TWOPI * u0;
        const float wndsize = rdf.x1 * std::min(0.5f, std::max(0.2f, 4.f * u0 * wstep));
        for (int j = 0; j < tmp.size(); ++j) {
            float x = rdf.ToX(j);
            float wnd = BlackmanWindow(x, wndsize);
            tmp[j] = (rdf[j] - 1) * j0f(u*x) * x * wnd;
        }
        (*rp)[i] = fabsf(1.f + TWOPI * Integrate(tmp) * npoints);
    }
}

#ifndef PSA_HAS_CGAL
static void Distances(const PointSet &points, int npoints,
                      float *mindist, float *avgmindist)
{
    *mindist = FLT_MAX;
    *avgmindist = 0;
    for (int i = 0; i < npoints; ++i) {
        float localmd = FLT_MAX;
        for (int j = 0; j < npoints; ++j) {
            if (i == j) continue;
            float dist = points[i].SquaredDistUnitTorus(points[j]);
            localmd = std::min(dist, localmd);
        }
        *mindist = std::min(localmd, *mindist);
        *avgmindist += sqrtf(localmd);
    }
    *mindist = sqrtf(*mindist);
    *avgmindist /= points.size();
}
#endif


void SpatialStatistics(const PointSet &points, int npoints, Statistics *stats) {
#ifdef PSA_HAS_CGAL
    std::vector<Point_2> cgalPoints(npoints);
    for (int i = 0; i < npoints; ++i)
        cgalPoints[i] = Point_2(points[i].x, points[i].y);
    Delaunay dt(cgalPoints, true);
    dt.GetStatistics(stats);
#else
    Distances(points, npoints, &stats->mindist, &stats->avgmindist);
    stats->orientorder = 0;
#endif
}

void SpectralStatistics(const PointSet &points, int npoints, Statistics *stats) {
    std::vector<PointSet> set(1);
    set[0] = points;
    SpectralStatistics(set, npoints, stats);
}

void SpectralStatistics(std::vector<PointSet> &sets, int npoints, Statistics *stats) {
    // We need the full radial power spectrum, so for performance reasons, we
    // derive the radial power spectrum directly from full RDFs here
    const int nsets = (int) sets.size();
    const int nbins = 100 * sqrtf(npoints);
    Curve avgrp(nbins, 0, 0.5f * npoints);
    Curve rdf(nbins, 0, 0.5f);
    Curve rp(nbins, 0, 0.5f * npoints);
    
    if (nsets > 1) PrintProgress("Stats", 0);
    for (int i = 0; i < nsets; ++i) {
        rdf.SetZero();
        sets[i].RDF(&rdf);
        
        rp.SetZero();
        RDFtoRP(rdf, npoints, &rp);
        avgrp.Accumulate(rp);
        
        if (nsets > 1) PrintProgress("Stats", (i+1) / (float) nsets);
    }
    avgrp.Divide(sets.size());
    if (nsets > 1) std::cout << std::endl;
    
    stats->effnyquist = EffectiveNyquist(avgrp, npoints);
    stats->oscillations = OscillationsMetric(avgrp, npoints);
}

