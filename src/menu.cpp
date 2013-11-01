/*
    reup
    Automatization of support YMZ-530 ECU SW repository.

    File: menu.cpp

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

#include "menu.h"
#include "identification.h"
#include "configuration.h"

#include <iostream>

using std::cout;

void showMenu() {

    cout << "\nMenu:\n"
         << "  0. Trim available hex files\n"
         << "  1. Archive trimmed hex files\n"
         << "  2. Add new hex and mpk files to repository\n"
         << "  3. Archive repository\n"
         << "  4. Clean trimhex directory\n"
         << "  5. Reload program configuration\n"
         << "  6. Exit\n\n"
         << "Your select: ";
}

void trimHex() {

    //
}

void archHex() {

    //
}

void addNewToRepo() {

    //
}

void archRepo() {

    //
}

void cleanDir() {

    //
}
