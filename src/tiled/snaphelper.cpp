/*
 * snaphelper.cpp
 * Copyright 2015, Thorbjørn Lindeijer <thorbjorn@lindeijer.nl>
 * Copyright 2016, Mamed Ibrahimov <ibramlab@gmail.com>
 *
 * This file is part of Tiled.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "snaphelper.h"

#include "preferences.h"

#include "documentmanager.h"
#include "mapeditor.h"
#include "tilesetdock.h"

namespace Tiled {

SnapHelper::SnapHelper(const MapRenderer *renderer,
                       Qt::KeyboardModifiers modifiers)
    : mRenderer(renderer)
{
    Preferences *preferences = Preferences::instance();
    if (preferences->snapToGrid())
        mSnapMode = SnapToGrid;
    else if (preferences->snapToFineGrid())
        mSnapMode = SnapToFineGrid;
    else if (preferences->snapToOverride())
        mSnapMode = SnapToOverride;

    mSnapToPixels = preferences->snapToPixels();

    if (modifiers & Qt::ControlModifier) {
        if (modifiers & Qt::ShiftModifier) {
            toggleFineSnap();
        } else { 
            toggleSnap();
        }
    }
}

void SnapHelper::toggleSnap()
{
    switch (mSnapMode) {
    case NoSnap:
        mSnapMode = SnapToGrid;
        break;
    case SnapToGrid:
    case SnapToFineGrid:
    case SnapToOverride:
        mSnapMode = NoSnap;
        break;
    }
}
  
void SnapHelper::toggleFineSnap()
{
    switch (mSnapMode) {
    case NoSnap:
    case SnapToGrid:
    case SnapToOverride:
        mSnapMode = SnapToFineGrid;
        break;
    case SnapToFineGrid:
        mSnapMode = SnapToGrid;
        break;
    }
}

void SnapHelper::toggleOverrideSnap()
{
    switch (mSnapMode) {
    case NoSnap:
    case SnapToGrid:
    case SnapToFineGrid:
        mSnapMode = SnapToOverride;
        break;
    case SnapToOverride:
        mSnapMode = SnapToGrid;
        break;
    }
}

void SnapHelper::snap(QPointF &pixelPos) const
{
    if (mSnapMode != NoSnap) {
        if (mSnapMode == SnapToFineGrid) {
            const int gridFine = Preferences::instance()->gridFine();
            pixelPos = mRenderer->snapToGrid(pixelPos, gridFine);
        } else if (mSnapMode == SnapToOverride){
            auto documentManager = DocumentManager::instance();
            auto mapEditor = static_cast<MapEditor*>(documentManager->editor(Document::MapDocumentType));
            TilesetDock *tilesetDock = mapEditor->tilesetDock();
            if ( Tile *currentTile = tilesetDock->currentTile())
            {
                pixelPos = mRenderer->snapToOverride(pixelPos, QPointF(currentTile->width(), currentTile->height()));
            }
        } else {
            pixelPos = mRenderer->snapToGrid(pixelPos);
        }
    } else if (mSnapToPixels) {
        QPointF screenPos = mRenderer->pixelToScreenCoords(pixelPos);
        pixelPos = mRenderer->screenToPixelCoords(screenPos.toPoint());
    }
}

} // namespace Tiled
