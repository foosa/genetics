bin_PROGRAMS = evolve
evolve_SOURCES = evolution.cpp \
				 evolution.h \
				 information.cpp \
				 information.h \
				 main.cpp \
				 parameters.h
evolve_CXXFLAGS = $(BOOST_CPPFLAGS)
evolve_LDADD = $(BOOST_LDFLAGS) \
			   $(BOOST_SERIALIZATION_LIB) \
			   $(BOOST_PROGRAM_OPTIONS_LIB)

# Library just for testing
noinst_LIBRARIES = libevolve.a
libevolve_a_SOURCES = evolution.cpp \
					  evolution.h \
					  information.cpp \
					  information.h \
					  parameters.h

