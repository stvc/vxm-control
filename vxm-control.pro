QT += core gui widgets multimedia multimediawidgets serialport

SOURCES += src/main.cpp src/mainwindow.cpp src/serialconfigdialog.cpp src/vxmcontroller.cpp src/cameraconfigdialog.cpp

HEADERS += src/mainwindow.h src/serialconfigdialog.h src/vxmcontroller.h src/cameraconfigdialog.h

FORMS += src/mainwindow.ui src/serialconfigdialog.ui src/cameraconfigdialog.ui

UI_DIR = src

release:DESTDIR = release
release:OBJECTS_DIR = release/.obj
release:MOC_DIR = release/.moc
release:MCC_DIR = release/.ui

debug:DESTDIR = debug
debug:OBJECTS_DIR = debug/.obj
debug:MOC_DIR = debug/.moc
debug:MCC_DIR = debug/.ui
