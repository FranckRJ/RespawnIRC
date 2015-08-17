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
      36,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x0a,
      27,   16,   16,   16, 0x0a,
      39,   16,   16,   16, 0x0a,
      54,   16,   16,   16, 0x0a,
      66,   16,   16,   16, 0x0a,
      77,   16,   16,   16, 0x0a,
      89,   16,   16,   16, 0x0a,
     100,   16,   16,   16, 0x0a,
     110,   16,   16,   16, 0x0a,
     121,   16,   16,   16, 0x0a,
     135,   16,   16,   16, 0x0a,
     153,   16,   16,   16, 0x0a,
     176,   16,   16,   16, 0x0a,
     204,   16,   16,   16, 0x0a,
     247,   16,   16,   16, 0x0a,
     265,  259,   16,   16, 0x0a,
     280,   16,   16,   16, 0x0a,
     294,   16,   16,   16, 0x0a,
     313,   16,   16,   16, 0x0a,
     340,  332,   16,   16, 0x0a,
     374,  364,   16,   16, 0x0a,
     420,  413,   16,   16, 0x0a,
     454,  413,   16,   16, 0x0a,
     477,  413,   16,   16, 0x0a,
     538,  502,   16,   16, 0x0a,
     597,  588,   16,   16, 0x0a,
     623,  618,   16,   16, 0x0a,
     650,   16,   16,   16, 0x0a,
     672,   16,   16,   16, 0x0a,
     721,  711,   16,   16, 0x0a,
     746,   16,   16,   16, 0x0a,
     772,   16,   16,   16, 0x0a,
     806,  797,   16,   16, 0x0a,
     829,   16,   16,   16, 0x0a,
     843,   16,   16,   16, 0x0a,
     871,   16,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_respawnIrcClass[] = {
    "respawnIrcClass\0\0addBold()\0addItalic()\0"
    "addUnderLine()\0addStrike()\0addUList()\0"
    "addOListe()\0addQuote()\0addCode()\0"
    "addSpoil()\0showConnect()\0showSelectTopic()\0"
    "showIgnoreListWindow()\0"
    "showUpdateTopicTimeWindow()\0"
    "showNumberOfMessageShowedFirstTimeWindow()\0"
    "addNewTab()\0index\0removeTab(int)\0"
    "reloadTopic()\0goToCurrentTopic()\0"
    "goToCurrentForum()\0newTime\0"
    "setUpdateTopicTime(int)\0newNumber\0"
    "setNumberOfMessageShowedFirstTime(int)\0"
    "newVal\0setShowTextDecorationButton(bool)\0"
    "setMultilineEdit(bool)\0setLoadTwoLastPage(bool)\0"
    "newCookies,newPseudoOfUser,saveInfo\0"
    "setNewCookies(QList<QNetworkCookie>,QString,bool)\0"
    "newTopic\0setNewTopic(QString)\0code\0"
    "setCodeForCaptcha(QString)\0"
    "setNewMessageStatus()\0"
    "setNewNumberOfConnectedAndPseudoUsed()\0"
    "topicName\0setNewTopicName(QString)\0"
    "saveListOfIgnoredPseudo()\0"
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
        case 0: _t->addBold(); break;
        case 1: _t->addItalic(); break;
        case 2: _t->addUnderLine(); break;
        case 3: _t->addStrike(); break;
        case 4: _t->addUList(); break;
        case 5: _t->addOListe(); break;
        case 6: _t->addQuote(); break;
        case 7: _t->addCode(); break;
        case 8: _t->addSpoil(); break;
        case 9: _t->showConnect(); break;
        case 10: _t->showSelectTopic(); break;
        case 11: _t->showIgnoreListWindow(); break;
        case 12: _t->showUpdateTopicTimeWindow(); break;
        case 13: _t->showNumberOfMessageShowedFirstTimeWindow(); break;
        case 14: _t->addNewTab(); break;
        case 15: _t->removeTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->reloadTopic(); break;
        case 17: _t->goToCurrentTopic(); break;
        case 18: _t->goToCurrentForum(); break;
        case 19: _t->setUpdateTopicTime((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: _t->setNumberOfMessageShowedFirstTime((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 21: _t->setShowTextDecorationButton((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 22: _t->setMultilineEdit((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 23: _t->setLoadTwoLastPage((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 24: _t->setNewCookies((*reinterpret_cast< QList<QNetworkCookie>(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 25: _t->setNewTopic((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 26: _t->setCodeForCaptcha((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 27: _t->setNewMessageStatus(); break;
        case 28: _t->setNewNumberOfConnectedAndPseudoUsed(); break;
        case 29: _t->setNewTopicName((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 30: _t->saveListOfIgnoredPseudo(); break;
        case 31: _t->warnUserForNewMessages(); break;
        case 32: _t->currentTabChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 33: _t->postMessage(); break;
        case 34: _t->deleteReplyForSendMessage(); break;
        case 35: _t->clipboardChanged(); break;
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
        if (_id < 36)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 36;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
