CC = gcc
FLAGS = -Wall -Wextra -pedantic -std=c99

shell: parser.o prompt.o echo.o tilde.o path.o evariables.o bgjobs.o
	$(CC) $(FLAGS) -o shell parser.o prompt.o echo.o tilde.o path.o evariables.o bgjobs.o
parser.o: parser.c shell.h
	$(CC) $(FLAGS) -c parser.c
prompt.o: prompt.c shell.h
	$(CC) $(FLAGS) -c prompt.c
echo.o: echo.c shell.h
	$(CC) $(FLAGS) -c echo.c
tilde.o: tilde.c shell.h
	$(CC) $(FLAGS) -c tilde.c
path.o: path.c shell.h
	$(CC) $(FLAGS) -c path.c
evariables.o: evariables.c shell.h
	$(CC) $(FLAGS) -c evariables.c
bgjobs.o: bgjobs.c shell.h
	$(CC) $(FLAGS) -c bgjobs.c
clean:
	rm -rf *.o shell
