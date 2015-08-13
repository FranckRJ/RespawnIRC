/****************************************************************************
** Meta object code from reading C++ file 'respawnIrc.hpp'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../respawnIrc/respawnIrc.hpp"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'respawnIrc.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_respawnIrcClass[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x0a,
      31,   16,   16,   16, 0x0a,
      49,   16,   16,   16, 0x0a,
      67,   61,   16,   16, 0x0a,
     118,   82,   16,   16, 0x0a,
     177,  168,   16,   16, 0x0a,
     203,  198,   16,   16, 0x0a,
     230,   16,   16,   16, 0x0a,
     262,  252,   16,   16, 0x0a,
     287,   16,   16,   16, 0x0a,
     321,  312,   16,   16, 0x0a,
     344,   16,   16,   16, 0x0a,
     358,   16,   16,   16, 0x0a,
     386,   16,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_respawnIrcClass[] = {
    "respawnIrcClass\0\0showConnect()\0"
    "showSelectTopic()\0addNewTab()\0index\0"
    "removeTab(int)\0newCookies,newPseudoOfUser,saveInfo\0"
    "setNewCookies(QList<QNetworkCookie>,QString,bool)\0"
    "newTopic\0setNewTopic(QString)\0code\0"
    "setCodeForCaptcha(QString)\0"
    "setNewMessageStatus()\0topicName\0"
    "setNewTopicName(QString)\0"
    "warnUserForNewMessages()\0newIndex\0"
    "currentTabChanged(int)\0postMessage()\0"
    "deleteReplyForSendMessage()\0"
    "clipboardChanged()\0"
};

void respawnIrcClass::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        respawnIrcClass *_t = static_cast<respawnIrcClass *>(_o);
        switch (_id) {
        case 0: _t->showConnect(); break;
        case 1: _t->showSelectTopic(); break;
        case 2: _t->addNewTab(); break;
        case 3: _t->removeTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->setNewCookies((*reinterpret_cast< QList<QNetworkCookie>(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 5: _t->setNewTopic((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->setCodeForCaptcha((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: _t->setNewMessageStatus(); break;
        case 8: _t->setNewTopicName((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 9: _t->warnUserForNewMessages(); break;
        case 10: _t->currentTabChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->postMessage(); break;
        case 12: _t->deleteReplyForSendMessage(); break;
        case 13: _t->clipboardChanged(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData respawnIrcClass::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject respawnIrcClass::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_respawnIrcClass,
      qt_meta_data_respawnIrcClass, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &respawnIrcClass::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *respawnIrcClass::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *respawnIrcClass::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_respawnIrcClass))
        return static_cast<void*>(const_cast< respawnIrcClass*>(this));
    return QWidget::qt_metacast(_clname);
}

int respawnIrcClass::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
