RayTracing:
	g++ -g -std=c++14 -o RayTracing *.cpp -lsfml-system -lsfml-graphics -lsfml-window

run:
	./RayTracing

rerun:
	make -B
	make run

debug:
	gdb ./RayTracing
