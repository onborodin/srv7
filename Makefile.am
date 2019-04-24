#
# $Id: 98b35556e53ee67aa5787bbf8af2bf5f27cb1cea $
#
AUTOMAKE_OPTIONS = foreign no-installinfo
ACLOCAL_AMFLAGS = -I m4

LIBS = -L${LOCALBASE}/lib -pthread

LOCALBASE = /usr/local
AM_CXXFLAGS= -pthread -I${LOCALBASE}/include -Wall

bin_PROGRAMS = srv7

srv7_SOURCES = \
	main.cpp \
	connection.cpp \
	server.cpp

clean-local:
	rm -f *~


#EOF
