/****************************************************************************
** Meta object code from reading C++ file 'multiTypeTextBox.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../respawnIrc/multiTypeTextBox.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'multiTypeTextBox.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_multiTypeTextBoxClass_t {
    QByteArrayData data[15];
    char stringdata0[166];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_multiTypeTextBoxClass_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_multiTypeTextBoxClass_t qt_meta_stringdata_multiTypeTextBoxClass = {
    {
QT_MOC_LITERAL(0, 0, 21), // "multiTypeTextBoxClass"
QT_MOC_LITERAL(1, 22, 13), // "returnPressed"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 19), // "setTextEditSelected"
QT_MOC_LITERAL(4, 57, 6), // "newVal"
QT_MOC_LITERAL(5, 64, 15), // "returnIsPressed"
QT_MOC_LITERAL(6, 80, 7), // "addBold"
QT_MOC_LITERAL(7, 88, 9), // "addItalic"
QT_MOC_LITERAL(8, 98, 12), // "addUnderLine"
QT_MOC_LITERAL(9, 111, 9), // "addStrike"
QT_MOC_LITERAL(10, 121, 8), // "addUList"
QT_MOC_LITERAL(11, 130, 9), // "addOListe"
QT_MOC_LITERAL(12, 140, 8), // "addQuote"
QT_MOC_LITERAL(13, 149, 7), // "addCode"
QT_MOC_LITERAL(14, 157, 8) // "addSpoil"

    },
    "multiTypeTextBoxClass\0returnPressed\0"
    "\0setTextEditSelected\0newVal\0returnIsPressed\0"
    "addBold\0addItalic\0addUnderLine\0addStrike\0"
    "addUList\0addOListe\0addQuote\0addCode\0"
    "addSpoil"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_multiTypeTextBoxClass[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   75,    2, 0x0a /* Public */,
       5,    0,   78,    2, 0x0a /* Public */,
       6,    0,   79,    2, 0x0a /* Public */,
       7,    0,   80,    2, 0x0a /* Public */,
       8,    0,   81,    2, 0x0a /* Public */,
       9,    0,   82,    2, 0x0a /* Public */,
      10,    0,   83,    2, 0x0a /* Public */,
      11,    0,   84,    2, 0x0a /* Public */,
      12,    0,   85,    2, 0x0a /* Public */,
      13,    0,   86,    2, 0x0a /* Public */,
      14,    0,   87,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void multiTypeTextBoxClass::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        multiTypeTextBoxClass *_t = static_cast<multiTypeTextBoxClass *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->returnPressed(); break;
        case 1: _t->setTextEditSelected((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->returnIsPressed(); break;
        case 3: _t->addBold(); break;
        case 4: _t->addItalic(); break;
        case 5: _t->addUnderLine(); break;
        case 6: _t->addStrike(); break;
        case 7: _t->addUList(); break;
        case 8: _t->addOListe(); break;
        case 9: _t->addQuote(); break;
        case 10: _t->addCode(); break;
        case 11: _t->addSpoil(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (multiTypeTextBoxClass::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&multiTypeTextBoxClass::returnPressed)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject multiTypeTextBoxClass::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_multiTypeTextBoxClass.data,
      qt_meta_data_multiTypeTextBoxClass,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *multiTypeTextBoxClass::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *multiTypeTextBoxClass::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_multiTypeTextBoxClass.stringdata0))
        return static_cast<void*>(const_cast< multiTypeTextBoxClass*>(this));
    return QWidget::qt_metacast(_clname);
}

int multiTypeTextBoxClass::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void multiTypeTextBoxClass::returnPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
