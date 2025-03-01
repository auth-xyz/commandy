BIN=commandy
LDFLAGS=-Wl,-Bstatic -lclicky -Wl,-Bdynamic -lcurl

all:
	g++ libs/cmdy.hpp src/main.cpp -o $(BIN) $(LDFLAGS)

clean:
	rm -f $(BIN)

install:
	cp $(BIN) ~/.local/bin/ 

remove:
	rm -f ~/.local/bin/$(BIN)

.PHONY: all clean install remove

