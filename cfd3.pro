TEMPLATE = app
TARGET = cfd3
QT += core \
    gui \
    svg \
    opengl
HEADERS += RenderWidget.h \
    cfd3.h
SOURCES += main.cpp \
    RenderWidget.cpp \
    cfd3.cpp
FORMS += cfd3.ui
RESOURCES += 
CUDA_SOURCES += kernels.cu

# #######################################################################
# CUDA
# #######################################################################
win32 { 
    INCLUDEPATH += $(CUDA_INC_DIR)
    QMAKE_LIBDIR += $(CUDA_LIB_DIR)
    LIBS += -lcudart
    cuda.output = $$OBJECTS_DIR/${QMAKE_FILE_BASE}_cuda.obj
    cuda.commands = $(CUDA_BIN_DIR)/nvcc.exe \
        -c \
        -Xcompiler \
        $$join(QMAKE_CXXFLAGS,",") \
        $$join(INCLUDEPATH,'" -I "','-I "','"') \
        ${QMAKE_FILE_NAME} \
        -o \
        ${QMAKE_FILE_OUT}
}
unix { 
    QMAKESPEC = linux-g++-64
    
    # auto-detect CUDA path
    CUDA_DIR = $$system(which nvcc | sed 's,/bin/nvcc$,,')
    INCLUDEPATH += $$CUDA_DIR/include /home/az/NVIDIA_GPU_Computing_SDK/C/common/inc
    QMAKE_LIBDIR += $$CUDA_DIR/lib64 /home/az/NVIDIA_GPU_Computing_SDK/C/common/obj/x86_64/release
    LIBS += -lcudart -lGLEW
    cuda.output = ${OBJECTS_DIR}${QMAKE_FILE_BASE}_cuda.obj
    cuda.commands = nvcc \
        -c \
        -Xcompiler \
        $$join(QMAKE_CXXFLAGS,",") \
        $$join(INCLUDEPATH,'" -I "','-I "','"') \
        ${QMAKE_FILE_NAME} \
        -o \
        ${QMAKE_FILE_OUT}
    
    # cuda.depends = nvcc -M -Xcompiler $$join(QMAKE_CXXFLAGS,",") $$join(INCLUDEPATH,'" -I "','-I "','"') ${QMAKE_FILE_NAME} | sed "s,^.*: ,," | sed "s,^ *,," | tr -d '\\\n'
    cuda.dependcy_type = TYPE_C
    cuda.depend_command = nvcc \
        -M \
        -Xcompiler \
        $$join(QMAKE_CXXFLAGS,",") \
        $$join(INCLUDEPATH,'" -I "','-I "','"') \
        ${QMAKE_FILE_NAME} \
        | \
        sed \
        "s,^.*: ,," \
        | \
        sed \
        "s,^ *,," \
        | \
        tr \
        -d \
        '\\\n'
}
cuda.input = CUDA_SOURCES
QMAKE_EXTRA_COMPILERS += cuda
