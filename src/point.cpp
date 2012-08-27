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

#include "point.h"

#include "util.h"
#include <algorithm>
#include <fstream>
#include <stdint.h>


enum Endianness {
    BigEndian, LittleEndian
};

static Endianness SystemEndianness() {
    union { uint32_t i; char c[4]; } e;
    e.i = 1;
    return e.c[0] == 0 ? BigEndian : LittleEndian;
}

static void SwapEndian4(uint8_t *buf, int nn) {
    for (int i=0; i<nn; i++) {
        std::swap(buf[4*i  ], buf[4*i+3]);
        std::swap(buf[4*i+1], buf[4*i+2]);
    }
}

static bool ReadFloats(FILE *fp, float *p, int n, Endianness endian) {
    Endianness sysEndian = SystemEndianness();
    const int nbuf = 256;
    uint8_t buf[nbuf*sizeof(float)];
    while (n > 0) {
        int nn = std::min(n, nbuf);
        if (fread(buf, sizeof(float), nn, fp) != (size_t)nn)
            return false;
        if (endian != sysEndian)
            SwapEndian4(buf, nn);
        memcpy(p, buf, nn*sizeof(float));
        p += nn; n -= nn;
    }
    return true;
}

static bool WriteFloats(FILE *fp, const float *p, int n, Endianness endian) {
    Endianness sysEndian = SystemEndianness();
    const int nbuf = 256;
    uint8_t buf[nbuf*sizeof(float)];
    while (n > 0) {
        int nn = std::min(n, nbuf);
        memcpy(buf, p, nn * sizeof(float));
        if (endian != sysEndian)
            SwapEndian4(buf, nn);
        if (fwrite(buf, sizeof(float), nn, fp) != (size_t)nn)
            return false;
        p += nn; n -= nn;
    }
    return true;
}

static bool HasSuffix(const std::string &s, const std::string &suffix) {
    int i = s.size() - suffix.size();
    if (i >= 0)
        return s.substr(i) == suffix;
    return false;
}


void PointSet::RDF(Curve *rdf) const
{
    const int npoints = this->size();
    std::vector<unsigned long> bins(rdf->size(), 0);
    
    for (int i = 0; i < npoints; ++i) {
        for (int j = i + 1; j < npoints; ++j) {
            float dist = points[i].DistUnitTorus(points[j]);
            int idx = rdf->ToIndex(dist);
            if (0 <= idx && idx < rdf->size())
                bins[idx]++;
        }
    }
    
    const float scale = npoints * (npoints - 1)/2 * PI * rdf->dx * rdf->dx;
    for (int i = 0; i < rdf->size(); ++i)
        (*rdf)[i] = bins[i] / (scale * (2*i + 1));
}

PointSet PointSet::Load(const std::string &fname)
{
    PointSet set;
    unsigned int npoints = 0;

    if (HasSuffix(fname, ".txt")) {
        std::ifstream fp(fname.c_str());
        if (!fp) {
            std::cerr << "Cannot load '" << fname << "'.\n";
            exit(1);
        }
        fp >> npoints;
        set.points.reserve(npoints);
        Point p;
        while (set.points.size() < npoints && (fp >> p.x >> p.y))
            set.points.push_back(p);
        fp.close();
    } else if (HasSuffix(fname, ".rps")) {
        FILE *fp = fopen(fname.c_str(), "rb");
        if (!fp) {
            std::cerr << "Cannot load '" << fname << "'.\n";
            exit(1);
        }
        fseek(fp, 0, SEEK_END);
        npoints = ftell(fp) / (2 * sizeof(float));
        set.points.resize(npoints);
        fseek(fp, 0, SEEK_SET);
        ReadFloats(fp, &set.points[0].x, npoints*2, LittleEndian);
        fclose(fp);
    } else if (HasSuffix(fname, ".eps")) {
        std::ifstream fp(fname.c_str());
        if (!fp) {
            std::cerr << "Cannot load '" << fname << "'.\n";
            exit(1);
        }
        while (fp.good()) {
            std::string line;
            getline(fp, line);
            if (line.empty()) continue;
            if (line[0] == '%') continue;
            
            std::vector<std::string> tokens;
            size_t prev = 0, next = 0;
            while ((next = line.find_first_of(" ", prev)) != std::string::npos) {
                if (next - prev != 0)
                    tokens.push_back(line.substr(prev, next - prev));
                prev = next + 1;
            }
            if (prev < line.size())
                tokens.push_back(line.substr(prev));
            
            if (tokens.size() != 3) continue;
            if (tokens[2][0] != 'p') continue;
            
            Point p;
            for (int i = 0; i < 2; ++i) {
                std::istringstream iss(tokens[i]);
                iss >> p[i];
            }
            set.points.push_back(p);
        }
        fp.close();
    } else {
        std::cerr << "No .txt, .rps, or compatible .eps file '" << fname << "'.\n";
        exit(1);
    }
    
    return set;
}

void PointSet::Save(const std::string &fname)
{
    if (HasSuffix(fname, ".txt")) {
        std::ofstream os(fname.c_str());
        if (!os) {
            std::cerr << "Cannot create '" << fname << "'.\n";
            exit(1);
        }
        os << points.size() << "\n";
        for (unsigned int i = 0; i < points.size(); ++i)
            os << points[i] << "\n";
        os.close();
    } else if (HasSuffix(fname, ".rps")) {
        FILE *fp = fopen(fname.c_str(), "wb");
        if (!fp) {
            std::cerr << "Cannot create '" << fname << "'.\n";
            exit(1);
        }
        WriteFloats(fp, &points[0].x, points.size() * 2, LittleEndian);
        fclose(fp);
    } else if (HasSuffix(fname, ".eps")) {
        double radius = 3.0, scale = 512.0;
        const Point BB[2] = {
            Point(-radius, -radius), Point(scale + radius, scale + radius)
        };
        std::ofstream os(fname.c_str());
        if (!os) {
            std::cerr << "Cannot create '" << fname << "'.\n";
            exit(1);
        }
        os << "%!PS-Adobe-3.1 EPSF-3.0\n";
        os << "%%HiResBoundingBox: " << BB[0] << " " << BB[1] << "\n";
        os << "%%BoundingBox: " << BB[0] << " " << BB[1] << "\n";
        os << "%%CropBox: " << BB[0] << " " << BB[1] << "\n";
        os << "/radius { " << (radius / scale) << " } def\n";
        os << "/p { radius 0 360 arc closepath fill stroke } def\n";
        os << "gsave " << scale << " " << scale << " scale\n";
        os << "0 setgray\n";
        for (unsigned int i = 0; i < points.size(); ++i)
            os << points[i] << " p\n";
        os << "grestore\n";
        os.close();
    } else
        std::cerr << "Extension not supported for '" << fname << "'.\n";
}

