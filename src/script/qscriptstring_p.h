/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL-ONLY$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSCRIPTSTRING_P_H
#define QSCRIPTSTRING_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qscriptshareddata_p.h"
#include "qscriptconverter_p.h"
#include "qscriptengine_p.h"
#include "qscriptstring.h"
#include <QtCore/qnumeric.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qhash.h>

QT_BEGIN_NAMESPACE

class QScriptStringPrivate : public QScriptSharedData {
public:
    static inline QScriptString get(QScriptStringPrivate* d);
    static inline QScriptString get(QScriptPassPointer<QScriptStringPrivate> d);
    static inline QScriptStringPrivate* get(const QScriptString& p);

    inline QScriptStringPrivate();
    inline QScriptStringPrivate(QScriptEnginePrivate *, v8::Handle<v8::String>);
    inline ~QScriptStringPrivate();

    inline bool operator==(const QScriptStringPrivate& other) const;
    inline bool operator!=(const QScriptStringPrivate& other) const;

    inline bool isValid() const;
    inline quint32 toArrayIndex(bool* ok = 0) const;
    inline QString toString() const;
    inline quint64 id() const;

private:
    Q_DISABLE_COPY(QScriptStringPrivate)
    QScriptSharedDataPointer<QScriptEnginePrivate> m_engine;
    v8::Persistent<v8::String> m_string;
    friend class QScriptString;
    friend class QScriptValue;
};


QScriptStringPrivate::QScriptStringPrivate()
{}

QScriptStringPrivate::QScriptStringPrivate(QScriptEnginePrivate *engine, v8::Handle<v8::String> str)
    : m_engine(engine), m_string(v8::Persistent<v8::String>::New(str))
{}

QScriptStringPrivate::~QScriptStringPrivate()
{
    m_string.Dispose();
}

QScriptString QScriptStringPrivate::get(QScriptStringPrivate* d)
{
    Q_ASSERT(d);
    return QScriptString(d);
}

QScriptString QScriptStringPrivate::get(QScriptPassPointer<QScriptStringPrivate> d)
{
    Q_ASSERT(d);
    return QScriptString(d);
}

QScriptStringPrivate* QScriptStringPrivate::get(const QScriptString& p)
{
    return p.d_ptr.data();
}

bool QScriptStringPrivate::isValid() const
{
    return !m_string.IsEmpty();
}

bool QScriptStringPrivate::operator==(const QScriptStringPrivate& other) const
{
    v8::HandleScope handleScope;
    return isValid() && other.isValid() && m_string->Equals(other.m_string);
}

bool QScriptStringPrivate::operator!=(const QScriptStringPrivate& other) const
{
    v8::HandleScope handleScope;
    return isValid() && other.isValid() && !m_string->Equals(other.m_string);
}

quint32 QScriptStringPrivate::toArrayIndex(bool* ok) const
{
    quint32 idx = 0xffffffff;
    if (isValid()) {
        v8::HandleScope handleScope;
        v8::Handle<v8::Uint32> converted = m_string->ToArrayIndex();
        if (!converted.IsEmpty())
            idx = converted->Uint32Value();
    }
    if (ok)
        *ok = (idx != 0xffffffff);
    return idx;
}

QString QScriptStringPrivate::toString() const
{
    return QScriptConverter::toString(m_string);
}

quint64 QScriptStringPrivate::id() const
{
    return m_string->Hash();
}

QT_END_NAMESPACE

#endif