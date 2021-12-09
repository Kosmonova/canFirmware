all: libConverters.a

ConverterAbstract.o: ConverterAbstract.cpp ConverterAbstract.h
	c++ -c ConverterAbstract.cpp

BEG75050.o: BEG75050.cpp BEG75050.h ConverterAbstract.o
	c++ -c BEG75050.cpp

libConverters.a: ConverterAbstract.o BEG75050.o
	ar rcs libConverters.a ConverterAbstract.o BEG75050.o

clean:
	rm *.o *.a
