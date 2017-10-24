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
    TCPSocket/TCPSocket.cpp \
    TCPSocket/TCPSocket_Linux.cpp \
    TCPSocket/TCPSocket_Windows.cpp \
    UDPDiscover/UDPDiscover.cpp \
    UDPSocket/UDPSocketMulticast.cpp \
    UDPSocket/UDPSocketMulticast_Linux.cpp \
    UDPSocket/UDPSocketMulticast_Windows.cpp \
    DataStruct/userlistsingleton.cpp \
    UserInterface/userselection.cpp \
    UserInterface/startui.cpp \
    UserInterface/showusers.cpp \
    UserInterface/notification2.cpp \
    UserInterface/notification.cpp

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
    DataStruct/userlistsingleton.h \
    UserInterface/userselection.h \
    UserInterface/startui.h \
    UserInterface/showusers.h \
    UserInterface/notification2.h \
    UserInterface/notification.h

FORMS    += settings.ui \
    settings_copy.ui \
    UserInterface/filesending.ui \
    UserInterface/progressdialog.ui \
    UserInterface/userselection.ui \
    UserInterface/startui.ui \
    UserInterface/showusers.ui \
    UserInterface/notification.ui

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
    DEPENDPATH += E:/boost_1_64_0/stage/lib-release/lib
    LIBS += -LE:/boost_1_64_0/stage/lib-release/lib -lboost_filesystem-vc140-mt-1_64
}
unix{
    LIBS += -L$$PWD/libraries_linux/lib/ -lboost_filesystem
    LIBS += -L$$PWD/libraries_linux/lib/ -lboost_system

    INCLUDEPATH += $$PWD/libraries_linux/include
    DEPENDPATH += $$PWD/libraries_linux/include
    PRE_TARGETDEPS += $$PWD/libraries_linux/lib/libboost_filesystem.a
    PRE_TARGETDEPS += $$PWD/libraries_linux/lib/libboost_system.a
}

DISTFILES += \
    PdSproject.config \
    rc/branch_closed-on.png \
    rc/branch_closed.png \
    rc/branch_open-on.png \
    rc/branch_open.png \
    rc/checkbox_checked.png \
    rc/checkbox_checked_disabled.png \
    rc/checkbox_checked_focus.png \
    rc/checkbox_indeterminate.png \
    rc/checkbox_indeterminate_disabled.png \
    rc/checkbox_indeterminate_focus.png \
    rc/checkbox_unchecked.png \
    rc/checkbox_unchecked_disabled.png \
    rc/checkbox_unchecked_focus.png \
    rc/close-hover.png \
    rc/close-pressed.png \
    rc/close.png \
    rc/down_arrow.png \
    rc/down_arrow_disabled.png \
    rc/Hmovetoolbar.png \
    rc/Hsepartoolbar.png \
    rc/left_arrow.png \
    rc/left_arrow_disabled.png \
    rc/radio_checked.png \
    rc/radio_checked_disabled.png \
    rc/radio_checked_focus.png \
    rc/radio_unchecked.png \
    rc/radio_unchecked_disabled.png \
    rc/radio_unchecked_focus.png \
    rc/right_arrow.png \
    rc/right_arrow_disabled.png \
    rc/sizegrip.png \
    rc/stylesheet-branch-end.png \
    rc/stylesheet-branch-more.png \
    rc/stylesheet-vline.png \
    rc/transparent.png \
    rc/undock.png \
    rc/up_arrow.png \
    rc/up_arrow_disabled.png \
    rc/Vmovetoolbar.png \
    rc/Vsepartoolbar.png
