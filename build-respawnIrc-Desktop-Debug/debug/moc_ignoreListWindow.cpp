/****************************************************************************
** Meta object code from reading C++ file 'ignoreListWindow.hpp'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../respawnIrc/ignoreListWindow.hpp"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ignoreListWindow.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ignoreListWindowClass[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x05,

 // slots: signature, parameters, type, tag, flags
      40,   22,   22,   22, 0x0a,
      52,   22,   22,   22, 0x0a,
      72,   22,   22,   22, 0x0a,
     104,   94,   22,   22, 0x0a,
     127,   94,   22,   22, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ignoreListWindowClass[] = {
    "ignoreListWindowClass\0\0listHasChanged()\0"
    "addPseudo()\0editCurrentPseudo()\0"
    "removeCurrentPseudo()\0newPseudo\0"
    "addThisPseudo(QString)\0setCurrentPseudo(QString)\0"
};

void ignoreListWindowClass::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ignoreListWindowClass *_t = static_cast<ignoreListWindowClass *>(_o);
        switch (_id) {
        case 0: _t->listHasChanged(); break;
        case 1: _t->addPseudo(); break;
        case 2: _t->editCurrentPseudo(); break;
        case 3: _t->removeCurrentPseudo(); break;
        case 4: _t->addThisPseudo((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->setCurrentPseudo((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ignoreListWindowClass::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ignoreListWindowClass::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ignoreListWindowClass,
      qt_meta_data_ignoreListWindowClass, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ignoreListWindowClass::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ignoreListWindowClass::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ignoreListWindowClass::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ignoreListWindowClass))
        return static_cast<void*>(const_cast< ignoreListWindowClass*>(this));
    return QDialog::qt_metacast(_clname);
}

int ignoreListWindowClass::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void ignoreListWindowClass::listHasChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
