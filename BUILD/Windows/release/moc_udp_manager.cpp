/****************************************************************************
** Meta object code from reading C++ file 'udp_manager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Network_sharing/Managers/udp_manager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'udp_manager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UDP_Manager_t {
    QByteArrayData data[7];
    char stringdata0[58];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UDP_Manager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UDP_Manager_t qt_meta_stringdata_UDP_Manager = {
    {
QT_MOC_LITERAL(0, 0, 11), // "UDP_Manager"
QT_MOC_LITERAL(1, 12, 10), // "showSignal"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 7), // "addUser"
QT_MOC_LITERAL(4, 32, 4), // "User"
QT_MOC_LITERAL(5, 37, 10), // "deleteUser"
QT_MOC_LITERAL(6, 48, 9) // "checkUser"

    },
    "UDP_Manager\0showSignal\0\0addUser\0User\0"
    "deleteUser\0checkUser"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UDP_Manager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       3,    1,   37,    2, 0x06 /* Public */,
       5,    1,   40,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   43,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, 0x80000000 | 4,    2,
    QMetaType::Void, 0x80000000 | 4,    2,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void UDP_Manager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        UDP_Manager *_t = static_cast<UDP_Manager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->showSignal((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->addUser((*reinterpret_cast< User(*)>(_a[1]))); break;
        case 2: _t->deleteUser((*reinterpret_cast< User(*)>(_a[1]))); break;
        case 3: _t->checkUser(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (UDP_Manager::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&UDP_Manager::showSignal)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (UDP_Manager::*_t)(User );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&UDP_Manager::addUser)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (UDP_Manager::*_t)(User );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&UDP_Manager::deleteUser)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject UDP_Manager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_UDP_Manager.data,
      qt_meta_data_UDP_Manager,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *UDP_Manager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UDP_Manager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UDP_Manager.stringdata0))
        return static_cast<void*>(const_cast< UDP_Manager*>(this));
    return QObject::qt_metacast(_clname);
}

int UDP_Manager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void UDP_Manager::showSignal(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void UDP_Manager::addUser(User _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void UDP_Manager::deleteUser(User _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
