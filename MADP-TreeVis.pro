QT += core gui widgets svg

VERSION = 0.1

TARGET = MADP-TreeVis
TEMPLATE = app

MOC_DIR = ./build/moc
OBJECTS_DIR = ./build/obj

DESTDIR = ./bin

# MADP
INCLUDEPATH += /usr/local/include/madp
LIBS += -lMADP

# Boost with MADP
INCLUDEPATH += /home/user/path_to_madp/MADP/src/boost

# Our headers
INCLUDEPATH += ./src/headers

# CXX Flags
QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic

# Sources for TreeVis
SOURCES += \
    src/sources/MainWindow.cpp \
    src/sources/Main.cpp \
    src/sources/JESPPlanner.cpp \
    src/sources/BFSPlanner.cpp \
    src/sources/GMAAPlanner.cpp \
    src/sources/DICEPlanner.cpp \
    src/sources/PlannerManager.cpp \
    src/sources/Node.cpp \
    src/sources/Edge.cpp \
    src/sources/StartPlanWizard.cpp \
    src/sources/TreeVisGraphicsView.cpp \
    src/sources/FullTreeView.cpp \
    src/sources/PolicyVisualiserView.cpp \
    src/sources/NumSortTreeWidgetItem.cpp \
    src/sources/JointObservationSelectionDialog.cpp \
    src/sources/PreviousPlanWizard.cpp \
    src/sources/TreeVisGraphicsScene.cpp \
    src/sources/SettingsDialog.cpp \
    src/sources/GeneralUtils.cpp

# Headers for TreeVis
HEADERS += \
    src/headers/MainWindow.h \
    src/headers/JESPPlanner.h \
    src/headers/BFSPlanner.h \
    src/headers/GMAAPlanner.h \
    src/headers/DICEPlanner.h \
    src/headers/PlannerManager.h \
    src/headers/Planner.h \
    src/headers/Node.h \
    src/headers/Edge.h \
    src/headers/StartPlanWizard.h \
    src/headers/TreeVisGraphicsView.h \
    src/headers/FullTreeView.h \
    src/headers/PolicyVisualiserView.h \
    src/headers/UIMainWindow.h \
    src/headers/NumSortTreeWidgetItem.h \
    src/headers/JointObservationSelectionDialog.h \
    src/headers/PreviousPlanWizard.h \
    src/headers/TreeVisGraphicsScene.h \
    src/headers/SettingsDialog.h \
    src/headers/GeneralUtils.h