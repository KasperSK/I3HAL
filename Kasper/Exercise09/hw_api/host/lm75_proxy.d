host/lm75_proxy.o: src/lm75_proxy.c src/temp_sensor.h
	${CXX} -c -o $@ $< ${CXXFLAGS}
