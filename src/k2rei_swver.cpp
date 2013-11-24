/*
    reup
    Automatization of support YMZ-530 ECU SW repository.

    File: k2rei_swver.cpp

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

#include "k2rei_swver.hpp"
#include "constants.hpp"
#include "auxfunctions.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

using std::string;
using std::vector;
using std::cout;
using std::ifstream;
using std::ofstream;
using std::stringstream;
using std::hex;

k2rei_swver::k2rei_swver(const std::string &hexP,
                         const std::string &addr,
                         size_t l) {
    m_hexFilePath = hexP;
    m_address = addr;
    m_dataLength = l;
}

string k2rei_swver::fromHex() {

    string str;

    //

    return str;
}

bool k2rei_swver::toHex(const std::string &str) {

    //

    return true;
}

bool k2rei_swver::readHex() {

    if ( !boost::filesystem::exists(boost::filesystem::path(m_hexFilePath)) ) {

        cout << Constants{}.errorMsgBlank() << "Hex file \"" << m_hexFilePath << "\" not found!\n";
        return false;
    }

    ifstream fin(m_hexFilePath);

    if ( !fin ) {
        cout << Constants{}.errorMsgBlank() << "Can not open file \"" << m_hexFilePath << "\" to read!\n";
        return false;
    }

    string s;

    while ( !fin.eof() ) {

        getline(fin, s);

        if ( !s.empty() ) {
            ma_hexData.push_back(s);
        }

        s.clear();
    }

    fin.close();

    return true;
}

string k2rei_swver::checksum(const string &str) const {

    vector<size_t> v = hexToNumBS(str);
    char cs = 0;

    for ( size_t i=0; i<v.size(); i++ ) {
        cs -= v[i];
    }

    stringstream ss;
    ss << hex << static_cast<size_t>(cs);

    string ret;
    ss >> ret;

    return ret.substr(14, 2);
}

bool k2rei_swver::writeHex() {

    //

    return true;
}

void k2rei_swver::findAddrExt() {

    string str = "0200000400" + m_address.substr(0, 2);
    string addrext = ":" + str + checksum(str);

    for ( size_t i=0; i<ma_hexData.size(); i++ ) {

        if ( ma_hexData[i] == addrext ) {
            m_addrExtStrNum = i;
            break;
        }
    }
}

bool k2rei_swver::findData() {

    vector<size_t> v = hexToNumBS(ma_hexData[m_addrExtStrNum]);

    if ( v.empty() ) {
        return false;
    }

    short strtAddr = hexToNum(m_address) & (0xFFFF - v[0] + 1);

    boost::regex regexp = R"(^:)" + numToHex(v[0]) + numToHex(strtAddr) + R"(00.*)";

    for ( size_t i=m_addrExtStrNum; i<ma_hexData.size(); i++ ) {

        if ( boost::regex_match(ma_hexData[i], regexp) ) {
            m_beginStrNum = i;
            break;
        }
    }

    ////

    return true;
}

bool k2rei_swver::readData() {

    //

    return true;
}
