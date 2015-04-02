/*
 * Kexi Report Plugin
 * Copyright (C) 2009-2010 by Adam Pigg (adam@piggz.co.uk)
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

#include "KoReportItemCheck.h"
#include "common/renderobjects.h"
#include "renderer/scripting/krscripthandler.h"

#include <KProperty/Set.h>

#include <QPalette>

KoReportItemCheck::KoReportItemCheck()
{
    createProperties();
}

KoReportItemCheck::KoReportItemCheck(QDomNode &element)
{
    createProperties();
    QDomNodeList nl = element.childNodes();
    QString n;
    QDomNode node;

    m_name->setValue(element.toElement().attribute("report:name"));
    m_controlSource->setValue(element.toElement().attribute("report:item-data-source"));
    Z = element.toElement().attribute("report:z-index").toDouble();
    m_foregroundColor->setValue(QColor(element.toElement().attribute("fo:foreground-color")));
    m_checkStyle->setValue(element.toElement().attribute("report:check-style"));
    m_staticValue->setValue(QVariant(element.toElement().attribute("report:value")).toBool());

    parseReportRect(element.toElement(), &m_pos, &m_size);

    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();

        if (n == "report:line-style") {
            KRLineStyleData ls;
            if (parseReportLineStyleData(node.toElement(), ls)) {
                m_lineWeight->setValue(ls.weight);
                m_lineColor->setValue(ls.lineColor);
                m_lineStyle->setValue(QPen(ls.style));
            }
        } else {
            qWarning() << "while parsing check element encountered unknow element: " << n;
        }
    }

}

KoReportItemCheck::~KoReportItemCheck()
{
    delete m_set;
}

void KoReportItemCheck::createProperties()
{
    m_set = new KoProperty::Set(0, "Check");

    QStringList keys, strings;

    keys << "Cross" << "Tick" << "Dot";
    strings << tr("Cross") << tr("Tick") << tr("Dot");
    m_checkStyle = new KoProperty::Property("check-style", keys, strings, "Cross", tr("Style"));

    m_controlSource = new KoProperty::Property("item-data-source", QStringList(), QStringList(), QString(), tr("Data Source"));
    m_controlSource->setOption("extraValueAllowed", "true");

    m_foregroundColor = new KoProperty::Property("foreground-color", QPalette().color(QPalette::Foreground), tr("Foreground Color"));

    m_lineWeight = new KoProperty::Property("line-weight", 1, tr("Line Weight"));
    m_lineColor = new KoProperty::Property("line-color", QColor(Qt::black), tr("Line Color"));
    m_lineStyle = new KoProperty::Property("line-style", QPen(Qt::SolidLine), tr("Line Style"), tr("Line Style"), KoProperty::LineStyle);
    m_staticValue = new KoProperty::Property("value", QVariant(false), tr("Value"), tr("Value used if not bound to a field"));

    addDefaultProperties();
    m_set->addProperty(m_controlSource);
    m_set->addProperty(m_staticValue);
    m_set->addProperty(m_checkStyle);
    m_set->addProperty(m_foregroundColor);
    m_set->addProperty(m_lineWeight);
    m_set->addProperty(m_lineColor);
    m_set->addProperty(m_lineStyle);
}

KRLineStyleData KoReportItemCheck::lineStyle()
{
    KRLineStyleData ls;
    ls.weight = m_lineWeight->value().toInt();
    ls.lineColor = m_lineColor->value().value<QColor>();
    ls.style = (Qt::PenStyle)m_lineStyle->value().toInt();
    return ls;
}

QString KoReportItemCheck::itemDataSource() const
{
    return m_controlSource->value().toString();
}

// RTTI
QString KoReportItemCheck::typeName() const
{
    return "check";
}

int KoReportItemCheck::renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset,
                                        const QVariant &data, KRScriptHandler *script)
{
    OROCheck *chk = new OROCheck();

    chk->setPosition(m_pos.toScene() + offset);
    chk->setSize(m_size.toScene());

    chk->setLineStyle(lineStyle());
    chk->setForegroundColor(m_foregroundColor->value().value<QColor>());
    chk->setCheckType(m_checkStyle->value().toString());

    QString str;
    bool v = false;
    QString cs = itemDataSource();

    //kDebug() << "ControlSource:" << cs;
    if (!cs.isEmpty()) {
        if (cs.left(1) == "=" && script) {
            str = script->evaluate(cs.mid(1)).toString();
        } else {
            str = data.toString();
        }

        str = str.toLower();

        //kDebug() << "Check Value:" << str;
        if (str == "t" || str == "y" || str == "true" || str == "1")
            v = true;

    } else {
        v = value();
    }

    chk->setValue(v);

    if (page) {
        page->addPrimitive(chk);
    }

    if (section) {
        OROCheck *chk2 = dynamic_cast<OROCheck*>(chk->clone());
        chk2->setPosition(m_pos.toPoint());
        section->addPrimitive(chk2);
    }

    if (!page) {
        delete chk;
    }

    return 0; //Item doesn't stretch the section height
}

bool KoReportItemCheck::value()
{
    return m_staticValue->value().toBool();
}

void KoReportItemCheck::setValue(bool v)
{
    m_staticValue->setValue(v);
}
