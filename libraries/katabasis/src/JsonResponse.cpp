// SPDX-FileCopyrightText: 2012, Akos Polster
// SPDX-FileCopyrightText: 2021 MultiMC Contributors
// SPDX-License-Identifier: BSD-2-Clause AND MIT

#include "JsonResponse.h"

#include <QByteArray>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

namespace Katabasis {

QVariantMap parseJsonResponse(const QByteArray &data) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "parseTokenResponse: Failed to parse token response due to err:" << err.errorString();
        return QVariantMap();
    }

    if (!doc.isObject()) {
        qWarning() << "parseTokenResponse: Token response is not an object";
        return QVariantMap();
    }

    return doc.object().toVariantMap();
}

}
