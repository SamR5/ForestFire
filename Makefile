all: ff ff2 ffSim ffHexa ffTri

ff:
	g++ ForestFire/main.cpp -std=c++11 -lGL -lGLU -lglut -O3 -no-pie -o Forest_fire_1

ff2:
	g++ ForestFire2/main.cpp -std=c++11 -lGL -lGLU -lglut -O3 -no-pie -o Forest_fire_2

ffSim:
	g++ ForestFire\(simulation\)/main.cpp -std=c++11 -O3 -o Forest_fire_simulation

ffHexa:
	g++ ForestFireHexa/main.cpp -std=c++11 -lGL -lGLU -lglut -O3 -no-pie -o Forest_fire_hexa

ffTri:
	g++ ForestFireTri/main.cpp -std=c++11 -lGL -lGLU -lglut -O3 -no-pie -o Forest_fire_tri
