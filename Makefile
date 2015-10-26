# Jordan Lu (jjl4mb)
# 9/29/2015
# Makefile

#sudokuValidator was implemented in C++
CXX	= g++


main:	merge.cpp
	$(CXX) -pthread -o a.out merge.cpp

clean:
	/bin/rm -f *.o *~
