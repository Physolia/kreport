/* This file is part of the KDE project
   Copyright Shreya Pandit <shreya@shreyapandit.com>
   Copyright 2011 Adam Pigg <adam@piggz.co.uk>
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KoReportDesignerItemWeb.h"
#include <KoReportDesignerItemBase.h>
#include <KoReportDesigner.h>

#include <KProperty>
#include <KPropertySet>
#include <KPropertyEditorView>

#include <klocalizedstring.h>

#include <QWebFrame>
#include <QWebPage>
#include <QImageWriter>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QBuffer>
#include <QDomDocument>
#include <QPainter>
#include <QDebug>

void KoReportDesignerItemWeb::init(QGraphicsScene *scene, KoReportDesigner *d) //done,compared,add function if necessary
{
    qDebug();
    if (scene)
        scene->addItem(this);

    connect(m_set, SIGNAL(propertyChanged(KPropertySet&,KProperty&)), this, SLOT(slotPropertyChanged(KPropertySet&,KProperty&)));
    KoReportDesignerItemRectBase::init(&m_pos, &m_size, m_set, d);
    setZValue(Z);
}

KoReportDesignerItemWeb::KoReportDesignerItemWeb(KoReportDesigner *rw, QGraphicsScene *scene,
                                                 const QPointF &pos)     //done,compared
    : KoReportDesignerItemRectBase(rw)
{
    Q_UNUSED(pos);
    init(scene, rw);
    setSceneRect(properRect(*rw, KOREPORT_ITEM_RECT_DEFAULT_WIDTH, KOREPORT_ITEM_RECT_DEFAULT_WIDTH));
    m_name->setValue(m_reportDesigner->suggestEntityName(typeName()));
}

KoReportDesignerItemWeb::KoReportDesignerItemWeb(QDomNode &element, KoReportDesigner *rw,
                                                 QGraphicsScene *scene)      //done,compared
    : KoReportItemWeb(element), KoReportDesignerItemRectBase(rw)
{
    init(scene, rw);
    setSceneRect(m_pos.toScene(), m_size.toScene());
}

KoReportDesignerItemWeb *KoReportDesignerItemWeb::clone() //done,compared
{
    QDomDocument d;
    QDomElement e = d.createElement("clone");
    QDomNode n;
    buildXML(d, e);
    n = e.firstChild();
    return new KoReportDesignerItemWeb(n, designer(), 0);
}

KoReportDesignerItemWeb::~KoReportDesignerItemWeb() //done,compared
{
    // do we need to clean anything up?
}

void KoReportDesignerItemWeb::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    painter->drawRect(QGraphicsRectItem::rect());
    painter->drawText(rect(), 0, dataSourceAndObjectTypeName(itemDataSource(), "web-view"));
    
    painter->setBackgroundMode(Qt::TransparentMode);
    
    drawHandles(painter);
}

void KoReportDesignerItemWeb::buildXML(QDomDocument &doc, QDomElement &parent)
{
    Q_UNUSED(parent);
    QDomElement entity = doc.createElement(QLatin1String("report:") + typeName());

    // properties
    addPropertyAsAttribute(&entity, m_controlSource);
    addPropertyAsAttribute(&entity, m_name);
    entity.setAttribute("report:z-index", zValue());
    buildXMLRect(doc, entity, &m_pos, &m_size);
    parent.appendChild(entity);
}

void KoReportDesignerItemWeb::slotPropertyChanged(KPropertySet &s, KProperty &p)
{
    if (p.name() == "Name") {
        if (!m_reportDesigner->isEntityNameUnique(p.value().toString(), this)) {
            p.setValue(m_oldName);
        }
        else {
            m_oldName = p.value().toString();
        }
    }

    KoReportDesignerItemRectBase::propertyChanged(s, p);
    if (m_reportDesigner) {
        m_reportDesigner->setModified(true);
    }
}

void KoReportDesignerItemWeb::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_controlSource->setListData(m_reportDesigner->fieldKeys(), m_reportDesigner->fieldNames());
    KoReportDesignerItemRectBase::mousePressEvent(event);
}
