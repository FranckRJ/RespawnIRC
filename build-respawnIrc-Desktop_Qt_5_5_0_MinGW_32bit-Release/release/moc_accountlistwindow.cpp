/****************************************************************************
** Meta object code from reading C++ file 'accountListWindow.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../respawnIrc/accountListWindow.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'accountListWindow.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_accountListWindowClass_t {
    QByteArrayData data[18];
    char stringdata0[302];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_accountListWindowClass_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_accountListWindowClass_t qt_meta_stringdata_accountListWindowClass = {
    {
QT_MOC_LITERAL(0, 0, 22), // "accountListWindowClass"
QT_MOC_LITERAL(1, 23, 14), // "listHasChanged"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 14), // "useThisAccount"
QT_MOC_LITERAL(4, 54, 21), // "QList<QNetworkCookie>"
QT_MOC_LITERAL(5, 76, 15), // "withTheseCookie"
QT_MOC_LITERAL(6, 92, 14), // "withThisPseudo"
QT_MOC_LITERAL(7, 107, 15), // "saveAccountList"
QT_MOC_LITERAL(8, 123, 10), // "savePseudo"
QT_MOC_LITERAL(9, 134, 25), // "useThisAccountForOneTopic"
QT_MOC_LITERAL(10, 160, 17), // "showConnectWindow"
QT_MOC_LITERAL(11, 178, 10), // "addAccount"
QT_MOC_LITERAL(12, 189, 10), // "newCookies"
QT_MOC_LITERAL(13, 200, 15), // "newPseudoOfUser"
QT_MOC_LITERAL(14, 216, 8), // "saveInfo"
QT_MOC_LITERAL(15, 225, 20), // "removeCurrentAccount"
QT_MOC_LITERAL(16, 246, 22), // "connectWithThisAccount"
QT_MOC_LITERAL(17, 269, 32) // "connectToOneTopicWithThisAccount"

    },
    "accountListWindowClass\0listHasChanged\0"
    "\0useThisAccount\0QList<QNetworkCookie>\0"
    "withTheseCookie\0withThisPseudo\0"
    "saveAccountList\0savePseudo\0"
    "useThisAccountForOneTopic\0showConnectWindow\0"
    "addAccount\0newCookies\0newPseudoOfUser\0"
    "saveInfo\0removeCurrentAccount\0"
    "connectWithThisAccount\0"
    "connectToOneTopicWithThisAccount"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_accountListWindowClass[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x06 /* Public */,
       3,    4,   55,    2, 0x06 /* Public */,
       9,    3,   64,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    0,   71,    2, 0x0a /* Public */,
      11,    3,   72,    2, 0x0a /* Public */,
      15,    0,   79,    2, 0x0a /* Public */,
      16,    0,   80,    2, 0x0a /* Public */,
      17,    0,   81,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4, QMetaType::QString, QMetaType::Bool, QMetaType::Bool,    5,    6,    7,    8,
    QMetaType::Void, 0x80000000 | 4, QMetaType::QString, QMetaType::Bool,    5,    6,    8,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4, QMetaType::QString, QMetaType::Bool,   12,   13,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void accountListWindowClass::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        accountListWindowClass *_t = static_cast<accountListWindowClass *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->listHasChanged(); break;
        case 1: _t->useThisAccount((*reinterpret_cast< QList<QNetworkCookie>(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 2: _t->useThisAccountForOneTopic((*reinterpret_cast< QList<QNetworkCookie>(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 3: _t->showConnectWindow(); break;
        case 4: _t->addAccount((*reinterpret_cast< QList<QNetworkCookie>(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 5: _t->removeCurrentAccount(); break;
        case 6: _t->connectWithThisAccount(); break;
        case 7: _t->connectToOneTopicWithThisAccount(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QNetworkCookie> >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QNetworkCookie> >(); break;
            }
            break;
        case 4:
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
            typedef void (accountListWindowClass::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&accountListWindowClass::listHasChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (accountListWindowClass::*_t)(QList<QNetworkCookie> , QString , bool , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&accountListWindowClass::useThisAccount)) {
                *result = 1;
            }
        }
        {
            typedef void (accountListWindowClass::*_t)(QList<QNetworkCookie> , QString , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&accountListWindowClass::useThisAccountForOneTopic)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject accountListWindowClass::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_accountListWindowClass.data,
      qt_meta_data_accountListWindowClass,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *accountListWindowClass::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *accountListWindowClass::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_accountListWindowClass.stringdata0))
        return static_cast<void*>(const_cast< accountListWindowClass*>(this));
    return QDialog::qt_metacast(_clname);
}

int accountListWindowClass::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void accountListWindowClass::listHasChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void accountListWindowClass::useThisAccount(QList<QNetworkCookie> _t1, QString _t2, bool _t3, bool _t4)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void accountListWindowClass::useThisAccountForOneTopic(QList<QNetworkCookie> _t1, QString _t2, bool _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
