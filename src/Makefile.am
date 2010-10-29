AM_CXXFLAGS = -std=c++98 $(OPENMP_CXXFLAGS) -DUNICODE @AM_CXXFLAGS@

noinst_LIBRARIES = hubward.a

hubward_a_SOURCES = chunk.cpp chunk.hpp image.cpp image.hpp \
	intstring.cpp intstring.hpp level.cpp level.hpp nbt.cpp nbt.hpp \
	nbttags.cpp nbttags.hpp options.cpp options.hpp output.cpp output.hpp \
	pixel.cpp pixel.hpp pvector.cpp pvector.hpp \
	renderer.cpp renderer.hpp colours.cpp \
	render_contour.cpp render_contour.hpp

bin_PROGRAMS = hubwardcli
hubwardcli_SOURCES = cli.cpp
hubwardcli_LDADD = hubward.a
