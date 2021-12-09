all: libConverters.a

ConverterAbstract.o: ConverterAbstract.cpp ConverterAbstract.h
	c++ -c ConverterAbstract.cpp

BEG75050.o: BEG75050.cpp BEG75050.h ConverterAbstract.o
	c++ -c BEG75050.cpp

CEG1K0100G.o: CEG1K0100G.cpp CEG1K0100G.h ConverterAbstract.o
	c++ -c CEG1K0100G.cpp

libConverters.a: ConverterAbstract.o BEG75050.o CEG1K0100G.o
	ar rcs libConverters.a ConverterAbstract.o BEG75050.o CEG1K0100G.o

clean:
	rm *.o *.a
