/****************************************************************************
** Meta object code from reading C++ file 'captchaWindow.hpp'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../respawnIrc/captchaWindow.hpp"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'captchaWindow.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_captchaWindowClass[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      32,   20,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
      56,   19,   19,   19, 0x0a,
      70,   19,   19,   19, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_captchaWindowClass[] = {
    "captchaWindowClass\0\0captchaCode\0"
    "codeForCaptcha(QString)\0showCaptcha()\0"
    "sendCaptchaCode()\0"
};

void captchaWindowClass::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        captchaWindowClass *_t = static_cast<captchaWindowClass *>(_o);
        switch (_id) {
        case 0: _t->codeForCaptcha((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->showCaptcha(); break;
        case 2: _t->sendCaptchaCode(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData captchaWindowClass::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject captchaWindowClass::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_captchaWindowClass,
      qt_meta_data_captchaWindowClass, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &captchaWindowClass::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *captchaWindowClass::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *captchaWindowClass::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_captchaWindowClass))
        return static_cast<void*>(const_cast< captchaWindowClass*>(this));
    return QDialog::qt_metacast(_clname);
}

int captchaWindowClass::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void captchaWindowClass::codeForCaptcha(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
