/*
    reup
    Automatization of support YMZ-530 ECU SW repository.

    File: identification.h

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

#ifndef IDENTIFICATION_H
#define IDENTIFICATION_H

#include <string>

#define REUP_VERSION "1.0.0"

struct reupIdentification {

    std::string name() const;
    std::string version() const;
    std::string description() const;
    std::string copyrightYears() const;
    std::string authors() const;
    std::string codeHosting() const;
    std::string authorsBlog() const;
    std::string licenseInformation() const;
};

#endif // IDENTIFICATION_H
