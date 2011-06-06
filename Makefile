#############################################################################
# Makefile for building: cfd3
# Generated by qmake (2.01a) (Qt 4.7.2) on: Sun Jun 5 20:24:09 2011
# Project:  cfd3.pro
# Template: app
# Command: /usr/bin/qmake -o Makefile cfd3.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_NO_DEBUG -DQT_SVG_LIB -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtOpenGL -I/usr/include/qt4/QtSvg -I/usr/include/qt4 -I/usr/local/cuda/include -I../../NVIDIA_GPU_Computing_SDK/C/common/inc -I/usr/X11R6/include -I. -I.
LINK          = g++
LFLAGS        = -Wl,-O1
LIBS          = $(SUBLIBS)  -L/usr/local/cuda/lib64 -L/home/az/NVIDIA_GPU_Computing_SDK/C/common/obj/x86_64/release -L/usr/lib -L/usr/X11R6/lib -lcudart -lGLEW -lQtSvg -lQtOpenGL -lQtGui -lQtCore -lGLU -lGL -lpthread 
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = perlin.c \
		main.cpp \
		RenderWidget.cpp \
		cfd3.cpp moc_RenderWidget.cpp \
		moc_cfd3.cpp
OBJECTS       = ${OBJECTS_DIR}kernels_cuda.obj \
		perlin.o \
		main.o \
		RenderWidget.o \
		cfd3.o \
		moc_RenderWidget.o \
		moc_cfd3.o
DIST          = /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/opengl.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		cfd3.pro
QMAKE_TARGET  = cfd3
DESTDIR       = 
TARGET        = cfd3

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET): ui_cfd3.h $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: cfd3.pro  /usr/share/qt4/mkspecs/linux-g++/qmake.conf /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/opengl.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		/usr/lib/libQtSvg.prl \
		/usr/lib/libQtOpenGL.prl \
		/usr/lib/libQtGui.prl \
		/usr/lib/libQtCore.prl
	$(QMAKE) -o Makefile cfd3.pro
/usr/share/qt4/mkspecs/common/g++.conf:
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/release.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/opengl.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
/usr/lib/libQtSvg.prl:
/usr/lib/libQtOpenGL.prl:
/usr/lib/libQtGui.prl:
/usr/lib/libQtCore.prl:
qmake:  FORCE
	@$(QMAKE) -o Makefile cfd3.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/cfd31.0.0 || $(MKDIR) .tmp/cfd31.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/cfd31.0.0/ && $(COPY_FILE) --parents kernels.cu .tmp/cfd31.0.0/ && $(COPY_FILE) --parents perlin.h RenderWidget.h cfd3.h .tmp/cfd31.0.0/ && $(COPY_FILE) --parents perlin.c main.cpp RenderWidget.cpp cfd3.cpp .tmp/cfd31.0.0/ && $(COPY_FILE) --parents cfd3.ui .tmp/cfd31.0.0/ && (cd `dirname .tmp/cfd31.0.0` && $(TAR) cfd31.0.0.tar cfd31.0.0 && $(COMPRESS) cfd31.0.0.tar) && $(MOVE) `dirname .tmp/cfd31.0.0`/cfd31.0.0.tar.gz . && $(DEL_FILE) -r .tmp/cfd31.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_cuda_make_all: ${OBJECTS_DIR}kernels_cuda.obj
compiler_cuda_clean:
	-$(DEL_FILE) ${OBJECTS_DIR}kernels_cuda.obj
${OBJECTS_DIR}kernels_cuda.obj: kernels.cu
	nvcc -c -Xcompiler -pipe -I "/usr/local/cuda/include" -I "/home/az/NVIDIA_GPU_Computing_SDK/C/common/inc" kernels.cu -o ${OBJECTS_DIR}kernels_cuda.obj

compiler_moc_header_make_all: moc_RenderWidget.cpp moc_cfd3.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_RenderWidget.cpp moc_cfd3.cpp
moc_RenderWidget.cpp: perlin.h \
		RenderWidget.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) RenderWidget.h -o moc_RenderWidget.cpp

moc_cfd3.cpp: ui_cfd3.h \
		cfd3.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) cfd3.h -o moc_cfd3.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all: ui_cfd3.h
compiler_uic_clean:
	-$(DEL_FILE) ui_cfd3.h
ui_cfd3.h: cfd3.ui
	/usr/bin/uic-qt4 cfd3.ui -o ui_cfd3.h

compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_cuda_clean compiler_moc_header_clean compiler_uic_clean 

####### Compile

perlin.o: perlin.c perlin.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o perlin.o perlin.c

main.o: main.cpp cfd3.h \
		ui_cfd3.h \
		RenderWidget.h \
		perlin.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o main.cpp

RenderWidget.o: RenderWidget.cpp RenderWidget.h \
		perlin.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o RenderWidget.o RenderWidget.cpp

cfd3.o: cfd3.cpp RenderWidget.h \
		perlin.h \
		cfd3.h \
		ui_cfd3.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o cfd3.o cfd3.cpp

moc_RenderWidget.o: moc_RenderWidget.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_RenderWidget.o moc_RenderWidget.cpp

moc_cfd3.o: moc_cfd3.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_cfd3.o moc_cfd3.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

