project_DB.out : main.o
	gcc -o project_DB.out main.c -lsqlite3
	rm -f *.o

clean :
	rm -f *.o *.out

clean_DB :
	rm -f *.o *.out *.db