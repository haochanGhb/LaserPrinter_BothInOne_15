#ifndef HARDWAREIDPROVIDER_H
#define HARDWAREIDPROVIDER_H


#pragma once

#include <QString>

class HardwareIdProvider
{
public:
    static QString cpuId();
    static QString baseBoardSerial();
    static QString hardwareFingerprint();

private:
    enum class Backend {
        WmiCom,
        Wmic,
        None
    };

    static Backend detectBackend();

    // WMI COM
    static QString wmiQuerySingleValue(
        const wchar_t* wql,
        const wchar_t* property
    );

    // wmic
    static QString wmicQuery(const QString& args);

    static QString normalize(const QString& s);
};

#endif // HARDWAREIDPROVIDER_H
