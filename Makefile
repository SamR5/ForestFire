all: ForestFire ForestFire2 ForestFire(simulation) ForestFireHexa ForestFireTri

ForestFire:
	g++ main.cpp -std=c++11 -lGL -lGLU -lglut -O3 -no-pie -o ForestFire1

ForestFire2:
	g++ main.cpp -std=c++11 -lGL -lGLU -lglut -O3 -no-pie -o ForestFire2

ForestFire(simulation):
	g++ main.cpp -std=c++11 -O3 -o ForestFireSimulation

ForestFireHexa:
	g++ main.cpp -std=c++11 -lGL -lGLU -lglut -O3 -no-pie -o HexagonalForestFire

ForestFireTri:
	g++ main.cpp -std=c++11 -lGL -lGLU -lglut -O3 -no-pie -o TriangularForestFire
