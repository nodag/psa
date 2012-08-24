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

#include "image.h"
#include "util.h"
#include <cassert>
#include <cairo/cairo.h>

void Image::reallocate(int w, int h)
{
    this->width = w;
    this->height = h;
    
    if (pixels)
        delete[] pixels;
    pixels = new float[width * height];
}

Image::Image() {
    width = height = 0;
    pixels = NULL;
}

Image::Image(int w, int h) {
    pixels = NULL;
    reallocate(w, h);
    Clear();
}

Image::Image(const Image &i) {
    pixels = NULL;
    this->reallocate(i.width, i.height);
    memcpy(this->pixels, i.pixels, width * height * sizeof(float));
}

Image& Image::operator= (const Image &i) {
    this->reallocate(i.width, i.height);
    memcpy(this->pixels, i.pixels, width * height * sizeof(float));
    return *this;
}

Image::~Image() {
    if (pixels)
        delete[] pixels;
}

void Image::Clear() {
    for (int i = 0; i < width * height; ++i)
        pixels[i] = 0.f;
}

float Image::GetPixel(int x, int y) const {
    return pixels[x + y*width];
}

void Image::SetPixel(int x, int y, float f) {
    pixels[x + y*width] = f;
}

void Image::GetRGBA(unsigned char*& data, bool flipped) const {
    if (data) delete[] data;
    data = new unsigned char[width * height * 4];
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            int fy = (flipped ? height-1 - y : y);
            float f = this->GetPixel(x, fy);
            Clamp01(f);
            data[4*(x + fy*width)  ] = (unsigned char)(f * 255.0f);
            data[4*(x + fy*width)+1] = (unsigned char)(f * 255.0f);
            data[4*(x + fy*width)+2] = (unsigned char)(f * 255.0f);
            data[4*(x + fy*width)+3] = 255;
        }
    }
}

void Image::ToneMap(bool square_root, float scale)
{
    for (int i = 0; i < width * height; ++i) {
        float &f = pixels[i];
        if (square_root) f = sqrtf(f);
        f = Log2f(1.f + scale * f);
        Clamp01(f);
    }
}

void Image::Save(const std::string &fname, bool flipped)
{
    unsigned char *data = NULL;
    this->GetRGBA(data, flipped);
    int stride = cairo_format_stride_for_width(CAIRO_FORMAT_RGB24, width);
    cairo_surface_t *surface =
        cairo_image_surface_create_for_data(data, CAIRO_FORMAT_RGB24,
                                            width, height, stride);
    cairo_surface_write_to_png(surface, fname.c_str());
    cairo_surface_destroy(surface);
    delete[] data;
}

