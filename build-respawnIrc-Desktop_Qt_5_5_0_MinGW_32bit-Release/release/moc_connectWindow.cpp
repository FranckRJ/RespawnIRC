/****************************************************************************
** Meta object code from reading C++ file 'connectWindow.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../respawnIrc/connectWindow.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'connectWindow.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_connectWindowClass_t {
    QByteArrayData data[10];
    char stringdata0[134];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_connectWindowClass_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_connectWindowClass_t qt_meta_stringdata_connectWindowClass = {
    {
QT_MOC_LITERAL(0, 0, 18), // "connectWindowClass"
QT_MOC_LITERAL(1, 19, 19), // "newCookiesAvailable"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 21), // "QList<QNetworkCookie>"
QT_MOC_LITERAL(4, 62, 10), // "newCookies"
QT_MOC_LITERAL(5, 73, 15), // "newPseudoOfUser"
QT_MOC_LITERAL(6, 89, 8), // "saveInfo"
QT_MOC_LITERAL(7, 98, 12), // "getFormInput"
QT_MOC_LITERAL(8, 111, 10), // "startLogin"
QT_MOC_LITERAL(9, 122, 11) // "showCaptcha"

    },
    "connectWindowClass\0newCookiesAvailable\0"
    "\0QList<QNetworkCookie>\0newCookies\0"
    "newPseudoOfUser\0saveInfo\0getFormInput\0"
    "startLogin\0showCaptcha"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_connectWindowClass[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   41,    2, 0x0a /* Public */,
       8,    0,   42,    2, 0x0a /* Public */,
       9,    0,   43,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString, QMetaType::Bool,    4,    5,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void connectWindowClass::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        connectWindowClass *_t = static_cast<connectWindowClass *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->newCookiesAvailable((*reinterpret_cast< QList<QNetworkCookie>(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: _t->getFormInput(); break;
        case 2: _t->startLogin(); break;
        case 3: _t->showCaptcha(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QNetworkCookie> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (connectWindowClass::*_t)(QList<QNetworkCookie> , QString , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&connectWindowClass::newCookiesAvailable)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject connectWindowClass::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_connectWindowClass.data,
      qt_meta_data_connectWindowClass,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *connectWindowClass::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *connectWindowClass::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_connectWindowClass.stringdata0))
        return static_cast<void*>(const_cast< connectWindowClass*>(this));
    return QDialog::qt_metacast(_clname);
}

int connectWindowClass::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void connectWindowClass::newCookiesAvailable(QList<QNetworkCookie> _t1, QString _t2, bool _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
