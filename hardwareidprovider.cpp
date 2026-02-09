#include "hardwareidprovider.h"

#include <QProcess>
#include <QCryptographicHash>
#include <QRegularExpression>

//#ifdef Q_OS_WIN
#include <windows.h>
#include <comdef.h>
#include <Wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")
//#endif

// ---------------- Backend Detection ----------------

HardwareIdProvider::Backend HardwareIdProvider::detectBackend()
{
//#ifdef Q_OS_WIN
    // Try WMI COM
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (SUCCEEDED(hr)) {
        IWbemLocator* loc = nullptr;
        hr = CoCreateInstance(
            CLSID_WbemLocator, nullptr,
            CLSCTX_INPROC_SERVER,
            IID_IWbemLocator,
            (void**)&loc
        );

        if (SUCCEEDED(hr)) {
            IWbemServices* svc = nullptr;
            hr = loc->ConnectServer(
                BSTR(L"ROOT\\CIMV2"),
                nullptr, nullptr, nullptr,
                0, nullptr, nullptr, &svc
            );

            if (SUCCEEDED(hr)) {
                svc->Release();
                loc->Release();
                return Backend::WmiCom;
            }
            loc->Release();
        }
    }

    // Try wmic
    wchar_t path[MAX_PATH] = {0};
    if (SearchPathW(nullptr, L"wmic.exe", nullptr, MAX_PATH, path, nullptr) > 0)
        return Backend::Wmic;
//#endif

    return Backend::None;
}

// ---------------- Public API ----------------

QString HardwareIdProvider::cpuId()
{
//    switch (detectBackend()) {
//    case Backend::WmiCom:
//        return wmiQuerySingleValue(
//            L"SELECT ProcessorId FROM Win32_Processor",
//            L"ProcessorId"
//        );
//    case Backend::Wmic:
//        return wmicQuery("cpu get ProcessorId");
//    default:
//        return QString();
//    }
    return wmiQuerySingleValue(
        L"SELECT ProcessorId FROM Win32_Processor",
        L"ProcessorId"
    );
}

QString HardwareIdProvider::baseBoardSerial()
{
//    switch (detectBackend()) {
//    case Backend::WmiCom:
//        return wmiQuerySingleValue(
//            L"SELECT SerialNumber FROM Win32_BaseBoard",
//            L"SerialNumber"
//        );
//    case Backend::Wmic:
//        return wmicQuery("baseboard get SerialNumber");
//    default:
//        return QString();
//    }
    return wmiQuerySingleValue(
        L"SELECT SerialNumber FROM Win32_BaseBoard",
        L"SerialNumber"
    );
}

QString HardwareIdProvider::hardwareFingerprint()
{
    QString raw = normalize(cpuId() + baseBoardSerial());
    if (raw.isEmpty())
        return QString();

    return QCryptographicHash::hash(
        raw.toUtf8(),
        QCryptographicHash::Sha256
    ).toHex();
}

// ---------------- WMI COM ----------------

QString HardwareIdProvider::wmiQuerySingleValue(
    const wchar_t* wql,
    const wchar_t* property)
{
//#ifdef Q_OS_WIN
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    CoInitializeSecurity(
        nullptr, -1, nullptr, nullptr,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        nullptr, EOAC_NONE, nullptr
    );

    IWbemLocator* loc = nullptr;
    IWbemServices* svc = nullptr;

    HRESULT hr = CoCreateInstance(
        CLSID_WbemLocator, nullptr,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator,
        (void**)&loc
    );
    if (FAILED(hr)) return QString();

    hr = loc->ConnectServer(
        BSTR(L"ROOT\\CIMV2"),
        nullptr, nullptr, nullptr,
        0, nullptr, nullptr, &svc
    );
    loc->Release();
    if (FAILED(hr)) return QString();

    IEnumWbemClassObject* enumerator = nullptr;
    hr = svc->ExecQuery(
        BSTR(L"WQL"),
        BSTR(wql),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        nullptr,
        &enumerator
    );
    if (FAILED(hr)) {
        svc->Release();
        return QString();
    }

    IWbemClassObject* obj = nullptr;
    ULONG ret = 0;
    hr = enumerator->Next(WBEM_INFINITE, 1, &obj, &ret);
    if (ret == 0) {
        enumerator->Release();
        svc->Release();
        return QString();
    }

    VARIANT vt;
    VariantInit(&vt);
    obj->Get(property, 0, &vt, nullptr, nullptr);

    QString result = QString::fromWCharArray(vt.bstrVal);

    VariantClear(&vt);
    obj->Release();
    enumerator->Release();
    svc->Release();

    return result;
//#else
//    return QString();
//#endif
}

// ---------------- WMIC ----------------

QString HardwareIdProvider::wmicQuery(const QString& args)
{
    QProcess p;
    p.start("wmic", args.split(' '));
    if (!p.waitForFinished(2000))
        return QString();

    QString out = QString::fromLocal8Bit(p.readAllStandardOutput());
    out.remove(QRegularExpression("(?i)processorid|serialnumber"));
    return normalize(out);
}

// ---------------- Utils ----------------

QString HardwareIdProvider::normalize(const QString& s)
{
    QString r = s;
    r.remove(QRegularExpression("\\s"));
    r = r.toUpper();
    return r;
}
