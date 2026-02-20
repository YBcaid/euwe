/****************************************************************************
** Meta object code from reading C++ file 'wordsdatabase.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../wordsdatabase.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'wordsdatabase.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN13WordsDatabaseE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN13WordsDatabaseE = QtMocHelpers::stringData(
    "WordsDatabase",
    "onDataChanged",
    "",
    "QModelIndex",
    "topLeft",
    "bottomRight",
    "QList<int>",
    "roles",
    "onSaveChanges",
    "updateCountLabel",
    "QLabel*",
    "label",
    "importFromCsv",
    "exportToCsv",
    "exportToJson"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN13WordsDatabaseE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    3,   50,    2, 0x08,    1 /* Private */,
       8,    0,   57,    2, 0x08,    5 /* Private */,
       9,    1,   58,    2, 0x08,    6 /* Private */,
      12,    0,   61,    2, 0x08,    8 /* Private */,
      13,    0,   62,    2, 0x08,    9 /* Private */,
      14,    0,   63,    2, 0x08,   10 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3, 0x80000000 | 6,    4,    5,    7,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject WordsDatabase::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ZN13WordsDatabaseE.offsetsAndSizes,
    qt_meta_data_ZN13WordsDatabaseE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN13WordsDatabaseE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<WordsDatabase, std::true_type>,
        // method 'onDataChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVector<int> &, std::false_type>,
        // method 'onSaveChanges'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'updateCountLabel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QLabel *, std::false_type>,
        // method 'importFromCsv'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'exportToCsv'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'exportToJson'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void WordsDatabase::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<WordsDatabase *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onDataChanged((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QList<int>>>(_a[3]))); break;
        case 1: _t->onSaveChanges(); break;
        case 2: _t->updateCountLabel((*reinterpret_cast< std::add_pointer_t<QLabel*>>(_a[1]))); break;
        case 3: _t->importFromCsv(); break;
        case 4: _t->exportToCsv(); break;
        case 5: _t->exportToJson(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 2:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<int> >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QLabel* >(); break;
            }
            break;
        }
    }
}

const QMetaObject *WordsDatabase::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WordsDatabase::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN13WordsDatabaseE.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int WordsDatabase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
