/*
    reup
    Automatization of support YMZ-530 ECU SW repository.

    File: menu.h

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

#ifndef MENU_H
#define MENU_H

#include <memory>

#include "configuration.h"

enum {

    MENU_TRIMHEX,
    MENU_ARCHHEX,
    MENU_ADDNEW,
    MENU_ARCHREPO,
    MENU_CLEANDIR,
    MENU_RELOADCONF,
    MENU_EXIT
};

void showMenu();

void trimHex();
void archHex();
void addNewToRepo();
void archRepo();
void cleanDir();

#endif // MENU_H
