/*
    reup
    Automatization of support YMZ-530 ECU SW repository.

    File: auxfunctions.cpp

    Copyright (C) 2013-2014 Artem Petrov <pa2311@gmail.com>

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
#include "constants.hpp"

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <iostream>

#define BOOST_NO_CXX11_SCOPED_ENUMS

#include <boost/filesystem.hpp>

using std::string;
using std::vector;
using std::stringstream;
using std::hex;
using std::ifstream;
using std::ios;
using std::streampos;
using std::cout;

vector<string> findFiles(
        const boost::filesystem::path &rootDir,
        const string &extension
        ) {

    vector<string> fileNames;

    if ( !boost::filesystem::exists(rootDir) ||
         !boost::filesystem::is_directory(rootDir) ) {
        return fileNames;
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

    return fileNames;
}

string trimDate(const string &str) {

    if ( str.size() == 1 ) {
        return "0" + str;
    }
    else if ( str.size() == 2 ) {
        return str;
    }

    return "00";
}

string readFile(const string &filename) {

    string fstr;

    ifstream fin(filename, ios::in|ios::binary|ios::ate);

    if ( !fin ) {
        cout << ERRORMSGBLANK << "Can not open file \"" << filename << "\" to read!\n";
        return fstr;
    }

    streampos size = fin.tellg();
    vector<char> fdata(size);

    fin.seekg(0, ios::beg);
    fin.read(fdata.data(), size);

    fstr = fdata.data();

    fin.close();

    return fstr;
}

string hexToString(const string &srcStr) {

    string destStr;

    if ( srcStr.size() < 2 ) {
        return destStr;
    }

    stringstream ss;
    size_t code = 0;

    for ( size_t i=1; i<srcStr.size(); i+=2 ) {

        ss << hex << srcStr.substr(i-1, 2);
        ss >> code;

        if ( code > 31 && code < 127 ) {
            destStr.push_back(static_cast<char>(code));
        }

        ss.clear();
    }

    return destStr;
}

string stringToHex(const string &srcStr) {

    string destStr;
    stringstream ss;
    size_t code = 0;

    for ( size_t i=0; i<srcStr.size(); i++ ) {

        code = static_cast<size_t>(srcStr[i]);

        if ( code > 31 && code < 127 ) {
            ss << hex << code;
        }
    }

    ss >> destStr;
    transform(destStr.begin(), destStr.end(), destStr.begin(), ::toupper);

    return destStr;
}

vector<size_t> hexToNumBS(const string &srcStr) {

    vector<size_t> v;

    if ( srcStr.size() < 2 ) {
        return v;
    }

    stringstream ss;
    size_t code = 0;

    for ( size_t i=1; i<srcStr.size(); i+=2 ) {

        ss << hex << srcStr.substr(i-1, 2);
        ss >> code;
        v.push_back(code);

        ss.clear();
    }

    return v;
}

size_t hexToNum(const string &str) {

    size_t num = 0;
    stringstream ss;

    ss << hex << str;
    ss >> num;

    return num;
}

string numToHex(size_t num) {

    string str;
    stringstream ss;

    ss << hex << num;
    ss >> str;
    transform(str.begin(), str.end(), str.begin(), ::toupper);

    return str;
}
