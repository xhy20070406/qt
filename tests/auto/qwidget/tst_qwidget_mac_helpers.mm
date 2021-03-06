/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the documentation of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "tst_qwidget_mac_helpers.h"
#include <private/qt_mac_p.h>
#include <private/qt_cocoa_helpers_mac_p.h>


QString nativeWindowTitle(QWidget *window, Qt::WindowState state)
{
    OSWindowRef windowRef = qt_mac_window_for(window);
    QCFString macTitle;
    if (state == Qt::WindowMinimized) {
#ifndef QT_MAC_USE_COCOA
        CopyWindowAlternateTitle(windowRef, &macTitle);
#else
        macTitle = reinterpret_cast<CFStringRef>([[windowRef miniwindowTitle] retain]);
#endif
    } else {
#ifndef QT_MAC_USE_COCOA
        CopyWindowTitleAsCFString(windowRef, &macTitle);
#else
        macTitle = reinterpret_cast<CFStringRef>([[windowRef title] retain]);
#endif
    }
    return macTitle;
}

bool nativeWindowModified(QWidget *widget)
{
#ifndef QT_MAC_USE_COCOA
    return IsWindowModified(qt_mac_window_for(widget));
#else
    return [qt_mac_window_for(widget) isDocumentEdited];
#endif
}

#ifndef QT_MAC_USE_COCOA
bool testAndRelease(const HIViewRef view)
{
//    qDebug() << CFGetRetainCount(view);
    if (CFGetRetainCount(view) != 2)
        return false;
    CFRelease(view);
    CFRelease(view);
    return true;
}

WidgetViewPair createAndRetain(QWidget * const parent)
{
    QWidget * const widget = new QWidget(parent);
    const HIViewRef view = (HIViewRef)widget->winId();
    // Retain twice so we can safely call CFGetRetaintCount even if the retain count
    // is off by one because of a double release.
    CFRetain(view);
    CFRetain(view);
    return qMakePair(widget, view);
}
#else
bool testAndRelease(const WId view)
{
    if ([id(view) retainCount] != 2)
        return false;
    [id(view) release];
    [id(view) release];
    return true;
}

WidgetViewPair createAndRetain(QWidget * const parent)
{
    QWidget * const widget = new QWidget(parent);
    const WId view = widget->winId();
    // Retain twice so we can safely call retainCount even if the retain count
    // is off by one because of a double release.
    [id(view) retain];
    [id(view) retain];
    return qMakePair(widget, view);
}
#endif

