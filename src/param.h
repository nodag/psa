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

#ifndef PARAM_H
#define PARAM_H

#include <string>
#include <vector>

struct Param {
    std::string name;
    std::string value;
    bool set;
    bool used;
};

class ParamList {
    std::vector<Param> list;
public:
    ParamList();

    Param *Define(const std::string &key, const std::string &dflt);
    void Parse(int argc, char * const argv[], std::vector<std::string> &remaining);

    float GetFloat(const std::string &key);
    float GetFloat(const std::string &key, float dflt);
    int GetInt(const std::string &key, int dflt = 0);
    std::string GetString(const std::string &key, std::string dflt = "");
    bool GetBool(const std::string &key, bool dflt = false);

    const Param *UnusedOption() const;
    int NumSet() const;
    void Print() const;
private:
    Param *Set(const std::string &key, const std::string &val);
    Param *Find(const std::string &key);
    Param *Insert(const std::string &key);
};

#endif  // PARAM_H
