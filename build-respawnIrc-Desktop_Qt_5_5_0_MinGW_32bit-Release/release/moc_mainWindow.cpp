/****************************************************************************
** Meta object code from reading C++ file 'mainWindow.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../respawnIrc/mainWindow.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainWindow.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_mainWindowClass_t {
    QByteArrayData data[10];
    char stringdata0[132];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_mainWindowClass_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_mainWindowClass_t qt_meta_stringdata_mainWindowClass = {
    {
QT_MOC_LITERAL(0, 0, 15), // "mainWindowClass"
QT_MOC_LITERAL(1, 16, 6), // "goToMp"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 18), // "useFavoriteClicked"
QT_MOC_LITERAL(4, 43, 18), // "addFavoriteClicked"
QT_MOC_LITERAL(5, 62, 18), // "delFavoriteClicked"
QT_MOC_LITERAL(6, 81, 11), // "setNewTheme"
QT_MOC_LITERAL(7, 93, 12), // "newThemeName"
QT_MOC_LITERAL(8, 106, 18), // "saveWindowGeometry"
QT_MOC_LITERAL(9, 125, 6) // "newVal"

    },
    "mainWindowClass\0goToMp\0\0useFavoriteClicked\0"
    "addFavoriteClicked\0delFavoriteClicked\0"
    "setNewTheme\0newThemeName\0saveWindowGeometry\0"
    "newVal"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_mainWindowClass[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x0a /* Public */,
       3,    0,   45,    2, 0x0a /* Public */,
       4,    0,   46,    2, 0x0a /* Public */,
       5,    0,   47,    2, 0x0a /* Public */,
       6,    1,   48,    2, 0x0a /* Public */,
       8,    1,   51,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::Bool,    9,

       0        // eod
};

void mainWindowClass::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        mainWindowClass *_t = static_cast<mainWindowClass *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->goToMp(); break;
        case 1: _t->useFavoriteClicked(); break;
        case 2: _t->addFavoriteClicked(); break;
        case 3: _t->delFavoriteClicked(); break;
        case 4: _t->setNewTheme((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->saveWindowGeometry((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject mainWindowClass::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_mainWindowClass.data,
      qt_meta_data_mainWindowClass,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *mainWindowClass::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *mainWindowClass::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_mainWindowClass.stringdata0))
        return static_cast<void*>(const_cast< mainWindowClass*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int mainWindowClass::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
