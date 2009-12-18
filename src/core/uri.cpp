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

#include "uri.h"

namespace IdealCore{

class Uri::Private
{
public:
    Private()
        : m_parserPos(0)
        , m_port(-1)
        , m_isValidUri(false)
        , m_refs(1)
        , m_initialized(false)
    {
    }

    virtual ~Private()
    {
    }

    Private *copy() const
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

    void initializeContents();

    static Private *empty();

    bool expectChar(Char c);
    bool parseURI();
    bool parseScheme();
    bool parseHierPart();
    bool parseQuery();
    bool parseFragment();
    bool parseAuthority();
    bool parsePathAbempty();
    bool parsePathAbsolute();
    bool parsePathRootless();
    bool parsePathEmpty();
    bool parseURIReference();
    bool parseRelativeRef();
    bool parseAbsoluteURI();
    bool parseRelativePart();
    bool parseUserinfo();
    bool parseHost();
    bool parsePort();
    bool parsePctEncoded();
    bool parseIPLiteral();
    bool parseIPv4Address();
    bool parseRegName();
    bool parseIPv6Address();
    bool parseIPvFuture();
    bool parseH16();
    bool parseLs32();
    bool parseDecOctet();
    bool parsePathNoScheme();
    bool parseSegment();
    bool parseSegmentNz();
    bool parseSegmentNzNc();
    bool parsePchar();
    bool parseReserved();
    String  m_parserAux;
    size_t  m_parserPos;

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

static const bool is_alpha[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0 - 15
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 16 - 31
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 32 - 47
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 48 - 63
                                 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 64 - 79
                                 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, // 80 - 95
                                 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 96 - 111
                                 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 }; // 112 - 127

static const bool is_digit[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0 - 15
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 16 - 31
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 32 - 47
                                 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, // 48 - 63
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 64 - 79
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 80 - 95
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 96 - 111
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // 112 - 127

static const bool is_gendelim[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0 - 15
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 16 - 31
                                    0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, // 32 - 47
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, // 48 - 63
                                    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 64 - 79
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, // 80 - 95
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 96 - 111
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // 112 - 127

static const bool is_subdelim[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0 - 15
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 16 - 31
                                    0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, // 32 - 47
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, // 48 - 63
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 64 - 79
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 80 - 95
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 96 - 111
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // 112 - 127

static const bool is_hexdig[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0 - 15
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 16 - 31
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 32 - 47
                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, // 48 - 63
                                  0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 64 - 79
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 80 - 95
                                  0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 96 - 111
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // 112 - 127

static const bool is_unreserved[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0 - 15
                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 16 - 31
                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, // 32 - 47
                                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, // 48 - 63
                                      0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 64 - 79
                                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, // 80 - 95
                                      0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 96 - 111
                                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0 }; // 112 - 127

bool Uri::Private::expectChar(Char c)
{
    const Char curr = m_uri[m_parserPos];
    const iuint32 currValue = curr.value();
    if (!currValue || currValue > 127 || curr != c) {
        return false;
    }
    ++m_parserPos;
    return true;
}

bool Uri::Private::parseURI()
{
    if (!parseScheme()) {
        return false;
    }
    if (!expectChar(':')) {
        return false;
    }
    if (!parseHierPart()) {
        return false;
    }
    if (expectChar('?')) {
        if (!parseQuery()) {
            return false;
        }
    }
    if (expectChar('#')) {
        if (!parseFragment()) {
            return false;
        }
    }
    return true;
}

bool Uri::Private::parseScheme()
{
    Char curr = m_uri[m_parserPos];
    iuint32 currValue = curr.value();
    if (!currValue || currValue > 127 || !is_alpha[currValue]) {
        return false;
    }
    while (currValue && currValue < 128 && (is_alpha[currValue] || is_digit[currValue] ||
                                            curr == '+' || curr == '-' || curr == '.')) {
        m_parserAux += curr;
        ++m_parserPos;
        curr = m_uri[m_parserPos];
        currValue = curr.value();
    }
    m_scheme = m_parserAux;
    m_parserAux.clear();
    return true;
}

bool Uri::Private::parseHierPart()
{
    if (expectChar('/')) {
        if (expectChar('/')) {
            if (!parseAuthority()) {
                return false;
            }
            if (!parsePathAbempty()) {
                return false;
            }
            return true;
        } else {
            --m_parserPos;
        }
    }
    if (parsePathAbsolute()) {
        return true;
    }
    if (parsePathRootless()) {
        return true;
    }
    return parsePathEmpty();
}

bool Uri::Private::parseQuery()
{
    while (true) {
        const Char curr = m_uri[m_parserPos];
        if (parsePchar()) {
            continue;
        }
        if (curr == '/' || curr == '?') {
            m_parserAux += curr;
            ++m_parserPos;
            continue;
        }
        return true;
    }
}

bool Uri::Private::parseFragment()
{
    while (true) {
        const Char curr = m_uri[m_parserPos];
        if (parsePchar()) {
            continue;
        }
        if (curr == '/' || curr == '?') {
            m_parserAux += curr;
            ++m_parserPos;
            continue;
        }
        return true;
    }
}

bool Uri::Private::parseAuthority()
{
    const size_t oldParserPos = m_parserPos;
    if (!parseUserinfo()) {
        return false;
    }
    const Char curr = m_uri[m_parserPos];
    if (curr != '@') {
        m_parserPos = oldParserPos;
    }
    if (!parseHost()) {
        return false;
    }
    if (expectChar(':')) {
        if (!parsePort()) {
            return false;
        }
    }
    return true;
}

bool Uri::Private::parsePathAbempty()
{
    Char curr = m_uri[m_parserPos];
    while (curr == '/') {
        m_parserAux += '/';
        ++m_parserPos;
        if (!parseSegment()) {
            return false;
        }
        curr = m_uri[m_parserPos];
    }
    m_path = m_parserAux;
    m_parserAux.clear();
    return true;
}

bool Uri::Private::parsePathAbsolute()
{
    if (!expectChar('/')) {
        return false;
    }
    m_parserAux = '/';
    const size_t oldParserPos = m_parserPos;
    if (parseSegmentNz()) {
        Char curr = m_uri[m_parserPos];
        while (curr == '/') {
            m_parserAux += '/';
            ++m_parserPos;
            if (!parseSegment()) {
                return false;
            }
            curr = m_uri[m_parserPos];
        }
    } else {
        m_parserPos = oldParserPos;
    }
    m_path = m_parserAux;
    m_parserAux.clear();
    return true;
}

bool Uri::Private::parsePathRootless()
{
    if (!parseSegmentNz()) {
        return false;
    }
    Char curr = m_uri[m_parserPos];
    while (curr == '/') {
        m_parserAux += '/';
        ++m_parserPos;
        if (!parseSegment()) {
            return false;
        }
        curr = m_uri[m_parserPos];
    }
    m_path = m_parserAux;
    m_parserAux.clear();
    return true;
}

bool Uri::Private::parsePathEmpty()
{
    const Char curr = m_uri[m_parserPos];
    return !curr.value();
}

bool Uri::Private::parseURIReference()
{
    if (parseURI()) {
        return true;
    }
    m_parserPos = 0;
    return parseRelativeRef();
}

bool Uri::Private::parseAbsoluteURI()
{
    if (!parseScheme()) {
        return false;
    }
    if (!expectChar(':')) {
        return false;
    }
    if (!parseHierPart()) {
        return false;
    }
    if (expectChar('?')) {
        if (!parseQuery()) {
            return false;
        }
    }
    return true;
}

bool Uri::Private::parseRelativePart()
{
    if (expectChar('/')) {
        if (expectChar('/')) {
            if (!parseAuthority()) {
                return false;
            }
            if (!parsePathAbempty()) {
                return false;
            }
            return true;
        } else {
            --m_parserPos;
        }
    }
    if (parsePathAbsolute()) {
        return true;
    }
    if (parsePathNoScheme()) {
        return true;
    }
    return parsePathEmpty();
}

bool Uri::Private::parseRelativeRef()
{
    if (!parseRelativePart()) {
        return false;
    }
    if (expectChar('?')) {
        if (!parseQuery()) {
            return false;
        }
    }
    if (expectChar('#')) {
        if (!parseFragment()) {
            return false;
        }
    }
    return true;
}

bool Uri::Private::parseUserinfo()
{
    while (true) {
        const Char curr = m_uri[m_parserPos];
        const iuint32 currValue = curr.value();
        if (!currValue) {
            return true;
        }
        if (currValue < 128 && is_unreserved[currValue]) {
            m_parserAux += curr;
            ++m_parserPos;
            continue;
        }
        if (parsePctEncoded()) {
            continue;
        }
        if (currValue < 128 && (is_subdelim[currValue] ||
                                curr == ':')) {
            m_parserAux += curr;
            ++m_parserPos;
            continue;
        }
        return true;
    }
}

bool Uri::Private::parseHost()
{
    if (parseIPLiteral()) {
        return true;
    }
    if (parseIPv4Address()) {
        return true;
    }
    return parseRegName();
}

bool Uri::Private::parsePort()
{
    Char curr = m_uri[m_parserPos];
    iuint32 currValue = curr.value();
    while (currValue && currValue < 128 && is_digit[currValue]) {
        m_parserAux += curr;
        m_parserPos++;
        curr = m_uri[m_parserPos];
        currValue = curr.value();
    }
    return true;
}

bool Uri::Private::parsePctEncoded()
{
    if (!expectChar('%')) {
        return false;
    }
    m_parserAux += '%';
    Char curr = m_uri[m_parserPos];
    size_t currValue = curr.value();
    if (!currValue || currValue > 127 || !is_hexdig[currValue]) {
        return false;
    }
    m_parserAux += curr;
    ++m_parserPos;
    curr = m_uri[m_parserPos];
    currValue = curr.value();
    if (!currValue || currValue > 127 || !is_hexdig[currValue]) {
        return false;
    }
    m_parserAux += curr;
    ++m_parserPos;
    return true;
}

bool Uri::Private::parseIPLiteral()
{
    if (!expectChar('[')) {
        return false;
    }
    if (parseIPv6Address()) {
        if (!expectChar(']')) {
            return false;
        }
        return true;
    }
    if (parseIPvFuture()) {
        if (!expectChar(']')) {
            return false;
        }
        return true;
    }
    return false;
}

bool Uri::Private::parseIPv4Address()
{
    if (!parseDecOctet()) {
        return false;
    }
    if (!expectChar('.')) {
        return false;
    }
    if (!parseDecOctet()) {
        return false;
    }
    if (!expectChar('.')) {
        return false;
    }
    if (!parseDecOctet()) {
        return false;
    }
    if (!expectChar('.')) {
        return false;
    }
    if (!parseDecOctet()) {
        return false;
    }
    return true;
}

bool Uri::Private::parseRegName()
{
    while (true) {
        const Char curr = m_uri[m_parserPos];
        const size_t currValue = curr.value();
        if (is_unreserved[currValue]) {
            m_parserAux += curr;
            ++m_parserPos;
            continue;
        }
        if (parsePctEncoded()) {
            continue;
        }
        if (is_subdelim[currValue]) {
            m_parserAux += curr;
            ++m_parserPos;
            continue;
        }
        return true;
    }
}

bool Uri::Private::parseIPv6Address()
{
    // TODO
    return true;
}

bool Uri::Private::parseIPvFuture()
{
    if (!expectChar('v')) {
        return false;
    }
    Char curr = m_uri[m_parserPos];
    size_t currValue = curr.value();
    if (!is_hexdig[currValue]) {
        return false;
    }
    ++m_parserPos;
    if (!expectChar('.')) {
        return false;
    }
    curr = m_uri[m_parserPos];
    currValue = curr.value();
    if (is_unreserved[currValue]) {
        ++m_parserPos;
        return true;
    }
    if (is_subdelim[currValue]) {
        ++m_parserPos;
        return true;
    }
    if (curr == ':') {
        ++m_parserPos;
        return true;
    }
    return false;
}

bool Uri::Private::parseH16()
{
    for (size_t i = 0; i < 4; ++i) {
        const Char curr = m_uri[m_parserPos];
        const size_t currValue = curr.value();
        if (!currValue || currValue > 127 || !is_hexdig[currValue]) {
            return false;
        }
        m_parserAux += curr;
        ++m_parserPos;
    }
    return true;
}

bool Uri::Private::parseLs32()
{
    if (parseH16()) {
        if (!expectChar(':')) {
            return false;
        }
        return parseH16();
    }
    return parseIPv4Address();
}

bool Uri::Private::parseDecOctet()
{
    // TODO
    return true;
}

bool Uri::Private::parsePathNoScheme()
{
    if (!parseSegmentNzNc()) {
        return false;
    }
    Char curr = m_uri[m_parserPos];
    while (curr == '/') {
        m_parserAux += '/';
        ++m_parserPos;
        if (!parseSegment()) {
            return false;
        }
        curr = m_uri[m_parserPos];
    }
    m_path = m_parserAux;
    m_parserAux.clear();
    return true;
}

bool Uri::Private::parseSegment()
{
    while (parsePchar()) { }
    return true;
}

bool Uri::Private::parseSegmentNz()
{
    return parsePchar();
}

bool Uri::Private::parseSegmentNzNc()
{
    const Char curr = m_uri[m_parserPos];
    const iuint32 currValue = curr.value();
    if (is_unreserved[currValue]) {
        m_parserAux += curr;
        ++m_parserPos;
        return true;
    }
    if (parsePctEncoded()) {
        return true;
    }
    if (is_subdelim[currValue]) {
        m_parserAux += curr;
        ++m_parserPos;
        return true;
    }
    if (curr == '@') {
        m_parserAux += '@';
        ++m_parserPos;
        return true;
    }
    return false;
}

bool Uri::Private::parsePchar()
{
    const Char curr = m_uri[m_parserPos];
    const iuint32 currValue = curr.value();
    if (!currValue) {
        return false;
    }
    if (is_unreserved[currValue]) {
        m_parserAux += curr;
        ++m_parserPos;
        return true;
    }
    if (parsePctEncoded()) {
        return true;
    }
    if (is_subdelim[currValue]) {
        m_parserAux += curr;
        ++m_parserPos;
        return true;
    }
    if (curr == ':' || curr == '@') {
        m_parserAux += curr;
        ++m_parserPos;
        return true;
    }
    return false;
}

bool Uri::Private::parseReserved()
{
    const Char curr = m_uri[m_parserPos];
    const iuint32 currValue = curr.value();
    if (is_gendelim[currValue] || is_subdelim[currValue]) {
        m_parserAux += curr;
        ++m_parserPos;
        return true;
    }
    return false;
}

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
#if 0
    for (size_t i = 0; i < uri.size(); ++i) {
        const Char c = uri[i];
        const iuint32 value = c.value();
        if (value < 128 && uri_unreserved[value]) {
            res += c;
        } else {
            res += getHex(c);
        }
    }
#else
    // Parse here
#endif
    return res;
}

String Uri::Private::decodeUri(const String &uri) const
{
    if (!uri.contains('%')) {
        return uri;
    }
    String res;
#if 0
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
#else
    // Parse here
#endif
    return res;
}

void Uri::Private::initializeContents()
{
    m_initialized = true;
    parseURIReference();
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

String Uri::query() const
{
    if (!d->m_initialized) {
        d->initializeContents();
    }
    return d->m_query;
}

String Uri::fragment() const
{
    if (!d->m_initialized) {
        d->initializeContents();
    }
    return d->m_fragment;
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
