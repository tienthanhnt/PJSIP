# If your application is in a file named myapp.cpp or myapp.c
# this is the line you will need to build the binary.
all: app

app: main.c
	$(CC) -o $@ read_config.c $< `pkg-config --cflags --libs libpjproject` -L/usr/bin/lib -lbcg729

clean:
	rm -f app.o app
