/*
 * tilestamp.h
 * Copyright 2015, Thorbjørn Lindeijer <bjorn@lindeijer.nl>
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

#include "map.h"
#include "randompicker.h"
#include "tiled.h"

#include <QDir>
#include <QJsonObject>
#include <QSharedData>
#include <QString>
#include <QVector>

namespace Tiled {

struct TileStampVariation
{
    TileStampVariation()
        : map(nullptr), probability(1.0)
    {
    }

    explicit TileStampVariation(Map *map, qreal probability = 1.0)
        : map(map), probability(probability)
    {
        Q_ASSERT(map->layerCount() >= 1);
        Q_ASSERT(map->layerAt(0)->isTileLayer());
    }

    /// EDEN CHANGES
    TileLayer *tileLayer() const;
    /// EDEN CHANGES END

    Tile* tile(){ return mTile; }

    void setTile(Tile* tile){ mTile = tile; }

    Map *map;
    qreal probability;

    Tile* mTile;
};

class TileStampData;

class TileStamp
{
public:
    TileStamp();
    explicit TileStamp(std::unique_ptr<Map> map);

    TileStamp(const TileStamp &other);
    TileStamp &operator=(const TileStamp &other);

    bool operator==(const TileStamp &other) const;

    ~TileStamp();

    QString name() const;
    void setName(const QString &name);

    QString fileName() const;
    void setFileName(const QString &fileName);

    qreal probability(int index) const;
    void setProbability(int index, qreal probability);

    QSize maxSize() const;

    const QVector<TileStampVariation> &variations() const;
    void addVariation(std::unique_ptr<Map> map, qreal probability = 1.0);
    void addVariation(const TileStampVariation &variation);
    Map *takeVariation(int index);
    bool isEmpty() const;

    void setVariationTile(int iVariationIndex, Tile* pTile);

    int quickStampIndex() const;
    void setQuickStampIndex(int quickStampIndex);

    RandomPicker<Map *> randomVariations() const;

    TileStamp flipped(FlipDirection direction) const;
    TileStamp rotated(RotateDirection direction) const;

    TileStamp clone() const;

    QJsonObject toJson(const QDir &dir) const;

    static TileStamp fromJson(const QJsonObject &json,
                              const QDir &mapDir);

private:
    QExplicitlySharedDataPointer<TileStampData> d;
};


/**
 * Adds a \a variation to this tile stamp.
 */
inline void TileStamp::addVariation(const TileStampVariation &variation)
{
    addVariation(variation.map->clone(),
                 variation.probability);
}

} // namespace Tiled
