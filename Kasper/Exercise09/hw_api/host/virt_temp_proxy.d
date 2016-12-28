host/virt_temp_proxy.o: src/virt_temp_proxy.c src/temp_sensor.h
	${CXX} -c -o $@ $< ${CXXFLAGS}
