bin_PROGRAMS = evolve
evolve_SOURCES = agent.cpp \
				 agent.h \
				 ecosystem.cpp \
				 ecosystem.h \
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
libevolve_a_SOURCES = agent.cpp \
					  agent.h \
					  ecosystem.cpp \
					  ecosystem.h \
					  information.cpp \
					  information.h \
					  parameters.h

