# Automatically Generated Makefile by EDE.
# For use with: make
#
# DO NOT MODIFY THIS FILE OR YOUR CHANGES MAY BE LOST.
# EDE is the Emacs Development Environment.
# http://cedet.sourceforge.net/ede.shtml
#

top="$(CURDIR)"/
ede_FILES=Project.ede Makefile

debug_SOURCES=main.cpp parser.cpp exception.cpp scanner.cpp syntax_node.cpp statement_base.cpp syntax_node_base.cpp sym_table.cpp generator.cpp statement.cpp
debug_OBJ= main.o parser.o exception.o scanner.o syntax_node.o statement_base.o syntax_node_base.o sym_table.o generator.o statement.o
CXX= g++ -g
CXX_COMPILE=$(CXX) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS)
CXX_DEPENDENCIES=-Wp,-MD,.deps/$(*F).P
CXX_LINK=$(CXX) $(CFLAGS) $(LDFLAGS) -L.
VERSION=1.0
DISTDIR=$(top)compiler-$(VERSION)
top_builddir = 

DEP_FILES=.deps/main.P .deps/parser.P .deps/exception.P .deps/scanner.P .deps/syntax_node.P .deps/statement_base.P .deps/syntax_node_base.P .deps/sym_table.P .deps/generator.P .deps/statement.P .deps/exception.P .deps/parser.P .deps/scanner.P .deps/syntax_node.P .deps/sym_table.P .deps/statement_base.P .deps/syntax_node_base.P .deps/generator.P .deps/statement.P .deps/asm_commands.P .deps/optimizer.P

all: debug

DEPS_MAGIC := $(shell mkdir .deps > /dev/null 2>&1 || :)
-include $(DEP_FILES)

%.o: %.cpp
	@echo '$(CXX_COMPILE) -c $<'; \
	$(CXX_COMPILE) $(CXX_DEPENDENCIES) -o $@ -c $<

debug: $(debug_OBJ)
	$(CXX_LINK) -o $@ $^ $(LDDEPS)

tags: 


clean:
	rm -f *.mod *.o *.obj .deps/*.P .lo

.PHONY: dist

dist:
	rm -rf $(DISTDIR)
	mkdir $(DISTDIR)
	cp $(debug_SOURCES) $(ede_FILES) $(DISTDIR)
	tar -cvzf $(DISTDIR).tar.gz $(DISTDIR)
	rm -rf $(DISTDIR)

Makefile: Project.ede
	@echo Makefile is out of date!  It needs to be regenerated by EDE.
	@echo If you have not modified Project.ede, you can use 'touch' to update the Makefile time stamp.
	@false



# End of Makefile
