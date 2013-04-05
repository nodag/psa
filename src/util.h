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
 * but WITHOUT ANY WARRANTY; without even the> implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UTIL_H
#define UTIL_H

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cfloat>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <string>
#include <vector>

#if defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER)
#define snprintf _snprintf
#define NOMINMAX
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#endif

#define PI_4    0.7853981633974483096f
#define PI_2    1.5707963267948966192f
#define PI      3.1415926535897932384f
#define TWOPI   6.2831854820251464843f
#define SQRT1_2 0.7071067811865475244f
#define SQRT2   1.4142135623730950488f
#define SQRT3   1.7320508075688772935f
#define DR1     5.7831859629467845211f


// Precomputed arrays for Bessel function computation
static const float JP[5] = {
    -6.068350350393235E-008f,
    6.388945720783375E-006f,
    -3.969646342510940E-004f,
    1.332913422519003E-002f,
    -1.729150680240724E-001f
};
static const float MO[8] = {
    -6.838999669318810E-002f,
    1.864949361379502E-001f,
    -2.145007480346739E-001f,
    1.197549369473540E-001f,
    -3.560281861530129E-003f,
    -4.969382655296620E-002f,
    -3.355424622293709E-006f,
    7.978845717621440E-001f
};
static const float PH[8] = {
    3.242077816988247E+001f,
    -3.630592630518434E+001f,
    1.756221482109099E+001f,
    -4.974978466280903E+000f,
    1.001973420681837E+000f,
    -1.939906941791308E-001f,
    6.490598792654666E-002f,
    -1.249992184872738E-001f
};


// Math utility functions
inline void Clamp0(float &f) {
    f = (f < 0.f ? 0.f : f);
}

inline void Clamp1(float &f) {
    f = (f > 1.f ? 1.f : f);
}

inline void Clamp01(float &f) {
    Clamp0(f);
    Clamp1(f);
}

inline float Log2f(float f) {
    return log10f(f) / log10f(2.f);
}

inline float Decibel(float f) {
    return 10.0f * log10f(f);
}

inline float Round0f(float f) {
    float r = (f > 0.f) ? floorf(f) : ceilf(f);
    return (r == -0.f) ? 0.f : r;
}

inline float Polynomial(float x, const float *p, int n) {
    float y = p[0];
    for (int i = 1; i < n; ++i)
        y = y * x + p[i];
    return y;
}

// Single precision approximation to j0, about twice as fast as the standard
// implementation. From cephes lib, available at http://www.netlib.org/cephes/
inline float j0f(float f) {
    float x = fabsf(f);
    if (x <= 2.f) {
        float z = x*x;
        if (x < 1.0e-3f)
            return (1.f - 0.25f*z);
        return (z - DR1) * Polynomial(z, JP, 5);
    }
    float q = 1.f / x;
    float w = sqrtf(q);
    float p = w * Polynomial(q, MO, 8);
    w = q*q;
    float xn = q * Polynomial(w, PH, 8) - PI_4;
    return p * cosf(xn + x);
}


// IO utility functions
inline std::string BaseName(std::string &fname, bool strip_suffix = false)
{
    size_t found = fname.find_last_of("/\\");
    std::string base = (found != std::string::npos) ? fname.substr(found+1) :
                                                      fname;
    if (strip_suffix) {
        found = base.find_last_of(".");
        base = (found != std::string::npos) ? base.substr(0, found) : base;
    }
    return base;
}

inline int TerminalWidth() {
#if defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER)
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h == INVALID_HANDLE_VALUE || h == NULL) {
        fprintf(stderr, "GetStdHandle() call failed");
        return 80;
    }
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo = { 0 };
    GetConsoleScreenBufferInfo(h, &bufferInfo);
    return bufferInfo.dwSize.X;
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) < 0) {
        fprintf(stderr, "Error in ioctl() in TerminalWidth(): %d", errno);
        return 80;
    }
    return w.ws_col;
#endif
}

inline void PrintProgress(const std::string &label, float fract)
{
    const int maxbarlen = TerminalWidth() - 28;
    const int maxplusses = std::max(2, maxbarlen - (int)label.size());
    const int curplusses = fract * maxplusses;
    
    const int buflen = label.size() + maxplusses + 64;
    std::vector<char> buf(buflen);
    snprintf(&buf[0], buflen, "\r%s [", label.c_str());
    char *c = &buf[0] + strlen(&buf[0]);
    for (int i = 0; i < curplusses; ++i)
        *c++ = '+';
    for (int i = curplusses; i < maxplusses; ++i)
        *c++ = ' ';
    *c = '\0';
    snprintf(c, buflen - strlen(&buf[0]), "] %.1f%% done", fract * 100.f);
    
    fputs(&buf[0], stdout);
    fflush(stdout);
}


#endif  // UTIL_H

