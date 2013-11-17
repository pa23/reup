/*
    reup
    Automatization of support YMZ-530 ECU SW repository.

    File: identification.hpp

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

#ifndef IDENTIFICATION_HPP
#define IDENTIFICATION_HPP

#include <string>

struct Identification {

    std::string name() const {
        return "reup";
    }

    std::string version() const {
        return "1.1.0";
    }

    std::string description() const {
        return "Automatization of support YMZ-530 ECU SW repository.";
    }

    std::string copyrightYears() const {
        return "2013";
    }

    std::string authors() const {
        return "Artem Petrov <pa2311@gmail.com>";
    }

    std::string sourceCodeHosting() const {
        return "https://github.com/pa23/reup";
    }

    std::string authorsBlog() const {
        return "http://pa2311.blogspot.com";
    }

    std::string licenseInformation() const {
        return "This program comes with ABSOLUTELY NO WARRANTY. This is free software,\n"
                "and you are welcome to redistribute it under the terms of the GNU General\n"
                "Public License version 3.\n"
                "You should have received a copy of the GNU General Public License along\n"
                "with this program. If not, see <http://www.gnu.org/licenses/>.";
    }
};

#endif // IDENTIFICATION_HPP
