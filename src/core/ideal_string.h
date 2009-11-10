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
    String(const std::string &str);
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
      * String myString("Tést");
      * const size_t stringSize = myString.size(); // stringSize would contain a 4
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

    /**
      * @return The first occurrence of @p str. npos is returned if @p str is not found.
      */
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
    iint32 compare(const char *s) const;

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

    iint8 toChar(iuint32 base = 10) const;
    iuint8 toUChar(iuint32 base = 10) const;
    iint16 toShort(iuint32 base = 10) const;
    iuint16 toUShort(iuint32 base = 10) const;
    iint32 toInt(iuint32 base = 10) const;
    iuint32 toUInt(iuint32 base = 10) const;
    long toLong(iuint32 base = 10) const;
    iulong toULong(iuint32 base = 10) const;
    iint64 toLongLong(iuint32 base = 10) const;
    iuint64 toULongLong(iuint32 base = 10) const;
    float toFloat() const;
    ireal toDouble() const;

    void setNumber(iint32 n, iint32 base = 10);
    void setNumber(iuint32 n, iint32 base = 10);
    void setNumber(long n, iint32 base = 10);
    void setNumber(iulong n, iint32 base = 10);
    void setNumber(iint64 n, iint32 base = 10);
    void setNumber(iuint64 n, iint32 base = 10);
    void setNumber(double n, iint32 base = 10);

    static String number(iint32 n, iint32 base = 10);
    static String number(iuint32 n, iint32 base = 10);
    static String number(long n, iint32 base = 10);
    static String number(iulong n, iint32 base = 10);
    static String number(iint64 n, iint32 base = 10);
    static String number(iuint64 n, iint32 base = 10);
    static String number(double n, iint32 base = 10);

    /**
      * @return the character found at position @p pos on the string. If @p pos is out of
      *         bounds, a default constructed Char is returned.
      */
    Char operator[](iuint32 pos) const;

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
