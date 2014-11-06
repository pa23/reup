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
#include "configuration.hpp"

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <ctime>

#define BOOST_NO_CXX11_SCOPED_ENUMS

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

using std::string;
using std::vector;
using std::stringstream;
using std::hex;
using std::ifstream;
using std::ios;
using std::streampos;
using std::cout;
using std::unique_ptr;
using std::ostringstream;

vector<string> readDir(
        const boost::filesystem::path &rootDir,
        const string &extension,
        size_t selMode
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

        if ( selMode == READDIR_FILESONLY ) {

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
        }
        else if ( selMode == READDIR_DIRSONLY ) {

            if ( boost::filesystem::is_directory(fs) ) {
                fileNames.push_back(dit->path().string());
            }
        }

        ++dit;
    }

    return fileNames;
}

string currDateTime() {

    time_t t = time(NULL);
    struct tm *dtnow = localtime(&t);

    const string year = boost::lexical_cast<string>(dtnow->tm_year + 1900);
    const string mon  = boost::lexical_cast<string>(dtnow->tm_mon + 1);
    const string day  = boost::lexical_cast<string>(dtnow->tm_mday);
    const string hour = boost::lexical_cast<string>(dtnow->tm_hour);
    const string min  = boost::lexical_cast<string>(dtnow->tm_min);

    return year + "-" + trimDate(mon) + "-" + trimDate(day) + "_" + trimDate(hour) + "-" + trimDate(min);
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

    ifstream fin(filename, ios::in|ios::binary);

    if ( fin ) {
        ostringstream ostrm;
        ostrm << fin.rdbuf();
        fin.close();
        return ostrm.str();
    }
    else {
        cout << ERRORMSGBLANK << "Can not open file \"" << filename << "\" to read!\n";
        return "";
    }
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

void archDir(const string &command, const string &path, bool withDateTime) {

    if ( !boost::filesystem::exists(path) ) {
        cout << ERRORMSGBLANK << "Path \"" << path << "\" not exists!\n";
        return;
    }

    if ( withDateTime ) {
        system((command + " " + path + "__" + currDateTime() + ".7z " + path + " > " + TEMPFILE).c_str());
    }
    else {
        system((command + " " + path + ".7z " + path + " > " + TEMPFILE).c_str());
    }

    boost::filesystem::remove(TEMPFILE);
}
