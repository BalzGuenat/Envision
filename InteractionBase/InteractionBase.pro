ENVISION_ROOT_DIR = $$PWD/..
CONFIG(release, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/ReleaseBuild
CONFIG(debug, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/DebugBuild
PLUGINS_DIR = $${BUILD_DIR}/plugins
CONFIG(debug, debug|release):DEFINES += DEBUG
QMAKE_CXXFLAGS += -Werror
INCLUDEPATH += ./headers \
    ./src \
    ./test \
    $${ENVISION_ROOT_DIR}/Core/headers \
    $${ENVISION_ROOT_DIR}
TARGET = interactionbase
DEFINES += INTERACTIONBASE_LIBRARY
win32:LIBS += -L$${PLUGINS_DIR} \
    -llogger \
    -lselftest \
    -lmodelbase \
    -lfilepersistence \
    -lvisualizationbase
QT = core
TEMPLATE = lib
CONFIG += plugin \
    warn_on \
    thread
target.path = $$PLUGINS_DIR
pluginmeta.path = $$PLUGINS_DIR
pluginmeta.files = $${TARGET}.plugin
INSTALLS += target \
    pluginmeta
HEADERS += headers/commands/CommandExecutionEngine.h \
    headers/commands/CommandHelp.h \
    headers/commands/CommandError.h \
    headers/commands/CommandSuggestion.h \
    headers/commands/CommandResult.h \
    headers/commands/Command.h \
    headers/HExtendable.h \
    headers/GenericHandler.h \
    headers/HText.h \
    headers/interactionbase_api.h \
    src/interactionbase.h
SOURCES += src/commands/CommandExecutionEngine.cpp \
    src/commands/CommandHelp.cpp \
    src/commands/CommandError.cpp \
    src/commands/CommandSuggestion.cpp \
    src/commands/CommandResult.cpp \
    src/commands/Command.cpp \
    src/HExtendable.cpp \
    src/GenericHandler.cpp \
    src/HText.cpp \
    test/SimpleTest.cpp \
    src/interactionbase.cpp
