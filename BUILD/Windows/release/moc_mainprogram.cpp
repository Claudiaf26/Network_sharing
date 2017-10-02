/****************************************************************************
** Meta object code from reading C++ file 'mainprogram.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Network_sharing/mainprogram.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainprogram.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainProgram_t {
    QByteArrayData data[10];
    char stringdata0[86];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainProgram_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainProgram_t qt_meta_stringdata_MainProgram = {
    {
QT_MOC_LITERAL(0, 0, 11), // "MainProgram"
QT_MOC_LITERAL(1, 12, 14), // "changeSettings"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 7), // "uint8_t"
QT_MOC_LITERAL(4, 36, 6), // "string"
QT_MOC_LITERAL(5, 43, 5), // "start"
QT_MOC_LITERAL(6, 49, 12), // "startProgram"
QT_MOC_LITERAL(7, 62, 7), // "addUser"
QT_MOC_LITERAL(8, 70, 4), // "User"
QT_MOC_LITERAL(9, 75, 10) // "deleteUser"

    },
    "MainProgram\0changeSettings\0\0uint8_t\0"
    "string\0start\0startProgram\0addUser\0"
    "User\0deleteUser"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainProgram[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   46,    2, 0x0a /* Public */,
       6,    3,   47,    2, 0x0a /* Public */,
       7,    1,   54,    2, 0x0a /* Public */,
       9,    1,   57,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 4, 0x80000000 | 4,    2,    2,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 4, 0x80000000 | 4,    2,    2,    2,
    QMetaType::Void, 0x80000000 | 8,    2,
    QMetaType::Void, 0x80000000 | 8,    2,

       0        // eod
};

void MainProgram::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainProgram *_t = static_cast<MainProgram *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->changeSettings((*reinterpret_cast< uint8_t(*)>(_a[1])),(*reinterpret_cast< string(*)>(_a[2])),(*reinterpret_cast< string(*)>(_a[3]))); break;
        case 1: _t->start(); break;
        case 2: _t->startProgram((*reinterpret_cast< uint8_t(*)>(_a[1])),(*reinterpret_cast< string(*)>(_a[2])),(*reinterpret_cast< string(*)>(_a[3]))); break;
        case 3: _t->addUser((*reinterpret_cast< User(*)>(_a[1]))); break;
        case 4: _t->deleteUser((*reinterpret_cast< User(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (MainProgram::*_t)(uint8_t , string , string );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainProgram::changeSettings)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject MainProgram::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MainProgram.data,
      qt_meta_data_MainProgram,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *MainProgram::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainProgram::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainProgram.stringdata0))
        return static_cast<void*>(const_cast< MainProgram*>(this));
    return QObject::qt_metacast(_clname);
}

int MainProgram::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void MainProgram::changeSettings(uint8_t _t1, string _t2, string _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
