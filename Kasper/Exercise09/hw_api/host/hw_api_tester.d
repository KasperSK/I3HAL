host/hw_api_tester.o: src/hw_api_tester.c src/temp_sensor.h
	${CXX} -c -o $@ $< ${CXXFLAGS}
