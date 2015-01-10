/*
    reup
    Automatization of support YMZ-530 ECU SW repository.

    File: auxfunctions.hpp

    Copyright (C) 2013-2015 Artem Petrov <pa2311@gmail.com>

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

#ifndef AUXFUNCTIONS_HPP
#define AUXFUNCTIONS_HPP

#include <string>
#include <vector>
#include <memory>

#define BOOST_NO_CXX11_SCOPED_ENUMS

#include <boost/filesystem.hpp>

#include "configuration.hpp"

std::vector<std::string> readDir(
        const boost::filesystem::path &, // path
        const std::string &,             // file extension
        size_t                           // selection mode
        );

std::string currDateTime();
std::string trimDate(const std::string &);

std::string readFile(const std::string &);

std::string hexToString(const std::string &);
std::string stringToHex(const std::string &);

std::vector<size_t> hexToNumBS(const std::string &);
size_t hexToNum(const std::string &);
std::string numToHex(size_t);

void archDir(
    const std::string &, // command
    const std::string &, // path
    bool                 // withDateTime flag
    );

#endif // AUXFUNCTIONS_HPP
