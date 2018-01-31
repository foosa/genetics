srcDir = ../src
clusterDir = $(srcDir)/cluster
evolutionDir = $(srcDir)/evolution

check_PROGRAMS = testInformation testGenetics

testInformation_SOURCES = testInformation.cpp
testInformation_CXXFLAGS = $(gtest_CFLAGS) -I$(evolutionDir)
testInformation_LDADD = $(gtest_LIBS) -L$(evolutionDir) -levolve

testGenetics_SOURCES = testGenetics.cpp
testGenetics_CXXFLAGS = $(gtest_CFLAGS) -I$(evolutionDir)
testGenetics_LDADD = $(gtest_LIBS) -L$(evolutionDir) -levolve

subdirs = $(srcDir)
TESTS = $(check_PROGRAMS)
