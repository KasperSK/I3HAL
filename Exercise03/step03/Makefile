ARCH=host
BUILD_DIR=${ARCH}
SOURCES=leds.c
OBJECTS=$(addprefix ${BUILD_DIR}/, ${SOURCES:.c=.o})
EXECUTABLE=leds
CXXFLAGS=
HFILES=
ifeq (${ARCH},target)
        CXX=arm-angstrom-linux-gnueabi-g++
else
        CXX=g++
endif

ifdef DEBUG
	CXXFLAGS += -DDEBUG -g 
endif

${EXECUTABLE}: ${OBJECTS} ${HFILES}
	  ${CXX} -o ${BUILD_DIR}/${EXECUTABLE} ${OBJECTS} ${CXXFLAGS}
	  
${BUILD_DIR}/%.o: %.c ${BUILD_DIR}
	  ${CXX} -c -o $@ $< ${CXXFLAGS}
	  
clean:	 
	  rm -rf ${BUILD_DIR}

rebuild: clean ${EXECUTABLE}

run:
	./${BUILD_DIR}/${EXECUTABLE}

rerun: rebuild run

help:
	  @echo "\t hello:\n\t Invoke compiler g++ to create executable hello from object file hello.o\n"
	  @echo "\t hello.o:\n\t Invoke compiler g++ to create an object file hello.o from source file hello.cpp\n"
	  @echo "\t clean:\n\t removees all the object files as well as the executable\n"
	  
${BUILD_DIR}:
	mkdir -p ${BUILD_DIR}
