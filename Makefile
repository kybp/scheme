CPPFLAGS = -g -std=c++11 -Wall -Wextra
SRC_DIR = src
TEST_DIR = test

scheme: $(SRC_DIR)/repl.cc parser.o builtins.o eval.o
	$(CXX) $(CPPFLAGS) $^ -o $@

eval.o: $(SRC_DIR)/scheme_types.hh $(SRC_DIR)/eval.hh
	$(CXX) $(CPPFLAGS) -I$(SRC_DIR) -c $(SRC_DIR)/eval.cc

parser.o: $(SRC_DIR)/scheme_types.hh $(SRC_DIR)/parser.hh $(SRC_DIR)/parser.cc
	$(CXX) $(CPPFLAGS) -I$(SRC_DIR) -c $(SRC_DIR)/parser.cc

builtins.o: $(SRC_DIR)/scheme_types.hh $(SRC_DIR)/builtins.hh\
	    $(SRC_DIR)/eval.hh $(SRC_DIR)/builtins.cc
	$(CXX) $(CPPFLAGS) -I$(SRC_DIR) -c $(SRC_DIR)/builtins.cc

# start of gtest stuffa

GTEST_DIR = $(TEST_DIR)/googletest
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

# start of our tests

TESTS += parser_tests
parser_tests.o: $(TEST_DIR)/parser_tests.cc\
	        $(SRC_DIR)/parser.hh $(SRC_DIR)/scheme_types.hh\
	        $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) -I$(SRC_DIR) -c $(TEST_DIR)/parser_tests.cc

parser_tests: parser.o parser_tests.o gtest_main.a
	$(CXX) $(CPPFLAGS) -pthread $^ -o $@

TESTS += eval_tests
eval_tests.o: $(TEST_DIR)/eval_tests.cc $(SRC_DIR)/eval.hh\
	      $(SRC_DIR)/scheme_types.hh\
	      $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) -I$(SRC_DIR) -c $(TEST_DIR)/eval_tests.cc

eval_tests: parser.o builtins.o eval.o eval_tests.o gtest_main.a
	$(CXX) $(CPPFLAGS) -pthread $^ -o $@

TESTS += printer_tests
printer_tests.o: $(TEST_DIR)/printer_tests.cc $(SRC_DIR)/scheme_types.hh\
	         $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) -I$(SRC_DIR) -c $(TEST_DIR)/printer_tests.cc

printer_tests: parser.o printer_tests.o gtest_main.a
	$(CXX) $(CPPFLAGS) -pthread $^ -o $@

TESTS += builtin_tests
builtin_tests.o: $(TEST_DIR)/builtin_tests.cc $(SRC_DIR)/scheme_types.hh\
	         $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) -I$(SRC_DIR) -c $(TEST_DIR)/builtin_tests.cc

builtin_tests: eval.o parser.o builtins.o builtin_tests.o gtest_main.a
	$(CXX) $(CPPFLAGS) -pthread $^ -o $@

tests: $(TESTS)

clean:
	rm -f $(TESTS) gtest.a gtest_main.a *.o scheme
