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

#include "curve.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
#include "util.h"

Curve::Curve(const Curve &c) {
    x0 = c.x0;
    x1 = c.x1;
    dx = c.dx;
    y = c.y;
}

Curve::Curve(int size, float x0, float x1) {
    this->x0 = x0;
    this->x1 = x1;
    dx = (x1 - x0) / size;
    y.resize(size, 0);
}

void Curve::Accumulate(const Curve &c) {
    assert(this->size() == c.size());
    for (int i = 0; i < size(); ++i)
        y[i] += c[i];
}

void Curve::Divide(float f) {
    assert(f != 0.f);
    const float inv = 1.f / f;
    for (int i = 0; i < size(); ++i)
        y[i] *= inv;
}

void Curve::FilterGauss(float sigma) {
    if (sigma <= 0)
        return;
    
    Curve tmp(*this);

    for (int i = 0; i < size(); ++i) {
        float a = 0.f, sumw = 0.f;
        
        int jmin = std::max(0, (int)floor(i - 5.f * sigma));
        int jmax = std::min(size() - 1, (int)ceil(i + 5.f * sigma));
        
        for (int j = jmin; j <= jmax; ++j) {
            float d = j - i;
            float w = exp(-d*d / (2 * sigma*sigma));
            a += tmp[j] * w;
            sumw += w;
        }
        y[i] = a / sumw;
    }
}

void Curve::SetZero() {
    for (int i = 0; i < size(); ++i)
        y[i] = 0.f;
}

Curve Curve::Load(const std::string &fname) {
    std::ifstream is(fname.c_str());
    std::vector<float> x, y;
    float xx, yy;
    while (is >> xx >> yy) {
        x.push_back(xx);
        y.push_back(yy);
    }
    if (x.empty()) {
        std::cerr << "Could not load RDF from '" << fname << "'.\n";
        exit(1);
    }
    
    int n = x.size();
    float dx = n > 1 ? x[1] - x[0] : 1;
    
    Curve c(n, x[0] - dx / 2.f, x[n-1] + dx / 2.f);
    for (int i = 0; i < n; ++i)
        c.y[i] = y[i];
    
    return c;
}

void Curve::SaveTXT(const std::string &fname) {
    std::ofstream os(fname.c_str());
    for (int i = 0; i < size(); ++i)
        os << ToX(i) << " " << y[i] << "\n";
}

void Curve::SaveTEX(const std::string &fname, std::string labels[2],
                    float yrange[2], float refLvl, float xscale)
{
    const float width  = 6.4f;
    const float height = 4.0f;
    const float ratio  = width / height;
    const float ticklen = 0.04;
    const int xsteps  = std::min(4, (int)((x1 - x0) * xscale));
    const float xstep = 1.f / xsteps;
    const int ysteps  = std::min(4, (int)(yrange[1] - yrange[0]));
    const float ystep = 1.f / ysteps;
    
    FILE *fp = fopen(fname.c_str(), "w");
    if (!fp) {
        std::cerr << "Cannot create '" << fname << "'.\n";
        exit(1);
    }
    
    fprintf(fp, "\\documentclass{standalone}\n");
    fprintf(fp, "\\usepackage{tikz}\n");
    fprintf(fp, "\\begin{document}\n");
    fprintf(fp, "\\begin{tikzpicture}[xscale=%f,yscale=%f]\n",
            width / ((x1 - x0) * xscale), height / (yrange[1] - yrange[0]));
    fprintf(fp, "  \\draw[help lines,dashed] (%f,%f) -- (%f,%f);\n",
            x0 * xscale, refLvl, x1 * xscale, refLvl);
    fprintf(fp, "  \\begin{scope}\n");
    fprintf(fp, "    \\clip (%f,%f) rectangle (%f,%f);\n",
            x0 * xscale, yrange[0], x1 * xscale, yrange[1]);
    fprintf(fp, "    \\draw\n");
    for (int i = 0; i < size(); ++i) {
        float cy = std::min(std::max(y[i], yrange[0]), yrange[1]);
        fprintf(fp, "(%f,%f)", ToX(i) * xscale, cy);
        if (i < size() - 1) fprintf(fp, " -- ");
        if ((i + 1) % 3 == 0) fprintf(fp, "\n");
    }
    fprintf(fp, ";\n");
    fprintf(fp, "  \\end{scope}\n");
    fprintf(fp, "  \\foreach \\x in {%.f", Round0f(x0 * xscale));
    for (int i = 1; i <= xsteps; ++i)
        fprintf(fp, ",%.1f", Round0f(x1 * xscale) * i * xstep);
    fprintf(fp, "}\n");
    fprintf(fp, "    \\draw (\\x cm,%f) -- (\\x cm,%f) node[below] {\\x};\n",
            yrange[0] + (yrange[1] - yrange[0]) * ticklen, yrange[0]);
    fprintf(fp, "  \\foreach \\y in {%.f", Round0f(yrange[0]));
    for (int i = 1; i <= ysteps; ++i)
        fprintf(fp, ",%.f", Round0f(yrange[0]) + (Round0f(yrange[1]) - Round0f(yrange[0])) * i * ystep);
    fprintf(fp, "}\n");
    fprintf(fp, "    \\draw (%f,\\y cm) -- (%f,\\y cm) node[left] {\\y};\n",
            x0 * xscale + ((x1 - x0) * xscale) * ticklen / ratio, x0 * xscale);
    fprintf(fp, "  \\draw (%f,%f) rectangle (%f,%f);\n",
            x0 * xscale, yrange[0], x1 * xscale, yrange[1]);
    fprintf(fp, "  \\node[below=0.4cm,text height=10pt,text depth=3pt] at (%f,%f) {%s};\n",
            (x0 + (x1 - x0) / 2.f) * xscale, yrange[0], labels[0].c_str());
    fprintf(fp, "  \\node[rotate=90,above=0.5cm,text height=10pt,text depth=3pt] at (%f,%f) {%s};\n",
            x0 * xscale, yrange[0] + (yrange[1] - yrange[0]) / 2.f, labels[1].c_str());
    fprintf(fp, "\\end{tikzpicture}\n");
    fprintf(fp, "\\end{document}\n");
    
    fclose(fp);
}

