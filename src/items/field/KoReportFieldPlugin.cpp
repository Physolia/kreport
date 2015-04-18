/* This file is part of the KDE project
   Copyright (C) 2010 by Adam Pigg (adam@piggz.co.uk)

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "KoReportFieldPlugin.h"
#include "KoReportItemField.h"
#include "KoReportDesignerItemField.h"
#include "common/KoReportPluginInfo.h"
#ifdef KREPORT_SCRIPTING
#include "krscriptfield.h"
#endif

#include <QIcon>

KoReportFieldPlugin::KoReportFieldPlugin(QObject *parent, const QVariantList &args) : KoReportPluginInterface(parent)
{
    Q_UNUSED(args)

    KoReportPluginInfo *info = new KoReportPluginInfo();
    info->setClassName("field");
    info->setIcon(QIcon::fromTheme(QLatin1String("edit-rename")));
    info->setName(tr("Field"));
    info->setPriority(2);
    setInfo(info);
}

KoReportFieldPlugin::~KoReportFieldPlugin()
{

}

QObject* KoReportFieldPlugin::createRendererInstance(QDomNode& element)
{
    return new KoReportItemField(element);
}

QObject* KoReportFieldPlugin::createDesignerInstance(QDomNode& element, KoReportDesigner* designer, QGraphicsScene* scene)
{
    return new KoReportDesignerItemField(element, designer, scene);
}

QObject* KoReportFieldPlugin::createDesignerInstance(KoReportDesigner* designer, QGraphicsScene* scene, const QPointF& pos)
{
    return new KoReportDesignerItemField(designer, scene, pos);
}

#ifdef KREPORT_SCRIPTING
QObject* KoReportFieldPlugin::createScriptInstance(KoReportItemBase* item)
{
    KoReportItemField *field = dynamic_cast<KoReportItemField*>(item);
    if (field) {
        return new Scripting::Field(field);
    }
    return 0;
}
#endif
