/****************************************************************************
** Meta object code from reading C++ file 'utils.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../utils.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'utils.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_UTILS_t {
    QByteArrayData data[7];
    char stringdata0[55];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UTILS_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UTILS_t qt_meta_stringdata_UTILS = {
    {
QT_MOC_LITERAL(0, 0, 5), // "UTILS"
QT_MOC_LITERAL(1, 6, 5), // "_logd"
QT_MOC_LITERAL(2, 12, 0), // ""
QT_MOC_LITERAL(3, 13, 10), // "_open_file"
QT_MOC_LITERAL(4, 24, 10), // "_save_file"
QT_MOC_LITERAL(5, 35, 9), // "open_file"
QT_MOC_LITERAL(6, 45, 9) // "save_file"

    },
    "UTILS\0_logd\0\0_open_file\0_save_file\0"
    "open_file\0save_file"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UTILS[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       3,    1,   42,    2, 0x06 /* Public */,
       4,    1,   45,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   48,    2, 0x0a /* Public */,
       6,    0,   49,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void UTILS::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        UTILS *_t = static_cast<UTILS *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->_logd((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->_open_file((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->_save_file((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->open_file(); break;
        case 4: _t->save_file(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (UTILS::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&UTILS::_logd)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (UTILS::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&UTILS::_open_file)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (UTILS::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&UTILS::_save_file)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject UTILS::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_UTILS.data,
      qt_meta_data_UTILS,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *UTILS::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UTILS::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_UTILS.stringdata0))
        return static_cast<void*>(const_cast< UTILS*>(this));
    return QWidget::qt_metacast(_clname);
}

int UTILS::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void UTILS::_logd(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void UTILS::_open_file(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void UTILS::_save_file(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
struct qt_meta_stringdata_WORKER_THREAD_t {
    QByteArrayData data[3];
    char stringdata0[28];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WORKER_THREAD_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WORKER_THREAD_t qt_meta_stringdata_WORKER_THREAD = {
    {
QT_MOC_LITERAL(0, 0, 13), // "WORKER_THREAD"
QT_MOC_LITERAL(1, 14, 12), // "result_ready"
QT_MOC_LITERAL(2, 27, 0) // ""

    },
    "WORKER_THREAD\0result_ready\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WORKER_THREAD[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,

       0        // eod
};

void WORKER_THREAD::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        WORKER_THREAD *_t = static_cast<WORKER_THREAD *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->result_ready(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (WORKER_THREAD::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WORKER_THREAD::result_ready)) {
                *result = 0;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject WORKER_THREAD::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_WORKER_THREAD.data,
      qt_meta_data_WORKER_THREAD,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *WORKER_THREAD::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WORKER_THREAD::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_WORKER_THREAD.stringdata0))
        return static_cast<void*>(const_cast< WORKER_THREAD*>(this));
    return QThread::qt_metacast(_clname);
}

int WORKER_THREAD::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void WORKER_THREAD::result_ready()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
