build:
	gcc -O3 -o main main.c -lSDL2 -lGL -lGLEW

run: build
	./main

wasm:
	emcc main.c -o main.html -s WASM=1 -s USE_SDL=2

server:
	python -c 'import SimpleHTTPServer; SimpleHTTPServer.test()'
