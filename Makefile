CXX=g++
CXXFLAGS=-Wall -O3
RM=rm -f

SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)
BIN=kana_to_romaji

LINK=$(CXX) $(CXXFLAGS) -o $@ $^
COMPILE=$(CXX) $(CXXFLAGS) -o $@ -c $<

all: $(BIN)

kana_to_romaji: kana_to_romaji.o romaji.o utf8.o
	$(LINK)

%.o: %.cpp
	$(COMPILE)

clean:
	$(RM) $(BIN) $(OBJ)

depend:
	sed -i '/^# DEPENDENCIES/q' Makefile
	$(CXX) -MM $(SRC) >> Makefile

test: kana_to_romaji
	python ./tests/run_test.py ./kana_to_romaji ./tests/tests.txt

# DEPENDENCIES
kana_to_romaji.o: kana_to_romaji.cpp romaji.h utf8.h
romaji.o: romaji.cpp romaji.h utf8.h
utf8.o: utf8.cpp utf8.h
