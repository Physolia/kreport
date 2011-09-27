/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2011  Adam Pigg <adam@piggz.co.uk>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "KoReportASyncItemManager.h"
#include "common/KoReportASyncItemBase.h"

KoReportASyncItemManager::KoReportASyncItemManager(QObject* parent): QObject(parent)
{

}

KoReportASyncItemManager::~KoReportASyncItemManager()
{

}

void KoReportASyncItemManager::addItem(KoReportASyncItemBase* item, OROPage* page, OROSection* section, QPointF offset, QVariant data, KRScriptHandler* script)
{
    RenderData *rdata = new RenderData();
    rdata->item = item;
    rdata->page = page;
    rdata->section = section;
    rdata->offset = offset;
    rdata->data = data;
    rdata->script = script;
    
    m_renderList.enqueue(rdata);
    
    connect(item, SIGNAL(finishedRendering()), this, SLOT(itemFinished()));
}

void KoReportASyncItemManager::itemFinished()
{
    if (m_renderList.count() > 0) {
        RenderData *rdata = m_renderList.dequeue();
    } else {
        emit(finished());
    }
}

void KoReportASyncItemManager::startRendering()
{
    if (m_renderList.count() > 0) {
        RenderData *rdata = m_renderList.dequeue();
        rdata->item->render(rdata->page, rdata->section, rdata->offset, rdata->data, rdata->script);
    } else {
        emit(finished());
    }
}