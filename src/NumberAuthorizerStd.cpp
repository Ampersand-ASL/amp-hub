/**
 * Copyright (C) 2025, Bruce MacKinnon KC1FSZ
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <sstream>

#include "NumberAuthorizerStd.h"

using namespace std;

namespace kc1fsz {

    namespace amp {

bool NumberAuthorizerStd::isAuthorized(const char* nodeNumber) const {
    if (_numberList.empty())
        return true;
    string target(nodeNumber);
    trim(target);
    return std::find(_numberList.begin(), _numberList.end(), target) != _numberList.end();
}

NumberAuthorizerStd::NumberAuthorizerStd(const char* numberList) {
    if (numberList) {
        // Parse comma-delimited list
        string s(numberList);
        stringstream ss(s);
        string token;
        while (std::getline(ss, token, ',')) {
            trim(token);
            _numberList.push_back(token);
        }
    } else {
        _numberList.clear();
    }
}

}

}

