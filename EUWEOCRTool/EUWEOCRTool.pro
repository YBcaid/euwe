QT = core

CONFIG += c++17 cmdline

#配置Tesseract库路径
INCLUDEPATH += E:\TEMP_BCAID\Project\EUWEOCRTool\tesseracte_lib\tesseract_x64-windows\include
LIBS+= E:\TEMP_BCAID\Project\EUWEOCRTool\tesseracte_lib\tesseract_x64-windows\lib\tesseract41.lib
#配置Leptonica库路径
INCLUDEPATH += E:\TEMP_BCAID\Project\EUWEOCRTool\tesseracte_lib\leptonica_x64-windows\include
LIBS+= E:\TEMP_BCAID\Project\EUWEOCRTool\tesseracte_lib\leptonica_x64-windows\lib\leptonica-1.78.0.lib


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
