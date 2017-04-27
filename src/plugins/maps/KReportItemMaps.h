/*
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 * Copyright (C) 2011-2015 by Radoslaw Wicik (radoslaw@wicik.pl)
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

#ifndef KREPORTITEMMAPS_H
#define KREPORTITEMMAPS_H

#include "KReportAsyncItemBase.h"

#include <marble/MarbleWidget.h>
#include <marble/MapThemeManager.h>

#include <KProperty>

#include "KReportMapRenderer.h"

#include <QDomNode>

class OROImage;
class OROPicture;
class OROPage;
class OROSection;

namespace Scripting
{
class Maps;
}

class KReportItemMaps : public KReportAsyncItemBase
{
    Q_OBJECT
public:
    KReportItemMaps();
    explicit KReportItemMaps(const QDomNode &element);
    ~KReportItemMaps() override;

    QString typeName() const override;
    int renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset, const QVariant &data, KReportScriptHandler *script) override;

    QString itemDataSource() const override;
    virtual QVariant realItemData(const QVariant &itemData) const;

    void renderFinished();

    qreal longtitude() const;
    qreal latitude() const;
    int zoom() const;
    QString themeId() const;

    OROPicture* oroImage();

protected:
    KProperty* m_controlSource;
    KProperty* m_latitudeProperty;
    KProperty* m_longitudeProperty;
    KProperty* m_zoomProperty;
    KProperty* m_themeProperty;

    void setColumn(const QString&);

    qreal m_longtitude;
    qreal m_latitude;
    int m_zoom;
    OROPage *m_pageId;
    OROSection *m_sectionId;
    QPointF m_offset;
    OROPicture * m_oroPicture;
    KReportMapRenderer m_mapRenderer;
    Marble::MapThemeManager m_themeManager;

private:
    void createProperties() override;
    void deserializeData(const QVariant& serialized);

    bool m_longDataSetFromScript;
    bool m_latDataSetFromScript;
    bool m_zoomDataSetFromScript;

    friend class Scripting::Maps;
};

#endif
