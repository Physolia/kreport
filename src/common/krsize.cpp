/* This file is part of the KDE project
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "krsize.h"

#include <KProperty>

#include <klocale.h>
#include <QScreen>
#include <QApplication>

KRSize::KRSize(const KoUnit& unit)
{
    m_unit = unit;
    m_property = new KProperty("size", toScene(), i18n("Size"));
}


KRSize::~KRSize()
{
}

void KRSize::setSceneSize(const QSizeF& s, UpdatePropertyFlag update)
{
    qreal w, h;

    QScreen *srn = QApplication::screens().at(0);

    w = INCH_TO_POINT(s.width() / srn->logicalDotsPerInchX());
    h = INCH_TO_POINT(s.height() / srn->logicalDotsPerInchY());
    m_pointSize.setWidth(w);
    m_pointSize.setHeight(h);

    if (update == UpdateProperty)
        m_property->setValue(toUnit());
}

void KRSize::setUnitSize(const QSizeF& s, UpdatePropertyFlag update)
{
    qreal w, h;
    w = m_unit.fromUserValue(s.width());
    h = m_unit.fromUserValue(s.height());
    m_pointSize.setWidth(w);
    m_pointSize.setHeight(h);

    if (update == UpdateProperty)
        m_property->setValue(toUnit());
}

void KRSize::setPointSize(const QSizeF& s, UpdatePropertyFlag update)
{
    m_pointSize.setWidth(s.width());
    m_pointSize.setHeight(s.height());

    if (update == UpdateProperty)
        m_property->setValue(toUnit());
}

void KRSize::setUnit(KoUnit u)
{
    m_unit = u;
    m_property->setValue(toUnit());
}

QSizeF KRSize::toPoint() const
{
    return m_pointSize;
}

QSizeF KRSize::toScene() const
{
    qreal w, h;
    QScreen *srn = QApplication::screens().at(0);
    w = POINT_TO_INCH(m_pointSize.width()) * srn->logicalDotsPerInchX();
    h = POINT_TO_INCH(m_pointSize.height()) * srn->logicalDotsPerInchY();
    return QSizeF(w, h);
}

QSizeF KRSize::toUnit() const
{
    qreal w, h;
    w = m_unit.toUserValue(m_pointSize.width());
    h = m_unit.toUserValue(m_pointSize.height());

    return QSizeF(w, h);
}
