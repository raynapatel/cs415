cc = gcc

sources = main.c command.c string_parser.c
headers = command.h string_parser.h
objects = $(sources:.c=.o)

flags = -g -std=c11 

target = psuedo-shell

all: $(target) 

$(target) : $(objects)
	$(cc) $(flags) -o $(target) $(objects)

%.o : %.c $(headers)
	$(cc) -c $(flags) $< -o $@

clean:
	rm -rf $(target) $(objects) 
