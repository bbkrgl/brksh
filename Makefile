all:
	gcc -O3 src/main.c -o brksh
clean:
	rm -rf brksh
