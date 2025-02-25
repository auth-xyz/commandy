BIN=commandy
FLAGS=
LDFLAGS=-Wl,-Bstatic -lclicky -Wl,-Bdynamic -lcurl

all:
	g++ $(FLAGS) libs/cmdy.hpp src/main.cpp -o $(BIN) $(LDFLAGS)

clean:
	rm -f $(BIN)

install:
	cp $(BIN) /usr/local/bin/

remove:
	rm -f /usr/local/bin/$(BIN)

.PHONY: all clean install remove

