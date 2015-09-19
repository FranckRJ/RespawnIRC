/****************************************************************************
** Meta object code from reading C++ file 'respawnIrc.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../respawnIrc/respawnIrc.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'respawnIrc.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_respawnIrcClass_t {
    QByteArrayData data[69];
    char stringdata0[1181];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_respawnIrcClass_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_respawnIrcClass_t qt_meta_stringdata_respawnIrcClass = {
    {
QT_MOC_LITERAL(0, 0, 15), // "respawnIrcClass"
QT_MOC_LITERAL(1, 16, 11), // "showConnect"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 21), // "showAccountListWindow"
QT_MOC_LITERAL(4, 51, 15), // "showSelectTopic"
QT_MOC_LITERAL(5, 67, 20), // "showIgnoreListWindow"
QT_MOC_LITERAL(6, 88, 25), // "showColorPseudoListWindow"
QT_MOC_LITERAL(7, 114, 25), // "showUpdateTopicTimeWindow"
QT_MOC_LITERAL(8, 140, 40), // "showNumberOfMessageShowedFirs..."
QT_MOC_LITERAL(9, 181, 9), // "showAbout"
QT_MOC_LITERAL(10, 191, 9), // "addNewTab"
QT_MOC_LITERAL(11, 201, 18), // "addNewTabWithTopic"
QT_MOC_LITERAL(12, 220, 12), // "newTopicLink"
QT_MOC_LITERAL(13, 233, 9), // "removeTab"
QT_MOC_LITERAL(14, 243, 5), // "index"
QT_MOC_LITERAL(15, 249, 14), // "checkForUpdate"
QT_MOC_LITERAL(16, 264, 11), // "updateTopic"
QT_MOC_LITERAL(17, 276, 11), // "reloadTopic"
QT_MOC_LITERAL(18, 288, 16), // "goToCurrentTopic"
QT_MOC_LITERAL(19, 305, 16), // "goToCurrentForum"
QT_MOC_LITERAL(20, 322, 16), // "quoteThisMessage"
QT_MOC_LITERAL(21, 339, 14), // "messageToQuote"
QT_MOC_LITERAL(22, 354, 23), // "addThisPeudoToBlacklist"
QT_MOC_LITERAL(23, 378, 11), // "pseudoToAdd"
QT_MOC_LITERAL(24, 390, 18), // "setUpdateTopicTime"
QT_MOC_LITERAL(25, 409, 7), // "newTime"
QT_MOC_LITERAL(26, 417, 33), // "setNumberOfMessageShowedFirst..."
QT_MOC_LITERAL(27, 451, 9), // "newNumber"
QT_MOC_LITERAL(28, 461, 18), // "setShowQuoteButton"
QT_MOC_LITERAL(29, 480, 6), // "newVal"
QT_MOC_LITERAL(30, 487, 22), // "setShowBlacklistButton"
QT_MOC_LITERAL(31, 510, 17), // "setShowEditButton"
QT_MOC_LITERAL(32, 528, 27), // "setShowTextDecorationButton"
QT_MOC_LITERAL(33, 556, 18), // "setShowListOfTopic"
QT_MOC_LITERAL(34, 575, 16), // "setMultilineEdit"
QT_MOC_LITERAL(35, 592, 18), // "setLoadTwoLastPage"
QT_MOC_LITERAL(36, 611, 21), // "setIgnoreNetworkError"
QT_MOC_LITERAL(37, 633, 26), // "setSearchForUpdateAtLaunch"
QT_MOC_LITERAL(38, 660, 13), // "setNewCookies"
QT_MOC_LITERAL(39, 674, 21), // "QList<QNetworkCookie>"
QT_MOC_LITERAL(40, 696, 10), // "newCookies"
QT_MOC_LITERAL(41, 707, 15), // "newPseudoOfUser"
QT_MOC_LITERAL(42, 723, 15), // "saveAccountList"
QT_MOC_LITERAL(43, 739, 10), // "savePseudo"
QT_MOC_LITERAL(44, 750, 28), // "setNewCookiesForCurrentTopic"
QT_MOC_LITERAL(45, 779, 11), // "setNewTopic"
QT_MOC_LITERAL(46, 791, 8), // "newTopic"
QT_MOC_LITERAL(47, 800, 17), // "setCodeForCaptcha"
QT_MOC_LITERAL(48, 818, 4), // "code"
QT_MOC_LITERAL(49, 823, 19), // "setNewMessageStatus"
QT_MOC_LITERAL(50, 843, 36), // "setNewNumberOfConnectedAndPse..."
QT_MOC_LITERAL(51, 880, 15), // "setNewTopicName"
QT_MOC_LITERAL(52, 896, 9), // "topicName"
QT_MOC_LITERAL(53, 906, 17), // "saveListOfAccount"
QT_MOC_LITERAL(54, 924, 23), // "saveListOfIgnoredPseudo"
QT_MOC_LITERAL(55, 948, 21), // "saveListOfColorPseudo"
QT_MOC_LITERAL(56, 970, 22), // "warnUserForNewMessages"
QT_MOC_LITERAL(57, 993, 17), // "currentTabChanged"
QT_MOC_LITERAL(58, 1011, 8), // "newIndex"
QT_MOC_LITERAL(59, 1020, 11), // "postMessage"
QT_MOC_LITERAL(60, 1032, 25), // "deleteReplyForSendMessage"
QT_MOC_LITERAL(61, 1058, 14), // "setEditMessage"
QT_MOC_LITERAL(62, 1073, 17), // "idOfMessageToEdit"
QT_MOC_LITERAL(63, 1091, 21), // "setInfoForEditMessage"
QT_MOC_LITERAL(64, 1113, 15), // "idOfMessageEdit"
QT_MOC_LITERAL(65, 1129, 11), // "messageEdit"
QT_MOC_LITERAL(66, 1141, 10), // "infoToSend"
QT_MOC_LITERAL(67, 1152, 11), // "captchaLink"
QT_MOC_LITERAL(68, 1164, 16) // "clipboardChanged"

    },
    "respawnIrcClass\0showConnect\0\0"
    "showAccountListWindow\0showSelectTopic\0"
    "showIgnoreListWindow\0showColorPseudoListWindow\0"
    "showUpdateTopicTimeWindow\0"
    "showNumberOfMessageShowedFirstTimeWindow\0"
    "showAbout\0addNewTab\0addNewTabWithTopic\0"
    "newTopicLink\0removeTab\0index\0"
    "checkForUpdate\0updateTopic\0reloadTopic\0"
    "goToCurrentTopic\0goToCurrentForum\0"
    "quoteThisMessage\0messageToQuote\0"
    "addThisPeudoToBlacklist\0pseudoToAdd\0"
    "setUpdateTopicTime\0newTime\0"
    "setNumberOfMessageShowedFirstTime\0"
    "newNumber\0setShowQuoteButton\0newVal\0"
    "setShowBlacklistButton\0setShowEditButton\0"
    "setShowTextDecorationButton\0"
    "setShowListOfTopic\0setMultilineEdit\0"
    "setLoadTwoLastPage\0setIgnoreNetworkError\0"
    "setSearchForUpdateAtLaunch\0setNewCookies\0"
    "QList<QNetworkCookie>\0newCookies\0"
    "newPseudoOfUser\0saveAccountList\0"
    "savePseudo\0setNewCookiesForCurrentTopic\0"
    "setNewTopic\0newTopic\0setCodeForCaptcha\0"
    "code\0setNewMessageStatus\0"
    "setNewNumberOfConnectedAndPseudoUsed\0"
    "setNewTopicName\0topicName\0saveListOfAccount\0"
    "saveListOfIgnoredPseudo\0saveListOfColorPseudo\0"
    "warnUserForNewMessages\0currentTabChanged\0"
    "newIndex\0postMessage\0deleteReplyForSendMessage\0"
    "setEditMessage\0idOfMessageToEdit\0"
    "setInfoForEditMessage\0idOfMessageEdit\0"
    "messageEdit\0infoToSend\0captchaLink\0"
    "clipboardChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_respawnIrcClass[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      47,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  249,    2, 0x0a /* Public */,
       3,    0,  250,    2, 0x0a /* Public */,
       4,    0,  251,    2, 0x0a /* Public */,
       5,    0,  252,    2, 0x0a /* Public */,
       6,    0,  253,    2, 0x0a /* Public */,
       7,    0,  254,    2, 0x0a /* Public */,
       8,    0,  255,    2, 0x0a /* Public */,
       9,    0,  256,    2, 0x0a /* Public */,
      10,    0,  257,    2, 0x0a /* Public */,
      11,    1,  258,    2, 0x0a /* Public */,
      13,    1,  261,    2, 0x0a /* Public */,
      15,    0,  264,    2, 0x0a /* Public */,
      16,    0,  265,    2, 0x0a /* Public */,
      17,    0,  266,    2, 0x0a /* Public */,
      18,    0,  267,    2, 0x0a /* Public */,
      19,    0,  268,    2, 0x0a /* Public */,
      20,    1,  269,    2, 0x0a /* Public */,
      22,    1,  272,    2, 0x0a /* Public */,
      24,    1,  275,    2, 0x0a /* Public */,
      26,    1,  278,    2, 0x0a /* Public */,
      28,    1,  281,    2, 0x0a /* Public */,
      30,    1,  284,    2, 0x0a /* Public */,
      31,    1,  287,    2, 0x0a /* Public */,
      32,    1,  290,    2, 0x0a /* Public */,
      33,    1,  293,    2, 0x0a /* Public */,
      34,    1,  296,    2, 0x0a /* Public */,
      35,    1,  299,    2, 0x0a /* Public */,
      36,    1,  302,    2, 0x0a /* Public */,
      37,    1,  305,    2, 0x0a /* Public */,
      38,    4,  308,    2, 0x0a /* Public */,
      44,    3,  317,    2, 0x0a /* Public */,
      45,    1,  324,    2, 0x0a /* Public */,
      47,    1,  327,    2, 0x0a /* Public */,
      49,    0,  330,    2, 0x0a /* Public */,
      50,    0,  331,    2, 0x0a /* Public */,
      51,    1,  332,    2, 0x0a /* Public */,
      53,    0,  335,    2, 0x0a /* Public */,
      54,    0,  336,    2, 0x0a /* Public */,
      55,    0,  337,    2, 0x0a /* Public */,
      56,    0,  338,    2, 0x0a /* Public */,
      57,    1,  339,    2, 0x0a /* Public */,
      59,    0,  342,    2, 0x0a /* Public */,
      60,    0,  343,    2, 0x0a /* Public */,
      61,    1,  344,    2, 0x0a /* Public */,
      61,    0,  347,    2, 0x2a /* Public | MethodCloned */,
      63,    4,  348,    2, 0x0a /* Public */,
      68,    0,  357,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void, QMetaType::Int,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   21,
    QMetaType::Void, QMetaType::QString,   23,
    QMetaType::Void, QMetaType::Int,   25,
    QMetaType::Void, QMetaType::Int,   27,
    QMetaType::Void, QMetaType::Bool,   29,
    QMetaType::Void, QMetaType::Bool,   29,
    QMetaType::Void, QMetaType::Bool,   29,
    QMetaType::Void, QMetaType::Bool,   29,
    QMetaType::Void, QMetaType::Bool,   29,
    QMetaType::Void, QMetaType::Bool,   29,
    QMetaType::Void, QMetaType::Bool,   29,
    QMetaType::Void, QMetaType::Bool,   29,
    QMetaType::Void, QMetaType::Bool,   29,
    QMetaType::Void, 0x80000000 | 39, QMetaType::QString, QMetaType::Bool, QMetaType::Bool,   40,   41,   42,   43,
    QMetaType::Void, 0x80000000 | 39, QMetaType::QString, QMetaType::Bool,   40,   41,   43,
    QMetaType::Void, QMetaType::QString,   46,
    QMetaType::Void, QMetaType::QString,   48,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   52,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   58,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   62,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QString, QMetaType::QString,   64,   65,   66,   67,
    QMetaType::Void,

       0        // eod
};

void respawnIrcClass::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        respawnIrcClass *_t = static_cast<respawnIrcClass *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->showConnect(); break;
        case 1: _t->showAccountListWindow(); break;
        case 2: _t->showSelectTopic(); break;
        case 3: _t->showIgnoreListWindow(); break;
        case 4: _t->showColorPseudoListWindow(); break;
        case 5: _t->showUpdateTopicTimeWindow(); break;
        case 6: _t->showNumberOfMessageShowedFirstTimeWindow(); break;
        case 7: _t->showAbout(); break;
        case 8: _t->addNewTab(); break;
        case 9: _t->addNewTabWithTopic((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 10: _t->removeTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->checkForUpdate(); break;
        case 12: _t->updateTopic(); break;
        case 13: _t->reloadTopic(); break;
        case 14: _t->goToCurrentTopic(); break;
        case 15: _t->goToCurrentForum(); break;
        case 16: _t->quoteThisMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 17: _t->addThisPeudoToBlacklist((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 18: _t->setUpdateTopicTime((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->setNumberOfMessageShowedFirstTime((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: _t->setShowQuoteButton((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 21: _t->setShowBlacklistButton((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 22: _t->setShowEditButton((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 23: _t->setShowTextDecorationButton((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 24: _t->setShowListOfTopic((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 25: _t->setMultilineEdit((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 26: _t->setLoadTwoLastPage((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 27: _t->setIgnoreNetworkError((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 28: _t->setSearchForUpdateAtLaunch((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 29: _t->setNewCookies((*reinterpret_cast< QList<QNetworkCookie>(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 30: _t->setNewCookiesForCurrentTopic((*reinterpret_cast< QList<QNetworkCookie>(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 31: _t->setNewTopic((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 32: _t->setCodeForCaptcha((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 33: _t->setNewMessageStatus(); break;
        case 34: _t->setNewNumberOfConnectedAndPseudoUsed(); break;
        case 35: _t->setNewTopicName((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 36: _t->saveListOfAccount(); break;
        case 37: _t->saveListOfIgnoredPseudo(); break;
        case 38: _t->saveListOfColorPseudo(); break;
        case 39: _t->warnUserForNewMessages(); break;
        case 40: _t->currentTabChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 41: _t->postMessage(); break;
        case 42: _t->deleteReplyForSendMessage(); break;
        case 43: _t->setEditMessage((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 44: _t->setEditMessage(); break;
        case 45: _t->setInfoForEditMessage((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4]))); break;
        case 46: _t->clipboardChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 29:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QNetworkCookie> >(); break;
            }
            break;
        case 30:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QNetworkCookie> >(); break;
            }
            break;
        }
    }
}

const QMetaObject respawnIrcClass::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_respawnIrcClass.data,
      qt_meta_data_respawnIrcClass,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *respawnIrcClass::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *respawnIrcClass::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_respawnIrcClass.stringdata0))
        return static_cast<void*>(const_cast< respawnIrcClass*>(this));
    return QWidget::qt_metacast(_clname);
}

int respawnIrcClass::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 47)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 47;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 47)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 47;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
