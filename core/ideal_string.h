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

#ifndef IDEAL_STRING_H
#define IDEAL_STRING_H

#include <iostream>
#include <ideal_export.h>
#include <core/list.h>
#include <core/char.h>

namespace IdealCore {

/**
  * @class String ideal_string.h core/ideal_string.h
  *
  * This class represents a UTF-8 string.
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT String
{
public:
    /// Returned when a char or a substring has not been found.
    static const size_t npos = -1;

    String();
    String(const String &str);
    String(const char *str);
    String(const char *str, size_t n);
    String(Char c);
    virtual ~String();

    /**
      * @return An ANSI character pointer to the string.
      */
    const char *data() const;

    /**
      * Empties the string.
      */
    void clear();

    /**
      * @return True if the string is empty. False otherwise.
      */
    bool empty() const;

    /**
      * @return The size of this string based on the wide character string. This
      *         means that for example:
      *
      * @code
      *     String myString("Tést");
      *     const size_t stringSize = myString.size(); // stringSize would contain a 4
      * @endcode
      */
    size_t size() const;

    /**
      * @return True if the string contains @p c. False otherwise.
      */
    bool contains(Char c) const;

    /**
      * @return The first position in which character @p c is found. If @p c is not found, npos is
      *         returned instead.
      */
    size_t find(Char c) const;

    /**
      * @return The first position in which character @p c is found starting to search from the right.
      *         If @p c is not found, npos is returned instead.
      */
    size_t rfind(Char c) const;

    size_t find(const String &str) const;

    /**
      * Returns a substring of this string starting at @p pos with @p n characters maximum.
      */
    String substr(size_t pos = 0, size_t n = npos) const;

    /**
      * Compares the current string to @p s.
      *
      * @return < 0, 0 or > 0 if this string is less than, equal to, or greater than @p s
      *         respectively.
      */
    int compare(const char *s) const;

    /**
      * Splits the current string by using as separator @s separator.
      *
      * @code
      * String myString("hey how are you?");
      * List<String> mySplittedString = myString.split(' ');
      * // now you have { "hey", "how", "are", "you?" }
      * @endcode
      *
      * @return a list of strings, separated by @p separator.
      */
    List<String> split(Char separator) const;

    Char operator[](unsigned int pos) const;

    String &operator=(const String &str);
    String &operator=(const char *str);
    String &operator=(Char c);
    String &operator+=(const String &str);
    String &operator+=(const char *str);
    String &operator+=(Char c);
    String operator+(const String &str) const;
    String operator+(const char *str) const;
    String operator+(Char c) const;
    bool operator==(const String &str) const;
    bool operator!=(const String &str) const;
    bool operator<(const String &str) const;
    bool operator>(const String &str) const;
    bool operator<=(const String &str) const;
    bool operator>=(const String &str) const;

private:
    class Private;
    Private *d;
};

}

IDEAL_EXPORT std::ostream &operator<<(std::ostream &stream, const IdealCore::String &str);

#endif //IDEAL_STRING_H
