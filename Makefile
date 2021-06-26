#Makefile
TARGET=DeadlockDetection
CXX=gcc
CXXFLAGS=-Wall
OBJECTS=detectDeadlock.o
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)
detectDeadlock.o: detectDeadlock.c
	$(CXX) $(CXXFLAGS) -c detectDeadlock.c
clean:
	rm $(OBJECTS) $(TARGET)

