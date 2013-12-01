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
    m_readedData.clear();
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

    if ( !writeData(str) ) {

        cout << Constants{}.errorMsgBlank() << "Errors during writing data in hex.\n";
        return false;
    }

    if ( !writeHex() ) {

        cout << Constants{}.errorMsgBlank() << "Errors during writing hex file.\n";
        return false;
    }

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
    unsigned char cs = 0;

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

    m_strDataLength = v[0];

    const short strtAddr = hexToNum(m_address) & (0xFFFF - m_strDataLength + 1);

    const boost::regex regexp(R"(^)" + numToHex(m_strDataLength) + numToHex(strtAddr) + R"(00.*)");

    for ( size_t i=m_addrExtStrNum+1; i<ma_hexData.size(); i++ ) {

        if ( boost::regex_match(ma_hexData[i], regexp) ) {
            m_beginStrNum = i;
            break;
        }
    }

    m_correctStrDataSize = 8 + m_strDataLength * 2 + 2;

    if ( ma_hexData[m_beginStrNum].size() != m_correctStrDataSize ) {
        return false;
    }

    m_firstByteInd = 8 + hexToNum(m_address.substr(m_address.size()-1, 1)) * 2;

    return true;
}

bool k2rei_swver::readData() {

    string str;
    m_readedData.clear();
    size_t maxsize = m_dataLength * 2;

    for ( size_t j=m_firstByteInd; j<(ma_hexData[m_beginStrNum].size()-2); j++ ) {

        if ( str.size() == maxsize ) {

            m_readedData = hexToString(str);
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

                m_readedData = hexToString(str);
                return true;
            }
            else {
                str.push_back(ma_hexData[i][j]);
            }
        }
    }

    return true;
}

bool k2rei_swver::writeData(const string &str) {

    if ( str.empty() ) {
        return false;
    }

    m_dataForWrite = stringToHex(str);

    size_t maxsize = m_dataLength * 2;

    if ( m_dataForWrite.size() > maxsize ) {
        return false;
    }
    else if ( m_dataForWrite.size() < maxsize ) {

        for ( size_t i=m_dataForWrite.size(); i<maxsize; i++ ) {
            m_dataForWrite.push_back('0');
        }
    }

    size_t pos = 0;

    for ( size_t j=m_firstByteInd; j<(ma_hexData[m_beginStrNum].size()-2); j++ ) {

        if ( pos == m_dataForWrite.size()-1 ) {
            break;
        }

        ma_hexData[m_beginStrNum][j] = m_dataForWrite[pos];
        pos++;
    }

    string strwoCS = ma_hexData[m_beginStrNum].substr(0, m_correctStrDataSize-2);
    ma_hexData[m_beginStrNum] = strwoCS + checksum(strwoCS);

    if ( pos == m_dataForWrite.size()-1 ) {
        return true;
    }

    bool stop = false;

    for ( size_t i=(m_beginStrNum+1); i<ma_hexData.size(); i++ ) {

        if ( ma_hexData[i].size() != m_correctStrDataSize ) {
            return false;
        }

        for ( size_t j=8; j<(m_correctStrDataSize-2); j++ ) {

            if ( pos == m_dataForWrite.size()-1 ) {

                stop = true;
                break;
            }

            ma_hexData[i][j] = m_dataForWrite[pos];
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

    ofstream fout(m_hexFilePath);

    if ( !fout ) {
        cout << Constants{}.errorMsgBlank() << "Can not open file \"" << m_hexFilePath << "\" to write!\n";
        return false;
    }

    for ( auto elem : ma_hexData ) {
        fout << ":" << elem << "\r\n";
    }

    fout.close();

    return true;
}
