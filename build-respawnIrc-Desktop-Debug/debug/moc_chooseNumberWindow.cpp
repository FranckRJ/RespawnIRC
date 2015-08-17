/****************************************************************************
** Meta object code from reading C++ file 'chooseNumberWindow.hpp'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../respawnIrc/chooseNumberWindow.hpp"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'chooseNumberWindow.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_chooseNumberWindowClass[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      35,   25,   24,   24, 0x05,

 // slots: signature, parameters, type, tag, flags
      53,   24,   24,   24, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_chooseNumberWindowClass[] = {
    "chooseNumberWindowClass\0\0newNumber\0"
    "newNumberSet(int)\0setNumber()\0"
};

void chooseNumberWindowClass::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        chooseNumberWindowClass *_t = static_cast<chooseNumberWindowClass *>(_o);
        switch (_id) {
        case 0: _t->newNumberSet((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->setNumber(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData chooseNumberWindowClass::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject chooseNumberWindowClass::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_chooseNumberWindowClass,
      qt_meta_data_chooseNumberWindowClass, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &chooseNumberWindowClass::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *chooseNumberWindowClass::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *chooseNumberWindowClass::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_chooseNumberWindowClass))
        return static_cast<void*>(const_cast< chooseNumberWindowClass*>(this));
    return QDialog::qt_metacast(_clname);
}

int chooseNumberWindowClass::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void chooseNumberWindowClass::newNumberSet(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
