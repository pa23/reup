/*
    reup
    Automatization of support YMZ-530 ECU SW repository.

    File: identification.cpp

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

#include "identification.h"

std::string reupIdentification::name() const {
    return "reup";
}

std::string reupIdentification::version() const {
    return REUP_VERSION;
}

std::string reupIdentification::description() const {
    return "Automatization of support YMZ-530 ECU SW repository.";
}

std::string reupIdentification::copyrightYears() const {
    return "2013";
}

std::string reupIdentification::authors() const {
    return "Artem Petrov <pa2311@gmail.com>";
}

std::string reupIdentification::codeHosting() const {
    return "";
}

std::string reupIdentification::authorsBlog() const {
    return "http://pa2311.blogspot.com";
}

std::string reupIdentification::licenseInformation() const {
    return "This program is free software: you can redistribute it "
            "and/or modify it under the terms of the GNU General "
            "Public License as published by the Free Software "
            "Foundation, either version 3 of the License, or (at "
            "your option) any later version. This program is "
            "distributed in the hope that it will be useful, but "
            "WITHOUT ANY WARRANTY; without even the implied warranty "
            "of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. "
            "See the GNU General Public License for more details. "
            "You should have received a copy of the GNU General Public "
            "License along with this program. If not, see "
            "<http://www.gnu.org/licenses/>.";
}
