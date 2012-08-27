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

#ifdef PSA_HAS_CGAL

#include "delaunay.h"
#include <complex>
#include <fstream>


static inline bool IsInBox(const Point_2 &p, const Point_2 box[2]) {
    return (box[0].x() <= p.x() && p.x() <= box[1].x()) &&
           (box[0].y() <= p.y() && p.y() <= box[1].y());
}

static inline bool IsInUnitTorus(const Point_2 &p) {
    return (0.0 <= p.x() && p.x() < 1.0) && (0.0 <= p.y() && p.y() < 1.0);
}

Delaunay::Delaunay(const std::vector<Point_2> &points, bool clip_heuristic) {
    // Excludes replications that don't influence the boundary regions
    if (clip_heuristic) {
        const double e = 4.0 / sqrt(points.size());
        clip[0] = Point_2(-e, -e);
        clip[1] = Point_2(1.0 + e, 1.0 + e);
    } else {
        clip[0] = Point_2(-1, -1);
        clip[1] = Point_2(2, 2);
    }
    // Add points one-by-one to DT
    sites.resize(points.size());
    for (int i = 0; i < (int) points.size(); ++i)
        SetVertex(i, points[i]);
}

Delaunay::~Delaunay()
{
    
}

void Delaunay::SetVertex(int i, const Point_2 &point) {
    assert(i < (int) sites.size());
    // Insert original vertex
    VH v = dt.insert(point);
    sites[i].vertex = v;
    // Insert replications
    for (int u = -1; u <= 1; ++u) {
        for (int v = -1; v <= 1; ++v) {
            if (u == 0 && v == 0) continue;
            
            Point_2 p(point.x() + u, point.y() + v);
            if (!IsInBox(p, clip)) continue;
            
            // Locate face where replicate would be inserted
            DT::Locate_type lt;
            int li;
            FH loc = dt.locate(p, lt, li);
            if (lt == DT::VERTEX) continue;
            
            // Insert replicate
            VH r = dt.insert(p, lt, loc, li);
            sites[i].replications[sites[i].nreplications] = r;
            sites[i].nreplications++;
        }
    }
}

void Delaunay::ClearVertex(int i) {
    // Remove replications
    for (int r = 0; r < sites[i].nreplications; ++r)
        dt.remove(sites[i].replications[r]);
    sites[i].nreplications = 0;

    // Remove original vertex
    dt.remove(sites[i].vertex);
    sites[i] = Site();
}

void Delaunay::GetStatistics(Statistics *stats) const {
    stats->mindist = FLT_MAX;
    stats->avgmindist = 0;
    stats->orientorder = 0;
    std::complex<double> acc = 0;
    unsigned long nacc = 0;
    
    for (int i = 0; i < (int) sites.size(); ++i) {
        float localmd = FLT_MAX;
        std::complex<double> localacc = 0;
        DT::Vertex_circulator vc = dt.incident_vertices(sites[i].vertex),
                                   done(vc), next;
        if (vc == NULL) continue;

        do {
            next = vc; ++next;
            const Point_2 &v1 = vc->point();
            const Point_2 &v2 = next->point();
            
            // Local mindist
            double dist = CGAL::squared_distance(sites[i].vertex->point(), v1);
            localmd = std::min(localmd, (float) dist);
            
            // Orientational order
            std::complex<double> c1(v1.x(), v1.y());
            std::complex<double> c2(v2.x(), v2.y());
            localacc += std::polar(1.0, 6.0 * arg(c1 - c2));
            ++nacc;
        }
        while (++vc != done);
        
        stats->mindist = std::min(stats->mindist, localmd);
        stats->avgmindist += sqrtf(localmd);
        acc += abs(localacc);
    }
    stats->mindist = sqrtf(stats->mindist);
    stats->avgmindist /= sites.size();
    stats->orientorder = abs(acc) / nacc;
}

void Delaunay::Save(const char *fname, bool points, bool debug) const {
    std::ofstream os;
    os.open(fname, std::ofstream::out | std::ofstream::trunc);
    
    double scale = 512.0;
    double radius = 3.0 / scale;
    Point_2 BB[2];
    if (debug) {
        BB[0] = Point_2( -scale,  -scale);
        BB[1] = Point_2(2*scale, 2*scale);
    } else {
        BB[0] = Point_2(0, 0);
        BB[1] = Point_2(scale, scale);
    }
    
    os << "%!PS-Adobe-3.1 EPSF-3.0\n";
    os << "%%HiResBoundingBox: " << BB[0] << " " << BB[1] << "\n";
    os << "%%BoundingBox: " << BB[0] << " " << BB[1] << "\n";
    os << "%%CropBox: " << BB[0] << " " << BB[1] << "\n";
    os << "/radius { " << radius << " } def\n";
    os << "/p { radius 0 360 arc closepath fill stroke } def\n";
    os << "gsave " << scale << " " << scale << " scale\n";
    os << (1.0 / scale) << " setlinewidth\n";
    // Faces
    os << "0.5 setgray\n";
    DT::Face_iterator fi;
    for (fi = dt.faces_begin(); fi != dt.faces_end(); ++fi) {
        if (!IsInUnitTorus(dt.circumcenter(fi))) continue;
        
        Point_2 p0 = fi->vertex(0)->point(),
                p1 = fi->vertex(1)->point(),
                p2 = fi->vertex(2)->point();
        os << p0 << " moveto "
           << p1 << " lineto " << p2 << " lineto closepath "
           << (debug ? "fill" : "stroke") << "\n";
    }
    // Edges
    if (debug) {
        os << "0.25 setgray\n";
        DT::Edge_iterator ei;
        for (ei = dt.edges_begin(); ei != dt.edges_end(); ++ei) {
            const FH fh = ei->first;
            const int i = ei->second;
            Point_2 p0 = fh->vertex(DT::cw(i))->point(),
                    p1 = fh->vertex(DT::ccw(i))->point();
            os << p0 << " moveto " << p1 << " lineto stroke\n";
        }
    }
    // Vertices
    if (points) {
        os << "0 setgray\n";
        for (unsigned i = 0; i < sites.size(); ++i)
            os << sites[i].vertex->point() << " p\n";
    }
    os << "grestore\n";
    // Bounding Box
    if (debug) {
        os << "0 setgray\n";
        os << "1.0 setlinewidth\n";
        os << "0 0 moveto 0 " << scale << " rlineto " << scale
           << " 0 rlineto 0 " << -scale << " rlineto closepath stroke\n";
    }
    os.close();
}

#endif  // PSA_HAS_CGAL
