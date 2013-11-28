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

k2rei_swver::k2rei_swver() {
}

void k2rei_swver::init(const string &hexP, const string &addr, size_t l) {

    m_hexFilePath = hexP;
    m_address = addr;
    m_dataLength = l;

    ma_hexData.clear();
    m_addrExtStrNum = 0;
    m_beginStrNum = 0;
    m_correctStrDataSize = 0;
    m_firstByteInd = 0;
    m_data.clear();
}

bool k2rei_swver::read() {

    if ( !readHex() ) {

        cout << Constants{}.errorMsgBlank() << "Errors during hex file reading.\n";
        return false;
    }

    findAddrExt();

    if ( !findData() ) {

        cout << Constants{}.errorMsgBlank() << "Errors during finding data in hex file.\n";
        return false;
    }

    if ( !readData() ) {

        cout << Constants{}.errorMsgBlank() << "Errors during k2rei_swver data reading.\n";
        return false;
    }

    return true;
}

bool k2rei_swver::write(const string &str) {

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

    ma_hexData.clear();

    while ( !fin.eof() ) {

        string s;
        getline(fin, s);

        if ( !s.empty() ) {

            s = s.substr(1, s.size()-1);

            stringstream ss;
            ss << s;
            ss >> s;

            ma_hexData.push_back(s);
        }
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

    ret = ret.substr(14, 2);
    transform(ret.begin(), ret.end(), ret.begin(), ::toupper);

    return ret;
}

bool k2rei_swver::writeHex() {

    //

    return true;
}

void k2rei_swver::findAddrExt() {

    const boost::regex regexp(R"(.*)" + string("0200000400") + m_address.substr(0, 2) + R"(.*)");

    for ( size_t i=0; i<ma_hexData.size(); i++ ) {

        if ( boost::regex_match(ma_hexData[i], regexp) ) {
            m_addrExtStrNum = i;
            break;
        }
    }
}

bool k2rei_swver::findData() {

    vector<size_t> v = hexToNumBS(ma_hexData[m_addrExtStrNum+1]);

    if ( v.empty() ) {
        return false;
    }

    const short strtAddr = hexToNum(m_address) & (0xFFFF - v[0] + 1);

    const boost::regex regexp(R"(^)" + numToHex(v[0]) + numToHex(strtAddr) + R"(00.*)");

    for ( size_t i=m_addrExtStrNum+1; i<ma_hexData.size(); i++ ) {

        if ( boost::regex_match(ma_hexData[i], regexp) ) {
            m_beginStrNum = i;
            break;
        }
    }

    m_correctStrDataSize = 8 + v[0] * 2 + 2;

    if ( ma_hexData[m_beginStrNum].size() != m_correctStrDataSize ) {
        return false;
    }

    m_firstByteInd = 8 + hexToNum(m_address.substr(m_address.size()-1, 1)) * 2;

    return true;
}

bool k2rei_swver::readData() {

    string str;
    m_data.clear();
    size_t maxsize = m_dataLength * 2;

    for ( size_t j=m_firstByteInd; j<(ma_hexData[m_beginStrNum].size()-2); j++ ) {

        if ( str.size() == maxsize ) {

            m_data = hexToString(str);
            return true;
        }
        else {
            str.push_back(ma_hexData[m_beginStrNum][j]);
        }
    }

    for ( size_t i=(m_beginStrNum+1); i<ma_hexData.size(); i++ ) {

        if ( ma_hexData[i].size() != m_correctStrDataSize ) {
            return false;
        }

        for ( size_t j=8; j<(m_correctStrDataSize-2); j++ ) {

            if ( str.size() == maxsize ) {

                m_data = hexToString(str);
                return true;
            }
            else {
                str.push_back(ma_hexData[i][j]);
            }
        }
    }

    return true;
}
