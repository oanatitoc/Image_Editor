# Copyright Oana-Alexandra Titoc 313CAb 2022-2023


# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -std=c99

# define targets
TARGETS=image_editor

build: $(TARGETS)

image_editor: image_editor.c
	$(CC) $(CFLAGS) image_editor.c -o image_editor -lm


pack:
	zip -FSr 313CA_TitocOana-Alexandra_Tema3.zip README Makefile *.c *.h

clean:
	rm -f *.o $(TARGETS)

.PHONY: pack clean

