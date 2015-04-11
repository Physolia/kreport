/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
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


#include "KoReportDesignerItemBase.h"
#include "KoReportDesignerItemBarcode.h"
#include "KoReportDesigner.h"

#include <QDomDocument>
#include <QPainter>
#include <kdebug.h>
#include <klocalizedstring.h>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include "barcodepaint.h"

#include <KProperty>
#include <KPropertySet>
#include <KPropertyEditorView>
//
// class ReportEntityBarcode
//

void KoReportDesignerItemBarcode::init(QGraphicsScene *scene, KoReportDesigner *d)
{
    if (scene)
        scene->addItem(this);

    connect(m_set, SIGNAL(propertyChanged(KPropertySet&,KProperty&)),
            this, SLOT(slotPropertyChanged(KPropertySet&,KProperty&)));

    KoReportDesignerItemRectBase::init(&m_pos, &m_size, m_set, d);
    setMaxLength(5);
    setZValue(Z);
    
    updateRenderText(m_itemValue->value().toString().isEmpty() ?  m_format->value().toString() : QString(), m_itemValue->value().toString(), QString());

}
// methods (constructors)
KoReportDesignerItemBarcode::KoReportDesignerItemBarcode(KoReportDesigner * rw, QGraphicsScene* scene, const QPointF &pos)
        : KoReportDesignerItemRectBase(rw)
{
    Q_UNUSED(pos);
    init(scene, rw);
    setSceneRect(properRect(*rw, m_minWidthTotal*m_dpiX, m_minHeight*m_dpiY));
    m_name->setValue(m_reportDesigner->suggestEntityName(typeName()));
}

KoReportDesignerItemBarcode::KoReportDesignerItemBarcode(QDomNode & element, KoReportDesigner * rw, QGraphicsScene* scene)
        : KoReportItemBarcode(element), KoReportDesignerItemRectBase(rw)
{
    init(scene, rw);
    setSceneRect(m_pos.toScene(), m_size.toScene());
}

KoReportDesignerItemBarcode* KoReportDesignerItemBarcode::clone()
{
    QDomDocument d;
    QDomElement e = d.createElement("clone");;
    QDomNode n;
    buildXML(d, e);
    n = e.firstChild();
    return new KoReportDesignerItemBarcode(n, designer(), 0);
}

// methods (deconstructor)
KoReportDesignerItemBarcode::~KoReportDesignerItemBarcode()
{}

QRect KoReportDesignerItemBarcode::getTextRect()
{
    QFont fnt = QFont();
    return QFontMetrics(fnt).boundingRect(int (x()), int (y()), 0, 0, 0, dataSourceAndObjectTypeName(itemDataSource(), "barcode"));
}

void KoReportDesignerItemBarcode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // store any values we plan on changing so we can restore them
    QPen  p = painter->pen();

    painter->setBackground(Qt::white);

    //Draw a border so user knows the object edge
    painter->setPen(QPen(QColor(224, 224, 224)));
    painter->drawRect(rect());

    drawHandles(painter);

    if (m_format->value().toString() == "i2of5")
        renderI2of5(rect().toRect(), m_renderText, alignment(), painter);
    else if (m_format->value().toString() == "3of9")
        render3of9(rect().toRect(), m_renderText, alignment(), painter);
    else if (m_format->value().toString() == "3of9+")
        renderExtended3of9(rect().toRect(), m_renderText, alignment(), painter);
    else if (m_format->value().toString() == "128")
        renderCode128(rect().toRect(), m_renderText, alignment(), painter);
    else if (m_format->value().toString() == "upc-a")
        renderCodeUPCA(rect().toRect(), m_renderText, alignment(), painter);
    else if (m_format->value().toString() == "upc-e")
        renderCodeUPCE(rect().toRect(), m_renderText, alignment(), painter);
    else if (m_format->value().toString() == "ean13")
        renderCodeEAN13(rect().toRect(), m_renderText, alignment(), painter);
    else if (m_format->value().toString() == "ean8")
        renderCodeEAN8(rect().toRect(), m_renderText, alignment(), painter);

    painter->setPen(Qt::black);
    painter->drawText(rect(), 0, dataSourceAndObjectTypeName(itemDataSource(), "barcode"));

    // restore an values before we started just in case
    painter->setPen(p);
}

void KoReportDesignerItemBarcode::buildXML(QDomDocument & doc, QDomElement & parent)
{
    //kdDebug() << "ReportEntityField::buildXML()");
    QDomElement entity = doc.createElement(QLatin1String("report:") + typeName());

    // properties
    addPropertyAsAttribute(&entity, m_name);
    addPropertyAsAttribute(&entity, m_controlSource);
    addPropertyAsAttribute(&entity, m_horizontalAlignment);
    addPropertyAsAttribute(&entity, m_format);
    addPropertyAsAttribute(&entity, m_maxLength);
    entity.setAttribute("report:z-index", zValue());
    addPropertyAsAttribute(&entity, m_itemValue);

    // bounding rect
    buildXMLRect(doc, entity, &m_pos, &m_size);

    parent.appendChild(entity);
}

void KoReportDesignerItemBarcode::slotPropertyChanged(KPropertySet &s, KProperty &p)
{
    if (p.name() == "Name") {
        //For some reason p.oldValue returns an empty string
        if (!m_reportDesigner->isEntityNameUnique(p.value().toString(), this)) {
            p.setValue(m_oldName);
        } else {
            m_oldName = p.value().toString();
        }
    }
    
    updateRenderText(m_itemValue->value().toString().isEmpty() ?  m_format->value().toString() : QString(), m_itemValue->value().toString(), QString());

    KoReportDesignerItemRectBase::propertyChanged(s, p);
    if (m_reportDesigner) m_reportDesigner->setModified(true);
}

void KoReportDesignerItemBarcode::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    m_controlSource->setListData(m_reportDesigner->fieldKeys(), m_reportDesigner->fieldNames());
    KoReportDesignerItemRectBase::mousePressEvent(event);
}

