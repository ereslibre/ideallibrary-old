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

#include <gui/matrix.h>

#include <cairo.h>

namespace IdealGUI {

class Matrix::Private
{
public:
    cairo_matrix_t m_matrix;
};

Matrix::Matrix(double xx, double yx, double xy, double yy, double x0, double y0)
    : d(new Private)
{
    cairo_matrix_init(&d->m_matrix, xx, yx, xy, yy, x0, y0);
}

Matrix::Matrix(const Matrix &matrix)
    : d(new Private)
{
    cairo_matrix_init(&d->m_matrix, matrix.d->m_matrix.xx, matrix.d->m_matrix.yx, matrix.d->m_matrix.xy, matrix.d->m_matrix.yy, matrix.d->m_matrix.x0, matrix.d->m_matrix.y0);
}

Matrix::~Matrix()
{
    delete d;
}

Matrix Matrix::identity()
{
    Matrix m;
    cairo_matrix_init_identity(&m.d->m_matrix);
    return m;
}

Matrix Matrix::translate(double tx, double ty)
{
    Matrix m;
    cairo_matrix_init_translate(&m.d->m_matrix, tx, ty);
    return m;
}

Matrix Matrix::scale(double sx, double sy)
{
    Matrix m;
    cairo_matrix_init_scale(&m.d->m_matrix, sx, sy);
    return m;
}

Matrix Matrix::rotate(double radians)
{
    Matrix m;
    cairo_matrix_init_rotate(&m.d->m_matrix, radians);
    return m;
}

Matrix::Matrix()
    : d(new Private)
{
}

}
