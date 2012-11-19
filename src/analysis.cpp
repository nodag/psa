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

#include "analysis.h"
#include "periodogram.h"
#include "result.h"
#include "spectrum.h"
#include "util.h"


static int MinNumPoints(std::vector<std::string> &files) {
    PointSet first = PointSet::Load(files[0]);
    int npoints = first.size();
    bool differing = false;
    for (unsigned int i = 1; i < files.size(); ++i) {
        PointSet points = PointSet::Load(files[i]);
        if (points.size() != npoints)
            differing = true;
        npoints = std::min(npoints, points.size());
    }
    if (differing)
        printf("Analyzing only the first %d points from each file\n", npoints);
    return npoints;
}

static void AnalyzeParams(ParamList &params, bool *ft, bool *summary) {
    bool other = !params.GetString("convert").empty() ||
        params.GetBool("spatial") || params.GetBool("spectral") ||
        params.GetBool("stats") || params.GetBool("rp") ||
        params.GetBool("rdf") || params.GetBool("ani") ||
        params.GetBool("pspectrum");
    *summary = params.GetBool("summary") || !other;
    *ft = params.GetBool("rp") || params.GetBool("ani") ||
          params.GetBool("pspectrum") || *summary;
}


void Analysis(std::vector<std::string> &files, ParamList &params,
              Config &config)
{
    // Configure variables
    bool ft, summary;
    AnalyzeParams(params, &ft, &summary);
    
    Result r;
    Periodogram p;
    
    // Process files
    for (unsigned int i = 0; i < files.size(); ++i) {
        r.points = PointSet::Load(files[i]);
        
        const int npoints = r.points.size();
        const float fnorm = 2.f / sqrtf(npoints);
        const float rnorm = 1.f / sqrtf(2.f / (SQRT3 * npoints));
        const int ftsize  = config.frange / fnorm;
        
        r.npoints = npoints;
        r.nsets = 1;
        
        // Fourier transform if necessary
        if (ft) {
            Spectrum s(ftsize * 2);
            Spectrum::PointSetSpectrum(&s, r.points, npoints);
            p = Periodogram(s);
            p.Divide(npoints);
        }
        
        // Process params
        if (params.GetBool("spatial") || params.GetBool("stats") || summary)
            SpatialStatistics(r.points, npoints, &r.stats);
        if (params.GetBool("spectral") || params.GetBool("stats") || summary)
            SpectralStatistics(r.points, npoints, &r.stats);
        if (params.GetBool("rp") || summary) {
            int nbins = ftsize * config.fbinsize;
            r.rp = Curve(nbins, 0, ftsize);
            p.RadialPower(&r.rp);
        }
        if (params.GetBool("rdf") || summary) {
            float maxdist = config.rrange / rnorm;
            int nbins = config.rbinsize * npoints;
            r.rdf = Curve(nbins, 0, maxdist);
            r.points.RDF(&r.rdf);
        }
        if (params.GetBool("ani") || summary) {
            int nbins = ftsize * config.fbinsize;
            r.ani = Curve(nbins, 0, ftsize);
            p.Anisotropy(&r.ani);
        }
        if (params.GetBool("pspectrum") || summary) {
            r.spectrum = Image(ftsize * 2, ftsize * 2);
            p.ToImage(&r.spectrum);
            r.spectrum.ToneMap(true);
        }

        // Output
        std::string base = BaseName(files[i], true);
        if (summary)
            SaveSummary(base+".pdf", r, config);
        else
            WriteResult(base, r, config, params);
    }
}

void AnalysisAverage(std::vector<std::string> &files, ParamList &params,
                     Config &config)
{
    // Configure variables
    bool ft, summary;
    AnalyzeParams(params, &ft, &summary);

    const int npoints = MinNumPoints(files);
    const float fnorm = 2.f / sqrtf(npoints);
    const float rnorm = 1.f / sqrtf(2.f / (SQRT3 * npoints));
    const int ftsize = config.frange / fnorm;
    const float maxdist = config.rrange / rnorm;
    
    Result r;
    Periodogram p(ftsize * 2);
    
    int nbins = config.rbinsize * npoints;
    r.rdf = Curve(nbins, 0, maxdist);
    r.npoints = npoints;
    r.nsets = files.size();
    
    // Process files
    if (ft) PrintProgress("FT", 0);
    for (unsigned int i = 0; i < files.size(); ++i) {
        PointSet points = PointSet::Load(files[i]);
        if (i == 0)
            r.points = points;
        
        // Fourier transform if necessary
        if (ft) {
            Spectrum s(ftsize * 2);
            Spectrum::PointSetSpectrum(&s, points, npoints);
            p.Accumulate(Periodogram(s));
        }
        
        // Accumulate other measures if necessary
        if (params.GetBool("spatial") || params.GetBool("stats") || summary) {
            Statistics stats;
            SpatialStatistics(points, npoints, &stats);
            r.stats.mindist += stats.mindist;
            r.stats.avgmindist += stats.avgmindist;
            r.stats.orientorder += stats.orientorder;
        }
        if (params.GetBool("rdf") || summary) {
            Curve rdf = r.rdf;
            rdf.SetZero();
            points.RDF(&rdf);
            r.rdf.Accumulate(rdf);
        }
        
        if (ft) PrintProgress("FT", (i+1) / (float) files.size());
    }
    if (ft) std::cout << std::endl;
    
    // Finish
    r.stats.Divide(files.size());
    p.Divide(npoints * files.size());
    r.rdf.Divide(files.size());
    
    // Process params
    if (params.GetBool("spectral") || params.GetBool("stats") || summary) {
        std::vector<PointSet> sets(files.size());
        for (unsigned int i = 0; i < files.size(); ++i)
            sets[i] = PointSet::Load(files[i]);
        SpectralStatistics(sets, npoints, &r.stats);
    }
    if (params.GetBool("rp") || summary) {
        int nbins = ftsize * config.fbinsize;
        r.rp = Curve(nbins, 0, ftsize);
        p.RadialPower(&r.rp);
    }
    if (params.GetBool("ani") || summary) {
        int nbins = ftsize * config.fbinsize;
        r.ani = Curve(nbins, 0, ftsize);
        p.Anisotropy(&r.ani);
    }
    if (params.GetBool("pspectrum") || summary) {
        r.spectrum = Image(ftsize * 2, ftsize * 2);
        p.ToImage(&r.spectrum);
        r.spectrum.ToneMap(true);
    }

    // Output
    std::string base = "avg";
    if (summary)
        SaveSummary(base+".pdf", r, config);
    else
        WriteResult(base, r, config, params);
}

