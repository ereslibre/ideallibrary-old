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

#ifndef MATRIX_P_H_XORG
#define MATRIX_P_H_XORG

#include <cairo.h>

#include <gui/private/matrix_p.h>

namespace IdealGUI {

class Matrix::PrivateImpl
    : public Matrix::Private
{
public:
    PrivateImpl(ireal xx, ireal yx, ireal xy, ireal yy, ireal x0, ireal y0);
    PrivateImpl(const Matrix &matrix);
    PrivateImpl();
    virtual ~PrivateImpl();

    cairo_matrix_t m_matrix;
};

}

#endif //MATRIX_P_H_XORG
