/* This file is part of the KDE project
 * Copyright (C) 2007-2010 by Adam Pigg (adam@piggz.co.uk)
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

#include "KReportData.h"
#include <QVariant>

#define KReportDataSortedFieldPrivateArgs(o) std::tie(o.field, o.order)

class KReportData::SortedField::Private 
{
    
public:
    Private() {}
    Private(const Private& other) {
            KReportDataSortedFieldPrivateArgs((*this)) = KReportDataSortedFieldPrivateArgs(other);
    }
    QString field;
    Qt::SortOrder order = Qt::AscendingOrder;
};

class KReportData::Private
{
public:
    bool dummy = true;
};

//==========KReportData::SortedField==========

KReportData::SortedField::SortedField()
    : d(new Private)
{
}

KReportData::SortedField::SortedField(const KReportData::SortedField& other) : d(new Private(*other.d))
{
}


KReportData::SortedField::~SortedField()
{
    delete d;
}

KReportData::SortedField & KReportData::SortedField::operator=(const KReportData::SortedField& other)
{
    if (this != &other) {
        setField(other.field());
        setOrder(other.order());
    }
    return *this;
}

bool KReportData::SortedField::operator==(const KReportData::SortedField& other) const
{
    return KReportDataSortedFieldPrivateArgs((*d)) == KReportDataSortedFieldPrivateArgs((*other.d));
}

bool KReportData::SortedField::operator!=(const KReportData::SortedField& other) const
{
    return KReportDataSortedFieldPrivateArgs((*d)) != KReportDataSortedFieldPrivateArgs((*other.d));
}

QString KReportData::SortedField::field() const
{
    return d->field;
}

Qt::SortOrder KReportData::SortedField::order() const
{
    return d->order;
}

void KReportData::SortedField::setField(const QString& field)
{
    d->field = field;
}

void KReportData::SortedField::setOrder(Qt::SortOrder order)
{
    d->order = order;
}


//==========KReportData==========

KReportData::KReportData() : d(new Private())
{
}

KReportData::~KReportData()
{
    delete d;
}

QStringList KReportData::fieldKeys() const
{
    return fieldNames();
}

QString KReportData::sourceName() const
{
    return QString();
}

QString KReportData::sourceClass() const
{
    return QString();
}

void KReportData::setSorting(const QList<SortedField> &sorting)
{
    Q_UNUSED(sorting);
}

void KReportData::addExpression(const QString &field, const QVariant &value, char relation)
{
    Q_UNUSED(field);
    Q_UNUSED(value);
    Q_UNUSED(relation);
}

#ifdef KREPORT_SCRIPTING
QStringList KReportData::scriptList() const
{
    return QStringList();
}

QString KReportData::scriptCode(const QString &script) const
{
    Q_UNUSED(script);
    return QString();
}
#endif

QStringList KReportData::dataSources() const
{
    return QStringList();
}

QStringList KReportData::dataSourceNames() const
{
    return dataSources();
}

KReportData* KReportData::create(const QString &source) const
{
    Q_UNUSED(source);
    return 0;
}
