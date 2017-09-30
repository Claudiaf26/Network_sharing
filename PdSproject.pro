#-------------------------------------------------
#
# Project created by QtCreator 2017-05-06T14:55:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PdSproject
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    mainprogram.cpp \
    SharedMemory/sharedmem.cpp \
    SharedMemory/sharedmem_linux.cpp \
    SharedMemory/sharedmem_windows.cpp \
    ContextMenu/contextmenu.cpp \
    ContextMenu/contextmenu_linux.cpp \
    ContextMenu/contextmenu_windows.cpp \
    Managers/notificationmanager.cpp \
    Managers/udp_manager.cpp \
    UserInterface/notification.cpp \
    UserInterface/settings.cpp \
    DataStruct/usersingleton.cpp \
    File/FileReceiver.cpp \
    File/FileTransfer.cpp \
    UserInterface/filesending.cpp \
    UserInterface/progressdialog.cpp \
    sendingprogram.cpp \
    Managers/sendermanager.cpp \
    Managers/receivermanager.cpp \
    TCPServerSocket/TCPServerSocket.cpp \
    TCPServerSocket/TCPServerSocket_Linux.cpp \
    TCPServerSocket/TCPServerSocket_Windows.cpp \
    TCPSocket/Main.cpp \
    TCPSocket/TCPSocket.cpp \
    TCPSocket/TCPSocket_Linux.cpp \
    TCPSocket/TCPSocket_Windows.cpp \
    UDPDiscover/UDPDiscover.cpp \
    UDPSocket/UDPSocketMulticast.cpp \
    UDPSocket/UDPSocketMulticast_Linux.cpp \
    UDPSocket/UDPSocketMulticast_Windows.cpp \
    DataStruct/userlistsingleton.cpp

HEADERS  += \
    mainprogram.h \
    define.h \
    systemswrapper.h \
    SharedMemory/sharedmem.h \
    SharedMemory/sharedmem_interface.h \
    SharedMemory/sharedmem_linux.h \
    SharedMemory/sharedmem_windows.h \
    ContextMenu/contextmenu.h \
    ContextMenu/contextmenu_interface.h \
    ContextMenu/contextmenu_linux.h \
    ContextMenu/contextmenu_windows.h \
    Managers/notificationmanager.h \
    Managers/udp_manager.h \
    UserInterface/notification.h \
    UserInterface/settings.h \
    DataStruct/usersingleton.h \
    File/FileReceiver.h \
    File/FileTransfer.h \
    UserInterface/filesending.h \
    UserInterface/progressdialog.h \
    sendingprogram.h \
    Managers/sendermanager.h \
    Managers/receivermanager.h \
    TCPServerSocket/TCPServerSocket.h \
    TCPServerSocket/TCPServerSocket_Interface.h \
    TCPServerSocket/TCPServerSocket_Linux.h \
    TCPServerSocket/TCPServerSocket_Windows.h \
    TCPSocket/TCPSocket.h \
    TCPSocket/TCPSocket_Interface.h \
    TCPSocket/TCPSocket_Linux.h \
    TCPSocket/TCPSocket_Windows.h \
    UDPDiscover/UDPDiscover.h \
    UDPSocket/UDPSocketMulticast.h \
    UDPSocket/UDPSocketMulticast_Interface.h \
    UDPSocket/UDPSocketMulticast_Linux.h \
    UDPSocket/UDPSocketMulticast_Windows.h \
    DataStruct/userlistsingleton.h

FORMS    += settings.ui \
    settings_copy.ui \
    UserInterface/filesending.ui \
    UserInterface/progressdialog.ui

RESOURCES += \
    resources.qrc

isEmpty(TARGET_EXT) {
    win32 {
        TARGET_CUSTOM_EXT = .exe
    }
    macx {
        TARGET_CUSTOM_EXT = .app
    }
} else {
    TARGET_CUSTOM_EXT = $${TARGET_EXT}
}

win32 {
    DEPLOY_COMMAND = windeployqt
}
macx {
    DEPLOY_COMMAND = macdeployqt
}

win32{
    CONFIG( debug, debug|release ) {
        # debug
        DEPLOY_TARGET = $$shell_quote($$shell_path($${OUT_PWD}/debug/$${TARGET}$${TARGET_CUSTOM_EXT}))
    } else {
        # release
        DEPLOY_TARGET = $$shell_quote($$shell_path($${OUT_PWD}/release/$${TARGET}$${TARGET_CUSTOM_EXT}))
    }
}
#  # Uncomment the following line to help debug the deploy command when running qmake
#  warning($${DEPLOY_COMMAND} $${DEPLOY_TARGET})

# Use += instead of = if you use multiple QMAKE_POST_LINKs
QMAKE_POST_LINK = $${DEPLOY_COMMAND} $${DEPLOY_TARGET}

win32:QMAKE_LFLAGS += /MANIFESTUAC:$$quote(\"level=\'requireAdministrator\' uiAccess=\'false\'\")

unix:QMAKE_CXXFLAGS += -std=c++0x -lpthread
unix:LIBS += -lpthread

win32{
    INCLUDEPATH += E:/boost_1_64_0
    DEPENDPATH += E:/boost_1_64_0/stage/lib
    LIBS += -LE:/boost_1_64_0/stage/lib -llibboost_filesystem-vc140-mt-gd-1_64
}
unix{
    LIBS += -L$$PWD/libraries_linux/lib/ -lboost_filesystem
    LIBS += -L$$PWD/libraries_linux/lib/ -lboost_system

    INCLUDEPATH += $$PWD/libraries_linux/include
    DEPENDPATH += $$PWD/libraries_linux/include
    PRE_TARGETDEPS += $$PWD/libraries_linux/lib/libboost_filesystem.a
    PRE_TARGETDEPS += $$PWD/libraries_linux/lib/libboost_system.a
}
