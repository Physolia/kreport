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

#include "KoReportImagePlugin.h"
#include "KoReportItemImage.h"
#include "KoReportDesignerItemImage.h"
#include "common/KReportPluginMetaData.h"
#ifdef KREPORT_SCRIPTING
#include "krscriptimage.h"
#endif

KREPORT_PLUGIN_FACTORY(KoReportImagePlugin, "image.json")

KoReportImagePlugin::KoReportImagePlugin(QObject *parent, const QVariantList &args)
    : KoReportPluginInterface(parent, args)
{
    Q_UNUSED(args)
}

KoReportImagePlugin::~KoReportImagePlugin()
{

}

QObject* KoReportImagePlugin::createRendererInstance(const QDomNode& element)
{
    return new KoReportItemImage(element);
}

QObject* KoReportImagePlugin::createDesignerInstance(const QDomNode& element, KoReportDesigner* designer, QGraphicsScene* scene)
{
    return new KoReportDesignerItemImage(element, designer, scene);
}

QObject* KoReportImagePlugin::createDesignerInstance(KoReportDesigner* designer, QGraphicsScene* scene, const QPointF& pos)
{
    return new KoReportDesignerItemImage(designer, scene, pos);
}

#ifdef KREPORT_SCRIPTING
QObject* KoReportImagePlugin::createScriptInstance(KoReportItemBase* item)
{
    KoReportItemImage *image = dynamic_cast<KoReportItemImage*>(item);
    if (image) {
        return new Scripting::Image(image);
    }
    return 0;
}
#endif

#include "KoReportImagePlugin.moc"
