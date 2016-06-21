CC=g++
CFLAGS= -c -g -I.
SOURCES= main.cpp assembly_reader.cpp settings.cpp extract_relevant_code.cpp insn_queue.cpp event_generator.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=tool

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *o tool
