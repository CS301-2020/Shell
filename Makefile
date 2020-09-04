
all: build

build: binaries
  @echo "Building"
  gcc shell.c -o shell

clean:
  @echo "Cleaning"
  rm shell
