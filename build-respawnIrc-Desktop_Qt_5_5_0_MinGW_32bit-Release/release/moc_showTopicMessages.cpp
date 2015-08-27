/****************************************************************************
** Meta object code from reading C++ file 'showTopicMessages.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../respawnIrc/showTopicMessages.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'showTopicMessages.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_showTopicMessagesClass_t {
    QByteArrayData data[18];
    char stringdata0[267];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_showTopicMessagesClass_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_showTopicMessagesClass_t qt_meta_stringdata_showTopicMessagesClass = {
    {
QT_MOC_LITERAL(0, 0, 22), // "showTopicMessagesClass"
QT_MOC_LITERAL(1, 23, 16), // "newMessageStatus"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 20), // "newNumberOfConnected"
QT_MOC_LITERAL(4, 62, 11), // "setEditInfo"
QT_MOC_LITERAL(5, 74, 15), // "idOfMessageEdit"
QT_MOC_LITERAL(6, 90, 11), // "messageEdit"
QT_MOC_LITERAL(7, 102, 10), // "infoToSend"
QT_MOC_LITERAL(8, 113, 11), // "cpatchaLink"
QT_MOC_LITERAL(9, 125, 20), // "newMessagesAvailable"
QT_MOC_LITERAL(10, 146, 15), // "newNameForTopic"
QT_MOC_LITERAL(11, 162, 7), // "newName"
QT_MOC_LITERAL(12, 170, 11), // "getEditInfo"
QT_MOC_LITERAL(13, 182, 11), // "getMessages"
QT_MOC_LITERAL(14, 194, 19), // "loadFirstPageFinish"
QT_MOC_LITERAL(15, 214, 20), // "loadSecondPageFinish"
QT_MOC_LITERAL(16, 235, 15), // "analyzeEditInfo"
QT_MOC_LITERAL(17, 251, 15) // "analyzeMessages"

    },
    "showTopicMessagesClass\0newMessageStatus\0"
    "\0newNumberOfConnected\0setEditInfo\0"
    "idOfMessageEdit\0messageEdit\0infoToSend\0"
    "cpatchaLink\0newMessagesAvailable\0"
    "newNameForTopic\0newName\0getEditInfo\0"
    "getMessages\0loadFirstPageFinish\0"
    "loadSecondPageFinish\0analyzeEditInfo\0"
    "analyzeMessages"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_showTopicMessagesClass[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x06 /* Public */,
       3,    0,   70,    2, 0x06 /* Public */,
       4,    4,   71,    2, 0x06 /* Public */,
       9,    0,   80,    2, 0x06 /* Public */,
      10,    1,   81,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    0,   84,    2, 0x0a /* Public */,
      13,    0,   85,    2, 0x0a /* Public */,
      14,    0,   86,    2, 0x0a /* Public */,
      15,    0,   87,    2, 0x0a /* Public */,
      16,    0,   88,    2, 0x0a /* Public */,
      17,    0,   89,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QString, QMetaType::QString,    5,    6,    7,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   11,

 // slots: parameters
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void showTopicMessagesClass::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        showTopicMessagesClass *_t = static_cast<showTopicMessagesClass *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->newMessageStatus(); break;
        case 1: _t->newNumberOfConnected(); break;
        case 2: _t->setEditInfo((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4]))); break;
        case 3: _t->newMessagesAvailable(); break;
        case 4: _t->newNameForTopic((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: { bool _r = _t->getEditInfo();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 6: _t->getMessages(); break;
        case 7: _t->loadFirstPageFinish(); break;
        case 8: _t->loadSecondPageFinish(); break;
        case 9: _t->analyzeEditInfo(); break;
        case 10: _t->analyzeMessages(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (showTopicMessagesClass::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&showTopicMessagesClass::newMessageStatus)) {
                *result = 0;
            }
        }
        {
            typedef void (showTopicMessagesClass::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&showTopicMessagesClass::newNumberOfConnected)) {
                *result = 1;
            }
        }
        {
            typedef void (showTopicMessagesClass::*_t)(int , QString , QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&showTopicMessagesClass::setEditInfo)) {
                *result = 2;
            }
        }
        {
            typedef void (showTopicMessagesClass::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&showTopicMessagesClass::newMessagesAvailable)) {
                *result = 3;
            }
        }
        {
            typedef void (showTopicMessagesClass::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&showTopicMessagesClass::newNameForTopic)) {
                *result = 4;
            }
        }
    }
}

const QMetaObject showTopicMessagesClass::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_showTopicMessagesClass.data,
      qt_meta_data_showTopicMessagesClass,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *showTopicMessagesClass::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *showTopicMessagesClass::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_showTopicMessagesClass.stringdata0))
        return static_cast<void*>(const_cast< showTopicMessagesClass*>(this));
    return QWidget::qt_metacast(_clname);
}

int showTopicMessagesClass::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void showTopicMessagesClass::newMessageStatus()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void showTopicMessagesClass::newNumberOfConnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void showTopicMessagesClass::setEditInfo(int _t1, QString _t2, QString _t3, QString _t4)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void showTopicMessagesClass::newMessagesAvailable()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}

// SIGNAL 4
void showTopicMessagesClass::newNameForTopic(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
