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

#include "config.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

Config LoadConfig(const std::string &fname) {
    // Default config
    Config config;
    config.frange   = 10;
    config.fbinsize =  0.5;
    config.rrange   =  8;
    config.rbinsize =  0.125;
    config.fymin    = -0.2;
    config.fymax    =  4.2;
    config.rymin    = -0.2;
    config.rymax    =  4.2;
    
    std::ifstream file(fname.c_str());
    if (!file) {
        std::cout << "Config file '" << fname << "' not found. "
                  << "Using defaults.\n";
        return config;
    }
    
    while (!file.eof()) {
        std::string line, key, val;
        getline(file, line);
        
        std::istringstream issline(line);
        issline >> std::ws >> key;
        
        if (key == "frange") {
            issline >> std::ws >> val;
            config.frange = std::max(atof(val.c_str()), 1.0);
        } else if (key == "fbinsize") {
            issline >> std::ws >> val;
            config.fbinsize = std::max(atof(val.c_str()), 0.1);
        } else if (key == "rrange") {
            issline >> std::ws >> val;
            config.rrange = std::max(atof(val.c_str()), 1.0);
        } else if (key == "rbinsize") {
            issline >> std::ws >> val;
            config.rbinsize = std::max(atof(val.c_str()), 0.1);
        } else if (key == "fymin") {
            issline >> std::ws >> val;
            config.fymin = atof(val.c_str());
        } else if (key == "fymax") {
            issline >> std::ws >> val;
            config.fymax = atof(val.c_str());
        } else if (key == "rymin") {
            issline >> std::ws >> val;
            config.rymin = atof(val.c_str());
        } else if (key == "rymax") {
            issline >> std::ws >> val;
            config.rymax = atof(val.c_str());
        }
    }
    assert(config.fymin < config.fymax);
    assert(config.rymin < config.rymax);
    
    return config;
}
