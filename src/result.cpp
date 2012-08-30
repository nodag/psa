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

#include "result.h"
#include "util.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cairo/cairo-pdf.h>


void SaveSummary(const std::string &fname, Result &result, Config &config)
{
    const float fnorm = 2.f / sqrtf(result.npoints);
    const float rnorm = 1.f / sqrtf(2.f / (SQRT3 * result.npoints));
    
    const int csize = 512;  // Composition cell size
    const double dashes[] = { 6.0, 3.0 };
    
    cairo_surface_t *surface =
    cairo_pdf_surface_create(fname.c_str(), 2*csize, 1.5*csize);
    cairo_pdf_surface_restrict_to_version(surface, CAIRO_PDF_VERSION_1_4);
    
    cairo_t *cr = cairo_create(surface);
    unsigned char *imgdata = NULL;
    cairo_surface_t *image = NULL;
    
    // Draw points
    const float radius = 2.0;
    cairo_identity_matrix(cr);
    cairo_set_source_rgba(cr, 0, 0, 0, 1);
    for (int i = 0; i < result.points.size(); ++i) {
        float x = result.points[i].x * csize;
        float y = (1.f - result.points[i].y) * csize;
        cairo_arc(cr, x, y, radius, 0, TWOPI);
        cairo_fill(cr);
    }
    
    // Draw radial power reference level
    cairo_identity_matrix(cr);
    cairo_set_source_rgba(cr, 0.6, 0.6, 0.6, 1);
    cairo_set_line_width(cr, 1.0);
    cairo_set_dash(cr, dashes, 2, 0);
    const float rpref = 1.f - (1.f - config.fymin) / (config.fymax - config.fymin);
    cairo_move_to(cr,   csize, csize + rpref*csize/2);
    cairo_line_to(cr, 2*csize, csize + rpref*csize/2);
    cairo_stroke(cr);
    
    // Draw radial power
    cairo_identity_matrix(cr);
    cairo_set_source_rgba(cr, 0, 0, 0, 1);
    cairo_set_line_width(cr, 1.0);
    cairo_set_dash(cr, NULL, 0, 0);
    for (int i = 0; i < result.rp.size(); ++i) {
        float x = i / (float) result.rp.size();
        float y = 1.f - (result.rp[i] - config.fymin) / (config.fymax - config.fymin);
        Clamp01(y);
        if (i == 0)
            cairo_move_to(cr, csize + x*csize, csize + y*csize/2);
        else
            cairo_line_to(cr, csize + x*csize, csize + y*csize/2);
    }
    cairo_stroke(cr);
    
    // Draw spectrum
    int stride = cairo_format_stride_for_width(CAIRO_FORMAT_RGB24,
                                               result.spectrum.width);
    result.spectrum.GetRGBA(imgdata);
    image = cairo_image_surface_create_for_data(imgdata, CAIRO_FORMAT_RGB24,
                                                result.spectrum.width,
                                                result.spectrum.height,
                                                stride);
    cairo_identity_matrix(cr);
    cairo_translate(cr, csize, 0);
    cairo_scale(cr, csize / (float) result.spectrum.width,
                    csize / (float) result.spectrum.height);
    cairo_set_source_surface(cr, image, 0, 0);
    cairo_paint(cr);
    
    // Draw RDF reference level
    cairo_identity_matrix(cr);
    cairo_set_source_rgba(cr, 0.6, 0.6, 0.6, 1);
    cairo_set_line_width(cr, 1.0);
    cairo_set_dash(cr, dashes, 2, 0);
    const float rdfref = 1.f - (1.f - config.rymin) / (config.rymax - config.rymin);
    cairo_move_to(cr, 0, csize + rdfref*csize/2);
    cairo_line_to(cr, csize, csize + rdfref*csize/2);
    cairo_stroke(cr);
    
    // Draw RDF
    cairo_identity_matrix(cr);
    cairo_set_source_rgba(cr, 0, 0, 0, 1);
    cairo_set_line_width(cr, 1.0);
    cairo_set_dash(cr, NULL, 0, 0);
    for (int i = 0; i < result.rdf.size(); ++i) {
        float x = i / (float) result.rdf.size();
        float y = 1.f - (result.rdf[i] - config.rymin) / (config.rymax - config.rymin);
        Clamp01(y);
        if (i == 0)
            cairo_move_to(cr, x*csize, csize + y*csize/2);
        else
            cairo_line_to(cr, x*csize, csize + y*csize/2);
    }
    cairo_stroke(cr);
    
    // Draw separators
    cairo_identity_matrix(cr);
    cairo_set_line_width(cr, 1.0);
    cairo_set_source_rgba(cr, 0, 0, 0, 1);
    cairo_move_to(cr, 0, csize);
    cairo_line_to(cr, 2*csize, csize);
    cairo_stroke(cr);
    cairo_move_to(cr, csize, 0);
    cairo_line_to(cr, csize, 1.5*csize);
    cairo_stroke(cr);
    
    // Draw labels
    cairo_identity_matrix(cr);
    cairo_set_font_size(cr, 12.0);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_source_rgba(cr, 0, 0, 0, 1);
    cairo_move_to(cr, 0.0125 * csize, 1.025 * csize);
    cairo_show_text(cr, "RDF");
    cairo_stroke(cr);
    cairo_move_to(cr, 1.0125 * csize, 1.025 * csize);
    cairo_show_text(cr, "Power Spectrum");
    cairo_stroke(cr);
    
    // Draw stats box
#ifdef PSA_HAS_CGAL
    int nlines = 5;
#else
    int nlines = 4;
#endif
    nlines += (result.nsets > 1);
    double offset = 0.03;
    double bsize[] = { 0.33 * csize, (nlines * offset + 0.01) * csize };
    double banchor = 0.0125 * csize;
    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.7);
    cairo_rectangle(cr, banchor, banchor, bsize[0], bsize[1]);
    cairo_fill(cr);
    
    // Draw stats and corresponding labels
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
    cairo_set_font_size(cr, 12.0);
    cairo_select_font_face(cr, "monospace", CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
    const int len = 128;
    char label[len];
    double tanchor[2] = { 1.75 * banchor, 0.9 * banchor };
    
    int i = 1;
    if (result.nsets > 1) {
        snprintf(label, len, "Averaged over %d sets", result.nsets);
        cairo_move_to(cr, tanchor[0], tanchor[1] + i * offset * csize);
        cairo_show_text(cr, label);
        ++i;
    }
    snprintf(label, len, "Gbl. Mindist   %.5f", result.stats.mindist * rnorm);
    cairo_move_to(cr, tanchor[0], tanchor[1] + i * offset * csize); ++i;
    cairo_show_text(cr, label);
    snprintf(label, len, "Avg. Mindist   %.5f", result.stats.avgmindist * rnorm);
    cairo_move_to(cr, tanchor[0], tanchor[1] + i * offset * csize); ++i;
    cairo_show_text(cr, label);
#ifdef PSA_HAS_CGAL
    snprintf(label, len, "Orient. order  %.5f", result.stats.orientorder);
    cairo_move_to(cr, tanchor[0], tanchor[1] + i * offset * csize); ++i;
    cairo_show_text(cr, label);
#endif
    snprintf(label, len, "Eff. Nyquist   %.5f", result.stats.effnyquist * fnorm);
    cairo_move_to(cr, tanchor[0], tanchor[1] + i * offset * csize); ++i;
    cairo_show_text(cr, label);
    snprintf(label, len, "Oscillations   %.5f", result.stats.oscillations);
    cairo_move_to(cr, tanchor[0], tanchor[1] + i * offset * csize); ++i;
    cairo_show_text(cr, label);

    cairo_stroke(cr);
    
    // Save and clean up
    cairo_show_page(cr);
    cairo_surface_destroy(image);
    if (imgdata) delete[] imgdata;
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}

void WriteResult(const std::string &base, Result &result, Config &config,
                 ParamList &params)
{
    const float fnorm = 2.f / sqrtf(result.npoints);
    const float rnorm = 1.f / sqrtf(2.f / (SQRT3 * result.npoints));
    
    // Statistics
    if (!params.GetString("convert").empty()) {
        std::string ext = params.GetString("convert");
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        std::string dot = (ext[0] != '.') ? "." : "";
        result.points.Save(base+dot+ext);
    }
    if (params.GetBool("spatial") || params.GetBool("spectral") || params.GetBool("stats")) {
        static bool first = true;
        if (first) {
#ifdef PSA_HAS_CGAL
            printf("%-16s\tG-MD\tA-MD\tBOO\tE-Nyq.\tOsci.\n", "File");
#else
            printf("%-16s\tG-MD\tA-MD\tE-Nyq.\tOsci.\n", "File");
#endif
            first = false;
        }
        printf("%-16s", base.c_str());
        std::cout << std::fixed << std::setprecision(3) << std::setw(3);
#ifdef PSA_HAS_CGAL
        if (params.GetBool("spatial") || params.GetBool("stats"))
            std::cout << "\t" << result.stats.mindist * rnorm
                      << "\t" << result.stats.avgmindist * rnorm
                      << "\t" << result.stats.orientorder;
        else
            std::cout << "\t-\t-\t-";
#else
        if (params.GetBool("spatial") || params.GetBool("stats"))
            std::cout << "\t" << result.stats.mindist * rnorm
                      << "\t" << result.stats.avgmindist * rnorm;
        else
            std::cout << "\t-\t-";
#endif
        if (params.GetBool("spectral") || params.GetBool("stats"))
            std::cout << "\t" << result.stats.effnyquist * fnorm
                      << "\t" << result.stats.oscillations;
        else
            std::cout << "\t-\t-";
        std::cout << "\n";
    }
    // 1D
    if (params.GetBool("rp")) {
        std::string ylabel = (result.nsets > 1) ? "power" : "amplitude";
        std::string labels[2] = { "frequency", ylabel };
        float yrange[2] = { config.fymin, config.fymax };
        if (params.GetBool("raw"))
            result.rp.SaveTXT(base+"_rp.txt");
        else
            result.rp.SaveTEX(base+"_rp.tex", labels, yrange, 1.f, fnorm);
    }
    if (params.GetBool("rdf")) {
        std::string labels[2] = { "distance", "rdf" };
        float yrange[2] = { config.rymin, config.rymax };
        if (params.GetBool("raw"))
            result.rdf.SaveTXT(base+"_rdf.txt");
        else
            result.rdf.SaveTEX(base+"_rdf.tex", labels, yrange, 1.f, rnorm);
    }
    if (params.GetBool("ani")) {
        std::string labels[2] = { "frequency", "anisotropy" };
        float yrange[2] = { std::min(-1.25f * result.nsets, -12.5f),
                            std::max( 1.25f * result.nsets,  12.5f) };
        if (params.GetBool("raw"))
            result.ani.SaveTXT(base+"_ani.txt");
        else
            result.ani.SaveTEX(base+"_ani.tex", labels, yrange,
                               (result.nsets > 1 ? -result.nsets : 0), fnorm);
    }
    // 2D
    if (params.GetBool("pspectrum"))
        result.spectrum.Save(base+"_spec.png");
}

