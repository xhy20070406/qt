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


#include <QtTest/QtTest>
#include <math.h>
#include <qglobal.h>
#include <qdir.h>
#include <qtextcodec.h>
#include <qdatetime.h>
#include <float.h>

#include <qlocale.h>
#include <qnumeric.h>

#ifdef Q_OS_LINUX
#    include <fenv.h>
#endif

#ifdef Q_OS_WINCE
#include <qcoreapplication.h>
#include <windows.h> // needed for GetUserDefaultLCID
#define _control87 _controlfp
extern "C" DWORD GetThreadLocale(void) {
	return GetUserDefaultLCID();
}

#endif

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
#    include <stdlib.h>
#endif

#if defined(Q_OS_SYMBIAN)
#    include <e32std.h>
#    include <private/qcore_symbian_p.h>
#endif

Q_DECLARE_METATYPE(qlonglong)
Q_DECLARE_METATYPE(QDate)
Q_DECLARE_METATYPE(QLocale::FormatType)

//TESTED_CLASS=
//TESTED_FILES=

class tst_QLocale : public QObject
{
    Q_OBJECT

public:
    tst_QLocale();

private slots:
    void initTestCase();
    void windowsDefaultLocale();
#ifdef Q_OS_MAC
    void macDefaultLocale();
#endif

    void ctor();
    void emptyCtor();
    void unixLocaleName();
    void double_conversion_data();
    void double_conversion();
    void long_long_conversion_data();
    void long_long_conversion();
    void long_long_conversion_extra();
    void testInfAndNan();
    void fpExceptions();
    void negativeZero();
    void dayOfWeek();
    void dayOfWeek_data();
    void formatDate();
    void formatDate_data();
    void formatTime();
    void formatTime_data();
    void formatDateTime();
    void formatDateTime_data();
    void toDateTime_data();
    void toDateTime();
    void negativeNumbers();
    void numberOptions();
    void testNames_data();
    void testNames();
    void dayName_data();
    void dayName();
    void standaloneDayName_data();
    void standaloneDayName();
    void underflowOverflow();
    void measurementSystems_data();
    void measurementSystems();
    void systemMeasurementSystems_data();
    void systemMeasurementSystems();

    void dateFormat();
    void timeFormat();
    void dateTimeFormat();
    void monthName();
    void standaloneMonthName();

    // QSystemLocale tests
    void queryDateTime();
    void queryMeasureSystem_data();
    void queryMeasureSystem();

    // defaultNumberingSystem test
    void defaultNumeringSystem();

#if defined(Q_OS_SYMBIAN)
    void symbianSystemLocale();
#endif

    void ampm();
    void currency();
    void quoteString();
    void uiLanguages();
    void weekendDays();
    void listPatterns();

private:
    QString m_decimal, m_thousand, m_sdate, m_ldate, m_time;
    QString m_sysLocaleApp;
};

tst_QLocale::tst_QLocale()
{
    qRegisterMetaType<QLocale::FormatType>("QLocale::FormatType");
}

void tst_QLocale::initTestCase()
{
    QDir workingDirectory = QDir::current();
    QString sysLocaleApp = QLatin1String("syslocaleapp/syslocaleapp");
    // Windows: cd up to be able to locate the binary of the sub-process.
#ifdef Q_OS_WIN
    sysLocaleApp.append(QLatin1String(".exe"));
    if (workingDirectory.absolutePath().endsWith(QLatin1String("/debug"), Qt::CaseInsensitive)
        || workingDirectory.absolutePath().endsWith(QLatin1String("/release"), Qt::CaseInsensitive)) {
        QVERIFY(workingDirectory.cdUp());
        QVERIFY(QDir::setCurrent(workingDirectory.absolutePath()));
    }
#endif
    m_sysLocaleApp = workingDirectory.absoluteFilePath(sysLocaleApp);
    QVERIFY2(QFileInfo(m_sysLocaleApp).exists(),
             qPrintable(QString::fromLatin1("SysLocalApp executable '%1' does not exist!")
                        .arg(QDir::toNativeSeparators(m_sysLocaleApp))));
}

void tst_QLocale::ctor()
{
#ifdef Q_OS_WINCE
    int argc = 1;
    char argv[20] = "tst_qlocale.exe";
    QCoreApplication app(argc, (char**)&argv);
#endif
    QLocale default_locale = QLocale::system();
    QLocale::Language default_lang = default_locale.language();
    QLocale::Country default_country = default_locale.country();

    qDebug("Default: %s/%s", QLocale::languageToString(default_lang).toLatin1().constData(),
            QLocale::countryToString(default_country).toLatin1().constData());

    {
	QLocale l;
	QVERIFY(l.language() == default_lang);
	QVERIFY(l.country() == default_country);
    }

#define TEST_CTOR(req_lang, req_country, exp_lang, exp_country) \
    { \
    	QLocale l(QLocale::req_lang, QLocale::req_country); \
	QCOMPARE(l.language(), exp_lang); \
	QCOMPARE(l.country(), exp_country); \
    }
    {
        QLocale l(QLocale::C, QLocale::AnyCountry);
        QCOMPARE(l.language(), QLocale::C);
        QCOMPARE(l.country(), QLocale::AnyCountry);
    }
    TEST_CTOR(AnyLanguage, AnyCountry, default_lang, default_country)
    TEST_CTOR(C, AnyCountry, QLocale::C, QLocale::AnyCountry)
    TEST_CTOR(Aymara, AnyCountry, default_lang, default_country)
    TEST_CTOR(Aymara, France, default_lang, default_country)

    TEST_CTOR(English, AnyCountry, QLocale::English, QLocale::UnitedStates)
    TEST_CTOR(English, UnitedStates, QLocale::English, QLocale::UnitedStates)
    TEST_CTOR(English, France, QLocale::English, QLocale::UnitedStates)
    TEST_CTOR(English, UnitedKingdom, QLocale::English, QLocale::UnitedKingdom)

    TEST_CTOR(French, France, QLocale::French, QLocale::France)
    TEST_CTOR(C, France, QLocale::C, QLocale::AnyCountry)
    TEST_CTOR(Spanish, LatinAmericaAndTheCaribbean, QLocale::Spanish, QLocale::LatinAmericaAndTheCaribbean)

    QLocale::setDefault(QLocale(QLocale::English, QLocale::France));

    {
	QLocale l;
	QVERIFY(l.language() == QLocale::English);
	QVERIFY(l.country() == QLocale::UnitedStates);
    }

    TEST_CTOR(French, France, QLocale::French, QLocale::France)
    TEST_CTOR(English, UnitedKingdom, QLocale::English, QLocale::UnitedKingdom)

    TEST_CTOR(French, France, QLocale::French, QLocale::France)
    TEST_CTOR(C, AnyCountry, QLocale::C, QLocale::AnyCountry)
    TEST_CTOR(C, France, QLocale::C, QLocale::AnyCountry)
    TEST_CTOR(Aymara, AnyCountry, QLocale::English, QLocale::UnitedStates)

    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedKingdom));

    {
	QLocale l;
	QVERIFY(l.language() == QLocale::English);
	QVERIFY(l.country() == QLocale::UnitedKingdom);
    }

    TEST_CTOR(French, France, QLocale::French, QLocale::France)
    TEST_CTOR(English, UnitedKingdom, QLocale::English, QLocale::UnitedKingdom)

    TEST_CTOR(C, AnyCountry, QLocale::C, QLocale::AnyCountry)
    TEST_CTOR(C, France, QLocale::C, QLocale::AnyCountry)

    QLocale::setDefault(QLocale(QLocale::Aymara, QLocale::France));

    {
	QLocale l;
	QVERIFY(l.language() == QLocale::English);
	QVERIFY(l.country() == QLocale::UnitedKingdom);
    }

    TEST_CTOR(Aymara, AnyCountry, QLocale::English, QLocale::UnitedKingdom)
    TEST_CTOR(Aymara, France, QLocale::English, QLocale::UnitedKingdom)

    TEST_CTOR(English, AnyCountry, QLocale::English, QLocale::UnitedStates)
    TEST_CTOR(English, UnitedStates, QLocale::English, QLocale::UnitedStates)
    TEST_CTOR(English, France, QLocale::English, QLocale::UnitedStates)
    TEST_CTOR(English, UnitedKingdom, QLocale::English, QLocale::UnitedKingdom)

    TEST_CTOR(French, France, QLocale::French, QLocale::France)
    TEST_CTOR(C, AnyCountry, QLocale::C, QLocale::AnyCountry)
    TEST_CTOR(C, France, QLocale::C, QLocale::AnyCountry)

    QLocale::setDefault(QLocale(QLocale::Aymara, QLocale::AnyCountry));

    {
	QLocale l;
	QVERIFY(l.language() == QLocale::English);
	QVERIFY(l.country() == QLocale::UnitedKingdom);
    }


    TEST_CTOR(Aymara, AnyCountry, QLocale::English, QLocale::UnitedKingdom)
    TEST_CTOR(Aymara, France, QLocale::English, QLocale::UnitedKingdom)

    TEST_CTOR(English, AnyCountry, QLocale::English, QLocale::UnitedStates)
    TEST_CTOR(English, UnitedStates, QLocale::English, QLocale::UnitedStates)
    TEST_CTOR(English, France, QLocale::English, QLocale::UnitedStates)
    TEST_CTOR(English, UnitedKingdom, QLocale::English, QLocale::UnitedKingdom)

    TEST_CTOR(French, France, QLocale::French, QLocale::France)
    TEST_CTOR(C, AnyCountry, QLocale::C, QLocale::AnyCountry)
    TEST_CTOR(C, France, QLocale::C, QLocale::AnyCountry)

    TEST_CTOR(Arabic, AnyCountry, QLocale::Arabic, QLocale::SaudiArabia)
    TEST_CTOR(Dutch, AnyCountry, QLocale::Dutch, QLocale::Netherlands)
    TEST_CTOR(German, AnyCountry, QLocale::German, QLocale::Germany)
    TEST_CTOR(Greek, AnyCountry, QLocale::Greek, QLocale::Greece)
    TEST_CTOR(Malay, AnyCountry, QLocale::Malay, QLocale::Malaysia)
    TEST_CTOR(Persian, AnyCountry, QLocale::Persian, QLocale::Iran)
    TEST_CTOR(Portuguese, AnyCountry, QLocale::Portuguese, QLocale::Portugal)
    TEST_CTOR(Serbian, AnyCountry, QLocale::Serbian, QLocale::SerbiaAndMontenegro)
    TEST_CTOR(Somali, AnyCountry, QLocale::Somali, QLocale::Somalia)
    TEST_CTOR(Spanish, AnyCountry, QLocale::Spanish, QLocale::Spain)
    TEST_CTOR(Swedish, AnyCountry, QLocale::Swedish, QLocale::Sweden)
    TEST_CTOR(Uzbek, AnyCountry, QLocale::Uzbek, QLocale::Uzbekistan)

#undef TEST_CTOR

#define TEST_CTOR(req_lc, exp_lang, exp_country) \
    { \
	QLocale l(req_lc); \
	QVERIFY2(l.language() == QLocale::exp_lang \
		&& l.country() == QLocale::exp_country, \
		QString("requested: \"" + QString(req_lc) + "\", got: " \
		+ QLocale::languageToString(l.language()) \
		+ "/" + QLocale::countryToString(l.country())).toLatin1().constData()); \
    }

    QLocale::setDefault(QLocale(QLocale::C));

    TEST_CTOR("C", C, AnyCountry)
    TEST_CTOR("bla", C, AnyCountry)
    TEST_CTOR("zz", C, AnyCountry)
    TEST_CTOR("zz_zz", C, AnyCountry)
    TEST_CTOR("zz...", C, AnyCountry)
    TEST_CTOR("", C, AnyCountry)
    TEST_CTOR("en/", C, AnyCountry)
    TEST_CTOR(QString::null, C, AnyCountry)
    TEST_CTOR("en", English, UnitedStates)
    TEST_CTOR("en", English, UnitedStates)
    TEST_CTOR("en.", English, UnitedStates)
    TEST_CTOR("en@", English, UnitedStates)
    TEST_CTOR("en.@", English, UnitedStates)
    TEST_CTOR("en_", English, UnitedStates)
    TEST_CTOR("en_U", English, UnitedStates)
    TEST_CTOR("en_.", English, UnitedStates)
    TEST_CTOR("en_.@", English, UnitedStates)
    TEST_CTOR("en.bla", English, UnitedStates)
    TEST_CTOR("en@bla", English, UnitedStates)
    TEST_CTOR("en_blaaa", English, UnitedStates)
    TEST_CTOR("en_zz", English, UnitedStates)
    TEST_CTOR("en_GB", English, UnitedKingdom)
    TEST_CTOR("en_GB.bla", English, UnitedKingdom)
    TEST_CTOR("en_GB@.bla", English, UnitedKingdom)
    TEST_CTOR("en_GB@bla", English, UnitedKingdom)
    TEST_CTOR("en-GB", English, UnitedKingdom)
    TEST_CTOR("en-GB@bla", English, UnitedKingdom)

    QVERIFY(QLocale::Norwegian == QLocale::NorwegianBokmal);
    TEST_CTOR("no", Norwegian, Norway)
    TEST_CTOR("nb", Norwegian, Norway)
    TEST_CTOR("nn", NorwegianNynorsk, Norway)
    TEST_CTOR("no_NO", Norwegian, Norway)
    TEST_CTOR("nb_NO", Norwegian, Norway)
    TEST_CTOR("nn_NO", NorwegianNynorsk, Norway)
    TEST_CTOR("es_ES", Spanish, Spain)
    TEST_CTOR("es_419", Spanish, LatinAmericaAndTheCaribbean)
    TEST_CTOR("es-419", Spanish, LatinAmericaAndTheCaribbean)

    // test default countries for languages
    TEST_CTOR("zh", Chinese, China)
    TEST_CTOR("zh-Hans", Chinese, China)
    TEST_CTOR("mn", Mongolian, Mongolia)
    TEST_CTOR("ne", Nepali, Nepal)

#undef TEST_CTOR
#define TEST_CTOR(req_lc, exp_lang, exp_script, exp_country) \
    { \
    QLocale l(req_lc); \
    QVERIFY2(l.language() == QLocale::exp_lang \
        && l.script() == QLocale::exp_script \
        && l.country() == QLocale::exp_country, \
        QString("requested: \"" + QString(req_lc) + "\", got: " \
        + QLocale::languageToString(l.language()) \
        + "/" + QLocale::scriptToString(l.script()) \
        + "/" + QLocale::countryToString(l.country())).toLatin1().constData()); \
    }

    TEST_CTOR("zh_CN", Chinese, AnyScript, China)
    TEST_CTOR("zh_Hans_CN", Chinese, SimplifiedHanScript, China)
    TEST_CTOR("zh_Hans", Chinese, SimplifiedHanScript, China)
    TEST_CTOR("zh_Hant", Chinese, TraditionalHanScript, HongKong)
    TEST_CTOR("zh_Hans_MO", Chinese, SimplifiedHanScript, Macau)
    TEST_CTOR("zh_Hant_MO", Chinese, TraditionalHanScript, Macau)
    TEST_CTOR("az_Latn_AZ", Azerbaijani, LatinScript, Azerbaijan)
    TEST_CTOR("ha_NG", Hausa, AnyScript, Nigeria)
    TEST_CTOR("ha_Latn_NG", Hausa, LatinScript, Nigeria)

#undef TEST_CTOR
}

void tst_QLocale::emptyCtor()
{
#if defined(Q_OS_WINCE) || defined(Q_OS_SYMBIAN)
    QSKIP("Uses unsupported Windows CE / Symbian QProcess functionality (std streams, env)", SkipAll);
#endif
#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
#define TEST_CTOR(req_lc, exp_str) \
    { \
    /* Test constructor without arguments. Needs separate process */ \
    /* because of caching of the system locale. */ \
    QProcess process; \
    process.setEnvironment(QStringList(env) << QString("LANG=%1").arg(req_lc)); \
    process.start(m_sysLocaleApp); \
    QVERIFY(process.waitForStarted()); \
    process.waitForReadyRead(); \
    QString ret = QString(process.readAll()); \
    process.waitForFinished(); \
    QVERIFY2(!ret.isEmpty(), "Cannot launch external process"); \
    QVERIFY2(QString(exp_str) == ret, QString("Expected: " + QString(exp_str) + ", got: " \
            + ret + ". Requested: " + QString(req_lc)).toLatin1().constData()); \
    }

    // Get an environment free of any locale-related variables
    QStringList env;
    foreach (QString const& entry, QProcess::systemEnvironment()) {
        if (entry.startsWith("LANG=") || entry.startsWith("LC_"))
            continue;
        env << entry;
    }

    // Get default locale.
    QProcess p;
    p.setEnvironment(env);
    p.start("syslocaleapp/syslocaleapp");
    p.waitForReadyRead();
    QString defaultLoc = QString(p.readAll());
    p.waitForFinished();

    TEST_CTOR("C", "C")
    TEST_CTOR("bla", "C")
    TEST_CTOR("zz", "C")
    TEST_CTOR("zz_zz", "C")
    TEST_CTOR("zz...", "C")
    TEST_CTOR("en", "en_US")
    TEST_CTOR("en", "en_US")
    TEST_CTOR("en.", "en_US")
    TEST_CTOR("en@", "en_US")
    TEST_CTOR("en.@", "en_US")
    TEST_CTOR("en_", "en_US")
    TEST_CTOR("en_.", "en_US")
    TEST_CTOR("en_.@", "en_US")
    TEST_CTOR("en.bla", "en_US")
    TEST_CTOR("en@bla", "en_US")
    TEST_CTOR("en_blaaa", "en_US")
    TEST_CTOR("en_zz", "en_US")
    TEST_CTOR("en_GB", "en_GB")
    TEST_CTOR("en_GB.bla", "en_GB")
    TEST_CTOR("en_GB@.bla", "en_GB")
    TEST_CTOR("en_GB@bla", "en_GB")
    TEST_CTOR("de", "de_DE")

    QVERIFY(QLocale::Norwegian == QLocale::NorwegianBokmal);
    TEST_CTOR("no", "nb_NO")
    TEST_CTOR("nb", "nb_NO")
    TEST_CTOR("nn", "nn_NO")
    TEST_CTOR("no_NO", "nb_NO")
    TEST_CTOR("nb_NO", "nb_NO")
    TEST_CTOR("nn_NO", "nn_NO")

    TEST_CTOR("DE", "de_DE");
    TEST_CTOR("EN", "en_US");

    TEST_CTOR("en/", defaultLoc)
    TEST_CTOR("asdfghj", defaultLoc);
    TEST_CTOR("123456", defaultLoc);

#undef TEST_CTOR
#endif
}

void tst_QLocale::unixLocaleName()
{
#define TEST_NAME(req_lang, req_country, exp_name) \
    { \
    	QLocale l(QLocale::req_lang, QLocale::req_country); \
	QCOMPARE(l.name(), QString(exp_name)); \
    }

    QLocale::setDefault(QLocale(QLocale::C));

    TEST_NAME(C, AnyCountry, "C")
    TEST_NAME(English, AnyCountry, "en_US")
    TEST_NAME(English, UnitedKingdom, "en_GB")
    TEST_NAME(Aymara, UnitedKingdom, "C")

#undef TEST_NAME
}

void tst_QLocale::double_conversion_data()
{
    QTest::addColumn<QString>("locale_name");
    QTest::addColumn<QString>("num_str");
    QTest::addColumn<bool>("good");
    QTest::addColumn<double>("num");

    QTest::newRow("C 1")         << QString("C") << QString("1")          << true  << 1.0;
    QTest::newRow("C 1.0")       << QString("C") << QString("1.0")        << true  << 1.0;
    QTest::newRow("C 1.234")     << QString("C") << QString("1.234")      << true  << 1.234;
    QTest::newRow("C 1.234e-10") << QString("C") << QString("1.234e-10")  << true  << 1.234e-10;
    QTest::newRow("C 1.234E10")  << QString("C") << QString("1.234E10")   << true  << 1.234e10;
    QTest::newRow("C 1e10")      << QString("C") << QString("1e10")       << true  << 1.0e10;
    QTest::newRow("C  1")        << QString("C") << QString(" 1")         << true  << 1.0;
    QTest::newRow("C   1")       << QString("C") << QString("  1")        << true  << 1.0;
    QTest::newRow("C 1 ")        << QString("C") << QString("1 ")         << true  << 1.0;
    QTest::newRow("C 1  ")       << QString("C") << QString("1  ")        << true  << 1.0;

    QTest::newRow("C 1,")	        << QString("C") << QString("1,")	 << false << 0.0;
    QTest::newRow("C 1,2")	        << QString("C") << QString("1,2")	 << false << 0.0;
    QTest::newRow("C 1,23")        << QString("C") << QString("1,23")	 << false << 0.0;
    QTest::newRow("C 1,234")       << QString("C") << QString("1,234")	 << true  << 1234.0;
    QTest::newRow("C 1,234,")      << QString("C") << QString("1,234,")	 << false << 0.0;
    QTest::newRow("C 1,234,5")     << QString("C") << QString("1,234,5")	 << false << 0.0;
    QTest::newRow("C 1,234,56")    << QString("C") << QString("1,234,56")   << false << 0.0;
    QTest::newRow("C 1,234,567")   << QString("C") << QString("1,234,567")  << true  << 1234567.0;
    QTest::newRow("C 1,234,567.")  << QString("C") << QString("1,234,567.") << true  << 1234567.0;
    QTest::newRow("C 1,234,567.8")	<< QString("C") << QString("1,234,567.8")<< true  << 1234567.8;
    QTest::newRow("C 1,234567.8")  << QString("C") << QString("1,234567.8") << false << 0.0;
    QTest::newRow("C 12,34567.8")  << QString("C") << QString("12,34567.8") << false << 0.0;
    QTest::newRow("C 1234,567.8")  << QString("C") << QString("1234,567.8") << false << 0.0;
    QTest::newRow("C 1234567.8")   << QString("C") << QString("1234567.8")  << true  << 1234567.8;
    QTest::newRow("C ,")	    	<< QString("C") << QString(",")	         << false << 0.0;
    QTest::newRow("C ,123")	<< QString("C") << QString(",123")       << false << 0.0;
    QTest::newRow("C ,3")	    	<< QString("C") << QString(",3")         << false << 0.0;
    QTest::newRow("C , 3")	    	<< QString("C") << QString(", 3")        << false << 0.0;
    QTest::newRow("C ,  3")	<< QString("C") << QString(",  3")       << false << 0.0;
    QTest::newRow("C ,  3.2")	<< QString("C") << QString(",  3.2")     << false << 0.0;
    QTest::newRow("C ,  3.2e2")	<< QString("C") << QString(",  3.2e2")   << false << 0.0;
    QTest::newRow("C ,  e2")	<< QString("C") << QString(",  e2")	 << false << 0.0;
    QTest::newRow("C 1,,234")	<< QString("C") << QString("1,,234")	 << false << 0.0;

    QTest::newRow("C empty")     << QString("C") << QString("")           << false << 0.0;
    QTest::newRow("C null")      << QString("C") << QString()         << false << 0.0;
    QTest::newRow("C .")         << QString("C") << QString(".")          << false << 0.0;
    QTest::newRow("C 1e")        << QString("C") << QString("1e")         << false << 0.0;
    QTest::newRow("C 1,0")       << QString("C") << QString("1,0")        << false << 0.0;
    QTest::newRow("C 1,000")     << QString("C") << QString("1,000")      << true  << 1000.0;
    QTest::newRow("C 1,000e-6")  << QString("C") << QString("1,000e-6")   << true  << 1000.0e-6;
    QTest::newRow("C 1e1.0")     << QString("C") << QString("1e1.0")      << false << 0.0;
    QTest::newRow("C 1e+")       << QString("C") << QString("1e+")        << false << 0.0;
    QTest::newRow("C 1e-")       << QString("C") << QString("1e-")        << false << 0.0;

    QTest::newRow("C .1")        << QString("C") << QString(".1")         << true  << 0.1;
    QTest::newRow("C -.1")       << QString("C") << QString("-.1")        << true  << -0.1;
    QTest::newRow("C 1.")        << QString("C") << QString("1.")         << true  << 1.0;
    QTest::newRow("C 1.E10")     << QString("C") << QString("1.E10")      << true  << 1.0e10;
    QTest::newRow("C 1e+10")     << QString("C") << QString("1e+10")      << true  << 1.0e+10;

    QTest::newRow("de_DE 1.")	    << QString("de_DE") << QString("1.")	 << false << 0.0;
    QTest::newRow("de_DE 1.2")	    << QString("de_DE") << QString("1.2")	 << false << 0.0;
    QTest::newRow("de_DE 1.23")        << QString("de_DE") << QString("1.23")       << false << 0.0;
    QTest::newRow("de_DE 1.234")       << QString("de_DE") << QString("1.234")	 << true  << 1234.0;
    QTest::newRow("de_DE 1.234,")      << QString("de_DE") << QString("1.234.")	 << false << 0.0;
    QTest::newRow("de_DE 1.234.5")     << QString("de_DE") << QString("1.234.5")	 << false << 0.0;
    QTest::newRow("de_DE 1.234.56")    << QString("de_DE") << QString("1.234.56")   << false << 0.0;
    QTest::newRow("de_DE 1.234.567")   << QString("de_DE") << QString("1.234.567")  << true  << 1234567.0;
    QTest::newRow("de_DE 1.234.567,")  << QString("de_DE") << QString("1.234.567,") << true  << 1234567.0;
    QTest::newRow("de_DE 1.234.567,8") << QString("de_DE") << QString("1.234.567,8")<< true  << 1234567.8;
    QTest::newRow("de_DE 1.234567,8")  << QString("de_DE") << QString("1.234567,8") << false << 0.0;
    QTest::newRow("de_DE 12.34567,8")  << QString("de_DE") << QString("12.34567,8") << false << 0.0;
    QTest::newRow("de_DE 1234.567,8")  << QString("de_DE") << QString("1234.567,8") << false << 0.0;
    QTest::newRow("de_DE 1234567,8")   << QString("de_DE") << QString("1234567,8")  << true  << 1234567.8;
    QTest::newRow("de_DE .123")	    << QString("de_DE") << QString(".123")       << false << 0.0;
    QTest::newRow("de_DE .3")	    << QString("de_DE") << QString(".3")         << false << 0.0;
    QTest::newRow("de_DE . 3")	    << QString("de_DE") << QString(". 3")        << false << 0.0;
    QTest::newRow("de_DE .  3")	    << QString("de_DE") << QString(".  3")       << false << 0.0;
    QTest::newRow("de_DE .  3,2")	    << QString("de_DE") << QString(".  3,2")     << false << 0.0;
    QTest::newRow("de_DE .  3,2e2")    << QString("de_DE") << QString(".  3,2e2")   << false << 0.0;
    QTest::newRow("de_DE .  e2")	    << QString("de_DE") << QString(".  e2")	 << false << 0.0;
    QTest::newRow("de_DE 1..234")	    << QString("de_DE") << QString("1..234")	 << false << 0.0;

    QTest::newRow("de_DE 1")         << QString("de_DE") << QString("1")          << true  << 1.0;
    QTest::newRow("de_DE 1.0")       << QString("de_DE") << QString("1.0")        << false << 0.0;
    QTest::newRow("de_DE 1.234e-10") << QString("de_DE") << QString("1.234e-10")  << true  << 1234.0e-10;
    QTest::newRow("de_DE 1.234E10")  << QString("de_DE") << QString("1.234E10")   << true  << 1234.0e10;
    QTest::newRow("de_DE 1e10")      << QString("de_DE") << QString("1e10")       << true  << 1.0e10;
    QTest::newRow("de_DE .1")        << QString("de_DE") << QString(".1")         << false << 0.0;
    QTest::newRow("de_DE -.1")       << QString("de_DE") << QString("-.1")        << false << 0.0;
    QTest::newRow("de_DE 1.E10")     << QString("de_DE") << QString("1.E10")      << false << 0.0;
    QTest::newRow("de_DE 1e+10")     << QString("de_DE") << QString("1e+10")      << true  << 1.0e+10;

    QTest::newRow("de_DE 1,0")       << QString("de_DE") << QString("1,0")        << true  << 1.0;
    QTest::newRow("de_DE 1,234")     << QString("de_DE") << QString("1,234")      << true  << 1.234;
    QTest::newRow("de_DE 1,234e-10") << QString("de_DE") << QString("1,234e-10")  << true  << 1.234e-10;
    QTest::newRow("de_DE 1,234E10")  << QString("de_DE") << QString("1,234E10")   << true  << 1.234e10;
    QTest::newRow("de_DE ,1")        << QString("de_DE") << QString(",1")         << true  << 0.1;
    QTest::newRow("de_DE -,1")       << QString("de_DE") << QString("-,1")        << true  << -0.1;
    QTest::newRow("de_DE 1,")        << QString("de_DE") << QString("1,")         << true  << 1.0;
    QTest::newRow("de_DE 1,E10")     << QString("de_DE") << QString("1,E10")      << true  << 1.0e10;

    QTest::newRow("de_DE empty")     << QString("de_DE") << QString("")           << false << 0.0;
    QTest::newRow("de_DE null")      << QString("de_DE") << QString()         << false << 0.0;
    QTest::newRow("de_DE .")         << QString("de_DE") << QString(".")          << false << 0.0;
    QTest::newRow("de_DE 1e")        << QString("de_DE") << QString("1e")         << false << 0.0;
    QTest::newRow("de_DE 1e1.0")     << QString("de_DE") << QString("1e1.0")      << false << 0.0;
    QTest::newRow("de_DE 1e+")       << QString("de_DE") << QString("1e+")        << false << 0.0;
    QTest::newRow("de_DE 1e-")       << QString("de_DE") << QString("1e-")        << false << 0.0;

    QTest::newRow("C 9,876543")      << QString("C") << QString("9,876543")        << false << 0.0;
    QTest::newRow("C 9,876543.2")    << QString("C") << QString("9,876543.2")      << false << 0.0;
    QTest::newRow("C 9,876543e-2")   << QString("C") << QString("9,876543e-2")     << false << 0.0;
    QTest::newRow("C 9,876543.0e-2") << QString("C") << QString("9,876543.0e-2")   << false << 0.0;

    QTest::newRow("de_DE 9.876543")      << QString("de_DE") << QString("9876.543")        << false << 0.0;
    QTest::newRow("de_DE 9.876543,2")    << QString("de_DE") << QString("9.876543,2")      << false << 0.0;
    QTest::newRow("de_DE 9.876543e-2")   << QString("de_DE") << QString("9.876543e-2")     << false << 0.0;
    QTest::newRow("de_DE 9.876543,0e-2") << QString("de_DE") << QString("9.876543,0e-2")   << false << 0.0;
    QTest::newRow("de_DE 9.876543e--2")   << QString("de_DE") << QString("9.876543e")+QChar(8722)+QString("2")     << false << 0.0;
    QTest::newRow("de_DE 9.876543,0e--2") << QString("de_DE") << QString("9.876543,0e")+QChar(8722)+QString("2")   << false << 0.0;
}

void tst_QLocale::double_conversion()
{
#define MY_DOUBLE_EPSILON (2.22045e-16)

    QFETCH(QString, locale_name);
    QFETCH(QString, num_str);
    QFETCH(bool, good);
    QFETCH(double, num);

    QLocale locale(locale_name);
    QCOMPARE(locale.name(), locale_name);

    bool ok;
    double d = locale.toDouble(num_str, &ok);
    QCOMPARE(ok, good);

    if (ok) {
    	double diff = d - num;
	if (diff < 0)
	    diff = -diff;
	QVERIFY(diff <= MY_DOUBLE_EPSILON);
    }
}

void tst_QLocale::long_long_conversion_data()
{
    QTest::addColumn<QString>("locale_name");
    QTest::addColumn<QString>("num_str");
    QTest::addColumn<bool>("good");
    QTest::addColumn<qlonglong>("num");

    QTest::newRow("C null")       << QString("C") << QString()	   << false  << (qlonglong) 0;
    QTest::newRow("C empty")      << QString("C") << QString("")	   << false  << (qlonglong) 0;
    QTest::newRow("C 1")          << QString("C") << "1"	   << true  << (qlonglong) 1;
    QTest::newRow("C 1,")         << QString("C") << "1,"     << false << (qlonglong) 0;
    QTest::newRow("C 1,2")        << QString("C") << "1,2"    << false << (qlonglong) 0;
    QTest::newRow("C 1,23")       << QString("C") << "1,23"   << false << (qlonglong) 0;
    QTest::newRow("C 1,234")      << QString("C") << "1,234"  << true  << (qlonglong) 1234;
    QTest::newRow("C 1234567")    << QString("C") << "1234567"<< true  << (qlonglong) 1234567;
    QTest::newRow("C 1,234567")    << QString("C") << "1,234567"<< false  << (qlonglong) 0;
    QTest::newRow("C 12,34567")    << QString("C") << "12,34567"<< false  << (qlonglong) 0;
    QTest::newRow("C 123,4567")    << QString("C") << "123,4567"<< false  << (qlonglong) 0;
    QTest::newRow("C 1234,567")    << QString("C") << "1234,567"<< false  << (qlonglong) 0;
    QTest::newRow("C 12345,67")    << QString("C") << "12345,67"<< false  << (qlonglong) 0;
    QTest::newRow("C 123456,7")    << QString("C") << "123456,7"<< false  << (qlonglong) 0;
    QTest::newRow("C 1,234,567")    << QString("C")<< "1,234,567"<< true  << (qlonglong) 1234567;

    QTest::newRow("de_DE 1")      << QString("de_DE") << "1"      << true  << (qlonglong) 1;
    QTest::newRow("de_DE 1.")     << QString("de_DE") << "1."     << false << (qlonglong) 0;
    QTest::newRow("de_DE 1.2")    << QString("de_DE") << "1.2"    << false << (qlonglong) 0;
    QTest::newRow("de_DE 1.23")   << QString("de_DE") << "1.23"   << false << (qlonglong) 0;
    QTest::newRow("de_DE 1.234")  << QString("de_DE") << "1.234"  << true  << (qlonglong) 1234;
    QTest::newRow("de_DE 1234567")     << QString("de_DE") << "1234567"<< true  << (qlonglong) 1234567;
    QTest::newRow("de_DE 1.234567")    << QString("de_DE") << "1.234567"<< false  << (qlonglong) 0;
    QTest::newRow("de_DE 12.34567")    << QString("de_DE") << "12.34567"<< false  << (qlonglong) 0;
    QTest::newRow("de_DE 123.4567")    << QString("de_DE") << "123.4567"<< false  << (qlonglong) 0;
    QTest::newRow("de_DE 1234.567")    << QString("de_DE") << "1234.567"<< false  << (qlonglong) 0;
    QTest::newRow("de_DE 12345.67")    << QString("de_DE") << "12345.67"<< false  << (qlonglong) 0;
    QTest::newRow("de_DE 123456.7")    << QString("de_DE") << "123456.7"<< false  << (qlonglong) 0;
    QTest::newRow("de_DE 1.234.567")   << QString("de_DE")<< "1.234.567"<< true  << (qlonglong) 1234567;
    QTest::newRow("de_DE 1.234.567 ldspcs")   << QString("de_DE")<< "  1.234.567" << true  << (qlonglong) 1234567;
    QTest::newRow("de_DE 1.234.567 trspcs")   << QString("de_DE")<< "1.234.567  "<< true  << (qlonglong) 1234567;
    QTest::newRow("de_DE 1.234.567 ldtrspcs")   << QString("de_DE")<< "  1.234.567  "<< true  << (qlonglong) 1234567;

    // test that space is also accepted whenever QLocale::groupSeparator() == 0xa0 (which looks like space).
    QTest::newRow("nb_NO 123 groupsep")   << QString("nb_NO")<< QString("1")+QChar(0xa0)+QString("234") << true  << (qlonglong) 1234;
    QTest::newRow("nb_NO 123 groupsep_space")   << QString("nb_NO")<< QString("1")+QChar(0x20)+QString("234") << true  << (qlonglong) 1234;

    QTest::newRow("nb_NO 123 ldspcs")   << QString("nb_NO")<< "  123" << true  << (qlonglong) 123;
    QTest::newRow("nb_NO 123 trspcs")   << QString("nb_NO")<< "123  "<< true  << (qlonglong) 123;
    QTest::newRow("nb_NO 123 ldtrspcs")   << QString("nb_NO")<< "  123  "<< true  << (qlonglong) 123;

    QTest::newRow("C   1234")       << QString("C") << "  1234"   << true  << (qlonglong) 1234;
    QTest::newRow("C 1234  ")       << QString("C") << "1234  "   << true  << (qlonglong) 1234;
    QTest::newRow("C   1234  ")     << QString("C") << "  1234  " << true  << (qlonglong) 1234;
}

void tst_QLocale::long_long_conversion()
{
    QFETCH(QString, locale_name);
    QFETCH(QString, num_str);
    QFETCH(bool, good);
    QFETCH(qlonglong, num);

    QLocale locale(locale_name);
    QCOMPARE(locale.name(), locale_name);

    bool ok;
    qlonglong l = locale.toLongLong(num_str, &ok);
    QCOMPARE(ok, good);

    if (ok) {
	QCOMPARE(l, num);
    }
}

void tst_QLocale::long_long_conversion_extra()
{
    QLocale l(QLocale::C);
    QCOMPARE(l.toString((qlonglong)1), QString("1"));
    QCOMPARE(l.toString((qlonglong)12), QString("12"));
    QCOMPARE(l.toString((qlonglong)123), QString("123"));
    QCOMPARE(l.toString((qlonglong)1234), QString("1,234"));
    QCOMPARE(l.toString((qlonglong)12345), QString("12,345"));
    QCOMPARE(l.toString((qlonglong)-1), QString("-1"));
    QCOMPARE(l.toString((qlonglong)-12), QString("-12"));
    QCOMPARE(l.toString((qlonglong)-123), QString("-123"));
    QCOMPARE(l.toString((qlonglong)-1234), QString("-1,234"));
    QCOMPARE(l.toString((qlonglong)-12345), QString("-12,345"));
    QCOMPARE(l.toString((qulonglong)1), QString("1"));
    QCOMPARE(l.toString((qulonglong)12), QString("12"));
    QCOMPARE(l.toString((qulonglong)123), QString("123"));
    QCOMPARE(l.toString((qulonglong)1234), QString("1,234"));
    QCOMPARE(l.toString((qulonglong)12345), QString("12,345"));
}

/*
void tst_QLocale::languageToString()
{
}

void tst_QLocale::setDefault()
{
}
*/

void tst_QLocale::testInfAndNan()
{
    double neginf = log(0.0);
    double nan = sqrt(-1.0);

#ifdef Q_OS_WIN
    // these cause INVALID floating point exception so we want to clear the status.
    _clear87();
#endif

    QVERIFY(qIsInf(-neginf));
    QVERIFY(!qIsNaN(-neginf));
    QVERIFY(!qIsFinite(-neginf));

    QVERIFY(!qIsInf(nan));
    QVERIFY(qIsNaN(nan));
    QVERIFY(!qIsFinite(nan));

    QVERIFY(!qIsInf(1.234));
    QVERIFY(!qIsNaN(1.234));
    QVERIFY(qIsFinite(1.234));
}

void tst_QLocale::fpExceptions()
{
#ifndef _MCW_EM
#define _MCW_EM 0x0008001F
#endif
#ifndef _EM_INEXACT
#define _EM_INEXACT 0x00000001
#endif

    // check that qdtoa doesn't throw floating point exceptions when they are enabled
#ifdef Q_OS_WIN
    unsigned int oldbits = _control87(0, 0);
    _control87( 0 | _EM_INEXACT, _MCW_EM );
#endif

#ifdef Q_OS_LINUX
    fenv_t envp;
    fegetenv(&envp);
    feclearexcept(FE_ALL_EXCEPT);
    feenableexcept(FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW | FE_INVALID);
#endif

    QString::number(1000.1245);
    QString::number(1.1);
    QString::number(0.0);

    QVERIFY(true);

#ifdef Q_OS_WIN
    _clear87();
    _control87(oldbits, 0xFFFFF);
#endif

#ifdef Q_OS_LINUX
    fesetenv(&envp);
#endif
}

void tst_QLocale::negativeZero()
{
    double negativeZero( 0.0 ); // Initialise to zero.
    uchar *ptr = (uchar *)&negativeZero;
#ifdef QT_ARMFPA
    ptr[3] = 0x80;
#else
    ptr[QSysInfo::ByteOrder == QSysInfo::BigEndian ? 0 : 7] = 0x80;
#endif
    QString s = QString::number(negativeZero);
    QCOMPARE(s, QString("0"));
}

void tst_QLocale::dayOfWeek_data()
{
    QTest::addColumn<QDate>("date");
    QTest::addColumn<QString>("shortName");
    QTest::addColumn<QString>("longName");

    QTest::newRow("Sun") << QDate(2006, 1, 1) << "Sun" << "Sunday";
    QTest::newRow("Mon") << QDate(2006, 1, 2) << "Mon" << "Monday";
    QTest::newRow("Tue") << QDate(2006, 1, 3) << "Tue" << "Tuesday";
    QTest::newRow("Wed") << QDate(2006, 1, 4) << "Wed" << "Wednesday";
    QTest::newRow("Thu") << QDate(2006, 1, 5) << "Thu" << "Thursday";
    QTest::newRow("Fri") << QDate(2006, 1, 6) << "Fri" << "Friday";
    QTest::newRow("Sat") << QDate(2006, 1, 7) << "Sat" << "Saturday";
}

void tst_QLocale::dayOfWeek()
{
    QFETCH(QDate, date);
    QFETCH(QString, shortName);
    QFETCH(QString, longName);

    QCOMPARE(QLocale::c().toString(date, "ddd"), shortName);
    QCOMPARE(QLocale::c().toString(date, "dddd"), longName);
}

void tst_QLocale::formatDate_data()
{
    QTest::addColumn<QDate>("date");
    QTest::addColumn<QString>("format");
    QTest::addColumn<QString>("result");

    QTest::newRow("1") << QDate(1974, 12, 1) << "d/M/yyyy" << "1/12/1974";
    QTest::newRow("2") << QDate(1974, 12, 1) << "d/M/yyyyy" << "1/12/1974y";
    QTest::newRow("4") << QDate(1974, 1, 1) << "d/M/yyyy" << "1/1/1974";
    QTest::newRow("5") << QDate(1974, 1, 1) << "dd/MM/yyy" << "01/01/74y";
    QTest::newRow("6") << QDate(1974, 12, 1) << "ddd/MMM/yy" << "Sun/Dec/74";
    QTest::newRow("7") << QDate(1974, 12, 1) << "dddd/MMMM/y" << "Sunday/December/y";
    QTest::newRow("8") << QDate(1974, 12, 1) << "ddddd/MMMMM/yy" << "Sunday1/December12/74";
    QTest::newRow("9") << QDate(1974, 12, 1) << "'dddd'/MMMM/yy" << "dddd/December/74";
    QTest::newRow("10") << QDate(1974, 12, 1) << "d'dd'd/MMMM/yyy" << "1dd1/December/74y";
    QTest::newRow("11") << QDate(1974, 12, 1) << "d'dd'd/MMM'M'/yy" << "1dd1/DecM/74";
    QTest::newRow("12") << QDate(1974, 12, 1) << "d'd'dd/M/yy" << "1d01/12/74";

    QTest::newRow("20") << QDate(1974, 12, 1) << "foo" << "foo";
    QTest::newRow("21") << QDate(1974, 12, 1) << "'" << "";
    QTest::newRow("22") << QDate(1974, 12, 1) << "''" << "'";
    QTest::newRow("23") << QDate(1974, 12, 1) << "'''" << "'";
    QTest::newRow("24") << QDate(1974, 12, 1) << "\"" << "\"";
    QTest::newRow("25") << QDate(1974, 12, 1) << "\"\"" << "\"\"";
    QTest::newRow("26") << QDate(1974, 12, 1) << "\"yy\"" << "\"74\"";
    QTest::newRow("27") << QDate(1974, 12, 1) << "'\"yy\"'" << "\"yy\"";
    QTest::newRow("28") << QDate() << "'\"yy\"'" << "";
    QTest::newRow("29") << QDate(1974, 12, 1) << "hh:mm:ss.zzz ap d'd'dd/M/yy" << "hh:mm:ss.zzz ap 1d01/12/74";

}

void tst_QLocale::formatDate()
{
    QFETCH(QDate, date);
    QFETCH(QString, format);
    QFETCH(QString, result);

    QLocale l(QLocale::C);
    QCOMPARE(l.toString(date, format), result);
}

Q_DECLARE_METATYPE(QTime)

void tst_QLocale::formatTime_data()
{
    QTest::addColumn<QTime>("time");
    QTest::addColumn<QString>("format");
    QTest::addColumn<QString>("result");

    QTest::newRow("1") << QTime(1, 2, 3) << "h:m:s" << "1:2:3";
    QTest::newRow("3") << QTime(1, 2, 3) << "H:m:s" << "1:2:3";
    QTest::newRow("4") << QTime(1, 2, 3) << "hh:mm:ss" << "01:02:03";
    QTest::newRow("5") << QTime(1, 2, 3) << "HH:mm:ss" << "01:02:03";
    QTest::newRow("6") << QTime(1, 2, 3) << "hhh:mmm:sss" << "011:022:033";

    QTest::newRow("8") << QTime(14, 2, 3) << "h:m:s" << "14:2:3";
    QTest::newRow("9") << QTime(14, 2, 3) << "H:m:s" << "14:2:3";
    QTest::newRow("10") << QTime(14, 2, 3) << "hh:mm:ss" << "14:02:03";
    QTest::newRow("11") << QTime(14, 2, 3) << "HH:mm:ss" << "14:02:03";
    QTest::newRow("12") << QTime(14, 2, 3) << "hhh:mmm:sss" << "1414:022:033";

    QTest::newRow("14") << QTime(14, 2, 3) << "h:m:s ap" << "2:2:3 pm";
    QTest::newRow("15") << QTime(14, 2, 3) << "H:m:s AP" << "14:2:3 PM";
    QTest::newRow("16") << QTime(14, 2, 3) << "hh:mm:ss aap" << "02:02:03 pmpm";
    QTest::newRow("17") << QTime(14, 2, 3) << "HH:mm:ss AP aa" << "14:02:03 PM pmpm";

    QTest::newRow("18") << QTime(1, 2, 3) << "h:m:s ap" << "1:2:3 am";
    QTest::newRow("19") << QTime(1, 2, 3) << "H:m:s AP" << "1:2:3 AM";

    QTest::newRow("20") << QTime(1, 2, 3) << "foo" << "foo";
    QTest::newRow("21") << QTime(1, 2, 3) << "'" << "";
    QTest::newRow("22") << QTime(1, 2, 3) << "''" << "'";
    QTest::newRow("23") << QTime(1, 2, 3) << "'''" << "'";
    QTest::newRow("24") << QTime(1, 2, 3) << "\"" << "\"";
    QTest::newRow("25") << QTime(1, 2, 3) << "\"\"" << "\"\"";
    QTest::newRow("26") << QTime(1, 2, 3) << "\"H\"" << "\"1\"";
    QTest::newRow("27") << QTime(1, 2, 3) << "'\"H\"'" << "\"H\"";

    QTest::newRow("28") << QTime(1, 2, 3, 456) << "H:m:s.z" << "1:2:3.456";
    QTest::newRow("29") << QTime(1, 2, 3, 456) << "H:m:s.zz" << "1:2:3.456456";
    QTest::newRow("30") << QTime(1, 2, 3, 456) << "H:m:s.zzz" << "1:2:3.456";
    QTest::newRow("31") << QTime(1, 2, 3, 4) << "H:m:s.z" << "1:2:3.4";
    QTest::newRow("32") << QTime(1, 2, 3, 4) << "H:m:s.zzz" << "1:2:3.004";
    QTest::newRow("33") << QTime() << "H:m:s.zzz" << "";
    QTest::newRow("34") << QTime(1, 2, 3, 4) << "dd MM yyyy H:m:s.zzz" << "dd MM yyyy 1:2:3.004";
}

void tst_QLocale::formatTime()
{
    QFETCH(QTime, time);
    QFETCH(QString, format);
    QFETCH(QString, result);

    QLocale l(QLocale::C);
    QCOMPARE(l.toString(time, format), result);
}


void tst_QLocale::formatDateTime_data()
{
    QTest::addColumn<QString>("localeName");
    QTest::addColumn<QDateTime>("dateTime");
    QTest::addColumn<QString>("format");
    QTest::addColumn<QString>("result");

    QTest::newRow("1C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(5, 14, 13))
                        << "d/M/yyyy hh:h:mm" << "1/12/1974 05:5:14";
    QTest::newRow("2C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                        << "d/M/yyyyy h" << "1/12/1974y 15";
    QTest::newRow("4C") << "C" << QDateTime(QDate(1974, 1, 1), QTime(15, 14, 13))
                        << "d/M/yyyy zzz" << "1/1/1974 000";
    QTest::newRow("5C") << "C" << QDateTime(QDate(1974, 1, 1), QTime(15, 14, 13))
                        << "dd/MM/yyy z" << "01/01/74y 0";
    QTest::newRow("6C") << "C" << QDateTime(QDate(1974, 12, 2), QTime(15, 14, 13))
                        << "ddd/MMM/yy AP" << "Mon/Dec/74 PM";
    QTest::newRow("7C") << "C" << QDateTime(QDate(1974, 12, 2), QTime(15, 14, 13))
                        << "dddd/MMMM/y apa" << "Monday/December/y pmpm";
    QTest::newRow("8C") << "C" << QDateTime(QDate(1974, 12, 2), QTime(15, 14, 13))
                        << "ddddd/MMMMM/yy ss" << "Monday2/December12/74 13";
    QTest::newRow("9C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                        << "'dddd'/MMMM/yy s" << "dddd/December/74 13";
    QTest::newRow("10C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 4, 13))
                         << "d'dd'd/MMMM/yyy m'm'mm" << "1dd1/December/74y 4m04";
    QTest::newRow("11C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 3))
                         << "d'dd'd/MMM'M'/yysss" << "1dd1/DecM/74033";
    QTest::newRow("12C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                         << "d'd'dd/M/yyh" << "1d01/12/7415";

    QTest::newRow("20C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                         << "foo" << "foo";
    QTest::newRow("21C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                         << "'" << "";
    QTest::newRow("22C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                         << "''" << "'";
    QTest::newRow("23C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                         << "'''" << "'";
    QTest::newRow("24C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                         << "\"" << "\"";
    QTest::newRow("25C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                         << "\"\"" << "\"\"";
    QTest::newRow("26C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                         << "\"yymm\"" << "\"7414\"";
    QTest::newRow("27C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                         << "'\"yymm\"'" << "\"yymm\"";
    QTest::newRow("27C") << "C" << QDateTime()
                         << "'\"yymm\"'" << "";

    QTest::newRow("1no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(5, 14, 13))
                            << "d/M/yyyy hh:h:mm" << "1/12/1974 05:5:14";
    QTest::newRow("2no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                            << "d/M/yyyyy h" << "1/12/1974y 15";
    QTest::newRow("4no_NO") << "no_NO" << QDateTime(QDate(1974, 1, 1), QTime(15, 14, 13))
                            << "d/M/yyyy zzz" << "1/1/1974 000";
    QTest::newRow("5no_NO") << "no_NO" << QDateTime(QDate(1974, 1, 1), QTime(15, 14, 13))
                            << "dd/MM/yyy z" << "01/01/74y 0";
    QTest::newRow("6no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 2), QTime(15, 14, 13))
                            << "ddd/MMM/yy AP" << "man./des./74 PM";
    QTest::newRow("7no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 2), QTime(15, 14, 13))
                            << "dddd/MMMM/y apa" << "mandag/desember/y pmpm";
    QTest::newRow("8no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 2), QTime(15, 14, 13))
                            << "ddddd/MMMMM/yy ss" << "mandag2/desember12/74 13";
    QTest::newRow("9no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                            << "'dddd'/MMMM/yy s" << "dddd/desember/74 13";
    QTest::newRow("10no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 4, 13))
                             << "d'dd'd/MMMM/yyy m'm'mm" << "1dd1/desember/74y 4m04";
    QTest::newRow("11no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 3))
                             << "d'dd'd/MMM'M'/yysss" << "1dd1/des.M/74033";
    QTest::newRow("12no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                             << "d'd'dd/M/yyh" << "1d01/12/7415";

    QTest::newRow("20no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                             << "foo" << "foo";
    QTest::newRow("21no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                             << "'" << "";
    QTest::newRow("22no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                             << "''" << "'";
    QTest::newRow("23no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                             << "'''" << "'";
    QTest::newRow("24no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                             << "\"" << "\"";
    QTest::newRow("25no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                             << "\"\"" << "\"\"";
    QTest::newRow("26no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                             << "\"yymm\"" << "\"7414\"";
    QTest::newRow("27no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                             << "'\"yymm\"'" << "\"yymm\"";
    QTest::newRow("27no_NO") << "no_NO" << QDateTime()
                             << "'\"yymm\"'" << "";

}

void tst_QLocale::formatDateTime()
{
    QFETCH(QString, localeName);
    QFETCH(QDateTime, dateTime);
    QFETCH(QString, format);
    QFETCH(QString, result);

    QLocale l(localeName);
    QCOMPARE(l.toString(dateTime, format), result);
}

void tst_QLocale::toDateTime_data()
{
    QTest::addColumn<QString>("localeName");
    QTest::addColumn<QDateTime>("result");
    QTest::addColumn<QString>("format");
    QTest::addColumn<QString>("string");

    QTest::newRow("1C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(5, 14, 0))
                        << "d/M/yyyy hh:h:mm" << "1/12/1974 05:5:14";
    QTest::newRow("2C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 0, 0))
                        << "d/M/yyyyy h" << "1/12/1974y 15";
    QTest::newRow("4C") << "C" << QDateTime(QDate(1974, 1, 1), QTime(0, 0, 0))
                        << "d/M/yyyy zzz" << "1/1/1974 000";
    QTest::newRow("5C") << "C" << QDateTime(QDate(1974, 1, 1), QTime(0, 0, 0))
                        << "dd/MM/yyy z" << "01/01/74y 0";
    QTest::newRow("8C") << "C" << QDateTime(QDate(1974, 12, 2), QTime(0, 0, 13))
                        << "ddddd/MMMMM/yy ss" << "Monday2/December12/74 13";
    QTest::newRow("9C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(0, 0, 13))
                        << "'dddd'/MMMM/yy s" << "dddd/December/74 13";
    QTest::newRow("10C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(0, 4, 0))
                         << "d'dd'd/MMMM/yyy m'm'mm" << "1dd1/December/74y 4m04";
    QTest::newRow("11C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(0, 0, 3))
                         << "d'dd'd/MMM'M'/yysss" << "1dd1/DecM/74033";
    QTest::newRow("12C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 0, 0))
                         << "d'd'dd/M/yyh" << "1d01/12/7415";

    QTest::newRow("1no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(5, 14, 0))
                            << "d/M/yyyy hh:h:mm" << "1/12/1974 05:5:14";
    QTest::newRow("2no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 0, 0))
                            << "d/M/yyyyy h" << "1/12/1974y 15";
    QTest::newRow("4no_NO") << "no_NO" << QDateTime(QDate(1974, 1, 1), QTime(0, 0, 0))
                            << "d/M/yyyy zzz" << "1/1/1974 000";
    QTest::newRow("5no_NO") << "no_NO" << QDateTime(QDate(1974, 1, 1), QTime(0, 0, 0))
                            << "dd/MM/yyy z" << "01/01/74y 0";
    QTest::newRow("8no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 2), QTime(0, 0, 13))
                            << "ddddd/MMMMM/yy ss" << "mandag2/desember12/74 13";
    QTest::newRow("9no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(0, 0, 13))
                            << "'dddd'/MMMM/yy s" << "dddd/desember/74 13";
    QTest::newRow("10no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(0, 4, 0))
                             << "d'dd'd/MMMM/yyy m'm'mm" << "1dd1/desember/74y 4m04";
    QTest::newRow("11no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(0, 0, 3))
                             << "d'dd'd/MMM'M'/yysss" << "1dd1/des.M/74033";
    QTest::newRow("12no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 0, 0))
                             << "d'd'dd/M/yyh" << "1d01/12/7415";

    QTest::newRow("RFC-1123") << "C" << QDateTime(QDate(2007, 11, 1), QTime(18, 8, 30))
                              << "ddd, dd MMM yyyy hh:mm:ss 'GMT'" << "Thu, 01 Nov 2007 18:08:30 GMT";

    QTest::newRow("longFormat") << "en_US" << QDateTime(QDate(2009, 1, 5), QTime(11, 48, 32))
                      << "dddd, MMMM d, yyyy h:mm:ss AP " << "Monday, January 5, 2009 11:48:32 AM ";
}

void tst_QLocale::toDateTime()
{
    QFETCH(QString, localeName);
    QFETCH(QDateTime, result);
    QFETCH(QString, format);
    QFETCH(QString, string);

    QLocale l(localeName);
    QCOMPARE(l.toDateTime(string, format), result);
    if (l.dateTimeFormat(QLocale::LongFormat) == format)
        QCOMPARE(l.toDateTime(string, QLocale::LongFormat), result);
}

#ifdef Q_OS_MAC
void tst_QLocale::macDefaultLocale()
{
    QLocale locale = QLocale::system();
    if (locale.name() != QLatin1String("en_US")) {
        QSKIP("This test only tests for en_US", SkipAll);
    }

    QTime invalidTime;
    QDate invalidDate;
    QCOMPARE(locale.toString(invalidTime, QLocale::ShortFormat), QString());
    QCOMPARE(locale.toString(invalidDate, QLocale::ShortFormat), QString());
    QCOMPARE(locale.toString(invalidTime, QLocale::NarrowFormat), QString());
    QCOMPARE(locale.toString(invalidDate, QLocale::NarrowFormat), QString());
    QCOMPARE(locale.toString(invalidTime, QLocale::LongFormat), QString());
    QCOMPARE(locale.toString(invalidDate, QLocale::LongFormat), QString());
    QCOMPARE(locale.decimalPoint(), QChar('.'));
    QCOMPARE(locale.groupSeparator(), QChar(','));
    QCOMPARE(locale.dateFormat(QLocale::ShortFormat), QString("M/d/yy"));
    if (QSysInfo::MacintoshVersion > QSysInfo::MV_10_6)
        QCOMPARE(locale.dateFormat(QLocale::LongFormat), QString("MMMM d, y"));
    else
        QCOMPARE(locale.dateFormat(QLocale::LongFormat), QString("MMMM d, yyyy"));
    QCOMPARE(locale.timeFormat(QLocale::ShortFormat), QString("h:mm AP"));
    QCOMPARE(locale.timeFormat(QLocale::LongFormat), QString("h:mm:ss AP t"));

    // make sure we are using the system to parse them
    QCOMPARE(locale.toString(1234.56), QString("1,234.56"));
    QCOMPARE(locale.toString(QDate(1974, 12, 1), QLocale::ShortFormat), QString("12/1/74"));
    QCOMPARE(locale.toString(QDate(1974, 12, 1), QLocale::NarrowFormat), locale.toString(QDate(1974, 12, 1), QLocale::ShortFormat));
    QCOMPARE(locale.toString(QDate(1974, 12, 1), QLocale::LongFormat), QString("December 1, 1974"));
    QCOMPARE(locale.toString(QTime(1,2,3), QLocale::ShortFormat), QString("1:02 AM"));
    QCOMPARE(locale.toString(QTime(1,2,3), QLocale::NarrowFormat), locale.toString(QTime(1,2,3), QLocale::ShortFormat));

    QTime currentTime = QTime::currentTime();
    QTime utcTime = QDateTime::currentDateTime().toUTC().time();

    int diff = currentTime.hour() - utcTime.hour();

     // Check if local time and utc time are on opposite sides of the 24-hour wrap-around.
	if (diff < -12)
	   diff += 24;
	if (diff > 12)
	   diff -= 24;

	const QString timeString = locale.toString(QTime(1,2,3), QLocale::LongFormat);
    QVERIFY(timeString.contains(QString("1:02:03")));

    // System Preferences->Language & Text, Region Tab, should choose "United States" for Region field
    QCOMPARE(locale.toCurrencyString(qulonglong(1234)), QString("$1,234.00"));
    QCOMPARE(locale.toCurrencyString(qlonglong(-1234)), QString("($1,234.00)"));
    QCOMPARE(locale.toCurrencyString(double(1234.56)), QString("$1,234.56"));
    QCOMPARE(locale.toCurrencyString(double(-1234.56)), QString("($1,234.56)"));

    // Depending on the configured time zone, the time string might not
    // contain a GMT specifier. (Sometimes it just names the zone, like "CEST")
    if (timeString.contains(QString("GMT"))) {
        QString expectedGMTSpecifier("GMT");
        if (diff >= 0)
            expectedGMTSpecifier.append("+");
        else
            expectedGMTSpecifier.append("-");
        if (qAbs(diff) < 10)
            expectedGMTSpecifier.append(QString("0%1").arg(qAbs(diff)));
        else
            expectedGMTSpecifier.append(QString("%1").arg(qAbs(diff)));
        QVERIFY2(timeString.contains(expectedGMTSpecifier), qPrintable(
            QString("timeString `%1', expectedGMTSpecifier `%2'")
            .arg(timeString)
            .arg(expectedGMTSpecifier)
        ));
    }
    QCOMPARE(locale.dayName(1), QString("Monday"));
    QCOMPARE(locale.dayName(7), QString("Sunday"));
    QCOMPARE(locale.monthName(1), QString("January"));
    QCOMPARE(locale.monthName(12), QString("December"));
    QCOMPARE(locale.firstDayOfWeek(), Qt::Sunday);
    QCOMPARE(locale.quoteString("string"), QString::fromUtf8("\xe2\x80\x9c" "string" "\xe2\x80\x9d"));
    QCOMPARE(locale.quoteString("string", QLocale::AlternateQuotation), QString::fromUtf8("\xe2\x80\x98" "string" "\xe2\x80\x99"));

    QList<Qt::DayOfWeek> days;
    days << Qt::Monday << Qt::Tuesday << Qt::Wednesday << Qt::Thursday << Qt::Friday;
    QCOMPARE(locale.weekdays(), days);

}
#endif

#ifdef Q_OS_WIN
#include <qt_windows.h>

static QString getWinLocaleInfo(LCTYPE type)
{
    LCID id = GetThreadLocale();
    int cnt = GetLocaleInfo(id, type, 0, 0) * 2;

    if (cnt == 0) {
        qWarning("QLocale: empty windows locale info (%d)", type);
        return QString();
    }

    QByteArray buff(cnt, 0);

    cnt = GetLocaleInfo(id, type, reinterpret_cast<wchar_t*>(buff.data()), buff.size() / 2);

    if (cnt == 0) {
        qWarning("QLocale: empty windows locale info (%d)", type);
        return QString();
    }

    return QString::fromWCharArray(reinterpret_cast<wchar_t*>(buff.data()));
}

static void setWinLocaleInfo(LCTYPE type, const QString &value)
{
    LCID id = GetThreadLocale();
    SetLocaleInfo(id, type, reinterpret_cast<const wchar_t*>(value.utf16()));
}

class RestoreLocaleHelper {
public:
    RestoreLocaleHelper() {
        m_decimal = getWinLocaleInfo(LOCALE_SDECIMAL);
        m_thousand = getWinLocaleInfo(LOCALE_STHOUSAND);
        m_sdate = getWinLocaleInfo(LOCALE_SSHORTDATE);
        m_ldate = getWinLocaleInfo(LOCALE_SLONGDATE);
        m_time = getWinLocaleInfo(LOCALE_STIMEFORMAT);
    }

    ~RestoreLocaleHelper() {
        // restore these, or the user will get a surprise
        setWinLocaleInfo(LOCALE_SDECIMAL, m_decimal);
        setWinLocaleInfo(LOCALE_STHOUSAND, m_thousand);
        setWinLocaleInfo(LOCALE_SSHORTDATE, m_sdate);
        setWinLocaleInfo(LOCALE_SLONGDATE, m_ldate);
        setWinLocaleInfo(LOCALE_STIMEFORMAT, m_time);
    }

    QString m_decimal, m_thousand, m_sdate, m_ldate, m_time;

};

#endif

void tst_QLocale::windowsDefaultLocale()
{
#ifndef Q_OS_WIN
    QSKIP("This is a Windows test", SkipAll);
#else
    RestoreLocaleHelper systemLocale;
    // set weird system defaults and make sure we're using them
    setWinLocaleInfo(LOCALE_SDECIMAL, QLatin1String("@"));
    setWinLocaleInfo(LOCALE_STHOUSAND, QLatin1String("?"));
    setWinLocaleInfo(LOCALE_SSHORTDATE, QLatin1String("d*M*yyyy"));
    setWinLocaleInfo(LOCALE_SLONGDATE, QLatin1String("d@M@yyyy"));
    setWinLocaleInfo(LOCALE_STIMEFORMAT, QLatin1String("h^m^s"));
    QLocale locale = QLocale::system();

    // make sure we are seeing the system's format strings
    QCOMPARE(locale.decimalPoint(), QChar('@'));
    QCOMPARE(locale.groupSeparator(), QChar('?'));
    QCOMPARE(locale.dateFormat(QLocale::ShortFormat), QString("d*M*yyyy"));
    QCOMPARE(locale.dateFormat(QLocale::LongFormat), QString("d@M@yyyy"));
    QCOMPARE(locale.timeFormat(QLocale::ShortFormat), QString("h^m^s"));
    QCOMPARE(locale.timeFormat(QLocale::LongFormat), QString("h^m^s"));
    QCOMPARE(locale.dateTimeFormat(QLocale::ShortFormat), QString("d*M*yyyy h^m^s"));
    QCOMPARE(locale.dateTimeFormat(QLocale::LongFormat), QString("d@M@yyyy h^m^s"));

    // make sure we are using the system to parse them
    QCOMPARE(locale.toString(1234.56), QString("1?234@56"));
    QCOMPARE(locale.toString(QDate(1974, 12, 1), QLocale::ShortFormat), QString("1*12*1974"));
    QCOMPARE(locale.toString(QDate(1974, 12, 1), QLocale::NarrowFormat), locale.toString(QDate(1974, 12, 1), QLocale::ShortFormat));
    QCOMPARE(locale.toString(QDate(1974, 12, 1), QLocale::LongFormat), QString("1@12@1974"));
    QCOMPARE(locale.toString(QTime(1,2,3), QLocale::ShortFormat), QString("1^2^3"));
    QCOMPARE(locale.toString(QTime(1,2,3), QLocale::NarrowFormat), locale.toString(QTime(1,2,3), QLocale::ShortFormat));
    QCOMPARE(locale.toString(QTime(1,2,3), QLocale::LongFormat), QString("1^2^3"));
    QCOMPARE(locale.toString(QDateTime(QDate(1974, 12, 1), QTime(1,2,3)), QLocale::ShortFormat),
             QString("1*12*1974 1^2^3"));
    QCOMPARE(locale.toString(QDateTime(QDate(1974, 12, 1), QTime(1,2,3)), QLocale::NarrowFormat),
             locale.toString(QDateTime(QDate(1974, 12, 1), QTime(1,2,3)), QLocale::ShortFormat));
    QCOMPARE(locale.toString(QDateTime(QDate(1974, 12, 1), QTime(1,2,3)), QLocale::LongFormat),
             QString("1@12@1974 1^2^3"));
    QCOMPARE(locale.toString(QTime(1,2,3), QLocale::LongFormat), QString("1^2^3"));
#endif
}

void tst_QLocale::numberOptions()
{
    bool ok;

    QLocale locale(QLocale::C);
    QCOMPARE(locale.numberOptions(), 0);
    QCOMPARE(locale.toInt(QString("12,345"), &ok), 12345);
    QVERIFY(ok);
    QCOMPARE(locale.toInt(QString("12345"), &ok), 12345);
    QVERIFY(ok);
    QCOMPARE(locale.toString(12345), QString("12,345"));

    locale.setNumberOptions(QLocale::OmitGroupSeparator);
    QCOMPARE(locale.numberOptions(), QLocale::OmitGroupSeparator);
    QCOMPARE(locale.toInt(QString("12,345"), &ok), 12345);
    QVERIFY(ok);
    QCOMPARE(locale.toInt(QString("12345"), &ok), 12345);
    QVERIFY(ok);
    QCOMPARE(locale.toString(12345), QString("12345"));

    locale.setNumberOptions(QLocale::RejectGroupSeparator);
    QCOMPARE(locale.numberOptions(), QLocale::RejectGroupSeparator);
    locale.toInt(QString("12,345"), &ok);
    QVERIFY(!ok);
    QCOMPARE(locale.toInt(QString("12345"), &ok), 12345);
    QVERIFY(ok);
    QCOMPARE(locale.toString(12345), QString("12,345"));

    QLocale locale2 = locale;
    QCOMPARE(locale2.numberOptions(), QLocale::RejectGroupSeparator);
}

void tst_QLocale::negativeNumbers()
{
    QLocale locale(QLocale::C);

    bool ok;
    int i;

    i = locale.toInt(QLatin1String("-100"), &ok);
    QVERIFY(ok);
    QCOMPARE(i, -100);

    i = locale.toInt(QLatin1String("-1,000"), &ok);
    QVERIFY(ok);
    QCOMPARE(i, -1000);

    i = locale.toInt(QLatin1String("-1000"), &ok);
    QVERIFY(ok);
    QCOMPARE(i, -1000);

    i = locale.toInt(QLatin1String("-10,000"), &ok);
    QVERIFY(ok);
    QCOMPARE(i, -10000);

    i = locale.toInt(QLatin1String("-10000"), &ok);
    QVERIFY(ok);
    QCOMPARE(i, -10000);

    i = locale.toInt(QLatin1String("-100,000"), &ok);
    QVERIFY(ok);
    QCOMPARE(i, -100000);

    i = locale.toInt(QLatin1String("-100000"), &ok);
    QVERIFY(ok);
    QCOMPARE(i, -100000);

    i = locale.toInt(QLatin1String("-1,000,000"), &ok);
    QVERIFY(ok);
    QCOMPARE(i, -1000000);

    i = locale.toInt(QLatin1String("-1000000"), &ok);
    QVERIFY(ok);
    QCOMPARE(i, -1000000);
}

#include <private/qlocale_p.h>
#include <private/qlocale_data_p.h>

static const int locale_data_count = sizeof(locale_data)/sizeof(locale_data[0]);

void tst_QLocale::testNames_data()
{
    QTest::addColumn<int>("language");
    QTest::addColumn<int>("country");

    for (int i = 0; i < locale_data_count; ++i) {
        const QLocalePrivate &item = locale_data[i];

        const QString testName = QString::fromLatin1("data_%1 (%2/%3)").arg(i)
                .arg(QLocale::languageToString((QLocale::Language)item.m_language_id))
                .arg(QLocale::countryToString((QLocale::Country)item.m_country_id));
        QTest::newRow(testName.toLatin1().constData()) << (int)item.m_language_id << (int)item.m_country_id;
    }
}

void tst_QLocale::testNames()
{
    QFETCH(int, language);
    QFETCH(int, country);

    QLocale l1((QLocale::Language)language, (QLocale::Country)country);
    if (language == QLocale::AnyLanguage && country == QLocale::AnyCountry)
        language = QLocale::C;
    QCOMPARE((int)l1.language(), language);
    QCOMPARE((int)l1.country(), country);

    QString name = l1.name();

    QLocale l2(name);
    QCOMPARE((int)l2.language(), language);
    QCOMPARE((int)l2.country(), country);
    QCOMPARE(l2.name(), name);

    QLocale l3(name + QLatin1String("@foo"));
    QCOMPARE((int)l3.language(), language);
    QCOMPARE((int)l3.country(), country);
    QCOMPARE(l3.name(), name);

    QLocale l4(name + QLatin1String(".foo"));
    QCOMPARE((int)l4.language(), language);
    QCOMPARE((int)l4.country(), country);
    QCOMPARE(l4.name(), name);

    if (language != QLocale::C) {
        int idx = name.indexOf(QLatin1Char('_'));
        QVERIFY(idx != -1);
        QString lang = name.left(idx);

        QCOMPARE((int)QLocale(lang).language(), language);
        QCOMPARE((int)QLocale(lang + QLatin1String("@foo")).language(), language);
        QCOMPARE((int)QLocale(lang + QLatin1String(".foo")).language(), language);
    }
}

void tst_QLocale::dayName_data()
{
    QTest::addColumn<QString>("locale_name");
    QTest::addColumn<QString>("dayName");
    QTest::addColumn<int>("day");
    QTest::addColumn<QLocale::FormatType>("format");

    QTest::newRow("no_NO")  << QString("no_NO") << QString("tirsdag") << 2 << QLocale::LongFormat;
    QTest::newRow("nb_NO")  << QString("nb_NO") << QString("tirsdag") << 2 << QLocale::LongFormat;
    QTest::newRow("nn_NO")  << QString("nn_NO") << QString("tysdag") << 2 << QLocale::LongFormat;

    QTest::newRow("C long")  << QString("C") << QString("Sunday") << 7 << QLocale::LongFormat;
    QTest::newRow("C short")  << QString("C") << QString("Sun") << 7 << QLocale::ShortFormat;
    QTest::newRow("C narrow")  << QString("C") << QString("7") << 7 << QLocale::NarrowFormat;

    QTest::newRow("ru_RU long")  << QString("ru_RU") << QString::fromUtf8("\320\262\320\276\321\201\320\272\321\200\320\265\321\201\320\265\320\275\321\214\320\265") << 7 << QLocale::LongFormat;
    QTest::newRow("ru_RU short")  << QString("ru_RU") << QString::fromUtf8("\320\262\321\201") << 7 << QLocale::ShortFormat;
    QTest::newRow("ru_RU narrow")  << QString("ru_RU") << QString::fromUtf8("\320\222") << 7 << QLocale::NarrowFormat;
}

void tst_QLocale::dayName()
{
    QFETCH(QString, locale_name);
    QFETCH(QString, dayName);
    QFETCH(int, day);
    QFETCH(QLocale::FormatType, format);

    QLocale l(locale_name);
    QCOMPARE(l.dayName(day, format), dayName);

    QLocale ir("ga_IE");
    QCOMPARE(ir.dayName(1, QLocale::ShortFormat), QLatin1String("Luan"));
    QCOMPARE(ir.dayName(7, QLocale::ShortFormat), QLatin1String("Domh"));
}

void tst_QLocale::standaloneDayName_data()
{
    QTest::addColumn<QString>("locale_name");
    QTest::addColumn<QString>("dayName");
    QTest::addColumn<int>("day");
    QTest::addColumn<QLocale::FormatType>("format");

    QTest::newRow("no_NO")  << QString("no_NO") << QString("tirsdag") << 2 << QLocale::LongFormat;
    QTest::newRow("nb_NO")  << QString("nb_NO") << QString("tirsdag") << 2 << QLocale::LongFormat;
    QTest::newRow("nn_NO")  << QString("nn_NO") << QString("tysdag") << 2 << QLocale::LongFormat;

    QTest::newRow("C invalid: 0 long")  << QString("C") << QString() << 0 << QLocale::LongFormat;
    QTest::newRow("C invalid: 0 short")  << QString("C") << QString() << 0 << QLocale::ShortFormat;
    QTest::newRow("C invalid: 0 narrow")  << QString("C") << QString() << 0 << QLocale::NarrowFormat;
    QTest::newRow("C invalid: 8 long")  << QString("C") << QString() << 8 << QLocale::LongFormat;
    QTest::newRow("C invalid: 8 short")  << QString("C") << QString() << 8 << QLocale::ShortFormat;
    QTest::newRow("C invalid: 8 narrow")  << QString("C") << QString() << 8 << QLocale::NarrowFormat;

    QTest::newRow("C long")  << QString("C") << QString("Sunday") << 7 << QLocale::LongFormat;
    QTest::newRow("C short")  << QString("C") << QString("Sun") << 7 << QLocale::ShortFormat;
    QTest::newRow("C narrow")  << QString("C") << QString("S") << 7 << QLocale::NarrowFormat;

    QTest::newRow("ru_RU long")  << QString("ru_RU") << QString::fromUtf8("\320\222\320\276\321\201\320\272\321\200\320\265\321\201\320\265\320\275\321\214\320\265") << 7 << QLocale::LongFormat;
    QTest::newRow("ru_RU short")  << QString("ru_RU") << QString::fromUtf8("\320\222\321\201") << 7 << QLocale::ShortFormat;
    QTest::newRow("ru_RU narrow")  << QString("ru_RU") << QString::fromUtf8("\320\222") << 7 << QLocale::NarrowFormat;
}

void tst_QLocale::standaloneDayName()
{
    QFETCH(QString, locale_name);
    QFETCH(QString, dayName);
    QFETCH(int, day);
    QFETCH(QLocale::FormatType, format);

    QLocale l(locale_name);
    QCOMPARE(l.standaloneDayName(day, format), dayName);
}

void tst_QLocale::underflowOverflow()
{
    QString
a(QLatin1String("0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001e10"));

    bool ok = false;
    a.toDouble(&ok);
    QVERIFY(!ok);

    a = QLatin1String("1e600");
    ok = false;
    a.toDouble(&ok);
    QVERIFY(!ok);

    a = QLatin1String("-9223372036854775809");
    a.toLongLong(&ok);
    QVERIFY(!ok);
}

void tst_QLocale::measurementSystems_data()
{
    QTest::addColumn<QString>("localeName");
    QTest::addColumn<int>("mSystem");

    QTest::newRow("no_NO") << QString("no_NO") << (int)QLocale::MetricSystem; // Norwegian/Norway
    QTest::newRow("sv_SE") << QString("sv_SE") << (int)QLocale::MetricSystem; // Swedish/Sweden
    QTest::newRow("en_US") << QString("en_US") << (int)QLocale::ImperialSystem; // English/United States
    QTest::newRow("en_GB") << QString("en_GB") << (int)QLocale::MetricSystem; // English/Great Britain
    QTest::newRow("no")    << QString("no")    << (int)QLocale::MetricSystem; // Norwegian
    QTest::newRow("en")    << QString("en")    << (int)QLocale::ImperialSystem; // English
    QTest::newRow("es_US") << QString("es_US") << (int)QLocale::ImperialSystem; // Spanish/United States
    QTest::newRow("es_ES") << QString("es_ES") << (int)QLocale::MetricSystem; // Spanish/Spain
    QTest::newRow("es")    << QString("es")    << (int)QLocale::MetricSystem; // Spanish
}

void tst_QLocale::measurementSystems()
{
    QSKIP("Meh, skip the test as we do not reread the environment variables anymore", SkipAll);
    QFETCH(QString, localeName);
    QFETCH(int, mSystem);

    // Method should be const.
    const QLocale locale(localeName);
    QCOMPARE((int)locale.measurementSystem(), mSystem);
}

void tst_QLocale::systemMeasurementSystems_data()
{
    QTest::addColumn<QString>("lcAllLocale");
    QTest::addColumn<QString>("lcMeasurementLocale");
    QTest::addColumn<QString>("langLocale");
    QTest::addColumn<int>("mSystem");

    QTest::newRow("row0") << QString("no_NO") << QString("no_NO") << QString("no_NO")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row1") << QString("no_NO") << QString("no_NO") << QString("en_US")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row2") << QString("no_NO") << QString("en_US") << QString("no_NO")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row3") << QString("no_NO") << QString("en_US") << QString("en_US")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row4") << QString("en_US") << QString("no_NO") << QString("no_NO")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row5") << QString("en_US") << QString("no_NO") << QString("en_US")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row6") << QString("en_US") << QString("en_US") << QString("no_NO")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row7") << QString("en_US") << QString("en_US") << QString("en_US")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row8") << QString("") << QString("") << QString("")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row9") << QString("") << QString("") << QString("no_NO")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row10") << QString("") << QString("") << QString("en_US")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row11") << QString("") << QString("no_NO") << QString("")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row12") << QString("") << QString("en_US") << QString("")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row13") << QString("") << QString("no_NO") << QString("en_US")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row14") << QString("") << QString("en_US") << QString("no_NO")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row15") << QString("no_NO") << QString("") << QString("")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row16") << QString("en_US") << QString("") << QString("")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row17") << QString("no_NO") << QString("en_US") << QString("")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row18") << QString("en_US") << QString("no_NO") << QString("")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row19") << QString("no_NO") << QString("") << QString("en_US")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row20") << QString("en_US") << QString("") << QString("no_NO")
            << (int)QLocale::ImperialSystem;
}

void tst_QLocale::systemMeasurementSystems()
{
    QSKIP("Meh, skip the test as we do not reread the environment variables anymore", SkipAll);
    // Theoretically, we could include HPUX in this test, but its setenv implementation
    // stinks. It's called putenv, and it requires you to keep the variable you pass
    // to it around forever.
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC) && !defined(Q_OS_SYMBIAN)
    QFETCH(QString, lcAllLocale);
    QFETCH(QString, lcMeasurementLocale);
    QFETCH(QString, langLocale);
    QFETCH(int, mSystem);

    // Save any old environment variables.
    QString oldLcAll = QString::fromLocal8Bit(getenv("LC_ALL"));
    QString oldLcMeasurement = QString::fromLocal8Bit(getenv("LC_MEASUREMENT"));
    QString oldLang = QString::fromLocal8Bit(getenv("LANG"));

    qputenv("LC_ALL", lcAllLocale.toLocal8Bit());
    qputenv("LC_MEASUREMENT", lcMeasurementLocale.toLocal8Bit());
    qputenv("LANG", langLocale.toLocal8Bit());

    // Method should be const.
    const QLocale locale(QLocale::system());
    QCOMPARE((int)locale.measurementSystem(), mSystem);

    // Restore environment.
    qputenv("LC_ALL", oldLcAll.toLocal8Bit());
    qputenv("LC_MEASUREMENT", oldLcMeasurement.toLocal8Bit());
    qputenv("LANG", oldLang.toLocal8Bit());
#else
    QSKIP("Test doesn't work on Mac, Windows or Symbian", SkipAll);
#endif
}

class SystemLocale : public QSystemLocale
{
public:
    virtual QVariant query(QueryType type, QVariant in) const
    {
        switch (type) {
        case DateTimeFormatLong: return QLatin1String("dddd ddd dd d MMMM MMM MM M yyyy hh:mm:ss.zzz");
        case DateTimeFormatShort: return QLatin1String("d M yy h:m");
        case DateTimeToStringLong:
        case DateTimeToStringShort:
            return in.toDateTime().toString(type == DateTimeToStringShort
                                            ? QLatin1String("dMyyhm")
                                            : QLatin1String("ddMMyyyyhhmmsszzz"));
        default:
            break;
        }
        return QSystemLocale::query(type, in);
    }
};



void tst_QLocale::queryDateTime()
{
    SystemLocale loc;
    QCOMPARE(QLocale::system().dateTimeFormat(QLocale::LongFormat),
             loc.query(QSystemLocale::DateTimeFormatLong, QVariant()).toString());
    QCOMPARE(QLocale::system().dateTimeFormat(QLocale::ShortFormat),
             loc.query(QSystemLocale::DateTimeFormatShort, QVariant()).toString());
    QCOMPARE(QLocale::system().toString(QDateTime(QDate(1974, 12, 1), QTime(1, 2, 3, 4)), QLocale::ShortFormat),
             QString("1127412"));
    QCOMPARE(QLocale::system().toString(QDateTime(QDate(1974, 12, 1), QTime(1, 2, 3, 4)), QLocale::NarrowFormat),
             QLocale::system().toString(QDateTime(QDate(1974, 12, 1), QTime(1, 2, 3, 4)), QLocale::ShortFormat));
    QCOMPARE(QLocale::system().toString(QDateTime(QDate(1974, 12, 1), QTime(1, 2, 3, 4)), QLocale::LongFormat),
             QString("01121974010203004"));
}

#ifndef QT_NO_SYSTEMLOCALE
void tst_QLocale::queryMeasureSystem_data()
{
    QTest::addColumn<QString>("lcAllLocale");
    QTest::addColumn<QString>("lcMeasurementLocale");
    QTest::addColumn<QString>("langLocale");
    QTest::addColumn<int>("mSystem");

    QTest::newRow("row0") << QString("no_NO") << QString("no_NO") << QString("no_NO")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row1") << QString("no_NO") << QString("no_NO") << QString("en_US")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row2") << QString("no_NO") << QString("en_US") << QString("no_NO")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row3") << QString("no_NO") << QString("en_US") << QString("en_US")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row4") << QString("en_US") << QString("no_NO") << QString("no_NO")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row5") << QString("en_US") << QString("no_NO") << QString("en_US")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row6") << QString("en_US") << QString("en_US") << QString("no_NO")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row7") << QString("en_US") << QString("en_US") << QString("en_US")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row8") << QString("") << QString("") << QString("")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row9") << QString("") << QString("") << QString("no_NO")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row10") << QString("") << QString("") << QString("en_US")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row11") << QString("") << QString("no_NO") << QString("")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row12") << QString("") << QString("en_US") << QString("")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row13") << QString("") << QString("no_NO") << QString("en_US")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row14") << QString("") << QString("en_US") << QString("no_NO")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row15") << QString("no_NO") << QString("") << QString("")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row16") << QString("en_US") << QString("") << QString("")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row17") << QString("no_NO") << QString("en_US") << QString("")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row18") << QString("en_US") << QString("no_NO") << QString("")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row19") << QString("no_NO") << QString("") << QString("en_US")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row20") << QString("en_US") << QString("") << QString("no_NO")
            << (int)QLocale::ImperialSystem;
}

void tst_QLocale::queryMeasureSystem()
{
    QSKIP("Meh, skip the test as we do not reread the environment variables anymore", SkipAll);
    // Theoretically, we could include HPUX in this test, but its setenv implementation
    // stinks. It's called putenv, and it requires you to keep the variable you pass
    // to it around forever.
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC) && !defined(Q_OS_SYMBIAN)
    QFETCH(QString, lcAllLocale);
    QFETCH(QString, lcMeasurementLocale);
    QFETCH(QString, langLocale);
    QFETCH(int, mSystem);

    // Save any old environment variables.
    QString oldLcAll = QString::fromLocal8Bit(getenv("LC_ALL"));
    QString oldLcMeasurement = QString::fromLocal8Bit(getenv("LC_MEASUREMENT"));
    QString oldLang = QString::fromLocal8Bit(getenv("LANG"));

    qputenv("LC_ALL", lcAllLocale.toLocal8Bit());
    qputenv("LC_MEASUREMENT", lcMeasurementLocale.toLocal8Bit());
    qputenv("LANG", langLocale.toLocal8Bit());

    // Method should be const.
    const QSystemLocale locale;
    QCOMPARE(locale.query(QSystemLocale::MeasurementSystem, QVariant()).toInt(), mSystem);

    // Restore environment.
    qputenv("LC_ALL", oldLcAll.toLocal8Bit());
    qputenv("LC_MEASUREMENT", oldLcMeasurement.toLocal8Bit());
    qputenv("LANG", oldLang.toLocal8Bit());
#else
    QSKIP("Test doesn't work on Mac, Windows or Symbian", SkipAll);
#endif
}
#endif // QT_NO_SYSTEMLOCALE

void tst_QLocale::defaultNumeringSystem()
{
    QLocale sk("sk_SK");
    QCOMPARE(sk.toString(123), QLatin1String("123"));

    QLocale ta("ta_IN");
    QCOMPARE(ta.toString(123), QLatin1String("123"));

    QLocale te("te_IN");
    QCOMPARE(te.toString(123), QLatin1String("123"));

    QLocale hi("hi_IN");
    QCOMPARE(hi.toString(123), QLatin1String("123"));

    QLocale gu("gu_IN");
    QCOMPARE(gu.toString(123), QLatin1String("123"));

    QLocale kn("kn_IN");
    QCOMPARE(kn.toString(123), QLatin1String("123"));

    QLocale pa("pa_IN");
    QCOMPARE(pa.toString(123), QLatin1String("123"));

    QLocale ne("ne_IN");
    QCOMPARE(ne.toString(123), QLatin1String("123"));

    QLocale mr("mr_IN");
    QCOMPARE(mr.toString(123), QLatin1String("123"));

    QLocale ml("ml_IN");
    QCOMPARE(ml.toString(123), QLatin1String("123"));

    QLocale kok("kok_IN");
    QCOMPARE(kok.toString(123), QLatin1String("123"));
}

void tst_QLocale::ampm()
{
    QLocale c(QLocale::C);
    QCOMPARE(c.amText(), QLatin1String("AM"));
    QCOMPARE(c.pmText(), QLatin1String("PM"));

    QLocale de("de_DE");
    QCOMPARE(de.amText(), QLatin1String("vorm."));
    QCOMPARE(de.pmText(), QLatin1String("nachm."));

    QLocale sv("sv_SE");
    QCOMPARE(sv.amText(), QLatin1String("fm"));
    QCOMPARE(sv.pmText(), QLatin1String("em"));

    QLocale nn("nl_NL");
    QCOMPARE(nn.amText(), QLatin1String("AM"));
    QCOMPARE(nn.pmText(), QLatin1String("PM"));

    QLocale ua("uk_UA");
    QCOMPARE(ua.amText(), QString::fromUtf8("\320\264\320\277"));
    QCOMPARE(ua.pmText(), QString::fromUtf8("\320\277\320\277"));

    QLocale tr("tr_TR");
    QCOMPARE(tr.amText(), QString::fromUtf8("\303\226\303\226"));
    QCOMPARE(tr.pmText(), QString::fromUtf8("\303\226\123"));

    QLocale id("id_ID");
    QCOMPARE(id.amText(), QLatin1String("AM"));
    QCOMPARE(id.pmText(), QLatin1String("PM"));

    QLocale ta("ta_LK");
    QCOMPARE(ta.amText(), QLatin1String("AM"));
    QCOMPARE(ta.pmText(), QLatin1String("PM"));
}

void tst_QLocale::dateFormat()
{
    const QLocale c(QLocale::C);
    // check that the NarrowFormat is the same as ShortFormat.
    QCOMPARE(c.dateFormat(QLocale::NarrowFormat), c.dateFormat(QLocale::ShortFormat));

    const QLocale no("no_NO");
    QCOMPARE(no.dateFormat(QLocale::NarrowFormat), QLatin1String("dd.MM.yy"));
    QCOMPARE(no.dateFormat(QLocale::ShortFormat), QLatin1String("dd.MM.yy"));
    QCOMPARE(no.dateFormat(QLocale::LongFormat), QLatin1String("dddd d. MMMM yyyy"));

    const QLocale ca("en_CA");
    QCOMPARE(ca.dateFormat(QLocale::ShortFormat), QLatin1String("M/d/yy"));
    QCOMPARE(ca.dateFormat(QLocale::LongFormat), QLatin1String("dddd, MMMM d, yyyy"));

    const QLocale ja("ja_JP");
    QCOMPARE(ja.dateFormat(QLocale::ShortFormat), QLatin1String("yyyy/MM/dd"));

    const QLocale ir("ga_IE");
    QCOMPARE(ir.dateFormat(QLocale::ShortFormat), QLatin1String("dd/MM/yyyy"));
}

void tst_QLocale::timeFormat()
{
    const QLocale c(QLocale::C);
    // check that the NarrowFormat is the same as ShortFormat.
    QCOMPARE(c.timeFormat(QLocale::NarrowFormat), c.timeFormat(QLocale::ShortFormat));

    const QLocale no("no_NO");
    QCOMPARE(no.timeFormat(QLocale::NarrowFormat), QLatin1String("HH:mm"));
    QCOMPARE(no.timeFormat(QLocale::ShortFormat), QLatin1String("HH:mm"));
    QCOMPARE(no.timeFormat(QLocale::LongFormat), QLatin1String("'kl'. HH:mm:ss t"));

    const QLocale id("id_ID");
    QCOMPARE(id.timeFormat(QLocale::ShortFormat), QLatin1String("HH.mm"));
    QCOMPARE(id.timeFormat(QLocale::LongFormat), QLatin1String("HH.mm.ss t"));

    const QLocale cat("ca_ES");
    QCOMPARE(cat.timeFormat(QLocale::ShortFormat), QLatin1String("H.mm"));
    QCOMPARE(cat.timeFormat(QLocale::LongFormat), QLatin1String("H.mm.ss t"));
}

void tst_QLocale::dateTimeFormat()
{
    const QLocale c(QLocale::C);
    // check that the NarrowFormat is the same as ShortFormat.
    QCOMPARE(c.dateTimeFormat(QLocale::NarrowFormat), c.dateTimeFormat(QLocale::ShortFormat));

    const QLocale no("no_NO");
    QCOMPARE(no.dateTimeFormat(QLocale::NarrowFormat), QLatin1String("dd.MM.yy HH:mm"));
    QCOMPARE(no.dateTimeFormat(QLocale::ShortFormat), QLatin1String("dd.MM.yy HH:mm"));
    QCOMPARE(no.dateTimeFormat(QLocale::LongFormat), QLatin1String("dddd d. MMMM yyyy 'kl'. HH:mm:ss t"));
}

void tst_QLocale::monthName()
{
    const QLocale c(QLocale::C);
    QCOMPARE(c.monthName(0, QLocale::ShortFormat), QString());
    QCOMPARE(c.monthName(0, QLocale::LongFormat), QString());
    QCOMPARE(c.monthName(0, QLocale::NarrowFormat), QString());
    QCOMPARE(c.monthName(13, QLocale::ShortFormat), QString());
    QCOMPARE(c.monthName(13, QLocale::LongFormat), QString());
    QCOMPARE(c.monthName(13, QLocale::NarrowFormat), QString());

    QCOMPARE(c.monthName(1, QLocale::LongFormat), QLatin1String("January"));
    QCOMPARE(c.monthName(1, QLocale::ShortFormat), QLatin1String("Jan"));
    QCOMPARE(c.monthName(1, QLocale::NarrowFormat), QLatin1String("1"));

    const QLocale de("de_DE");
    QCOMPARE(de.monthName(12, QLocale::LongFormat), QLatin1String("Dezember"));
    QCOMPARE(de.monthName(12, QLocale::ShortFormat), QLatin1String("Dez"));
    // 'de' locale doesn't have narrow month name
    QCOMPARE(de.monthName(12, QLocale::NarrowFormat), QLatin1String("D"));

    QLocale ru("ru_RU");
    QCOMPARE(ru.monthName(1, QLocale::LongFormat), QString::fromUtf8("\321\217\320\275\320\262\320\260\321\200\321\217"));
    QCOMPARE(ru.monthName(1, QLocale::ShortFormat), QString::fromUtf8("\321\217\320\275\320\262"));
    QCOMPARE(ru.monthName(1, QLocale::NarrowFormat), QString::fromUtf8("\320\257"));

    // check that our CLDR scripts handle surrogate pairs correctly
    QLocale dsrt("en-Dsrt-US");
    QCOMPARE(dsrt.monthName(1, QLocale::LongFormat), QString::fromUtf8("\xf0\x90\x90\x96\xf0\x90\x90\xb0\xf0\x90\x91\x8c\xf0\x90\x90\xb7\xf0\x90\x90\xad\xf0\x90\x90\xaf\xf0\x90\x91\x89\xf0\x90\x90\xa8"));

    QLocale ir("ga_IE");
    QCOMPARE(ir.monthName(1, QLocale::ShortFormat), QLatin1String("Ean"));
    QCOMPARE(ir.monthName(12, QLocale::ShortFormat), QLatin1String("Noll"));

    QLocale cz("cs_CZ");
    QCOMPARE(cz.monthName(1, QLocale::ShortFormat), QLatin1String("led"));
    QCOMPARE(cz.monthName(12, QLocale::ShortFormat), QLatin1String("pro"));
}

void tst_QLocale::standaloneMonthName()
{
    const QLocale c(QLocale::C);
    QCOMPARE(c.monthName(0, QLocale::ShortFormat), QString());
    QCOMPARE(c.monthName(0, QLocale::LongFormat), QString());
    QCOMPARE(c.monthName(0, QLocale::NarrowFormat), QString());
    QCOMPARE(c.monthName(13, QLocale::ShortFormat), QString());
    QCOMPARE(c.monthName(13, QLocale::LongFormat), QString());
    QCOMPARE(c.monthName(13, QLocale::NarrowFormat), QString());

    QCOMPARE(c.standaloneMonthName(1, QLocale::LongFormat), QLatin1String("January"));
    QCOMPARE(c.standaloneMonthName(1, QLocale::ShortFormat), QLatin1String("Jan"));

    const QLocale de("de_DE");
    // For de_DE locale Unicode CLDR database doesn't contain standalone long months
    // so just checking if the return value is the same as in monthName().
    QCOMPARE(de.standaloneMonthName(12, QLocale::LongFormat), QLatin1String("Dezember"));
    QCOMPARE(de.standaloneMonthName(12, QLocale::LongFormat), de.monthName(12, QLocale::LongFormat));
    QCOMPARE(de.standaloneMonthName(12, QLocale::ShortFormat), QLatin1String("Dez"));
    QCOMPARE(de.standaloneMonthName(12, QLocale::NarrowFormat), QLatin1String("D"));

    QLocale ru("ru_RU");
    QCOMPARE(ru.standaloneMonthName(1, QLocale::LongFormat), QString::fromUtf8("\320\257\320\275\320\262\320\260\321\200\321\214"));
    QCOMPARE(ru.standaloneMonthName(1, QLocale::ShortFormat), QString::fromUtf8("\320\257\320\275\320\262\56"));
    QCOMPARE(ru.standaloneMonthName(1, QLocale::NarrowFormat), QString::fromUtf8("\320\257"));
}

#if defined(Q_OS_SYMBIAN)
void tst_QLocale::symbianSystemLocale()
{
# if defined(__SERIES60_31__)
    QSKIP("S60 3.1 doesn't support system format properly", SkipAll);
# else
    // Simple test to verify that Symbian system locale works at all
    const QSystemLocale locale;
    TExtendedLocale s60Locale;
    s60Locale.LoadSystemSettings();

    TTime s60Date(_L("20090117:")); // Symbian offsets day and month from zero
    QDate date(2009,2,18);

    TPtrC s60DateFormat = s60Locale.GetShortDateFormatSpec();
    QString dateFormat = locale.query(QSystemLocale::DateFormatShort, QVariant()).toString();

    TBuf<50> s60FormattedDate;
    TRAPD(err, s60Date.FormatL(s60FormattedDate, s60DateFormat));
    QVERIFY(err == KErrNone);
    QString s60FinalResult = qt_TDesC2QString(s60FormattedDate);
    QString finalResult = date.toString(dateFormat);

    QCOMPARE(finalResult, s60FinalResult);
# endif
}
#endif

void tst_QLocale::currency()
{
    const QLocale c(QLocale::C);
    QCOMPARE(c.toCurrencyString(qulonglong(1234)), QString("1234"));
    QCOMPARE(c.toCurrencyString(qlonglong(-1234)), QString("-1234"));
    QCOMPARE(c.toCurrencyString(double(1234.56)), QString("1234.56"));
    QCOMPARE(c.toCurrencyString(double(-1234.56)), QString("-1234.56"));

    const QLocale ru_RU("ru_RU");
    QCOMPARE(ru_RU.toCurrencyString(qulonglong(1234)), QString::fromUtf8("1234\xc2\xa0\xd1\x80\xd1\x83\xd0\xb1."));
    QCOMPARE(ru_RU.toCurrencyString(qlonglong(-1234)), QString::fromUtf8("-1234\xc2\xa0\xd1\x80\xd1\x83\xd0\xb1."));
    QCOMPARE(ru_RU.toCurrencyString(double(1234.56)), QString::fromUtf8("1234,56\xc2\xa0\xd1\x80\xd1\x83\xd0\xb1."));
    QCOMPARE(ru_RU.toCurrencyString(double(-1234.56)), QString::fromUtf8("-1234,56\xc2\xa0\xd1\x80\xd1\x83\xd0\xb1."));

    const QLocale de_DE("de_DE");
    QCOMPARE(de_DE.toCurrencyString(qulonglong(1234)), QString::fromUtf8("1234\xc2\xa0\xe2\x82\xac"));
    QCOMPARE(de_DE.toCurrencyString(qulonglong(1234), QLatin1String("BAZ")), QString::fromUtf8("1234\xc2\xa0" "BAZ"));
    QCOMPARE(de_DE.toCurrencyString(qlonglong(-1234)), QString::fromUtf8("-1234\xc2\xa0\xe2\x82\xac"));
    QCOMPARE(de_DE.toCurrencyString(qlonglong(-1234), QLatin1String("BAZ")), QString::fromUtf8("-1234\xc2\xa0" "BAZ"));
    QCOMPARE(de_DE.toCurrencyString(double(1234.56)), QString::fromUtf8("1234,56\xc2\xa0\xe2\x82\xac"));
    QCOMPARE(de_DE.toCurrencyString(double(-1234.56)), QString::fromUtf8("-1234,56\xc2\xa0\xe2\x82\xac"));
    QCOMPARE(de_DE.toCurrencyString(double(-1234.56), QLatin1String("BAZ")), QString::fromUtf8("-1234,56\xc2\xa0" "BAZ"));

    const QLocale system = QLocale::system();
    QVERIFY(system.toCurrencyString(1, QLatin1String("FOO")).contains(QLatin1String("FOO")));
}

void tst_QLocale::quoteString()
{
    const QString someText("text");
    const QLocale c(QLocale::C);
    QCOMPARE(c.quoteString(someText), QString::fromUtf8("\x22" "text" "\x22"));
    QCOMPARE(c.quoteString(someText, QLocale::AlternateQuotation), QString::fromUtf8("\x27" "text" "\x27"));

    const QLocale de_CH("de_CH");
    QCOMPARE(de_CH.quoteString(someText), QString::fromUtf8("\xc2\xab" "text" "\xc2\xbb"));
    QCOMPARE(de_CH.quoteString(someText, QLocale::AlternateQuotation), QString::fromUtf8("\xe2\x80\xb9" "text" "\xe2\x80\xba"));

}

void tst_QLocale::uiLanguages()
{
    const QLocale c(QLocale::C);
    QCOMPARE(c.uiLanguages().size(), 1);
    QCOMPARE(c.uiLanguages().at(0), QLatin1String("C"));

    const QLocale en_US("en_US");
    QCOMPARE(en_US.uiLanguages().size(), 1);
    QCOMPARE(en_US.uiLanguages().at(0), QLatin1String("en-US"));

    const QLocale ru_RU("ru_RU");
    QCOMPARE(ru_RU.uiLanguages().size(), 1);
    QCOMPARE(ru_RU.uiLanguages().at(0), QLatin1String("ru-RU"));
}

void tst_QLocale::weekendDays()
{
    const QLocale c(QLocale::C);
    QList<Qt::DayOfWeek> days;
    days << Qt::Monday << Qt::Tuesday << Qt::Wednesday << Qt::Thursday << Qt::Friday;
    QCOMPARE(c.weekdays(), days);
}

void tst_QLocale::listPatterns()
{
    QStringList sl1;
    QStringList sl2;
    sl2 << "aaa";
    QStringList sl3;
    sl3 << "aaa" << "bbb";
    QStringList sl4;
    sl4 << "aaa" << "bbb" << "ccc";
    QStringList sl5;
    sl5 << "aaa" << "bbb" << "ccc" << "ddd";

    const QLocale c(QLocale::C);
    QCOMPARE(c.createSeparatedList(sl1), QString(""));
    QCOMPARE(c.createSeparatedList(sl2), QString("aaa"));
    QCOMPARE(c.createSeparatedList(sl3), QString("aaa, bbb"));
    QCOMPARE(c.createSeparatedList(sl4), QString("aaa, bbb, ccc"));
    QCOMPARE(c.createSeparatedList(sl5), QString("aaa, bbb, ccc, ddd"));

    const QLocale en_US("en_US");
    QCOMPARE(en_US.createSeparatedList(sl1), QString(""));
    QCOMPARE(en_US.createSeparatedList(sl2), QString("aaa"));
    QCOMPARE(en_US.createSeparatedList(sl3), QString("aaa and bbb"));
    QCOMPARE(en_US.createSeparatedList(sl4), QString("aaa, bbb, and ccc"));
    QCOMPARE(en_US.createSeparatedList(sl5), QString("aaa, bbb, ccc, and ddd"));

    const QLocale zh_CN("zh_CN");
    QCOMPARE(zh_CN.createSeparatedList(sl1), QString(""));
    QCOMPARE(zh_CN.createSeparatedList(sl2), QString("aaa"));
    QCOMPARE(zh_CN.createSeparatedList(sl3), QString::fromUtf8("aaa" "\xe5\x92\x8c" "bbb"));
    QCOMPARE(zh_CN.createSeparatedList(sl4), QString::fromUtf8("aaa" "\xe3\x80\x81" "bbb" "\xe5\x92\x8c" "ccc"));
    QCOMPARE(zh_CN.createSeparatedList(sl5), QString::fromUtf8("aaa" "\xe3\x80\x81" "bbb" "\xe3\x80\x81" "ccc" "\xe5\x92\x8c" "ddd"));
}

QTEST_APPLESS_MAIN(tst_QLocale)
#include "tst_qlocale.moc"
