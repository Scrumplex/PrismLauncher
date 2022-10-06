// SPDX-FileCopyrightText: 2012, Akos Polster
// SPDX-FileCopyrightText: 2021 MultiMC Contributors
// SPDX-License-Identifier: BSD-2-Clause AND MIT

#pragma once

#include <QVariantMap>

class QByteArray;

namespace Katabasis {

    /// Parse JSON data into a QVariantMap
QVariantMap parseJsonResponse(const QByteArray &data);

}
