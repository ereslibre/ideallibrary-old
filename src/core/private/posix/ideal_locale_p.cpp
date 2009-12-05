/*
 * This file is part of the Ideal Library
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

#include <langinfo.h>

#include <core/ideal_locale.h>
#include "ideal_locale_p.h"

namespace IdealCore {

Locale::PrivateImpl::PrivateImpl(Locale *q)
    : Private(q)
{
}

String Locale::codeSet() const
{
    return nl_langinfo(CODESET);
}

String Locale::weekDay(WeekDay weekDay, Format format) const
{
    switch (weekDay) {
        case Monday:
            if (format == NotAbbreviated) {
                return nl_langinfo(DAY_2);
            } else {
                return nl_langinfo(ABDAY_2);
            }
        case Tuesday:
            if (format == NotAbbreviated) {
                return nl_langinfo(DAY_3);
            } else {
                return nl_langinfo(ABDAY_4);
            }
        case Wednesday:
            if (format == NotAbbreviated) {
                return nl_langinfo(DAY_4);
            } else {
                return nl_langinfo(ABDAY_4);
            }
        case Thursday:
            if (format == NotAbbreviated) {
                return nl_langinfo(DAY_5);
            } else {
                return nl_langinfo(ABDAY_5);
            }
        case Friday:
            if (format == NotAbbreviated) {
                return nl_langinfo(DAY_6);
            } else {
                return nl_langinfo(ABDAY_6);
            }
        case Saturday:
            if (format == NotAbbreviated) {
                return nl_langinfo(DAY_7);
            } else {
                return nl_langinfo(ABDAY_7);
            }
        case Sunday:
            if (format == NotAbbreviated) {
                return nl_langinfo(DAY_1);
            } else {
                return nl_langinfo(ABDAY_1);
            }
        default:
            IDEAL_DEBUG_WARNING("unknown weekday");
            break;
    }
    return String();
}

String Locale::month(Month month, Format format) const
{
    switch (month) {
        case January:
            if (format == NotAbbreviated) {
                return nl_langinfo(MON_1);
            } else {
                return nl_langinfo(ABMON_1);
            }
        case February:
            if (format == NotAbbreviated) {
                return nl_langinfo(MON_2);
            } else {
                return nl_langinfo(ABMON_2);
            }
        case March:
            if (format == NotAbbreviated) {
                return nl_langinfo(MON_3);
            } else {
                return nl_langinfo(ABMON_3);
            }
        case April:
            if (format == NotAbbreviated) {
                return nl_langinfo(MON_4);
            } else {
                return nl_langinfo(ABMON_4);
            }
        case May:
            if (format == NotAbbreviated) {
                return nl_langinfo(MON_5);
            } else {
                return nl_langinfo(ABMON_5);
            }
        case June:
            if (format == NotAbbreviated) {
                return nl_langinfo(MON_6);
            } else {
                return nl_langinfo(ABMON_6);
            }
        case July:
            if (format == NotAbbreviated) {
                return nl_langinfo(MON_7);
            } else {
                return nl_langinfo(ABMON_7);
            }
        case August:
            if (format == NotAbbreviated) {
                return nl_langinfo(MON_8);
            } else {
                return nl_langinfo(ABMON_8);
            }
        case September:
            if (format == NotAbbreviated) {
                return nl_langinfo(MON_9);
            } else {
                return nl_langinfo(ABMON_9);
            }
        case October:
            if (format == NotAbbreviated) {
                return nl_langinfo(MON_10);
            } else {
                return nl_langinfo(ABMON_10);
            }
        case November:
            if (format == NotAbbreviated) {
                return nl_langinfo(MON_11);
            } else {
                return nl_langinfo(ABMON_11);
            }
        case December:
            if (format == NotAbbreviated) {
                return nl_langinfo(MON_12);
            } else {
                return nl_langinfo(ABMON_12);
            }
        default:
            IDEAL_DEBUG_WARNING("unknown month");
            break;
    }
    return String();
}

}
