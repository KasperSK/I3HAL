host/temp_sensor.o: src/temp_sensor.c src/temp_sensor.h src/lm75_proxy.h \
 src/virt_temp_proxy.h src/sample_temp_proxy.h
	${CXX} -c -o $@ $< ${CXXFLAGS}
