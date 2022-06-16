// SPDX-License-Identifier: GPL-3.0-only
/*
 *  PolyMC - Minecraft Launcher
 *  Copyright (C) 2022 Sefa Eyeoglu <contact@scrumplex.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <QDateTime>
#include <QObject>
#include <QUuid>
#include <QJsonObject>
#include "Usable.h"
#include "java/JavaVersion.h"

struct JavaRuntime {
    QJsonObject saveState() const;
    bool resumeState(QJsonObject data);

    QUuid uuid;
    QString name;
    bool enabled;

    QString path;
    JavaVersion version;
    QString architecture;
    QString vendor;

    QDateTime added;
    QDateTime lastSeen;
    QDateTime lastUsed;
};
