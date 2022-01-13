all: 
	echo "Making Weird Shell"
	gcc -Wall -Wextra -o main main.c

run:
	echo "Making Weird Shell"
	gcc -Wall -Wextra -o main main.c
	echo "Starting Weird Shell"
	./main

clean:
	rm -f main *.txt
	