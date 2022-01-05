all: libConverters.a

ConverterBase.o: ConverterBase.cpp ConverterBase.h
	c++ -I../ -c ConverterBase.cpp

REG1K0100G.o: REG1K0100G.cpp REG1K0100G.h ConverterBase.o
	c++ -I../ -c REG1K0100G.cpp

BEG75050.o: BEG75050.cpp BEG75050.h ConverterBase.o
	c++ -I../ -c BEG75050.cpp

CEG1K0100G.o: CEG1K0100G.cpp CEG1K0100G.h ConverterBase.o
	c++ -I../ -c CEG1K0100G.cpp

UXR100030.o: UXR100030.cpp UXR100030.h ConverterBase.o
	c++ -I../ -c UXR100030.cpp

libConverters.a: ConverterBase.o REG1K0100G.o BEG75050.o CEG1K0100G.o UXR100030.o
	ar rcs libConverters.a ConverterBase.o REG1K0100G.o BEG75050.o \
		CEG1K0100G.o UXR100030.o
# 	ar ru libConverters.a ConverterBase.o BEG75050.o CEG1K0100G.o UXR100030.o
# 	ranlib libConverters.a

clean:
	rm *.o *.a
