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

#ifndef RESULT_H
#define RESULT_H

#include "config.h"
#include "curve.h"
#include "image.h"
#include "param.h"
#include "point.h"
#include "statistics.h"

struct Result {
    Statistics stats;
    PointSet points;
    Curve rp;
    Curve rdf;
    Curve ani;
    Image spectrum;
    int npoints;
    int nsets;
};

void SaveSummary(const std::string &fname, Result &result, Config &config);
void WriteResult(const std::string &base, Result &result, Config &config,
                 ParamList &params);

#endif // RESULT_H

