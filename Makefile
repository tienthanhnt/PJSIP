# If your application is in a file named myapp.cpp or myapp.c
# this is the line you will need to build the binary.
all: myapp

myapp: myapp.c
	$(CC) -o $@ read_config.c $< `pkg-config --cflags --libs libpjproject`

clean:
	rm -f myapp.o myapp
