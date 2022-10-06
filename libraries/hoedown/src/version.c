// SPDX-FileCopyrightText: 2008, Natacha Porté
// SPDX-FileCopyrightText: 2011, Vicent Martí
// SPDX-FileCopyrightText: 2014, Xavier Mendez, Devin Torres and the Hoedown authors
// SPDX-License-Identifier: ISC

#include "hoedown/version.h"

void
hoedown_version(int *major, int *minor, int *revision)
{
    *major = HOEDOWN_VERSION_MAJOR;
    *minor = HOEDOWN_VERSION_MINOR;
    *revision = HOEDOWN_VERSION_REVISION;
}
