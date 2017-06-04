BINARYDIR := bin
BINARY := $(BINARYDIR)/main
WASMBIN := $(BINARYDIR)/main.html
CC := gcc
WASMCC := emcc
CFLAGS := -O3
LDFLAGS := -lSDL2 -lGL -lGLEW
WASMFLAGS := -s WASM=1 -s USE_SDL=2

SOURCEDIR := src
BUILDDIR := obj
WASMDIR := wasm

SOURCES := $(shell find $(SOURCEDIR) -name '*.c' | cut -d '/' -f2-)

# Get list of object files, with paths
OBJECTS := $(addprefix $(BUILDDIR)/,$(SOURCES:%.c=%.o))
WASMOBJ := $(addprefix $(WASMDIR)/,$(SOURCES:%.c=%.bc))

build: $(BINARY)

wasm: $(WASMBIN)

run: build
	$(BINARY)

all: build wasm

clean:
	-rm -rf $(BUILDDIR)
	-rm -rf $(WASMDIR)
	-rm -f $(BINARY)

$(BINARY): $(OBJECTS)
	@mkdir -p $(BINARYDIR)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(BINARY) $(LDFLAGS) 

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(WASMBIN): $(WASMOBJ)
	@mkdir -p $(BINARYDIR)
	$(WASMCC) $(CFLAGS) $(WASMOBJ) -o $(WASMBIN) $(LDFLAGS) $(WASMFLAGS)

$(WASMDIR)/%.bc: $(SOURCEDIR)/%.c
	@mkdir -p $(dir $@)
	$(WASMCC) $(CFLAGS) -c $< -o $@ $(WASMFLAGS)

gh-pages: wasm
	git reset --hard HEAD^
	cp $(BINARYDIR)/main.html ./index.html
	cp $(BINARYDIR)/main.js ./
	cp $(BINARYDIR)/main.wasm ./
	git add -f index.html main.js main.wasm
	git commit -m "Build gh-pages demo"
	git push -f origin gh-pages

server:
	python -c 'import SimpleHTTPServer; SimpleHTTPServer.test()'
