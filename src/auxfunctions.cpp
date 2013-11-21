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

#include "auxfunctions.hpp"

#include <string>
#include <vector>
#include <sstream>

#define BOOST_NO_CXX11_SCOPED_ENUMS

#include <boost/filesystem.hpp>

using std::string;
using std::vector;
using std::stringstream;
using std::hex;

void findFiles(
        const boost::filesystem::path &rootDir,
        const string &extension,
        vector<string> &fileNames
        ) {

    if ( !boost::filesystem::exists(rootDir) ||
         !boost::filesystem::is_directory(rootDir) ) {
        return;
    }

    boost::filesystem::directory_iterator dit(rootDir);
    boost::filesystem::directory_iterator enddit;

    bool selectAll = false;

    if ( extension.empty() ) {
        selectAll = true;
    }

    boost::filesystem::file_status fs;

    while ( dit != enddit ) {

        fs = boost::filesystem::status(*dit);

        if ( selectAll ) {

            if ( boost::filesystem::is_regular_file(fs) ) {
                fileNames.push_back(dit->path().filename().string());
            }
        }
        else {

            if ( boost::filesystem::is_regular_file(fs) &&
                 dit->path().extension() == extension ) {
                fileNames.push_back(dit->path().filename().string());
            }
        }

        ++dit;
    }
}

void hexToString(const string &srcStr, string &destStr) {

    if ( srcStr.size() < 2 ) {
        return;
    }

    destStr.clear();

    stringstream ss;
    size_t asciiCode = 0;

    for ( size_t i=1; i<srcStr.size(); i+=2 ) {

        ss << hex << srcStr.substr(i-1, 2);
        ss >> asciiCode;

        if ( asciiCode > 31 && asciiCode < 127 ) {
            destStr.push_back(static_cast<char>(asciiCode));
        }

        ss.clear();
    }
}

void stringToHex(const string &srcStr, string &destStr) {

    destStr.clear();

    stringstream ss;
    size_t asciiCode = 0;

    for ( size_t i=0; i<srcStr.size(); i++ ) {

        asciiCode = static_cast<size_t>(srcStr[i]);

        if ( asciiCode > 31 && asciiCode < 127 ) {
            ss << hex << asciiCode;
        }
    }

    ss >> destStr;
}

void hexToNum(const string &srcStr, vector<size_t> &v) {

    if ( srcStr.size() < 2 ) {
        return;
    }

    v.clear();

    stringstream ss;
    size_t code = 0;

    for ( size_t i=1; i<srcStr.size(); i+=2 ) {

        ss << hex << srcStr.substr(i-1, 2);
        ss >> code;
        v.push_back(code);

        ss.clear();
    }
}
