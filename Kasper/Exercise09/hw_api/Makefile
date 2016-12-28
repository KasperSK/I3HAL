MKDIR=mkdir -p
ECHO=echo
RM=rm -rf

ARGV=
ARCH=host
BUILD_DIR=${ARCH}
SRC_DIR=src
SOURCES=${SRC_DIR}/temp_sensor.c ${SRC_DIR}/virt_temp_proxy.c ${SRC_DIR}/hw_api_tester.c ${SRC_DIR}/lm75_proxy.c ${SRC_DIR}/sample_temp_proxy.c 
OBJECTS=${SOURCES:${SRC_DIR}/%.cpp=${BUILD_DIR}/%.o}
DEPS_CPP=${SOURCES:${SRC_DIR}/%.cpp=${BUILD_DIR}/%.dpp}
DEPS=${DEPS_CPP:${SRC_DIR}/%.c=${BUILD_DIR}/%.d}
EXECUTABLE=main
CXXFLAGS=
MAKEFILE_TARGETS_WITHOUT_INCLUDE := clean help 

ifeq (${ARCH},target)
	CXXFLAGS+= 
	CXX=arm-angstrom-linux-gnueabi-gcc
else
	CXXFLAGS+=
	CXX=gcc
endif

ifdef DEBUG
	CXXFLAGS+= -DDEBUG -g
endif

.PHONY: clean help run

${EXECUTABLE}: ${OBJECTS}
	${CXX} -o ${BUILD_DIR}/${EXECUTABLE} ${OBJECTS} ${CXXFLAGS}

${BUILD_DIR}/%.d: ${SRC_DIR}/%.c
	@${MKDIR} ${BUILD_DIR}
	@${CXX} -MT $(@:.d=.o) -MM $< > $@
	@${ECHO} '	$${CXX} -c -o $$@ $$< $${CXXFLAGS}' >> $@

${BUILD_DIR}/%.dpp: ${SRC_DIR}/%.cpp
	@${MKDIR} ${BUILD_DIR}
	@${CXX} -MT $(@:.dpp=.o) -MM $< > $@
	@${ECHO} '	$${CXX} -c -o $$@ $$< $${CXXFLAGS}' >> $@

clean:
	@if [ -d "${BUILD_DIR}" ]; then ${RM} -v  "${BUILD_DIR}"; fi

run: ${EXECUTABLE}
	./${BUILD_DIR}/${EXECUTABLE} ${ARGV}

help:
	@${ECHO} "Help"

ifeq ($(filter $(MAKECMDGOALS),$(MAKEFILE_TARGETS_WITHOUT_INCLUDE)),)
-include $(DEPS)
endif
