srcDir = ../src
clusterDir = $(srcDir)/cluster
evolutionDir = $(srcDir)/evolution

check_PROGRAMS = testInformation testGenetics testEcosystem

testInformation_SOURCES = testInformation.cpp
testInformation_CXXFLAGS = $(gtest_CFLAGS) -I$(evolutionDir) \
						   $(BOOST_CPPFLAGS)
testInformation_LDADD = $(gtest_LIBS) -L$(evolutionDir) \
						$(BOOST_LDFLAGS) $(BOOST_SERIALIZATION_LIB) \
						-levolve

testGenetics_SOURCES = testGenetics.cpp
testGenetics_CXXFLAGS = $(gtest_CFLAGS) -I$(evolutionDir) \
						$(NOOST_CPPFLAGS)
testGenetics_LDADD = $(gtest_LIBS) -L$(evolutionDir) \
					 $(BOOST_LDFLAGS) $(BOOST_SERIALIZATION_LIB) \
					 -levolve

testEcosystem_SOURCES = testEcosystem.cpp
testEcosystem_CXXFLAGS = $(gtest_CFLAGS) -I$(evolutionDir) \
						$(NOOST_CPPFLAGS)
testEcosystem_LDADD = $(gtest_LIBS) -L$(evolutionDir) \
					 $(BOOST_LDFLAGS) $(BOOST_SERIALIZATION_LIB) \
					 -levolve

subdirs = $(srcDir)
TESTS = $(check_PROGRAMS)
