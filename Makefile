CXX := g++
AR := ar

OBJ_DIR := obj
SRC_DIR := src
HDR_DIR := include
LIB_DIR := lib
TEST_DIR := test
LIB := $(LIB_DIR)/libchloros.a

CXXFLAGS += -g -Wall -Wextra -std=c++14 -I$(HDR_DIR) -DDEBUG
TEST_CXXFLAGS := $(CXXFLAGS) -L$(LIB_DIR) -lchloros -pthread
ARFLAGS := -rs

CHLOROS_HDRS := $(wildcard $(HDR_DIR)/*.h)
CHLOROS_SRCS := chloros.cpp context_switch.S common.cpp
TEST_BINS := phase_1 phase_2 phase_3 phase_4 phase_extra_credit
CHLOROS_OBJS := $(addprefix $(OBJ_DIR)/,$(addsuffix .o,$(CHLOROS_SRCS)))
TEST_HDRS := $(wildcard $(TEST_DIR/*.h))

all: test $(LIB)

test: $(TEST_BINS)

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR)

submit:
	tar -czf submit.tar.gz $(SRC_DIR) $(HDR_DIR)
	@echo "Your submission submit.tar.gz was successfully created."

$(OBJ_DIR)/%.o: $(SRC_DIR)/% $(CHLOROS_HDRS) | $(OBJ_DIR)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(OBJ_DIR):
	@mkdir -p $@

$(LIB): $(CHLOROS_OBJS)
	@mkdir -p $(@D)
	$(AR) $(ARFLAGS) $@ $^

$(TEST_BINS): %: $(TEST_DIR)/%.cpp $(TEST_HDRS) $(CHLOROS_HDRS) $(LIB)
	$(CXX) $< -o $@ $(TEST_CXXFLAGS)

.PHONY: all clean test compress
