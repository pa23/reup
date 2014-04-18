/*
    reup
    Automatization of support YMZ-530 ECU SW repository.

    File: k2rei_swver.hpp

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

#ifndef K2REI_SWVER_HPP
#define K2REI_SWVER_HPP

#include <string>
#include <vector>

class k2rei_swver {

public:

    k2rei_swver();

    void init(
            const std::string &,  // hex file path
            const std::string &,  // k2rei_swver address
            const size_t          // length
            );

    bool read();
    bool write(const std::string &);

    std::string val() const {
        return m_readedData;
    }

private:

    std::string m_hexFilePath;
    std::string m_address;
    size_t m_dataLength = 0;

    std::vector<std::string> ma_hexData;   // readHex()
    size_t m_addrExtStrNum = 0;            // findAddrExt()
    size_t m_beginStrNum = 0;              // findData()
    size_t m_correctStrDataSize = 0;
    size_t m_firstByteInd = 0;
    std::string m_readedData;              // readData()

    bool readHex();
    std::string checksum(const std::string &) const;
    void findAddrExt();
    bool findData();
    bool readData();
    bool writeData(const std::string &);
    bool writeHex() const;

};

#endif // K2REI_SWVER_HPP
