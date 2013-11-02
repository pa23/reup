/*
    reup
    Automatization of support YMZ-530 ECU SW repository.

    File: auxfunctions.cpp

    Copyright (C) 2013 Artem Petrov <pa2311@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "auxfunctions.h"

#include <string>
#include <vector>

#include <boost/filesystem.hpp>

using std::string;
using std::vector;

void findFiles(
        const string &path,
        const string &extension,
        vector<string> &fileNames
        ) {

    boost::filesystem::path rootDir(path);

    if ( !boost::filesystem::exists(rootDir) ||
         !boost::filesystem::is_directory(rootDir) ) {
        return;
    }

    boost::filesystem::directory_iterator dit(rootDir);
    boost::filesystem::directory_iterator enddit;

    while ( dit != enddit ) {

        if ( boost::filesystem::is_regular_file(*dit) &&
             dit->path().extension() == extension ) {
            fileNames.push_back(dit->path().filename().string());
        }

        ++dit;
    }
}
