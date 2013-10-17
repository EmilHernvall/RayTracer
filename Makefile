CPP = g++
OBJS = main.o vec3.o
DEBUG_FLAGS = -g -DDEBUG
CFLAGS = -DXP_UNIX $(DEBUG_FLAGS)
LIBS = -lgd -lm

main : $(OBJS)
	$(CPP) $(DEBUG_FLAG) -o raytracer $(OBJS) $(LIBS)

main.o : main.cc
vec3.o : vec3.cc

clean :
	rm $(OBJS) raytracer
