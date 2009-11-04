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
#include "matrix_p.h"

namespace IdealGUI {

Matrix::PrivateImpl::PrivateImpl(ireal xx, ireal yx, ireal xy, ireal yy, ireal x0, ireal y0)
{
    cairo_matrix_init(&m_matrix, xx, yx, xy, yy, x0, y0);
}

Matrix::PrivateImpl::PrivateImpl(const Matrix &matrix)
{
    cairo_matrix_init(&m_matrix,
                      static_cast<PrivateImpl*>(matrix.d)->m_matrix.xx,
                      static_cast<PrivateImpl*>(matrix.d)->m_matrix.yx,
                      static_cast<PrivateImpl*>(matrix.d)->m_matrix.xy,
                      static_cast<PrivateImpl*>(matrix.d)->m_matrix.yy,
                      static_cast<PrivateImpl*>(matrix.d)->m_matrix.x0,
                      static_cast<PrivateImpl*>(matrix.d)->m_matrix.y0);
}

Matrix::PrivateImpl::PrivateImpl()
{
}

Matrix::PrivateImpl::~PrivateImpl()
{
}

Matrix Matrix::identity()
{
    Matrix m;
    cairo_matrix_init_identity(&static_cast<PrivateImpl*>(m.d)->m_matrix);
    return m;
}

Matrix Matrix::translate(ireal tx, ireal ty)
{
    Matrix m;
    cairo_matrix_init_translate(&static_cast<PrivateImpl*>(m.d)->m_matrix, tx, ty);
    return m;
}

Matrix Matrix::scale(ireal sx, ireal sy)
{
    Matrix m;
    cairo_matrix_init_scale(&static_cast<PrivateImpl*>(m.d)->m_matrix, sx, sy);
    return m;
}

Matrix Matrix::rotate(ireal radians)
{
    Matrix m;
    cairo_matrix_init_rotate(&static_cast<PrivateImpl*>(m.d)->m_matrix, radians);
    return m;
}

}
