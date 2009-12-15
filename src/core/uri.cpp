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

#include <uriparser/Uri.h>

#include "uri.h"

namespace IdealCore{

class Uri::Private
{
public:
    Private()
        : m_port(-1)
        , m_isValidUri(false)
        , m_refs(1)
        , m_initialized(false)
    {
    }

    virtual ~Private()
    {
    }

    Private *copy()
    {
        Private *privateCopy = new Private;
        privateCopy->m_uri = m_uri;
        privateCopy->m_scheme = m_scheme;
        privateCopy->m_username = m_username;
        privateCopy->m_password = m_password;
        privateCopy->m_host = m_host;
        privateCopy->m_port = m_port;
        privateCopy->m_path = m_path;
        privateCopy->m_query = m_query;
        privateCopy->m_fragment = m_fragment;
        privateCopy->m_isValidUri = m_isValidUri;
        privateCopy->m_initialized = m_initialized;
        return privateCopy;
    }

    void copyAndDetach(Uri *uri)
    {
        if (m_refs > 1) {
            uri->d = copy();
            deref();
        } else if (this == m_privateEmpty) {
            m_privateEmpty = 0;
        }
    }

    void newAndDetach(Uri *uri)
    {
        if (m_refs > 1) {
            uri->d = new Private;
            deref();
        } else if (this == m_privateEmpty) {
            m_privateEmpty = 0;
        } else {
            clearContents();
        }
    }

    void ref()
    {
        ++m_refs;
    }

    void deref()
    {
        --m_refs;
        if (!m_refs) {
            if (this == m_privateEmpty) {
                m_privateEmpty = 0;
            }
            delete this;
        }
    }

    void clearContents()
    {
        m_uri = String();
        m_scheme = String();
        m_username = String();
        m_password = String();
        m_host = String();
        m_port = -1;
        m_path = String();
        m_query = String();
        m_fragment = String();
        m_isValidUri = false;
        m_initialized = false;
    }

    String getHex(Char ch) const;
    String encodeUri(const String &uri) const;
    String decodeUri(const String &uri) const;

    void reconstructPath(iint32 count, UriPathSegmentStructA *head, UriPathSegmentStructA *tail);
    String reconstructString(const UriTextRangeA &uriTextRange);
    void initializeContents();

    static Private *empty();

    String  m_uri;
    String  m_scheme;
    String  m_username;
    String  m_password;
    String  m_host;
    iint32  m_port;
    String  m_path;
    String  m_query;
    String  m_fragment;
    bool    m_isValidUri;
    size_t  m_refs;
    bool    m_initialized;

    static Private *m_privateEmpty;
};

Uri::Private *Uri::Private::m_privateEmpty = 0;

Uri::Private *Uri::Private::empty()
{
    if (!m_privateEmpty) {
        m_privateEmpty = new Private;
    } else {
        m_privateEmpty->ref();
    }
    return m_privateEmpty;
}

static const bool uri_unencoded[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0 - 15
                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 16 - 31
                                      0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 32 - 47
                                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, // 48 - 63
                                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 64 - 79
                                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, // 80 - 95
                                      0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 96 - 111
                                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0 }; // 112 - 127

static const ichar uri_hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                 'A', 'B', 'C', 'D', 'E', 'F' };

String Uri::Private::getHex(Char ch) const
{
    String res;
    union {
        iuint32 value;
        ichar v[4];
    } fragmentedValue;
    fragmentedValue.value = ch.value();
    const iint32 octetsRequired = ch.octetsRequired();
    for (iint32 i = 0; i < octetsRequired; ++i) {
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
        const iuint32 value = c.value();
        if (value < 128 && uri_unencoded[value]) {
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
            const iuint32 byte1Val = strtoul(byte1.data(), 0, 16);
            if (!(byte1Val & 0x80)) {
                newChar.c = byte1Val;
                i += 3;
            } else if (!(byte1Val & 0x20)) {
                String byte2(uri[i + 4]);
                byte2 += uri[i + 5];
                const iuint32 byte2Val = strtoul(byte2.data(), 0, 16);
                newChar.c |= (byte1Val << 8);
                newChar.c |= byte2Val;
                i += 6;
            } else if (!(byte1Val & 0x10)) {
                String byte2(uri[i + 4]);
                byte2 += uri[i + 5];
                String byte3(uri[i + 7]);
                byte3 += uri[i + 8];
                const iuint32 byte2Val = strtoul(byte2.data(), 0, 16);
                const iuint32 byte3Val = strtoul(byte3.data(), 0, 16);
                newChar.c |= (byte1Val << 16);
                newChar.c |= (byte2Val << 8);
                newChar.c |= byte3Val;
                i += 9;
            } else if (!(byte1Val & 0x8)) {
                String byte2(uri[i + 4]);
                byte2 += uri[i + 5];
                String byte3(uri[i + 7]);
                byte3 += uri[i + 8];
                String byte4(uri[i + 10]);
                byte4 += uri[i + 11];
                const iuint32 byte2Val = strtoul(byte2.data(), 0, 16);
                const iuint32 byte3Val = strtoul(byte3.data(), 0, 16);
                const iuint32 byte4Val = strtoul(byte4.data(), 0, 16);
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

void Uri::Private::reconstructPath(iint32 count, UriPathSegmentStructA *head, UriPathSegmentStructA *tail)
{
    String encodedUri;
    const ichar *curr = head->text.first;
    while (curr != head->text.afterLast) {
        encodedUri += *curr;
        ++curr;
    }
    const String currSegment = decodeUri(encodedUri);
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

String Uri::Private::reconstructString(const UriTextRangeA &uriTextRange)
{
    String res;
    const ichar *curr = uriTextRange.first;
    while (curr != uriTextRange.afterLast) {
        res += *(curr);
        ++curr;
    }
    return res;
}

void Uri::Private::initializeContents()
{
    m_initialized = true;
    const String uriP = encodeUri(m_uri);
    UriParserStateA parserState;
    UriUriA uri;
    parserState.uri = &uri;
    m_isValidUri = uriParseUriA(&parserState, uriP.data()) == URI_SUCCESS;
    if (m_isValidUri) {
        uriNormalizeSyntaxA(&uri);
        {
            ichar *uriString;
            iint32 charsRequired;
            uriToStringCharsRequiredA(&uri, &charsRequired);
            ++charsRequired;
            uriString = new ichar[charsRequired];
            uriToStringA(uriString, &uri, charsRequired, 0);
            m_uri = decodeUri(uriString);
            delete[] uriString;
        }
        if (uri.scheme.first && uri.scheme.afterLast) {
            m_scheme = reconstructString(uri.scheme);
        }
        if (uri.userInfo.first && uri.userInfo.afterLast) {
            const String userInfo = reconstructString(uri.userInfo);
            const size_t sepPos = userInfo.find(':');
            if (sepPos != String::npos) {
                m_username = userInfo.substr(0, sepPos);
                m_password = userInfo.substr(sepPos + 1);
            } else {
                m_username = userInfo;
            }
        }
        if (uri.hostText.first && uri.hostText.afterLast) {
            m_host = decodeUri(reconstructString(uri.hostText));
        }
        if (uri.portText.first && uri.portText.afterLast) {
            m_port = atoi(reconstructString(uri.portText).data());
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
            m_query = reconstructString(uri.query);
        }
        if (uri.fragment.first && uri.fragment.afterLast) {
            m_fragment = reconstructString(uri.fragment);
        }
    }
    uriFreeUriMembersA(&uri);
}

Uri::Uri()
    : d(Private::empty())
{
}

Uri::Uri(const Uri &uri)
{
    uri.d->ref();
    d = uri.d;
}

Uri::Uri(const String &uri)
    : d(new Private)
{
    d->m_uri = uri;
}

Uri::Uri(const String &path, const String &filename)
    : d(new Private)
{
    if (!path.empty() && !filename.empty() && path[path.size() - 1] == '/') {
        d->m_uri = path + filename;
    } else if (!path.empty() && !filename.empty()) {
        d->m_uri = path + '/' + filename;
    } else if (path.empty()) {
        d->m_uri = filename;
    } else {
        d->m_uri = path;
    }
}

Uri::Uri(const ichar *uri)
    : d(new Private)
{
    if (uri) {
        d->m_uri = uri;
    }
}

Uri::~Uri()
{
    d->deref();
}

String Uri::scheme() const
{
    if (!d->m_initialized) {
        d->initializeContents();
    }
    return d->m_scheme;
}

String Uri::username() const
{
    if (!d->m_initialized) {
        d->initializeContents();
    }
    return d->m_username;
}

String Uri::password() const
{
    if (!d->m_initialized) {
        d->initializeContents();
    }
    return d->m_password;
}

String Uri::host() const
{
    if (!d->m_initialized) {
        d->initializeContents();
    }
    return d->m_host;
}

iint32 Uri::port() const
{
    if (!d->m_initialized) {
        d->initializeContents();
    }
    return d->m_port;
}

String Uri::path() const
{
    if (!d->m_initialized) {
        d->initializeContents();
    }
    return d->m_path;
}

String Uri::filename() const
{
    if (!d->m_initialized) {
        d->initializeContents();
    }
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
    if (!d->m_initialized) {
        d->initializeContents();
    }
    return d->m_isValidUri;
}

bool Uri::empty() const
{
    return d->m_uri.empty();
}

bool Uri::contains(const Uri &uri) const
{
    if (!d->m_initialized) {
        d->initializeContents();
    }
    if (!uri.d->m_initialized) {
        uri.d->initializeContents();
    }
    return d->m_path.find(uri.d->m_path) == 0;
}

Uri &Uri::dirUp()
{
    if (!d->m_initialized) {
        d->initializeContents();
    }
    if (d == Private::m_privateEmpty || d->m_path.empty() || !d->m_path.compare("/")) {
        return *this;
    }
    d->copyAndDetach(this);
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
    if (this == &uri || d == uri.d) {
        return *this;
    }
    d->deref();
    uri.d->ref();
    d = uri.d;
    return *this;
}

Uri &Uri::operator=(const String &uri)
{
    if (uri == d->m_uri) {
        return *this;
    }
    if (uri.empty()) {
        d = Private::empty();
    } else {
        d->newAndDetach(this);
        d->m_uri = uri;
    }
    return *this;
}

Uri &Uri::operator=(const ichar *uri)
{
    if (!uri) {
        return *this;
    }
    d->newAndDetach(this);
    d->m_uri = uri;
    return *this;
}

bool Uri::operator==(const Uri &uri) const
{
    if (this == &uri || d == uri.d) {
        return true;
    }
    return d->m_uri == uri.d->m_uri;
}

bool Uri::operator!=(const Uri &uri) const
{
    return !(*this == uri);
}

}

std::ostream &operator<<(std::ostream &stream, const IdealCore::Uri &uri)
{
    stream << '\"' << uri.uri().data() << '\"';
    return stream;
}
