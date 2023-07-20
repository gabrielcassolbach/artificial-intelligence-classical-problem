all:
	gcc -c caverna.c decisor.c game_state.c main-automatica.c 
	gcc *.o -Wall -o robo
	rm -f *.o
clean:
	rm -f robo *~ *.o
