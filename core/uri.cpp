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

#include <uriparser/Uri.h>

#include "uri.h"

namespace IdealCore{

class Uri::Private
{
public:
    Private()
        : m_port(-1)
        , m_isValidUri(false)
    {
    }

    String getHex(Char ch) const;
    String encodeUri(const String &uri) const;
    String decodeUri(const String &uri) const;

    void reconstructPath(int count, UriPathSegmentStructA *head, UriPathSegmentStructA *tail);
    void initializeContents(const String &uri);

    String m_uri;
    String m_scheme;
    String m_username;
    String m_password;
    String m_host;
    int    m_port;
    String m_path;
    String m_query;
    String m_fragment;
    bool   m_isValidUri;
};

const String uri_unreserved = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                              "abcdefghijklmnopqrstuvwxyz-_.~";

const String uri_reserved = "!*'();:@&=+$,/?%#[]";

const char uri_hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                         'A', 'B', 'C', 'D', 'E', 'F' };

String Uri::Private::getHex(Char ch) const
{
    String res;
    union {
        unsigned int value;
        char v[4];
    } fragmentedValue;
    fragmentedValue.value = ch.value();
    const int octetsRequired = ch.octetsRequired();
    for (int i = 0; i < octetsRequired; ++i) {
        res += '%';
        res += uri_hex[(fragmentedValue.v[octetsRequired - i - 1] >> 4) & 0xf];
        res += uri_hex[fragmentedValue.v[octetsRequired - i - 1] & 0xf];
    }
    return res;
}

String Uri::Private::encodeUri(const String &uri) const
{
    String res;
    for (size_t i = 0; i < uri.size(); ++i) {
        const Char c = uri[i];
        if (uri_unreserved.find(c) != String::npos || uri_reserved.find(c) != String::npos) {
            res += c;
        } else {
            res += getHex(c);
        }
    }
    return res;
}

String Uri::Private::decodeUri(const String &uri) const
{
    if (!uri.contains('%')) {
        return uri;
    }
    String res;
    size_t i = 0;
    while (i < uri.size()) {
        const Char currChar = uri[i];
        if (currChar == '%') {
            Char newChar;
            String byte1(uri[i + 1]);
            byte1 += uri[i + 2];
            unsigned int byte1Val = strtoul(byte1.data(), 0, 16);
            if (!(byte1Val & (1 << 7))) {
                newChar.c = byte1Val;
                i += 3;
            } else if (((byte1Val & (1 << 7)) && (byte1Val & (1 << 6)) && !(byte1Val & (1 << 5)))) {
                String byte2(uri[i + 4]);
                byte2 += uri[i + 5];
                unsigned int byte2Val = strtoul(byte2.data(), 0, 16);
                newChar.c |= (byte1Val << 8);
                newChar.c |= byte2Val;
                i += 6;
            } else if ((byte1Val & (1 << 7)) && (byte1Val & (1 << 6)) && (byte1Val & (1 << 5)) && !(byte1Val & (1 << 4))) {
                String byte2(uri[i + 4]);
                byte2 += uri[i + 5];
                String byte3(uri[i + 7]);
                byte3 += uri[i + 8];
                unsigned int byte2Val = strtoul(byte2.data(), 0, 16);
                unsigned int byte3Val = strtoul(byte3.data(), 0, 16);
                newChar.c |= (byte1Val << 16);
                newChar.c |= (byte2Val << 8);
                newChar.c |= byte3Val;
                i += 9;
            } else if ((byte1Val & (1 << 7)) && (byte1Val & (1 << 6)) && (byte1Val & (1 << 5)) && (byte1Val & (1 << 4))) {
                String byte2(uri[i + 4]);
                byte2 += uri[i + 5];
                String byte3(uri[i + 7]);
                byte3 += uri[i + 8];
                String byte4(uri[i + 10]);
                byte4 += uri[i + 11];
                unsigned int byte2Val = strtoul(byte2.data(), 0, 16);
                unsigned int byte3Val = strtoul(byte3.data(), 0, 16);
                unsigned int byte4Val = strtoul(byte4.data(), 0, 16);
                newChar.c |= (byte1Val << 24);
                newChar.c |= (byte2Val << 16);
                newChar.c |= (byte3Val << 8);
                newChar.c |= byte4Val;
                i += 12;
            }
            res += newChar;
        } else {
            res += currChar;
            ++i;
        }
    }
    return res;
}

void Uri::Private::reconstructPath(int count, UriPathSegmentStructA *head, UriPathSegmentStructA *tail)
{
    const String currSegment = decodeUri(String(head->text.first, head->text.afterLast - head->text.first));
    if (head == tail) {
        m_path += currSegment;
    } else if (!count) {
        m_path = '/';
    }
    if (head->next) {
        m_path += currSegment + '/';
        reconstructPath(count + 1, head->next, tail);
    }
}

void Uri::Private::initializeContents(const String &uriP_)
{
    m_uri = uriP_;
    const String uriP = encodeUri(uriP_);
    UriParserStateA parserState;
    UriUriA uri;
    parserState.uri = &uri;
    m_isValidUri = uriParseUriA(&parserState, uriP.data()) == URI_SUCCESS;
    if (m_isValidUri) {
        uriNormalizeSyntaxA(&uri);
        {
            char *uriString;
            int charsRequired;
            uriToStringCharsRequiredA(&uri, &charsRequired);
            ++charsRequired;
            uriString = new char[charsRequired];
            uriToStringA(uriString, &uri, charsRequired, 0);
            m_uri = decodeUri(uriString);
            delete[] uriString;
        }
        if (uri.scheme.first && uri.scheme.afterLast) {
            m_scheme = String(uri.scheme.first, uri.scheme.afterLast - uri.scheme.first);
        }
        if (uri.userInfo.first && uri.userInfo.afterLast) {
            const String userInfo = String(uri.userInfo.first, uri.userInfo.afterLast - uri.userInfo.first);
            const size_t sepPos = userInfo.find(':');
            if (sepPos != String::npos) {
                m_username = userInfo.substr(0, sepPos);
                m_password = userInfo.substr(sepPos + 1);
            } else {
                m_username = userInfo;
            }
        }
        if (uri.hostText.first && uri.hostText.afterLast) {
            m_host = decodeUri(String(uri.hostText.first, uri.hostText.afterLast - uri.hostText.first));
        }
        if (uri.portText.first && uri.portText.afterLast) {
            String port(uri.portText.first, uri.portText.afterLast - uri.portText.first);
            m_port = atoi(port.data());
        }
        if (uri.pathHead) {
            reconstructPath(0, uri.pathHead, uri.pathTail);
            if (m_path.empty()) {
                m_path = '/';
            } else {
                if (m_path[m_path.size() - 1] == '/') {
                    m_path = m_path.substr(0, m_path.size() - 1);
                    m_uri = m_uri.substr(0, m_uri.size() - 1);
                }
            }
        } else {
            m_path = '/';
        }
        if (uri.query.first && uri.query.afterLast) {
            m_query = String(uri.query.first, uri.query.afterLast - uri.query.first);
        }
        if (uri.fragment.first && uri.fragment.afterLast) {
            m_fragment = String(uri.fragment.first, uri.fragment.afterLast - uri.fragment.afterLast);
        }
    }
    uriFreeUriMembersA(&uri);
}

Uri::Uri()
    : d(new Private)
{
}

Uri::Uri(const String &uri)
    : d(new Private)
{
    d->initializeContents(uri);
}

Uri::Uri(const String &path, const String &filename)
    : d(new Private)
{
    if (path[path.size() - 1] == '/') {
        d->initializeContents(path + filename);
    } else {
        d->initializeContents(path + '/' + filename);
    }
}

Uri::Uri(const char *uri)
    : d(new Private)
{
    d->initializeContents(uri);
}

Uri::Uri(const Uri &uri)
    : d(new Private)
{
    d->m_uri = uri.d->m_uri;
    d->m_scheme = uri.d->m_scheme;
    d->m_username = uri.d->m_username;
    d->m_password = uri.d->m_password;
    d->m_host = uri.d->m_host;
    d->m_port = uri.d->m_port;
    d->m_path = uri.d->m_path;
    d->m_query = uri.d->m_query;
    d->m_fragment = uri.d->m_fragment;
    d->m_isValidUri = uri.d->m_isValidUri;
}

Uri::~Uri()
{
    delete d;
}

String Uri::scheme() const
{
    return d->m_scheme;
}

String Uri::username() const
{
    return d->m_username;
}

String Uri::password() const
{
    return d->m_password;
}

String Uri::host() const
{
    return d->m_host;
}

int Uri::port() const
{
    return d->m_port;
}

String Uri::path() const
{
    return d->m_path;
}

String Uri::filename() const
{
    const size_t sepPos = d->m_path.rfind('/');
    if (sepPos != String::npos && sepPos < d->m_path.size() - 1) {
        return d->m_path.substr(sepPos + 1);
    }
    return String();
}

String Uri::uri() const
{
    return d->m_uri;
}

bool Uri::isValid() const
{
    return d->m_isValidUri;
}

bool Uri::contains(const Uri &uri) const
{
    return d->m_path.find(uri.d->m_path) == 0;
}

Uri &Uri::dirUp()
{
    if (d->m_path.empty() || !d->m_path.compare("/")) {
        return *this;
    }
    size_t size = d->m_uri.size();
    if (d->m_uri[size - 1] == '/') {
        d->m_uri = d->m_uri.substr(0, size - 1);
    }
    size = d->m_path.size();
    if (d->m_path[size - 1] == '/') {
        d->m_path = d->m_path.substr(0, size - 1);
    }
    size_t pos = d->m_uri.rfind('/');
    d->m_uri = d->m_uri.substr(0, d->m_uri.size() - (d->m_uri.size() - pos));
    pos = d->m_path.rfind('/');
    d->m_path = d->m_path.substr(0, d->m_path.size() - (d->m_path.size() - pos));
    if (d->m_path.empty()) {
        d->m_uri += '/';
        d->m_path = '/';
    }
    return *this;
}

Uri &Uri::operator=(const Uri &uri)
{
    if (this == &uri) {
        return *this;
    }
    d->m_uri = uri.d->m_uri;
    d->m_scheme = uri.d->m_scheme;
    d->m_username = uri.d->m_username;
    d->m_password = uri.d->m_password;
    d->m_host = uri.d->m_host;
    d->m_port = uri.d->m_port;
    d->m_path = uri.d->m_path;
    d->m_query = uri.d->m_query;
    d->m_fragment = uri.d->m_fragment;
    d->m_isValidUri = uri.d->m_isValidUri;
    return *this;
}

bool Uri::operator==(const Uri &uri) const
{
    if (this == &uri) {
        return true;
    }
    return d->m_uri == uri.d->m_uri &&
           d->m_scheme == uri.d->m_scheme &&
           d->m_username == uri.d->m_username &&
           d->m_password == uri.d->m_password &&
           d->m_host == uri.d->m_host &&
           d->m_port == uri.d->m_port &&
           d->m_path == uri.d->m_path &&
           d->m_query == uri.d->m_query &&
           d->m_fragment == uri.d->m_fragment &&
           d->m_isValidUri == uri.d->m_isValidUri;

}

bool Uri::operator!=(const Uri &uri) const
{
    return !(*this == uri);
}

}
