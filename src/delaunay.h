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

#ifndef DELAUNAY_H
#define DELAUNAY_H

#include <vector>
#include "statistics.h"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel  K;
typedef K::Circle_2  Circle_2;
typedef K::Point_2   Point_2;


class Delaunay
{
private:
    // CGAL Types and helper structs
    typedef CGAL::Triangulation_vertex_base_2<K>           Tvb;
    typedef CGAL::Triangulation_face_base_2<K>             Tfb;
    typedef CGAL::Triangulation_data_structure_2<Tvb,Tfb>  Tds;
    typedef CGAL::Delaunay_triangulation_2<K, Tds>         DT;
    typedef DT::Vertex_handle  VH;
    typedef DT::Face_handle    FH;
    
    // A Delaunay vertex and its toroidal replications
    struct Site {
        Site() : vertex(0), nreplications(0) {};
        VH vertex;
        VH replications[8]; // There can be a maximum of eight replications
        int nreplications;
    };
    DT dt;                      // The CGAL delaunay triangulation
    std::vector<Site> sites;    // The vertices and their replications
    Point_2 clip[2];            // Optional clipping box for vertex replications
    
    void SetVertex(int i, const Point_2 &point);
    void ClearVertex(int i);

public:
    Delaunay(const std::vector<Point_2> &points, bool clip_heuristic = false);
    ~Delaunay();

    void GetStatistics(Statistics *stats) const;
    void Save(const char *fname, bool points = true, bool debug = false) const;
};

#endif  // DELAUNAY_H

#endif  // PSA_HAS_CGAL
