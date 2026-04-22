# Shopping Management System Makefile
# C++17 compilation with modern standards

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS = 

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INCLUDE_DIR = include
TEST_DIR = tests

# Target executable (only built if main.cpp exists)
TARGET = $(BIN_DIR)/shopping_system
TEST_TARGET = $(BIN_DIR)/test_product

# Source files (exclude server.cpp from main app, exclude main.cpp from server)
ALL_SOURCES  = $(wildcard $(SRC_DIR)/*.cpp)
APP_SOURCES  = $(filter-out $(SRC_DIR)/server.cpp, $(ALL_SOURCES))
OBJECTS      = $(APP_SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Library objects (no main.cpp, no server.cpp, no customer_server.cpp) — shared by tests and server
LIB_SOURCES  = $(filter-out $(SRC_DIR)/main.cpp $(SRC_DIR)/server.cpp $(SRC_DIR)/customer_server.cpp, $(ALL_SOURCES))
LIB_OBJECTS  = $(LIB_SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Server objects
SERVER_TARGET = $(BIN_DIR)/shopping_server

# Default target: build library objects (main.cpp may not exist yet)
all: directories $(LIB_OBJECTS)
	@echo "Library objects built successfully."

# Create necessary directories
directories:
	-if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	-if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	-if not exist data mkdir data
	-if not exist data\receipts mkdir data\receipts

# Link full executable (requires main.cpp)
app: directories $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build the REST API server (links all lib objects + server.cpp)
server: directories $(LIB_OBJECTS) $(OBJ_DIR)/server.o
	$(CXX) $(LIB_OBJECTS) $(OBJ_DIR)/server.o -o $(SERVER_TARGET) $(LDFLAGS) -lws2_32
	@echo "Server build complete: $(SERVER_TARGET)"

$(OBJ_DIR)/server.o: $(SRC_DIR)/server.cpp
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/server.cpp -o $(OBJ_DIR)/server.o
# Start both servers in background then launch both frontend dev servers
run-all: server customer_server
	start "" $(SERVER_TARGET)
	start "" $(CUSTOMER_SERVER_TARGET)
	start "" cmd /c "cd frontend && npm run dev"
	cd customer-frontend && npm run dev

# Clean build artifacts
clean:
	-del /Q $(OBJ_DIR)\*.o 2>nul
	-del /Q $(BIN_DIR)\*.exe 2>nul
	@echo "Clean complete"

# Clean everything including directories
distclean:
	-rmdir /S /Q $(OBJ_DIR) 2>nul
	-rmdir /S /Q $(BIN_DIR) 2>nul

# Rebuild from scratch
rebuild: clean all

# Run the application
run: app
	$(TARGET)

# Test targets
test: directories $(TEST_TARGET)
	$(TEST_TARGET)

$(TEST_TARGET): $(LIB_OBJECTS) $(TEST_DIR)/test_product_simple.cpp
	$(CXX) $(CXXFLAGS) $(LIB_OBJECTS) $(TEST_DIR)/test_product_simple.cpp -o $(TEST_TARGET)
	@echo "Test build complete: $(TEST_TARGET)"

TEST_ORDER_TARGET = $(BIN_DIR)/test_order

test_order: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_OBJECTS) $(TEST_DIR)/test_order_simple.cpp -o $(TEST_ORDER_TARGET)
	$(TEST_ORDER_TARGET)

TEST_ORDER_HISTORY_TARGET = $(BIN_DIR)/test_order_history

test_order_history: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_OBJECTS) $(TEST_DIR)/test_order_history_simple.cpp -o $(TEST_ORDER_HISTORY_TARGET)
	$(TEST_ORDER_HISTORY_TARGET)

TEST_RECEIPT_TARGET = $(BIN_DIR)/test_receipt

test_receipt: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_OBJECTS) $(TEST_DIR)/test_receipt_simple.cpp -o $(TEST_RECEIPT_TARGET)
	$(TEST_RECEIPT_TARGET)

TEST_PAYMENT_TARGET = $(BIN_DIR)/test_payment

test_payment: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_OBJECTS) $(TEST_DIR)/test_payment_simple.cpp -o $(TEST_PAYMENT_TARGET)
	$(TEST_PAYMENT_TARGET)

TEST_BILL_TARGET = $(BIN_DIR)/test_bill

test_bill: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_OBJECTS) $(TEST_DIR)/test_bill_simple.cpp -o $(TEST_BILL_TARGET)
	$(TEST_BILL_TARGET)

TEST_COUPON_TARGET = $(BIN_DIR)/test_coupon

test_coupon: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_OBJECTS) $(TEST_DIR)/test_coupon_simple.cpp -o $(TEST_COUPON_TARGET)
	$(TEST_COUPON_TARGET)

TEST_CART_TARGET = $(BIN_DIR)/test_cart

test_cart: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_OBJECTS) $(TEST_DIR)/test_cart_operations_simple.cpp -o $(TEST_CART_TARGET)
	$(TEST_CART_TARGET)

TEST_INVENTORY_TARGET = $(BIN_DIR)/test_inventory

test_inventory: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_OBJECTS) $(TEST_DIR)/test_inventory_operations_simple.cpp -o $(TEST_INVENTORY_TARGET)
	$(TEST_INVENTORY_TARGET)

TEST_ADMIN_TARGET = $(BIN_DIR)/test_admin

test_admin: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_OBJECTS) $(TEST_DIR)/test_admin_simple.cpp -o $(TEST_ADMIN_TARGET)
	$(TEST_ADMIN_TARGET)

TEST_UI_TARGET = $(BIN_DIR)/test_ui

test_ui: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_OBJECTS) $(TEST_DIR)/test_ui_simple.cpp -o $(TEST_UI_TARGET)
	$(TEST_UI_TARGET)

TEST_INTEGRATION_TARGET = $(BIN_DIR)/test_integration

test_integration: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_OBJECTS) $(TEST_DIR)/test_integration.cpp -o $(TEST_INTEGRATION_TARGET)
	$(TEST_INTEGRATION_TARGET)

TEST_PRODUCT_FACTORY_TARGET = $(BIN_DIR)/test_product_factory

test_product_factory: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_OBJECTS) $(TEST_DIR)/test_product_factory.cpp -o $(TEST_PRODUCT_FACTORY_TARGET)
	$(TEST_PRODUCT_FACTORY_TARGET)

TEST_INVENTORY_DISPLAY_TARGET = $(BIN_DIR)/test_inventory_display

test_inventory_display: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_OBJECTS) $(TEST_DIR)/test_inventory_display.cpp -o $(TEST_INVENTORY_DISPLAY_TARGET)
	$(TEST_INVENTORY_DISPLAY_TARGET)

TEST_SAMPLE_DATA_TARGET = $(BIN_DIR)/test_sample_data

test_sample_data: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_OBJECTS) $(TEST_DIR)/test_sample_data.cpp -o $(TEST_SAMPLE_DATA_TARGET)
	$(TEST_SAMPLE_DATA_TARGET)

TEST_UNIT_CASES_TARGET = $(BIN_DIR)/test_unit_cases

test_unit_cases: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_OBJECTS) $(TEST_DIR)/test_unit_cases.cpp -o $(TEST_UNIT_CASES_TARGET)
	$(TEST_UNIT_CASES_TARGET)

TEST_TRANSACTION_TARGET = $(BIN_DIR)/test_transaction

test_transaction: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_OBJECTS) $(TEST_DIR)/test_transaction_simple.cpp -o $(TEST_TRANSACTION_TARGET)
	$(TEST_TRANSACTION_TARGET)

TEST_EXCEPTIONS_TARGET = $(BIN_DIR)/test_exceptions

test_exceptions: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_OBJECTS) $(TEST_DIR)/test_exceptions.cpp -o $(TEST_EXCEPTIONS_TARGET)
	$(TEST_EXCEPTIONS_TARGET)

TEST_UTILITY_TARGET = $(BIN_DIR)/test_utility

test_utility: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_OBJECTS) $(TEST_DIR)/test_utility.cpp -o $(TEST_UTILITY_TARGET)
	$(TEST_UTILITY_TARGET)

TEST_EDGE_CASES_TARGET = $(BIN_DIR)/test_edge_cases

test_edge_cases: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_OBJECTS) $(TEST_DIR)/test_edge_cases.cpp -o $(TEST_EDGE_CASES_TARGET)
	$(TEST_EDGE_CASES_TARGET)

TEST_NEGATIVE_CASES_TARGET = $(BIN_DIR)/test_negative_cases

test_negative_cases: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_OBJECTS) $(TEST_DIR)/test_negative_cases.cpp -o $(TEST_NEGATIVE_CASES_TARGET)
	$(TEST_NEGATIVE_CASES_TARGET)

TEST_BOUNDARY_CASES_TARGET = $(BIN_DIR)/test_boundary_cases

test_boundary_cases: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_OBJECTS) $(TEST_DIR)/test_boundary_cases.cpp -o $(TEST_BOUNDARY_CASES_TARGET)
	$(TEST_BOUNDARY_CASES_TARGET)

# Run all simple (non-PBT) tests
test_all: test test_order test_order_history test_receipt test_payment test_bill test_coupon test_cart test_inventory test_admin test_ui test_integration test_product_factory test_inventory_display test_sample_data test_unit_cases test_transaction test_exceptions test_utility test_edge_cases test_negative_cases test_boundary_cases
	@echo "All simple tests completed."

# PBT test targets (require RapidCheck)
RC_INC = -Irapidcheck/include
RC_LIB = obj/librapidcheck.a

TEST_PRODUCT_PBT = $(BIN_DIR)/test_product_pbt
test_product_pbt: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(RC_INC) $(LIB_OBJECTS) $(TEST_DIR)/test_product.cpp $(RC_LIB) -o $(TEST_PRODUCT_PBT)
	$(TEST_PRODUCT_PBT)

TEST_BILL_PBT = $(BIN_DIR)/test_bill_pbt
test_bill_pbt: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(RC_INC) $(LIB_OBJECTS) $(TEST_DIR)/test_bill.cpp $(RC_LIB) -o $(TEST_BILL_PBT)
	$(TEST_BILL_PBT)

TEST_COUPON_PBT = $(BIN_DIR)/test_coupon_pbt
test_coupon_pbt: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(RC_INC) $(LIB_OBJECTS) $(TEST_DIR)/test_coupon.cpp $(RC_LIB) -o $(TEST_COUPON_PBT)
	$(TEST_COUPON_PBT)

TEST_CART_PBT = $(BIN_DIR)/test_cart_pbt
test_cart_pbt: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(RC_INC) $(LIB_OBJECTS) $(TEST_DIR)/test_cart_operations.cpp $(RC_LIB) -o $(TEST_CART_PBT)
	$(TEST_CART_PBT)

TEST_INVENTORY_PBT = $(BIN_DIR)/test_inventory_pbt
test_inventory_pbt: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(RC_INC) $(LIB_OBJECTS) $(TEST_DIR)/test_inventory_operations.cpp $(RC_LIB) -o $(TEST_INVENTORY_PBT)
	$(TEST_INVENTORY_PBT)

TEST_PAYMENT_PBT = $(BIN_DIR)/test_payment_pbt
test_payment_pbt: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(RC_INC) $(LIB_OBJECTS) $(TEST_DIR)/test_payment.cpp $(RC_LIB) -o $(TEST_PAYMENT_PBT)
	$(TEST_PAYMENT_PBT)

# Run all PBT tests
test_pbt: test_product_pbt test_bill_pbt test_coupon_pbt test_cart_pbt test_inventory_pbt test_payment_pbt
	@echo "All PBT tests completed."

# Run ALL tests (simple + PBT)
tests: test_all test_pbt
	@echo "All tests (simple + PBT) completed successfully."

TEST_CUSTOMER_ENDPOINTS_TARGET = $(BIN_DIR)/test_customer_endpoints

test_customer_endpoints: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_OBJECTS) $(TEST_DIR)/test_customer_endpoints_simple.cpp -o $(TEST_CUSTOMER_ENDPOINTS_TARGET)
	$(TEST_CUSTOMER_ENDPOINTS_TARGET)

TEST_BILL_NO_LOYALTY_PBT = $(BIN_DIR)/test_bill_no_loyalty_pbt
test_bill_no_loyalty_pbt: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(RC_INC) $(LIB_OBJECTS) $(TEST_DIR)/test_bill_no_loyalty_pbt.cpp $(RC_LIB) -o $(TEST_BILL_NO_LOYALTY_PBT)
	$(TEST_BILL_NO_LOYALTY_PBT)

TEST_RANKED_CUSTOMER_PBT = $(BIN_DIR)/test_ranked_customer_pbt
test_ranked_customer_pbt: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(RC_INC) $(LIB_OBJECTS) $(TEST_DIR)/test_ranked_customer_pbt.cpp $(RC_LIB) -o $(TEST_RANKED_CUSTOMER_PBT)
	$(TEST_RANKED_CUSTOMER_PBT)

TEST_CUSTOMER_SERVER_TARGET = $(BIN_DIR)/test_customer_server

test_customer_server: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_OBJECTS) $(TEST_DIR)/test_customer_server_simple.cpp -o $(TEST_CUSTOMER_SERVER_TARGET) $(LDFLAGS)
	$(TEST_CUSTOMER_SERVER_TARGET)

# Build customer server binary
CUSTOMER_SERVER_TARGET = $(BIN_DIR)/customer_server
$(OBJ_DIR)/customer_server.o: $(SRC_DIR)/customer_server.cpp
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/customer_server.cpp -o $(OBJ_DIR)/customer_server.o

customer_server: directories $(LIB_OBJECTS) $(OBJ_DIR)/customer_server.o
	$(CXX) $(LIB_OBJECTS) $(OBJ_DIR)/customer_server.o -o $(CUSTOMER_SERVER_TARGET) $(LDFLAGS) -lws2_32
	@echo "Customer server build complete: $(CUSTOMER_SERVER_TARGET)"

TEST_DATA_ISOLATION_PBT = $(BIN_DIR)/test_data_isolation_pbt
test_data_isolation_pbt: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(RC_INC) $(LIB_OBJECTS) $(TEST_DIR)/test_data_isolation_pbt.cpp $(RC_LIB) -o $(TEST_DATA_ISOLATION_PBT)
	$(TEST_DATA_ISOLATION_PBT)

TEST_CUSTOMER_STATS_PBT = $(BIN_DIR)/test_customer_stats_pbt
test_customer_stats_pbt: directories $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(RC_INC) $(LIB_OBJECTS) $(TEST_DIR)/test_customer_stats_pbt.cpp $(RC_LIB) -o $(TEST_CUSTOMER_STATS_PBT)
	$(TEST_CUSTOMER_STATS_PBT)

.PHONY: all app server customer_server run-all clean distclean rebuild run directories test test_order test_order_history test_receipt test_payment test_bill test_coupon test_cart test_inventory test_admin test_ui test_integration test_product_factory test_inventory_display test_sample_data test_unit_cases test_transaction test_exceptions test_utility test_edge_cases test_negative_cases test_boundary_cases test_all test_product_pbt test_bill_pbt test_coupon_pbt test_cart_pbt test_inventory_pbt test_payment_pbt test_pbt tests test_customer_endpoints test_bill_no_loyalty_pbt test_ranked_customer_pbt test_customer_server test_data_isolation_pbt test_customer_stats_pbt
