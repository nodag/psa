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

#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>
#include <string>

// Config
struct Config {
    float frange;    // Frequency range relative to Nyq. freq. of hexagonal lattice
    float fbinsize;  // Bin size for RP/Ani such that nbins = fbinsize * maxfreq
    float rrange;    // Distance range relative to mindist of hexagonal lattice
    float rbinsize;  // Bin size for RDF such that nbins = rbinsize * npoints
    float fymin;     // Minimum y-value for RP/Ani plot output
    float fymax;     // Maximum y-value for RP/Ani plot output
    float rymin;     // Minimum y-value for RDF plot output
    float rymax;     // Minimum y-value for RDF plot output
};

Config LoadConfig(const std::string &fname);

#endif  // UTIL_H

