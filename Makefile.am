#
# $Id: 98b35556e53ee67aa5787bbf8af2bf5f27cb1cea $
#
AUTOMAKE_OPTIONS = foreign no-installinfo
ACLOCAL_AMFLAGS = -I m4

LIBS = -L${LOCALBASE}/lib -pthread -lreflex

LOCALBASE = /usr/local
AM_CXXFLAGS= -pthread -I${LOCALBASE}/include -Wall

bin_PROGRAMS = srv6

srv6_SOURCES = \
	manager.cpp \
	connection.cpp \
	main.cpp \
	server.cpp \
	requestll.cpp \
	requestyy.cpp \
	requestcc.cpp \
	keymap.cpp

requestll.cpp: request.ll requestyy.hpp
	reflex -o $@ $<

requestyy.hpp: requestyy.cpp

requestyy.cpp: request.yy
	${BISON} -o $@ $<

clean-local:
	rm -f *~ \
	requestll.cpp \
	requestll.hpp \
	requestyy.cpp \
	requestyy.hpp \
	requestloc.hpp


#EOF
