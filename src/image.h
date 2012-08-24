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

#ifndef IMAGE_H
#define IMAGE_H

#include <string>

class Image
{    
private:
    float *pixels;
    void reallocate(int w, int h);

public:
    int width, height;
    
    Image();
    Image(int w, int h);
    Image(const Image &i);
    Image& operator= (const Image &i);
    ~Image();
    
    void Clear();
    float GetPixel(int x, int y) const;
    void SetPixel(int x, int y, float f);
    void GetRGBA(unsigned char*& data, bool flipped = true) const;
    
    void ToneMap(bool square_root = false, float scale = 0.25f);
    void Save(const std::string &fname, bool flipped = true);
};

#endif  // IMAGE_H

