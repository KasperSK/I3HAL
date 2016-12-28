host/sample_temp_proxy.o: src/sample_temp_proxy.c src/temp_sensor.h
	${CXX} -c -o $@ $< ${CXXFLAGS}
