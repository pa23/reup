/*
    reup
    Automatization of YMZ ECU software repository support.

    File: k2rei_swver.cpp

    Copyright (C) 2013-2019 Artem Petrov <pa23666@yandex.ru>

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
#include <cstdint>

//#define BOOST_NO_CXX11_SCOPED_ENUMS

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

using std::string;
using std::vector;
using std::cout;
using std::ifstream;
using std::ofstream;
using std::stringstream;
using std::hex;

k2rei_swver::k2rei_swver() {
}

bool k2rei_swver::setFileName(const string &hexP) {

    m_hexFileName = hexP;

    if ( !setECUSW() ) {
        return false;
    }

    reset();

    return true;
}

bool k2rei_swver::setECUSW() {

    vector<string> elem;
    boost::split(elem, m_hexFileName, boost::is_any_of("_"));

    if      ( elem[1] == "986.2.0.0" ) {
        m_address       = IDADDR_986V200;
        m_byteIndOffset = 0;
        m_dataLength    = 64;
    }
    else if ( elem[1] == "986.3.2.0" ) {
        m_address       = IDADDR_986V320;
        m_byteIndOffset = 0;
        m_dataLength    = 64;
    }
    else if ( elem[1] == "766.1.2.0" ) {
        m_address       = IDADDR_766V120;
        m_byteIndOffset = 0;
        m_dataLength    = 64;
    }
    else if ( elem[1] == "1639.1.2.0" ) {
        m_address       = IDADDR_1639V120;
        m_byteIndOffset = 0;
        m_dataLength    = 64;
    }
    else if ( elem[1] == "1639.3.0.0" ) {
        m_address       = IDADDR_1639V300;
        m_byteIndOffset = 0;
        m_dataLength    = 64;
    }
    else {
        cout << ERRORMSGBLANK << "Unknown ECU software.\n";
        return false;
    }

    return true;
}

void k2rei_swver::reset() {

    ma_hexData.clear();
    m_addrExtStrNum = 0;
    m_beginStrNum = 0;
    m_correctStrDataSize = 0;
    m_firstByteInd = 0;
    m_readData.clear();
}

bool k2rei_swver::read() {

    if ( !readHex() ) {
        cout << ERRORMSGBLANK << "Errors during hex file reading.\n";
        return false;
    }

    findAddrExt();

    if ( !findData() ) {
        cout << ERRORMSGBLANK << "Errors during finding data in hex file.\n";
        return false;
    }

    if ( !readData() ) {
        cout << ERRORMSGBLANK << "Errors during k2rei_swver data reading.\n";
        return false;
    }

    return true;
}

bool k2rei_swver::write(const string &str) {

    if ( !writeData(str) ) {
        cout << ERRORMSGBLANK << "Errors during writing data in hex.\n";
        return false;
    }

    if ( !writeHex() ) {
        cout << ERRORMSGBLANK << "Errors during writing hex file.\n";
        return false;
    }

    return true;
}

bool k2rei_swver::readHex() {

    if ( !boost::filesystem::exists(boost::filesystem::path(m_hexFileName)) ) {
        cout << ERRORMSGBLANK << "Hex file \"" << m_hexFileName << "\" not found!\n";
        return false;
    }

    ifstream fin(m_hexFileName);

    if ( !fin ) {
        cout << ERRORMSGBLANK << "Can not open file \"" << m_hexFileName << "\" to read!\n";
        return false;
    }

    ma_hexData.clear();

    while ( !fin.eof() ) {

        string s;
        getline(fin, s);

        if ( !s.empty() ) {

            s = s.substr(1);

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
    uint8_t cs = 0;

    for ( size_t i=0; i<v.size(); i++ ) {
        cs -= v[i];
    }

    stringstream ss;
    ss << hex << static_cast<size_t>(cs);

    string ret;
    ss >> ret;

    if ( ret.size() < 2 ) {
        ret = "0" + ret;
    }
    else if ( ret.size() > 2 ) {
        return "";
    }

    transform(ret.begin(), ret.end(), ret.begin(), ::toupper);

    return ret;
}

void k2rei_swver::findAddrExt() {

    const boost::regex regexp(R"(.*)" + string("02000004") + m_address.substr(0, 4) + R"(.*)");

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

    const size_t strDataLength = v[0];

    const uint16_t strtAddr = hexToNum(m_address.substr(4)) & (0xFFFF - strDataLength + 1);
    string hexStrtAddr = numToHex(strtAddr);

    size_t addsymbnum = 4 - hexStrtAddr.size();
    if ( hexStrtAddr.size() < 4 ) {
        for ( size_t i=0; i<addsymbnum; i++ ) {
            hexStrtAddr.insert(0, "0");
        }
    }

    const boost::regex regexp(R"(^)" + numToHex(strDataLength) + hexStrtAddr + R"(00.*)");

    for ( size_t i=m_addrExtStrNum+1; i<ma_hexData.size(); i++ ) {

        if ( boost::regex_match(ma_hexData[i], regexp) ) {
            m_beginStrNum = i;
            break;
        }
    }

    m_correctStrDataSize = 8 + strDataLength * 2 + 2;

    if ( ma_hexData[m_beginStrNum].size() != m_correctStrDataSize ) {
        return false;
    }

    m_firstByteInd = 8 + (hexToNum(m_address.substr(m_address.size()-1, 1)) + m_byteIndOffset) * 2;

    return true;
}

bool k2rei_swver::readData() {

    string str;
    m_readData.clear();
    const size_t maxsize = m_dataLength * 2;

    for ( size_t j=m_firstByteInd; j<(ma_hexData[m_beginStrNum].size()-2); j++ ) {

        str.push_back(ma_hexData[m_beginStrNum][j]);

        if ( str.size() == maxsize ) {
            m_readData = hexToString(str);
            return true;
        }
    }

    for ( size_t i=(m_beginStrNum+1); i<ma_hexData.size(); i++ ) {

        if ( ma_hexData[i].size() != m_correctStrDataSize ) {
            continue;
        }

        for ( size_t j=8; j<(m_correctStrDataSize-2); j++ ) {

            str.push_back(ma_hexData[i][j]);

            if ( str.size() == maxsize ) {
                m_readData = hexToString(str);
                return true;
            }
        }
    }

    return true;
}

bool k2rei_swver::writeData(const string &str) {

    if ( str.empty() ) {
        return false;
    }

    string dataForWrite = stringToHex(str);

    const size_t maxsize = m_dataLength * 2;

    if ( dataForWrite.size() > maxsize ) {
        return false;
    }
    else if ( dataForWrite.size() < maxsize ) {

        for ( size_t i=dataForWrite.size(); i<maxsize; i++ ) {
            dataForWrite.push_back('0');
        }
    }

    size_t pos = 0;

    for ( size_t j=m_firstByteInd; j<(ma_hexData[m_beginStrNum].size()-2); j++ ) {

        if ( pos == dataForWrite.size()-1 ) {
            break;
        }

        ma_hexData[m_beginStrNum][j] = dataForWrite[pos];
        pos++;
    }

    string strwoCS = ma_hexData[m_beginStrNum].substr(0, m_correctStrDataSize-2);
    ma_hexData[m_beginStrNum] = strwoCS + checksum(strwoCS);

    if ( pos == dataForWrite.size()-1 ) {
        return true;
    }

    bool stop = false;

    for ( size_t i=(m_beginStrNum+1); i<ma_hexData.size(); i++ ) {

        if ( ma_hexData[i].size() != m_correctStrDataSize ) {
            continue;
        }

        for ( size_t j=8; j<(m_correctStrDataSize-2); j++ ) {

            if ( pos == dataForWrite.size()-1 ) {
                stop = true;
                break;
            }

            ma_hexData[i][j] = dataForWrite[pos];
            pos++;
        }

        strwoCS = ma_hexData[i].substr(0, m_correctStrDataSize-2);
        ma_hexData[i] = strwoCS + checksum(strwoCS);

        if ( stop ) {
            break;
        }
    }

    return true;
}

bool k2rei_swver::writeHex() const {

    ofstream fout(m_hexFileName);

    if ( !fout ) {
        cout << ERRORMSGBLANK << "Can not open file \"" << m_hexFileName << "\" to write!\n";
        return false;
    }

    for ( const string elem : ma_hexData ) {
        fout << ":" << elem << "\r\n";
    }

    fout.close();

    return true;
}
