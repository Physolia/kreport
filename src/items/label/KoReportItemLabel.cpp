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

#include "KoReportItemLabel.h"
#include "common/renderobjects.h"
#include "kreportplugin_debug.h"

#include <KPropertySet>

#include <klocalizedstring.h>

#include <QFontDatabase>
#include <QPalette>
#include <QDomNodeList>

KoReportItemLabel::KoReportItemLabel()
{
    createProperties();
}

KoReportItemLabel::KoReportItemLabel(QDomNode & element)
{
    createProperties();
    QDomNodeList nl = element.childNodes();
    QString n;
    QDomNode node;

    m_name->setValue(element.toElement().attribute(QLatin1String("report:name")));
    m_text->setValue(element.toElement().attribute(QLatin1String("report:caption")));
    Z = element.toElement().attribute(QLatin1String("report:z-index")).toDouble();
    m_horizontalAlignment->setValue(element.toElement().attribute(QLatin1String("report:horizontal-align")));
    m_verticalAlignment->setValue(element.toElement().attribute(QLatin1String("report:vertical-align")));

    parseReportRect(element.toElement(), &m_pos, &m_size);

    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();

        if (n == QLatin1String("report:text-style")) {
            KRTextStyleData ts;
            if (parseReportTextStyleData(node.toElement(), ts)) {
                m_backgroundColor->setValue(ts.backgroundColor);
                m_foregroundColor->setValue(ts.foregroundColor);
                m_backgroundOpacity->setValue(ts.backgroundOpacity);
                m_font->setValue(ts.font);

            }
        } else if (n == QLatin1String("report:line-style")) {
            KRLineStyleData ls;
            if (parseReportLineStyleData(node.toElement(), ls)) {
                m_lineWeight->setValue(ls.weight);
                m_lineColor->setValue(ls.lineColor);
                m_lineStyle->setValue(QPen(ls.style));
            }
        } else {
            kreportpluginWarning() << "while parsing label element encountered unknow element: " << n;
        }
    }
}

KoReportItemLabel::~KoReportItemLabel()
{
    delete m_set;
}

QString KoReportItemLabel::text() const
{
    return m_text->value().toString();
}

void KoReportItemLabel::setText(const QString& t)
{
    m_text->setValue(t);
}

void KoReportItemLabel::createProperties()
{
    m_set = new KPropertySet(0, QLatin1String("Label"));

    m_text = new KProperty("caption", QLatin1String("Label"), i18n("Caption"));
    QStringList keys, strings;

    keys << QLatin1String("left") << QLatin1String("center") << QLatin1String("right");
    strings << i18n("Left") << i18n("Center") << i18n("Right");
    m_horizontalAlignment = new KProperty("horizontal-align", keys, strings, QLatin1String("left"), i18n("Horizontal Alignment"));

    keys.clear();
    strings.clear();
    keys << QLatin1String("top") << QLatin1String("center") << QLatin1String("bottom");
    strings << i18n("Top") << i18n("Center") << i18n("Bottom");
    m_verticalAlignment = new KProperty("vertical-align", keys, strings, QLatin1String("center"), i18n("Vertical Alignment"));

    m_font = new KProperty("font", QFontDatabase::systemFont(QFontDatabase::GeneralFont), i18n("Font"), i18n("Font"));

    m_backgroundColor = new KProperty("background-color", QColor(Qt::white), i18n("Background Color"));
    m_foregroundColor = new KProperty("foreground-color", QPalette().color(QPalette::Foreground), i18n("Foreground Color"));

    m_backgroundOpacity = new KProperty("background-opacity", QVariant(0), i18n("Background Opacity"));
    m_backgroundOpacity->setOption("max", 100);
    m_backgroundOpacity->setOption("min", 0);
    m_backgroundOpacity->setOption("unit", QLatin1String("%"));

    m_lineWeight = new KProperty("line-weight", 1, i18n("Line Weight"));
    m_lineColor = new KProperty("line-color", QColor(Qt::black), i18n("Line Color"));
    m_lineStyle = new KProperty("line-style", QPen(Qt::NoPen), i18n("Line Style"), i18n("Line Style"), KProperty::LineStyle);

    addDefaultProperties();
    m_set->addProperty(m_text);
    m_set->addProperty(m_horizontalAlignment);
    m_set->addProperty(m_verticalAlignment);
    m_set->addProperty(m_font);
    m_set->addProperty(m_backgroundColor);
    m_set->addProperty(m_foregroundColor);
    m_set->addProperty(m_backgroundOpacity);
    m_set->addProperty(m_lineWeight);
    m_set->addProperty(m_lineColor);
    m_set->addProperty(m_lineStyle);
}

Qt::Alignment KoReportItemLabel::textFlags() const
{
    Qt::Alignment align;
    QString t;
    t = m_horizontalAlignment->value().toString();
    if (t == QLatin1String("center"))
        align = Qt::AlignHCenter;
    else if (t == QLatin1String("right"))
        align = Qt::AlignRight;
    else
        align = Qt::AlignLeft;

    t = m_verticalAlignment->value().toString();
    if (t == QLatin1String("center"))
        align |= Qt::AlignVCenter;
    else if (t == QLatin1String("bottom"))
        align |= Qt::AlignBottom;
    else
        align |= Qt::AlignTop;

    return align;
}

KRTextStyleData KoReportItemLabel::textStyle()
{
    KRTextStyleData d;
    d.backgroundColor = m_backgroundColor->value().value<QColor>();
    d.foregroundColor = m_foregroundColor->value().value<QColor>();
    d.font = m_font->value().value<QFont>();
    d.backgroundOpacity = m_backgroundOpacity->value().toInt();
    return d;
}

KRLineStyleData KoReportItemLabel::lineStyle()
{
    KRLineStyleData ls;
    ls.weight = m_lineWeight->value().toInt();
    ls.lineColor = m_lineColor->value().value<QColor>();
    ls.style = (Qt::PenStyle)m_lineStyle->value().toInt();
    return ls;
}

// RTTI
QString KoReportItemLabel::typeName() const
{
    return QLatin1String("label");
}

int KoReportItemLabel::renderSimpleData(OROPage *page, OROSection *section, const QPointF &offset,
                                        const QVariant &data, KRScriptHandler *script)
{
    Q_UNUSED(data)
    Q_UNUSED(script)

    OROTextBox * tb = new OROTextBox();
    tb->setPosition(m_pos.toScene() + offset);
    tb->setSize(m_size.toScene());
    tb->setFont(font());
    tb->setText(text());
    tb->setFlags(textFlags());
    tb->setTextStyle(textStyle());
    tb->setLineStyle(lineStyle());

    if (page) {
        page->addPrimitive(tb);
    }

    if (section) {
        OROPrimitive *clone = tb->clone();
        clone->setPosition(m_pos.toScene());
        section->addPrimitive(clone);
    }

    if (!page) {
        delete tb;
    }

    return 0; //Item doesn't stretch the section height
}

