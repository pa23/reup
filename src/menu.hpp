/*
    reup
    Automatization of support YMZ-530 ECU SW repository.

    File: menu.hpp

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

#ifndef MENU_HPP
#define MENU_HPP

#include <memory>

#include "configuration.hpp"

enum {
    MENU_EXIT,
    MENU_TRIMHEX,
    MENU_UPDHEXID,
    MENU_ARCHHEX,
    MENU_EDITENGDESCR,
    MENU_ADDNEW,
    MENU_CLEANDIR,
    MENU_PUBREPO,
    MENU_ARCHREPO,
    MENU_RELOADCONF
};

void showMenu();

void trimHex(const std::unique_ptr<Configuration> &);
void updHexIdent(const std::unique_ptr<Configuration> &);
void archHex(const std::unique_ptr<Configuration> &);
void editEngDescr(const std::unique_ptr<Configuration> &);
void addNewToRepo(const std::unique_ptr<Configuration> &);
void cleanDir(const std::unique_ptr<Configuration> &);
void publishRepo(const std::unique_ptr<Configuration> &);
void archLocalRepo(const std::unique_ptr<Configuration> &);

#endif // MENU_HPP
