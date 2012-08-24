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

#ifndef UTIL_HH
#define UTIL_HH

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cfloat>
#include <string>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#endif // !WIN32

#define PI_2    1.57079632679489661923f
#define PI      3.14159265358979323846f
#define TWOPI   6.28318548202514648437f
#define SQRT1_2 0.70710678118654752440f
#define SQRT2   1.41421356237309504880f
#define SQRT3   1.73205080756887729352f


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

inline float Nearbyint0f(float f) {
    float r = nearbyintf(f);
    return (r == 0.f && std::signbit(r)) ? 0.f : r;
}

template <typename T>
void SetZero(T *array, size_t size) {
    for (size_t i = 0; i < size; ++i)
        array[i] = 0;
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
#ifdef WIN32
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
#endif // WIN32
}

inline void PrintProgress(const std::string &label, float fract)
{
    const int maxbarlen = TerminalWidth() - 28;
    const int maxplusses = std::max(2ul, maxbarlen - label.size());
    const int curplusses = fract * maxplusses;
    
    const int buflen = label.size() + maxplusses + 64;
    char buf[buflen];
	snprintf(buf, buflen, "\r%s [", label.c_str());
    char *c = buf + strlen(buf);
	for (int i = 0; i < curplusses; ++i)
        *c++ = '+';
    for (int i = curplusses; i < maxplusses; ++i)
        *c++ = ' ';
    *c = '\0';
    snprintf(c, buflen - strlen(buf), "] %.1f%% done", fract * 100.f);
    
    fputs(buf, stdout);
	fflush(stdout);
}


#endif  // UTIL_HH

