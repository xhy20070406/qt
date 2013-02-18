/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
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

#include <private/qperformancetimer_p.h>
#include <QObject>
#include <qtest.h>

class tst_qperformancetimer : public QObject
{
    Q_OBJECT

public:
    tst_qperformancetimer() {}

private slots:
    void units();
};

void tst_qperformancetimer::units()
{
    QPerformanceTimer timer;
    timer.start();
    {
        QTest::qWait(300);
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed > 300000000 && elapsed < 310000000);
    }
    {
        timer.start();
        qint64 monotonic_start = -timer.elapsedToAbsoluteTime(0);
        qint64 wait_ms = 300;
        QTest::qWait(wait_ms);
        qint64 elapsed1 = timer.elapsed();
        qint64 elapsed2 = timer.elapsedToAbsoluteTime(monotonic_start + elapsed1);
        qint64 diff = elapsed1 - elapsed2;
        QVERIFY(diff == 0);
    }
}

QTEST_MAIN(tst_qperformancetimer)

#include "tst_qperformancetimer.moc"
