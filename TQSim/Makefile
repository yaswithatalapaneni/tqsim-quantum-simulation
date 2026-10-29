output: out/main.o out/noiseTreeSimulator.o out/baselineSimulator.o out/idealSim.o out/util.o 
	g++ -o out/tqsim-cpu -O2 -I ./../qulacs/include -L ./../qulacs/lib out/main.o out/noiseTreeSimulator.o out/idealSim.o out/util.o out/baselineSimulator.o -lvqcsim_static -lcppsim_static -lcsim_static -fopenmp

out/main.o: src/main.cpp
	g++ -o out/main.o -c src/main.cpp -O2 -I ./../qulacs/include -L ./../qulacs/lib -lvqcsim_static -lcppsim_static -lcsim_static -fopenmp

out/noiseTreeSimulator.o: src/noiseTreeSimulator.cpp src/noiseTreeSimulator.hpp
	g++ -o out/noiseTreeSimulator.o -c src/noiseTreeSimulator.cpp -O2 -I ./../qulacs/include -L ./../qulacs/lib -lvqcsim_static -lcppsim_static -lcsim_static -fopenmp

out/baselineSimulator.o: src/baselineSimulator.cpp src/baselineSimulator.hpp
	g++ -o out/baselineSimulator.o -c src/baselineSimulator.cpp -O2 -I ./../qulacs/include -L ./../qulacs/lib -lvqcsim_static -lcppsim_static -lcsim_static -fopenmp

out/idealSim.o: src/idealSim.cpp src/idealSim.hpp
	g++ -o out/idealSim.o -c src/idealSim.cpp -O2 -I ./../qulacs/include -L ./../qulacs/lib -lvqcsim_static -lcppsim_static -lcsim_static -fopenmp

out/util.o: src/util.cpp src/util.hpp
	g++ -o out/util.o -c src/util.cpp -O2 -I ./../qulacs/include -L ./../qulacs/lib -lvqcsim_static -lcppsim_static -lcsim_static -fopenmp

clean:
	rm out/*.o out/tqsim-cpu