/*
 * This file is part of the Ideal libraries
 * Copyright (C) 2009 Rafael Fernández López <ereslibre@ereslibre.es>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "reg_exp.h"

#include <boost/regex.hpp>

namespace IdealCore {

class RegExp::Private
{
public:
    String regExp;
};

RegExp::RegExp()
    : d(new Private)
{
}

RegExp::RegExp(const RegExp &regExp)
    : d(new Private)
{
    d->regExp = regExp.d->regExp;
}

RegExp::RegExp(const String &regExp)
    : d(new Private)
{
    d->regExp = regExp;
}

RegExp::~RegExp()
{
    delete d;
}

bool RegExp::match(const String &str) const
{
    boost::regex regExp(d->regExp.data());
    boost::cmatch what;
    return boost::regex_match(str.data(), what, regExp);
}

}
