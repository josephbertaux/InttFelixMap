GXX = g++

HXX = hpp
CXX = cpp
OXX = o

INCS =
FLGS =
LIBS =

TRGT = main
OBJS = main.o felix_autogen.o

$(TRGT): $(OBJS)
	$(GXX) $(FLGS) $(INCS) -o $@ $^ $(LIBS)

%.$(OXX): %.$(CXX)
	$(GXX) $(FLGS) $(INCS) -c $< -o $@ $(LIBS)

.PHONY: clean
clean:
	rm -rf $(TRGT) $(OBJS)
