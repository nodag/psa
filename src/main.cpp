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
 
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include "analysis.h"
#include "config.h"
#include "param.h"
#include <vector>


void Usage() {
    std::cout << "usage: psa filename [options]\n"
        "General options\n"
        "  --help            show this message\n"
        "  --convert ext     converts all given files to files with extension ext\n"
        "  --summary         single PDF with most measures (default)\n"
        "  --avg             average the measures over all given files\n"
        "Statistics\n"
        "  --spatial         Global mindist, average mindist"
#ifdef PSA_HAS_CGAL
        ", orientational order\n"
#else
        "\n"
#endif
        "  --spectral        Effective Nyquist frequency, Oscillations metric\n"
        "  --stats           All of the above\n"
        "1D Measures\n"
        "  --rp              Radial power spectrum\n"
        "  --rdf             Radial distribution function\n"
        "  --ani             Anisotropy\n"
        "  --raw             output raw data for these 1D measures\n"
        "2D Measures\n"
        "  --pspectrum       Power spectrum\n"
    ;
}

int main(int argc, char * const argv[])
{
    ParamList params;
    params.Define("help", "false");
    params.Define("convert", "");
    params.Define("summary", "false");
    params.Define("avg", "false");
    params.Define("spatial", "false");
    params.Define("spectral", "false");
    params.Define("stats", "false");
    params.Define("rp", "false");
    params.Define("rdf", "false");
    params.Define("ani", "false");
    params.Define("raw", "false");
    params.Define("pspectrum", "false");
    
    std::vector<std::string> input;
    params.Parse(argc, argv, input);
    
    bool show_usage = params.GetBool("help") || input.empty();
    if (const Param *p = params.UnusedOption()) {
        fprintf(stderr, "Unknown option '%s'.\n", p->name.c_str());
        show_usage = true;
    }
    if (show_usage) {
        Usage();
        exit(0);
    }
    
    Config config = LoadConfig("common/psa.cfg");
    
    if (params.GetBool("avg"))
        AnalysisAverage(input, params, config);
    else
        Analysis(input, params, config);
}

