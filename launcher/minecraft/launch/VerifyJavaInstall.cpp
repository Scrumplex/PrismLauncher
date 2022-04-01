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
 *
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 *      Copyright 2013-2021 MultiMC Contributors
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 */

#include "VerifyJavaInstall.h"

#include "Application.h"
#include "java/JavaVersion.h"
#include "minecraft/PackProfile.h"
#include "minecraft/MinecraftInstance.h"
#include <QMessageBox>

void VerifyJavaInstall::executeTask() {
    auto instance = std::dynamic_pointer_cast<MinecraftInstance>(m_parent->instance());
    auto packProfile = instance->getPackProfile();
    auto settings = instance->settings();
    auto storedVersion = settings->get("JavaVersion").toString();
    auto ignoreCompatibility = settings->get("IgnoreJavaCompatibility").toBool();

    auto compatibleMajors = packProfile->getProfile()->getCompatibleJavaMajors();
    auto currentMajor = JavaVersion(storedVersion).major();

    if (compatibleMajors.isEmpty() || compatibleMajors.contains(currentMajor))
    {
        emitSucceeded();
        return;
    }


    if (ignoreCompatibility)
    {
        emit logLine(tr("Java major version is incompatible. Things might break."), MessageLevel::Warning);
        emitSucceeded();
        return;
    }

    emit logLine(tr("This instance is not compatible with Java version %1.\n"
                    "Please switch to one of the following Java versions for this instance:").arg(currentMajor),
                 MessageLevel::Error);
    for (auto major : compatibleMajors)
    {
        emit logLine(tr("Java version %1").arg(major), MessageLevel::Error);
    }

    // Emit failed before showing messagebox, to "unlock" UI
    emitFailed(QString("Incompatible Java major version"));

    QMessageBox warning;
    if (compatibleMajors.size() == 1)
    {
        warning.setText(tr("This instance is not compatible with <b>Java version %1</b>.<br>"
                           "Please switch to <b>Java version %2</b> for this instance.").arg(currentMajor).arg(compatibleMajors[0]));
    }
    else
    {
        QStringList foo;
        for (auto major : compatibleMajors)
        {
            foo.append(QString::number(major));
        }
        auto bar = foo.join(", ");
        warning.setText(tr("This instance is not compatible with <b>Java version %1</b>.<br>"
                           "Please switch to one of the following Java versions for this instance: <b>%2</b>").arg(currentMajor).arg(bar));
    }
    warning.setInformativeText(tr("Do you want to open the instance's settings to change this?"));
    warning.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int result = warning.exec();
    if (result == QMessageBox::Yes)
    {
        APPLICATION->showInstanceWindow(instance, "settings");
    }
}
