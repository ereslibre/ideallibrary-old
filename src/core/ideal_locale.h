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

#ifndef LOCALE_H
#define LOCALE_H

#include <ideal_export.h>
#include <core/ideal_string.h>

namespace IdealCore {

/**
  * @class Locale ideal_locale.h core/ideal_locale.h
  *
  * Locale information.
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT Locale
{
public:
    Locale();
    Locale(const Locale &locale);
    virtual ~Locale();

    enum WeekDay {
        Monday = 0,
        Tuesday,
        Wednesday,
        Thursday,
        Friday,
        Saturday,
        Sunday
    };

    enum Month {
        January = 0,
        February,
        March,
        April,
        May,
        June,
        July,
        August,
        September,
        October,
        November,
        December
    };

    enum AmPm {
        Am = 0,
        Pm
    };

    enum Format {
        NotAbbreviated = 0,
        Abbreviated
    };

    /**
      * @return The current codeset of the application. Example: "UTF-8".
      */
    String codeSet() const;

    /**
      * @param weekDay The requested week day.
      *
      * @param format The desired format.
      *
      * @return The requested @p weekDay in the desired @p format.
      */
    String weekDay(WeekDay weekDay, Format format = NotAbbreviated) const;

    /**
      * @param month The requested month.
      *
      * @param format The desired format.
      *
      * @return The requested @p month in the desired @p format.
      */
    String month(Month month, Format format = NotAbbreviated) const;

    /**
      * @param amPm The requested time of day.
      *
      * @return The requested time of day. Depending on the locale, this can be an empty string.
      */
    String amPm(AmPm amPm) const;

    String dateTimeFormat() const;

    String dateFormat() const;

    String timeFormat() const;

    String timeAmPmFormat() const;

    /**
      * @return The currency symbol of the current locale.
      */
    String currencySymbol() const;

    /**
      * @return The international representation of the currency symbol of the current locale.
      */
    String intCurrencySymbol() const;

private:
    class Private;
    class PrivateImpl;
    Private *const d;
};

}

#endif //LOCALE_H
