CXX = clang++
CPPFLAGS = -g -std=c++11 -Wall -Wextra
SRC_DIR = src
TEST_DIR = test

TESTS = parser_tests

tests: $(TESTS)

clean:
	rm -f $(TESTS) gtest.a gtest_main.a *.o

# start of gtest stuffa

GTEST_DIR = googletest
CPPFLAGS += -isystem $(GTEST_DIR)/include

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

# Builds gtest.a and gtest_main.a.

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) -c \
            $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) -c \
            $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

# End of gtest stuff

scheme.o: $(SRC_DIR)/scheme.cc $(SRC_DIR)/scheme.hh
	$(CXX) $(CPPFLAGS) -c $(SRC_DIR)/scheme.cc

parser_tests.o: $(TEST_DIR)/parser_tests.cc $(SRC_DIR)/scheme.hh\
	        $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) -c $(TEST_DIR)/parser_tests.cc

parser_tests: scheme.o parser_tests.o gtest_main.a
	$(CXX) $(CPPFLAGS) $^ -o $@
