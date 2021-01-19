CC = gcc
FLAGS = -Wall -Wextra -pedantic -std=c99 -I.

shell.x: parser.o echo.o prompt.o tilde.o
	$(CC) $(FLAGS) -o shell parser.o echo.o prompt.o tilde.o
tilde.o: tilde.c
	$(CC) $(FLAGS) -c tilde.c
echo.o: echo.c
	$(CC) $(FLAGS) -c echo.c
prompt.o: prompt.c
	$(CC) $(FLAGS) -c prompt.c
parser.o: parser.c
	$(CC) $(FLAGS) -c parser.c
clean:
	rm -rf *.o shell
