#
# $Id: 98b35556e53ee67aa5787bbf8af2bf5f27cb1cea $
#
AUTOMAKE_OPTIONS = foreign no-installinfo
ACLOCAL_AMFLAGS = -I m4

LIBS = -L${LOCALBASE}/lib -pthread -lssl -lcrypto
LIBS += -lboost_thread

LOCALBASE = /usr/local
#AM_CXXFLAGS= -pthread -I${LOCALBASE}/include -Wall -DASIO_STANDALONE
AM_CXXFLAGS= -pthread -I${LOCALBASE}/include -Wall
#AM_CPPFLAGS = -Wall -Wextra

bin_PROGRAMS = srv

srv_SOURCES = \
	main.cpp \
	config.cpp \
	logger.cpp \
	keymap.cpp \
	cover.cpp \
	utils.cpp \
	server.cpp \
	connect.cpp \
	request.cpp \
	response.cpp \
	handler.cpp

noinst_HEADERS = \
	config.hpp \
	logger.hpp \
	keymap.hpp \
	utils.hpp \
	cover.hpp

clean-local:
	rm -f *~ *.pid *.log


#EOF
