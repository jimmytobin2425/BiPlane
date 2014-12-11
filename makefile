
CXX=g++
INCLUDES=
FLAGS=-D__MACOSX_CORE__ -c -std=c++11
LIBS=-framework CoreAudio -framework CoreMIDI -framework CoreFoundation \
	-framework IOKit -framework Carbon  -framework OpenGL \
	-framework GLUT -framework Foundation \
	-framework AppKit -lstdc++ -lm
SNDFLAGS=`pkg-config --cflags sndfile` -c -std=c++11
SNDLIBFLAGS=`pkg-config --libs sndfile`

OBJS=   RtAudio.o HRTFDatabase.o Recorder.o chuck_fft.o Plane.o

Plane: $(OBJS)
	$(CXX) -o Plane $(OBJS) $(LIBS) $(SNDLIBFLAGS)

RtAudio.o: RtAudio.h RtAudio.cpp RtError.h
	$(CXX) $(FLAGS) RtAudio.cpp

HRTFDatabase.o: HRTFDatabase.cpp HRTFDatabase.h
	$(CXX) $(SNDFLAGS) HRTFDatabase.cpp

Recorder.o: Recorder.cpp Recorder.h chuck_fft.h
	$(CXX) $(FLAGS) Recorder.cpp

chuck_fft.o: chuck_fft.h chuck_fft.c
	$(CXX) $(FLAGS) chuck_fft.c

Plane.o: Plane.cpp
	$(CXX) $(FLAGS) Plane.cpp

clean:
	rm -f *~ *# *.o Plane
