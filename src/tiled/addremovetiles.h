/*
 * addremovetiles.h
 * Copyright 2013, Thorbjørn Lindeijer <thorbjorn@lindeijer.nl>
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

#pragma once

#include <QUndoCommand>

/// EDEN CHANGES
#include "mapdocument.h"
#include "tilelayer.h"
#include "objectgroup.h"
#include "erasetiles.h"
#include "mapobject.h"
#include "addremovemapobject.h"

#include <QCoreApplication>

/// EDEN CHANGES END

namespace Tiled {

class Tile;

class TilesetDocument;

/// EDEN CHANGES
static bool hasMissingTiles(MapDocument *mapDocument)
{

    auto missingTile = [] (const Cell &cell) {
        return cell.tileset() && cell.tile() == nullptr;
    };

    LayerIterator it(mapDocument->map());
    while (Layer *layer = it.next()) {
        switch (layer->layerType()) {
        case Layer::TileLayerType: {
            auto tileLayer = static_cast<TileLayer*>(layer);
            const QRegion refs = tileLayer->region(missingTile);
            if (!refs.isEmpty())
                return true;
            break;
        }
        case Layer::ObjectGroupType: {
            auto objectGroup = static_cast<ObjectGroup*>(layer);
            for (MapObject *object : *objectGroup) {
                if (missingTile(object->cell()))
                    return true;
            }
            break;
        }
        case Layer::ImageLayerType:
        case Layer::GroupLayerType:
            break;
        }
    }
    return false;
}

static void removeTileReferences(MapDocument *mapDocument,
                                 std::function<bool(const Cell &)> condition)
{
    QUndoStack *undoStack = mapDocument->undoStack();
    undoStack->beginMacro(QCoreApplication::translate("Undo Commands", "Remove Tiles"));

    QList<MapObject*> objectsToRemove;

    LayerIterator it(mapDocument->map());
    while (Layer *layer = it.next()) {
        switch (layer->layerType()) {
        case Layer::TileLayerType: {
            auto tileLayer = static_cast<TileLayer*>(layer);
            const QRegion refs = tileLayer->region(condition);
            if (!refs.isEmpty())
                undoStack->push(new EraseTiles(mapDocument, tileLayer, refs));
            break;
        }
        case Layer::ObjectGroupType: {
            auto objectGroup = static_cast<ObjectGroup*>(layer);
            for (MapObject *object : *objectGroup) {
                if (condition(object->cell()))
                    objectsToRemove.append(object);
            }
            break;
        }
        case Layer::ImageLayerType:
        case Layer::GroupLayerType:
            break;
        }
    }

    if (!objectsToRemove.isEmpty())
        undoStack->push(new RemoveMapObjects(mapDocument, objectsToRemove));

    undoStack->endMacro();
}

/// EDEN CHANGES END

/**
 * Abstract base class for AddTiles and RemoveTiles.
 */
class AddRemoveTiles : public QUndoCommand
{
public:
    AddRemoveTiles(TilesetDocument *tilesetDocument,
                   const QList<Tile*> &tiles,
                   bool add);

    ~AddRemoveTiles() override;

protected:
    void addTiles();
    void removeTiles();

private:
    TilesetDocument *mTilesetDocument;
    QList<Tile*> mTiles;
    bool mTilesAdded;
};

/**
 * Undo command that adds tiles to a tileset.
 */
class AddTiles : public AddRemoveTiles
{
public:
    AddTiles(TilesetDocument *tilesetDocument,
             const QList<Tile*> &tiles);

    void undo() override
    { removeTiles(); }

    void redo() override
    { addTiles(); }
};

/**
 * Undo command that removes tiles from a tileset.
 */
class RemoveTiles : public AddRemoveTiles
{
public:
    RemoveTiles(TilesetDocument *tilesetDocument,
                const QList<Tile *> &tiles);

    void undo() override;
    void redo() override;
};

} // namespace Tiled
