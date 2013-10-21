CPP = g++
OBJS = main.o vec3.o lightsource.o material.o random.o surface.o color.o
DEBUG_FLAGS = -g -DDEBUG
CFLAGS = -DXP_UNIX $(DEBUG_FLAGS)
LIBS = -lgd -lm

main : $(OBJS)
	$(CPP) $(DEBUG_FLAG) -o raytracer $(OBJS) $(LIBS)

lightsource.o : lightsource.cc
main.o : main.cc
material.o : material.cc
random.o : random.cc
surface.o : surface.cc
vec3.o : vec3.cc
color.o : color.cc

clean :
	rm $(OBJS) raytracer
