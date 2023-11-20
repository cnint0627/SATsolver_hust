main : main.o  satSolve.o
	gcc main.o satSolve.o  -o main -fcommon
 
main.o : main.c SAT.h
	gcc main.c -c -o main.o -fcommon
 
satSolve.o : satSolve.c SAT.h
	gcc satSolve.c -c -o satSolve.o -fcommon
 
.PHONY : clean
clean:
	del *.o -rf
