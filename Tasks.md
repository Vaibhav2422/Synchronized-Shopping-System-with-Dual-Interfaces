# Implementation Plan: Shopping Management System

## Overview

This implementation plan breaks down the Shopping Management System into discrete, manageable coding tasks. The system will be built incrementally, starting with foundational classes and building up to the complete application. Each task builds on previous work, ensuring continuous integration and early validation.

The implementation follows the file structure defined in the design document with separate `include/` and `src/` directories. All code will use C++17 features including smart pointers, RAII principles, and modern STL containers.

## Tasks

- [x] 1. Set up project structure and build system
  - Create directory structure: `include/`, `src/`, `data/`, `obj/`, `bin/`
  - Create Makefile with C++17 compilation flags
  - Set up header guards template
  - Create .gitignore for build artifacts
  - _Requirements: 13.1, 13.2, 13.6_

- [x] 2. Implement exception hierarchy
  - [x] 2.1 Create base ShoppingException class with error codes and timestamps
    - Implement ShoppingException.h with errorCode, timestamp, and getDetailedMessage()
    - _Requirements: 21.1, 21.3_
  
  - [x] 2.2 Create InvalidInputException hierarchy
    - Implement InvalidProductIdException, InvalidQuantityException, InvalidMenuChoiceException
    - _Requirements: 11.1, 11.2, 21.1_
  
  - [x] 2.3 Create stock exception hierarchy
    - Implement OutOfStockException, ProductUnavailableException, InsufficientStockException
    - _Requirements: 3.6, 4.3, 17.4_
  
  - [x] 2.4 Create payment and coupon exception classes
    - Implement PaymentFailedException, InsufficientFundsException, InvalidCardException, PaymentTimeoutException
    - Implement InvalidCouponException, ExpiredCouponException, CouponNotApplicableException
    - _Requirements: 5.4, 21.1_
  
  - [x] 2.5 Create file I/O exception classes
    - Implement FileIOException, FileNotFoundException, FileWriteException, FileReadException
    - _Requirements: 21.1_

- [x] 3. Implement ANSIColors utility
  - [x] 3.1 Create ANSIColors.h with color constants
    - Define background colors (BG_BLACK, BG_RESET)
    - Define text colors (CYAN, YELLOW, GREEN, RED, WHITE, RESET)
    - Define text styles (BOLD, UNDERLINE)
    - _Requirements: 1.4, 10.1, 10.2, 10.3, 10.4, 10.5_
  
  - [x] 3.2 Implement ANSIColors utility functions
    - Implement clearScreen(), setCursorPosition(), hideCursor(), showCursor()
    - _Requirements: 22.1_

- [x] 4. Implement Product class hierarchy
  - [x] 4.1 Create Product base class
    - Define Product.h with protected members (productId, name, price, quantityAvailable)
    - Declare pure virtual functions: display(), calculateDiscount(), getCategory(), clone()
    - Implement getters and inventory management methods (decrementStock, incrementStock, isAvailable)
    - Implement Product.cpp with virtual destructor
    - _Requirements: 1.1, 1.5, 2.7, 12.1_
  
  - [x] 4.2 Write property test for Product base class
    - **Property 2: Product Display Completeness**
    - **Validates: Requirements 1.2**
  
  - [x] 4.3 Implement Electronics class
    - Create Electronics.h extending Product with brand and warrantyYears attributes
    - Implement display() with ANSI formatting showing brand and warranty
    - Implement calculateDiscount() returning 10% of price
    - Implement getCategory() returning "Electronics"
    - Implement clone() for deep copying
    - _Requirements: 2.1, 2.4_
  
  - [x] 4.4 Implement Clothing class
    - Create Clothing.h extending Product with size and fabric attributes
    - Implement display() with ANSI formatting showing size and fabric
    - Implement calculateDiscount() returning 20% of price
    - Implement getCategory() returning "Clothing"
    - Implement clone() for deep copying
    - _Requirements: 2.2, 2.5_
  
  - [x] 4.5 Implement Book class
    - Create Book.h extending Product with author and genre attributes
    - Implement display() with ANSI formatting showing author and genre
    - Implement calculateDiscount() returning 5% of price
    - Implement getCategory() returning "Books"
    - Implement clone() for deep copying
    - _Requirements: 2.3, 2.6_
  
  - [x] 4.6 Write property tests for category-specific discounts
    - **Property 1: Category-Specific Discount Rates**
    - **Validates: Requirements 2.4, 2.5, 2.6**

- [x] 5. Implement ProductFactory pattern
  - [x] 5.1 Create ProductFactory class
    - Define ProductType enum (ELECTRONICS, CLOTHING, BOOK)
    - Implement createProduct() with type and attributes map
    - Implement convenience methods: createElectronics(), createClothing(), createBook()
    - _Requirements: 1.1, 2.1, 2.2, 2.3_

- [-] 6. Implement Inventory class
  - [x] 6.1 Create Inventory class with product storage
    - Define Inventory.h with vector<unique_ptr<Product>> products
    - Implement addProduct(), removeProduct(), findProduct()
    - Implement updatePrice(), updateStock()
    - Implement searchByName() with case-insensitive partial matching
    - Implement filterByCategory()
    - _Requirements: 4.5, 16.1, 16.2, 16.3, 16.4_
  
  - [x] 6.2 Implement inventory display methods
    - Implement displayAllProducts() with category grouping
    - Implement displayByCategory()
    - Use ANSI colors for formatting (cyan text, yellow headings)
    - _Requirements: 1.3, 1.4, 4.4_
  
  - [x] 6.3 Implement sample data initialization
    - Create initializeSampleData() method
    - Add 3+ Electronics products (Laptop, Smartphone, Headphones)
    - Add 3+ Clothing products (T-Shirt, Jeans, Jacket)
    - Add 3+ Book products (C++ Programming, Clean Code, Pragmatic Programmer)
    - Assign unique IDs and realistic prices/quantities
    - _Requirements: 14.1, 14.2, 14.3, 14.4, 14.5_
  
  - [x] 6.4 Write property tests for inventory operations
    - **Property 28: Unique Product IDs**
    - **Property 26: Product Search Partial Matching**
    - **Property 27: Category Filter Exactness**
    - **Validates: Requirements 14.4, 16.2, 16.4, 16.6_

- [x] 7. Implement Cart class
  - [x] 7.1 Create CartItem structure and Cart class
    - Define CartItem struct with unique_ptr<Product> and quantity
    - Implement move constructor and move assignment for CartItem
    - Define Cart.h with vector<CartItem> items and Inventory* pointer
    - Implement Cart constructor accepting Inventory pointer
    - _Requirements: 3.1, 17.1_
  
  - [x] 7.2 Implement cart add/remove operations
    - Implement addProduct(productId, quantity) with inventory validation
    - Check product availability and sufficient stock
    - Clone product from inventory and add to cart
    - Decrement inventory stock by quantity
    - Handle duplicate products by incrementing quantity
    - Implement removeProduct(productId) restoring inventory stock
    - Implement clear() method
    - _Requirements: 3.2, 3.3, 3.4, 3.5, 4.1, 4.2, 17.2, 17.4, 17.5, 17.6_
  
  - [x] 7.3 Implement cart calculation methods
    - Implement calculateSubtotal() summing all product prices × quantities
    - Implement calculateProductDiscounts() summing category discounts × quantities
    - Implement calculateCartDiscount() returning 5% if subtotal > 5000
    - Implement isEmpty() and getItemCount()
    - _Requirements: 6.1, 6.4, 7.1, 7.2_
  
  - [x] 7.4 Implement cart display
    - Implement displayCart() showing all items with quantities and line totals
    - Use ANSI colors for formatting
    - _Requirements: 3.7, 17.3_
  
  - [x] 7.5 Write property tests for cart operations
    - **Property 4: Cart-Inventory Synchronization on Add**
    - **Property 5: Cart-Inventory Round Trip**
    - **Property 6: Product ID Validation**
    - **Property 7: Out-of-Stock Prevention**
    - **Property 8: Insufficient Stock Prevention**
    - **Property 9: Cart Display Completeness**
    - **Property 10: Quantity Increment on Duplicate Add**
    - **Property 13: Subtotal Calculation**
    - **Property 14: Product Discount Summation**
    - **Validates: Requirements 3.2, 3.3, 3.4, 3.5, 3.7, 4.1, 4.2, 4.3, 7.1, 7.2, 11.1, 17.2, 17.3, 17.4, 17.5, 17.6_

- [x] 8. Checkpoint - Ensure core product and cart functionality works
  - Ensure all tests pass, ask the user if questions arise.

- [x] 9. Implement Coupon class
  - [x] 9.1 Create Coupon class with validation
    - Define Coupon.h with code, discountPercentage, and VALID_COUPONS map
    - Implement apply(couponCode) validating against SAVE10 (10%) and FESTIVE20 (20%)
    - Implement calculateDiscount(amount) applying percentage
    - Implement isApplied(), getCode(), getDiscountPercentage(), reset()
    - _Requirements: 5.1, 5.2, 5.3, 5.4, 5.5_
  
  - [x] 9.2 Write property tests for coupon validation
    - **Property 18: Invalid Coupon Rejection**
    - **Property 19: Single Coupon Constraint**
    - **Validates: Requirements 5.4, 5.5_

- [x] 10. Implement Bill class
  - [x] 10.1 Create Bill class with discount calculation
    - Define Bill.h with subtotal, discounts, gstAmount, finalTotal, couponCode
    - Define constants: GST_RATE (0.18), CART_DISCOUNT_THRESHOLD (5000.0), CART_DISCOUNT_RATE (0.05)
    - Implement calculate(cart, coupon) computing all discounts in correct order
    - Calculate: subtotal → product discounts → cart discount → coupon discount → GST → final total
    - _Requirements: 6.1, 6.2, 6.3, 6.4, 7.1, 7.2, 7.3, 7.4, 7.5, 7.6_
  
  - [x] 10.2 Implement bill display
    - Implement display() showing complete breakdown with ANSI formatting
    - Use yellow for monetary values, clear section separators
    - Implement getSummary() for receipt generation
    - _Requirements: 7.7, 7.8_
  
  - [x] 10.3 Write property tests for bill calculations
    - **Property 11: Cart Discount Threshold**
    - **Property 12: Discount Calculation Order**
    - **Property 15: GST Calculation**
    - **Property 16: Final Total Calculation**
    - **Property 17: Bill Display Completeness**
    - **Validates: Requirements 6.1, 6.2, 6.3, 6.4, 7.1, 7.2, 7.3, 7.4, 7.5, 7.6_

- [x] 11. Implement Payment class hierarchy
  - [x] 11.1 Create Payment base class
    - Define Payment.h with protected amount member
    - Declare pure virtual pay() and getPaymentMethod()
    - Implement getAmount() getter
    - _Requirements: 8.2_
  
  - [x] 11.2 Implement UPIPayment class
    - Create UPIPayment.h extending Payment with upiId attribute
    - Implement pay() displaying UPI ID and simulating payment
    - Implement getPaymentMethod() returning "UPI"
    - _Requirements: 8.1, 8.3_
  
  - [x] 11.3 Implement CardPayment class
    - Create CardPayment.h extending Payment with cardNumber and cardHolderName
    - Implement maskCardNumber() showing only last 4 digits
    - Implement pay() displaying masked card and simulating authorization
    - Implement getPaymentMethod() returning "Card"
    - _Requirements: 8.1, 8.4_
  
  - [x] 11.4 Implement CashPayment class
    - Create CashPayment.h extending Payment with cashTendered attribute
    - Implement pay() verifying sufficient cash and calculating change
    - Implement calculateChange() returning difference
    - Implement getPaymentMethod() returning "Cash"
    - Throw InsufficientFundsException if cash < amount
    - _Requirements: 8.1, 8.5_
  
  - [x] 11.5 Write property tests for payment processing
    - **Property 20: Polymorphic Payment Processing**
    - **Validates: Requirements 8.6_

- [x] 12. Implement Order class
  - [x] 12.1 Create Order class with transaction details
    - Define Order.h with orderId, products vector, totalAmount, paymentMethod, orderDate, orderTime
    - Implement static nextOrderId counter
    - Implement constructor accepting Cart, total, and payment method
    - Extract product names and quantities from cart
    - Generate current date and time using chrono
    - _Requirements: 15.1, 15.2_
  
  - [x] 12.2 Implement order display and serialization
    - Implement display() showing order details with ANSI formatting
    - Implement serialize() converting order to pipe-delimited string format
    - Implement static deserialize(data) parsing string back to Order
    - _Requirements: 15.4_
  
  - [x] 12.3 Write property test for order creation
    - **Property 24: Order Creation on Checkout**
    - **Validates: Requirements 15.2, 15.3_

- [x] 13. Implement OrderHistory class
  - [x] 13.1 Create OrderHistory class with persistence
    - Define OrderHistory.h with vector<Order> orders and storageFile path
    - Implement addOrder(order) appending to vector
    - Implement displayHistory() showing all orders
    - _Requirements: 15.3, 15.4_
  
  - [x] 13.2 Implement file persistence
    - Implement saveToFile() writing all orders to data/orders.txt
    - Implement loadFromFile() reading orders from file at startup
    - Use Order::serialize() and Order::deserialize()
    - Handle FileIOException for missing or corrupted files
    - _Requirements: 15.5, 15.6_
  
  - [x] 13.3 Implement sales analytics
    - Implement getMostPurchasedProduct() using unordered_map for frequency counting
    - Implement getTotalRevenue() summing all order totals
    - Implement getTotalGST() calculating GST from all orders
    - Implement displaySalesAnalytics() showing all metrics
    - _Requirements: 19.1, 19.2, 19.3, 19.4, 19.5_
  
  - [x] 13.4 Write property tests for order history and analytics
    - **Property 23: Order History Persistence Round Trip**
    - **Property 25: Order History Display Completeness**
    - **Property 29: Most Purchased Product Calculation**
    - **Property 30: Total Revenue Calculation**
    - **Property 31: Total GST Calculation**
    - **Property 32: Sales Analytics Display Completeness**
    - **Validates: Requirements 15.4, 15.5, 15.6, 19.2, 19.3, 19.4, 19.5_

- [x] 14. Implement ReceiptWriter class
  - [x] 14.1 Create ReceiptWriter utility class
    - Define ReceiptWriter.h with static methods
    - Implement generateReceipt(order, bill) creating formatted receipt string
    - Include ASCII borders, transaction details, item list, bill breakdown
    - Implement saveReceipt(content, orderId) writing to data/receipts/receipt_XXXX.txt
    - Implement getReceiptFilename(orderId) generating unique filename
    - _Requirements: 9.1, 9.2, 9.3, 9.4, 9.5, 9.6, 9.7_
  
  - [x] 14.2 Write property tests for receipt generation
    - **Property 21: Receipt File Generation**
    - **Property 22: Receipt Content Completeness**
    - **Property 50: Receipt Filename Display**
    - **Validates: Requirements 9.1, 9.2, 9.3, 9.4, 9.5, 9.6, 9.7_

- [x] 15. Implement Logger class
  - [x] 15.1 Create Logger singleton class
    - Define Logger.h with LogLevel enum (INFO, WARNING, ERROR, DEBUG)
    - Implement singleton pattern with getInstance()
    - Implement private constructor opening data/system.log
    - Implement thread-safe logging with mutex
    - _Requirements: 21.2_
  
  - [x] 15.2 Implement logging methods
    - Implement getCurrentTimestamp() with millisecond precision
    - Implement writeLog(level, component, message) with formatting
    - Implement static convenience methods: logInfo(), logWarning(), logError(), logDebug()
    - Implement logException(component, exception) for exception logging
    - Implement clearLog() for log file management
    - _Requirements: 21.2, 21.3_
  
  - [x] 15.3 Integrate logging throughout system
    - Add logging to Admin authentication (success/failure)
    - Add logging to payment processing (success/failure)
    - Add logging to inventory changes (add/update/remove/stock changes)
    - Add logging to order creation
    - Add logging to file operations (save/load)
    - Add logging to exception handlers
    - _Requirements: 21.2, 21.3_

- [x] 16. Checkpoint - Ensure all business logic components work together
  - Ensure all tests pass, ask the user if questions arise.

- [x] 17. Implement Admin class
  - [x] 17.1 Create Admin class with authentication
    - Define Admin.h with username, password, and Inventory* pointer
    - Define constants: ADMIN_USERNAME ("admin"), ADMIN_PASSWORD ("admin123")
    - Implement authenticate(user, pass) validating credentials
    - Log authentication attempts (success/failure)
    - _Requirements: 18.1, 18.7_
  
  - [x] 17.2 Implement admin menu and operations
    - Implement showAdminMenu() displaying admin options
    - Implement addNewProduct() prompting for product details and using ProductFactory
    - Implement updateProductPrice() modifying product price by ID
    - Implement updateProductStock() modifying stock quantity by ID
    - Implement removeProduct() deleting product from inventory by ID
    - Implement viewInventory() displaying all products
    - _Requirements: 18.2, 18.3, 18.4, 18.5, 18.6_
  
  - [x] 17.3 Implement admin persistence
    - Ensure all admin modifications are saved to inventory storage
    - _Requirements: 18.8_
  
  - [x] 17.4 Write property tests for admin operations
    - **Property 33: Admin Authentication**
    - **Property 34: Admin Product Addition**
    - **Property 35: Admin Price Update**
    - **Property 36: Admin Stock Update**
    - **Property 37: Admin Product Removal**
    - **Property 51: Admin Menu Display After Authentication**
    - **Property 52: Admin Inventory Persistence**
    - **Validates: Requirements 18.1, 18.2, 18.3, 18.4, 18.5, 18.6, 18.7, 18.8_

- [x] 18. Implement UIManager class
  - [x] 18.1 Create UIManager class structure
    - Define UIManager.h with pointers to Inventory, Cart, OrderHistory
    - Define Coupon member for coupon management
    - Implement constructor accepting all dependencies
    - _Requirements: 10.6_
  
  - [x] 18.2 Implement main menu and navigation
    - Implement displayMainMenu() with all menu options using ANSI colors
    - Use cyan for menu options, yellow for headings, black background
    - Implement run() main loop handling menu selection
    - Implement input validation for menu choices
    - _Requirements: 10.1, 10.2, 10.3, 10.6, 10.7, 10.8, 11.2_
  
  - [x] 18.3 Implement product viewing handlers
    - Implement handleViewProducts() displaying all products by category
    - Implement handleSearchProducts() prompting for search term and displaying results
    - Implement handleFilterProducts() prompting for category and displaying filtered results
    - Handle empty search results gracefully
    - _Requirements: 1.2, 1.3, 16.1, 16.2, 16.3, 16.4, 16.5_
  
  - [x] 18.4 Implement cart management handlers
    - Implement handleAddToCart() prompting for product ID and quantity
    - Validate product ID and quantity input
    - Handle out-of-stock and insufficient stock errors with red error messages
    - Display success message in green on successful add
    - Implement handleRemoveFromCart() prompting for product ID
    - Implement handleViewCart() displaying cart contents
    - _Requirements: 3.2, 3.5, 3.6, 3.7, 11.1, 11.4_
  
  - [x] 18.5 Implement coupon handler
    - Implement handleApplyCoupon() prompting for coupon code
    - Validate coupon using Coupon class
    - Display error in red for invalid coupons
    - Display success in green for valid coupons
    - _Requirements: 5.1, 5.2, 5.3, 5.4, 5.6_
  
  - [x] 18.6 Implement checkout handler
    - Implement handleCheckout() orchestrating complete checkout flow
    - Verify cart is not empty
    - Display cart contents
    - Calculate bill with all discounts and GST
    - Display bill breakdown
    - Prompt for payment method selection (UPI/Card/Cash)
    - Create appropriate Payment object based on selection
    - Process payment using polymorphic pay() call
    - Display loading animation during processing
    - Create Order instance on successful payment
    - Add order to OrderHistory
    - Generate and save receipt using ReceiptWriter
    - Display receipt filename
    - Clear cart after successful checkout
    - Display success message in green
    - _Requirements: 6.1, 6.2, 6.3, 7.1, 7.2, 7.3, 7.4, 7.5, 7.6, 8.1, 8.6, 8.7, 9.1, 9.7, 15.2, 15.3, 22.4_
  
  - [x] 18.7 Implement order history and admin handlers
    - Implement handleViewOrderHistory() displaying all past orders
    - Implement handleAdminPanel() prompting for credentials and launching Admin menu
    - _Requirements: 15.4, 18.1, 18.2_
  
  - [x] 18.8 Implement UI utility methods
    - Implement clearScreen() using ANSIColors utility
    - Implement displayHeader(title) with ASCII borders and centered text
    - Implement displayLoadingAnimation() showing progress indicators
    - Implement waitForEnter() pausing for user acknowledgment
    - _Requirements: 22.1, 22.2, 22.3, 22.4, 22.5_
  
  - [x] 18.9 Implement exception handling in all handlers
    - Wrap all handler methods in try-catch blocks
    - Catch specific exceptions (OutOfStockException, InvalidProductIdException, etc.)
    - Display error messages in red using ANSI colors
    - Log exceptions using Logger
    - Continue operation without crashing
    - _Requirements: 11.3, 11.4, 21.2, 21.3, 21.4, 21.5_
  
  - [x] 18.10 Implement input validation and error recovery
    - Add cin.fail() checks for all numeric inputs
    - Use cin.clear() and cin.ignore() to recover from bad input
    - Display error messages and re-prompt on invalid input
    - Handle non-numeric input gracefully without crashing
    - _Requirements: 11.2, 11.4, 11.5_
  
  - [x] 18.11 Write property tests for UI error handling
    - **Property 38: Exception Throwing on Invalid Input**
    - **Property 39: Exception Handling Without Termination**
    - **Property 40: Non-Numeric Input Robustness**
    - **Property 41: Menu Input Validation**
    - **Property 42: Error Recovery Prompting**
    - **Property 43: ANSI Color Scheme Consistency**
    - **Property 44: Success Message Styling**
    - **Property 45: Error Message Styling**
    - **Property 46: Screen Clearing on Menu Display**
    - **Property 47: ASCII Border Display**
    - **Property 48: Checkout Loading Animation**
    - **Property 49: Stock Display in Product Listing**
    - **Validates: Requirements 1.4, 3.6, 4.4, 5.6, 7.8, 8.7, 10.1, 10.2, 10.3, 10.4, 10.5, 10.8, 11.2, 11.3, 11.4, 11.5, 21.1, 21.3, 21.4, 21.5, 22.1, 22.2, 22.4_

- [x] 19. Implement main.cpp
  - [x] 19.1 Create main function with initialization
    - Include all necessary headers
    - Clear screen on startup
    - Create Inventory instance and initialize sample data
    - Create Cart instance with Inventory pointer
    - Create OrderHistory instance with file path
    - Load order history from file
    - Create UIManager instance with all dependencies
    - _Requirements: 13.3, 14.1, 14.2, 14.3, 15.6_
  
  - [x] 19.2 Implement main loop and cleanup
    - Call UIManager::run() to start application
    - Wrap in try-catch block for top-level exception handling
    - Save order history to file on exit
    - Display sales analytics summary on exit
    - Display fatal errors in red if exceptions occur
    - Return appropriate exit codes
    - _Requirements: 19.6, 21.2, 21.5_
  
  - [x] 19.3 Write integration tests for complete checkout flow
    - Test end-to-end: browse → add to cart → apply coupon → checkout → receipt
    - Verify order is saved to history
    - Verify receipt file is created
    - Verify inventory is updated correctly
    - _Requirements: All checkout-related requirements_

- [x] 20. Checkpoint - Ensure complete application works end-to-end
  - Ensure all tests pass, ask the user if questions arise.

- [x] 21. Implement memory management and smart pointers
  - [x] 21.1 Convert raw pointers to smart pointers
    - Ensure all Product instances use unique_ptr in Inventory
    - Ensure all CartItem instances use unique_ptr for products
    - Use raw pointers only for non-owning references (Cart→Inventory, Admin→Inventory)
    - _Requirements: 12.2, 12.5, 20.1, 20.5_
  
  - [x] 21.2 Implement RAII principles
    - Ensure all file handles use RAII (ifstream, ofstream)
    - Ensure Logger uses RAII for file management
    - Ensure no manual delete calls in code
    - _Requirements: 12.3, 20.2, 20.3_
  
  - [x] 21.3 Implement move semantics
    - Implement move constructor and move assignment for CartItem
    - Use std::move for transferring ownership of unique_ptr
    - _Requirements: 20.6_
  
  - [x] 21.4 Run memory leak detection
    - Compile with -fsanitize=address or run valgrind
    - Verify no memory leaks detected
    - _Requirements: 12.4_

- [x] 22. Finalize build system and compilation
  - [x] 22.1 Update Makefile with all source files
    - Add all .cpp files from src/ directory
    - Ensure -std=c++17 flag is set
    - Add -Wall -Wextra for warnings
    - Add -Iinclude for header path
    - Create obj/ and bin/ directories automatically
    - _Requirements: 13.6_
  
  - [x] 22.2 Test compilation and fix any errors
    - Run make to compile entire project
    - Fix any compilation errors or warnings
    - Ensure clean compilation with no warnings
    - _Requirements: 13.6, 20.4_
  
  - [x] 22.3 Create README.md with build instructions
    - Document compilation steps
    - Document how to run the application
    - Document system requirements (C++17 compiler)
    - Document directory structure
    - Document sample credentials (admin/admin123)
    - Document available coupon codes (SAVE10, FESTIVE20)

- [x] 23. Testing and validation
  - [x] 23.1 Set up RapidCheck for property-based testing
    - Install RapidCheck library
    - Create test directory structure
    - Create CMakeLists.txt for tests
    - _Requirements: Testing strategy_
  
  - [x] 23.2 Run all property-based tests
    - Execute all 52 property tests with 100+ iterations each
    - Verify all properties pass
    - Fix any failing properties
    - _Requirements: All correctness properties_+++
  - [x] 23.3 Write and run unit tests
    - Test valid coupon codes (SAVE10, FESTIVE20)
    - Test sample data initialization
    - Test payment method availability
    - Test empty cart edge case
    - Test zero stock edge case
    - Test no search results edge case
    - _Requirements: Unit testing strategy_
  
  - [x] 23.4 Write and run integration tests
    - Test complete checkout flow
    - Test admin flow (login → modify inventory)
    - Test persistence flow (save → restart → load)
    - _Requirements: Integration testing strategy_

- [-] 24. Final polish and academic submission preparation
  - [x] 24.1 Code review and cleanup
    - Ensure all classes have proper header guards
    - Ensure all methods have meaningful names
    - Add comments explaining complex logic
    - Remove any debug print statements
    - Ensure consistent code formatting
    - _Requirements: 13.4, 13.5_
  
  - [-] 24.2 Verify all OOP principles are demonstrated
    - Verify encapsulation (all data members private)
    - Verify inheritance (Product hierarchy, Payment hierarchy)
    - Verify polymorphism (virtual functions, runtime dispatch)
    - Verify abstraction (abstract base classes)
    - _Requirements: 1.5, 2.7, 8.2, 8.6_
  
  - [x] 24.3 Verify all design patterns are implemented
    - Verify Factory pattern (ProductFactory)
    - Verify Strategy pattern (Payment hierarchy)
    - Verify Singleton pattern (Logger, optional for Inventory/OrderHistory)
    - Verify Prototype pattern (Product::clone())
    - _Requirements: Design patterns section_
  
  - [x] 24.4 Verify SOLID principles compliance
    - Verify Single Responsibility (each class has one purpose)
    - Verify Open/Closed (extensible without modification)
    - Verify Liskov Substitution (derived classes substitutable)
    - Verify Interface Segregation (minimal interfaces)
    - Verify Dependency Inversion (depend on abstractions)
    - _Requirements: SOLID principles section_
  
  - [x] 24.5 Create sample run demonstration
    - Run application and perform complete workflow
    - Browse products
    - Add items to cart
    - Apply coupon
    - Complete checkout with each payment method
    - View order history
    - Access admin panel and modify inventory
    - View sales analytics
    - Verify all features work correctly
    - _Requirements: All functional requirements_
  
  - [x] 24.6 Final compilation and execution test
    - Clean build: make clean && make
    - Run application: ./bin/shopping_system
    - Verify no crashes or errors
    - Verify all UI elements display correctly with ANSI colors
    - Verify all files are created (orders.txt, receipts/, system.log)
    - _Requirements: All requirements_

- [x] 25. Implement ConfigManager for external configuration
  - [x] 25.1 Create ConfigManager singleton class
    - Define ConfigManager.h with singleton pattern
    - Implement getInstance() method
    - Define configuration file path (data/config.json)
    - Implement private constructor loading config on initialization
    - _Requirements: 23.1, 23.2_
  
  - [x] 25.2 Implement JSON parsing for configuration
    - Use nlohmann/json library or implement simple JSON parser
    - Parse GST rate from config file
    - Parse discount thresholds (cart discount threshold, cart discount rate)
    - Parse admin credentials (username, password)
    - Parse coupon codes with discount percentages
    - _Requirements: 23.2, 23.3_
  
  - [x] 25.3 Implement configuration validation and defaults
    - Validate GST rate is between 0.0 and 1.0
    - Validate discount thresholds are positive numbers
    - Validate admin credentials are non-empty strings
    - Validate coupon discount percentages are between 0 and 100
    - Implement fallback to hardcoded defaults if config file missing or invalid
    - Log configuration loading status (success/failure/using defaults)
    - _Requirements: 23.4, 23.5_
  
  - [x] 25.4 Integrate ConfigManager with existing classes
    - Update Bill class to use ConfigManager::getGSTRate()
    - Update Bill class to use ConfigManager::getCartDiscountThreshold() and getCartDiscountRate()
    - Update Admin class to use ConfigManager::getAdminUsername() and getAdminPassword()
    - Update Coupon class to use ConfigManager::getCoupons() for dynamic coupon loading
    - Remove hardcoded constants from these classes
    - _Requirements: 23.6_
  
  - [x] 25.5 Create sample config.json file
    - Create data/config.json with default values
    - Include GST rate: 0.18
    - Include cart discount threshold: 5000.0
    - Include cart discount rate: 0.05
    - Include admin credentials: {"username": "admin", "password": "admin123"}
    - Include coupons: {"SAVE10": 10, "FESTIVE20": 20}
    - Add comments explaining each configuration option
    - _Requirements: 23.2_

- [x] 26. Implement transaction rollback mechanism
  - [x] 26.1 Create Transaction class for state management
    - Define Transaction.h with cart snapshot and inventory snapshot
    - Implement captureCartState(cart) storing deep copy of cart items
    - Implement captureInventoryState(inventory) storing product IDs and quantities
    - Implement commit() method (no-op, marks transaction as successful)
    - _Requirements: 24.1, 24.2_
  
  - [x] 26.2 Implement rollback functionality
    - Implement rollback(cart, inventory) method
    - Restore cart to captured state (clear and re-add items)
    - Restore inventory quantities to captured state
    - Log rollback operation with reason
    - _Requirements: 24.3, 24.4_
  
  - [x] 26.3 Integrate transaction rollback into checkout flow
    - Update UIManager::handleCheckout() to create Transaction at start
    - Capture cart and inventory state before payment processing
    - Wrap payment processing in try-catch block
    - Call transaction.rollback() if payment fails or exception occurs
    - Call transaction.commit() on successful payment
    - Display rollback message to user if rollback occurs
    - _Requirements: 24.5_
  
  - [x] 26.4 Test transaction rollback scenarios
    - Test rollback on payment failure (insufficient funds)
    - Test rollback on payment timeout
    - Test rollback on invalid card
    - Verify cart and inventory are restored correctly
    - Verify user can retry checkout after rollback
    - _Requirements: 24.6_

- [x] 27. Add system metadata and version information
  - [x] 27.1 Create Version namespace with constants
    - Define Version.h namespace with VERSION constant ("1.0.0")
    - Define BUILD_TIMESTAMP constant (compilation date/time)
    - Define AUTHOR constant ("Shopping Management System Team")
    - Define PROJECT_NAME constant ("Shopping Management System")
    - _Requirements: 25.1_
  
  - [x] 27.2 Implement version display banner
    - Implement displayBanner() function showing ASCII art logo
    - Display project name, version, author, and build timestamp
    - Use ANSI colors for attractive formatting
    - _Requirements: 25.2_
  
  - [x] 27.3 Add version information to application
    - Create VERSION file in project root with version number
    - Add license headers to all source files (MIT or GPL)
    - Include copyright notice and author information in headers
    - _Requirements: 25.3_
  
  - [x] 27.4 Integrate version display into application
    - Call Version::displayBanner() in main.cpp on startup
    - Add "About" option to main menu showing version info
    - Display version in receipt footer
    - Display version in log file header
    - _Requirements: 25.4_

- [x] 28. Document performance analysis and complexity
  - [x] 28.1 Add complexity analysis comments to code
    - Add time complexity comments to all search operations
    - Add space complexity comments to data structures
    - Document O(n) operations in Inventory (search, filter)
    - Document O(1) operations (product lookup by ID if using map)
    - Document O(n) operations in Cart (add, remove, calculate)
    - _Requirements: 26.1_
  
  - [x] 28.2 Create PERFORMANCE.md documentation
    - Document algorithmic complexity of key operations
    - Include benchmark results for typical operations (add to cart, checkout, search)
    - Document memory footprint estimates
    - Document scalability limits (max products, max cart items, max orders)
    - _Requirements: 26.2, 26.3_
  
  - [x] 28.3 Add optimization recommendations
    - Document potential optimizations (use unordered_map for O(1) product lookup)
    - Document trade-offs (memory vs speed)
    - Document when optimizations would be needed (>10,000 products)
    - _Requirements: 26.4_

- [x] 29. Implement security features and hardening
  - [x] 29.1 Implement card number masking
    - Update CardPayment::maskCardNumber() to show only last 4 digits
    - Ensure masked card number is used in all displays and logs
    - Never log or display full card numbers
    - _Requirements: 27.1_
  
  - [x] 29.2 Add input sanitization functions
    - Create InputValidator utility class
    - Implement sanitizeString() removing special characters
    - Implement validateNumericInput() checking for valid numbers
    - Implement validateProductId() checking format and range
    - Apply sanitization to all user inputs before processing
    - _Requirements: 27.2_
  
  - [x] 29.3 Add file path validation
    - Implement validateFilePath() checking for path traversal attacks
    - Validate all file paths before opening files
    - Restrict file operations to data/ directory only
    - _Requirements: 27.3_
  
  - [x] 29.4 Create security documentation
    - Create SECURITY.md documenting security measures
    - Document input validation approach
    - Document sensitive data handling (passwords, card numbers)
    - Document file access restrictions
    - Add security warnings in code comments
    - _Requirements: 27.4_
  
  - [x] 29.5 Implement secure file permissions
    - Set restrictive permissions on log files (600)
    - Set restrictive permissions on order history files (600)
    - Set restrictive permissions on receipt files (644)
    - Document permission requirements in SECURITY.md
    - _Requirements: 27.5_

- [x] 30. Create comprehensive deployment documentation
  - [x] 30.1 Create DEPLOYMENT.md with platform instructions
    - Document Linux compilation and execution steps
    - Document Windows compilation (MinGW/MSVC) and execution steps
    - Document WSL (Windows Subsystem for Linux) setup and execution
    - Document macOS compilation and execution steps
    - _Requirements: 28.1_
  
  - [x] 30.2 Document terminal requirements
    - Document ANSI color support requirements
    - Document terminal emulator recommendations (Windows Terminal, iTerm2, GNOME Terminal)
    - Document fallback for terminals without ANSI support
    - Provide instructions for enabling ANSI colors on Windows
    - _Requirements: 28.2_
  
  - [x] 30.3 Create troubleshooting guide
    - Document common compilation errors and solutions
    - Document runtime errors and solutions
    - Document file permission issues
    - Document missing directory issues
    - Document ANSI color display issues
    - _Requirements: 28.3_
  
  - [x] 30.4 Document dependencies and requirements
    - List required dependencies (C++17 compiler, make, nlohmann/json)
    - List optional dependencies (RapidCheck for testing, valgrind for memory checking)
    - Document minimum compiler versions (GCC 7+, Clang 5+, MSVC 2017+)
    - _Requirements: 28.4_
  
  - [x] 30.5 Create platform-specific Makefiles
    - Create Makefile.linux with Linux-specific flags
    - Create Makefile.windows with MinGW-specific flags
    - Create Makefile.macos with macOS-specific flags
    - Update main Makefile to detect platform and include appropriate file
    - _Requirements: 28.5_

- [x] 31. Set up CI/CD pipeline
  - [x] 31.1 Create GitHub Actions workflow file
    - Create .github/workflows/ci.yml
    - Configure workflow to trigger on push and pull request
    - _Requirements: 29.1_
  
  - [x] 31.2 Configure build matrix for multiple platforms
    - Add Linux build job (Ubuntu latest, GCC)
    - Add Linux build job (Ubuntu latest, Clang)
    - Add Windows build job (Windows latest, MSVC)
    - Add macOS build job (macOS latest, Clang)
    - _Requirements: 29.2_
  
  - [x] 31.3 Add test execution to CI pipeline
    - Add step to compile all tests
    - Add step to run unit tests
    - Add step to run property-based tests
    - Add step to run integration tests
    - Fail build if any tests fail
    - _Requirements: 29.3_
  
  - [x] 31.4 Integrate AddressSanitizer in CI
    - Add build job with -fsanitize=address flag
    - Run tests with AddressSanitizer enabled
    - Fail build if memory leaks or errors detected
    - _Requirements: 29.4_
  
  - [x] 31.5 Add code coverage reporting
    - Integrate codecov or coveralls
    - Generate coverage reports after test execution
    - Upload coverage reports to coverage service
    - Add coverage badge to README.md
    - _Requirements: 29.5_
  
  - [x] 31.6 Configure compiler warnings as errors
    - Add -Werror flag to CI builds
    - Ensure all warnings are treated as errors
    - Fail build if any warnings are present
    - _Requirements: 29.6_

- [x] 32. Achieve comprehensive test coverage
  - [x] 32.1 Write additional unit tests for 90%+ coverage
    - Add unit tests for all exception classes
    - Add unit tests for all utility functions
    - Add unit tests for edge cases in all classes
    - Add unit tests for boundary conditions
    - _Requirements: 30.1_
  
  - [x] 32.2 Create edge case test matrix
    - Test empty cart checkout attempt
    - Test zero quantity add to cart
    - Test negative quantity input
    - Test invalid product ID formats
    - Test extremely large quantities
    - Test extremely large prices
    - Test empty inventory operations
    - Test duplicate product IDs
    - _Requirements: 30.2_
  
  - [x] 32.3 Add negative test cases
    - Test invalid coupon codes
    - Test expired coupons (if expiration implemented)
    - Test invalid payment methods
    - Test insufficient cash payment
    - Test invalid card numbers
    - Test invalid UPI IDs
    - Test file I/O failures (missing files, permission denied)
    - _Requirements: 30.3_
  
  - [x] 32.4 Add boundary tests
    - Test minimum and maximum product prices
    - Test minimum and maximum quantities
    - Test cart discount threshold boundary (4999.99 vs 5000.00)
    - Test GST calculation precision
    - Test coupon discount calculation precision
    - _Requirements: 30.4_
  
  - [x] 32.5 Generate and analyze coverage reports
    - Generate HTML coverage reports using gcov/lcov
    - Identify uncovered code paths
    - Write tests to cover missing paths
    - Achieve 90%+ line coverage
    - Achieve 85%+ branch coverage
    - _Requirements: 30.5_
  
  - [x] 32.6 Document test coverage metrics
    - Add coverage metrics to README.md
    - Document coverage by module/class
    - Document untested code and justification
    - Add coverage trends over time
    - _Requirements: 30.6_

## Notes

- All tasks are now required to ensure comprehensive test coverage and production-grade quality
- Each task references specific requirements for traceability
- Checkpoints ensure incremental validation at key milestones
- Property tests validate universal correctness properties using RapidCheck
- Unit tests validate specific examples and edge cases
- Integration tests validate component interactions
- The implementation follows the exact file structure from the design document
- All code uses C++17 features including smart pointers and RAII
- The system demonstrates comprehensive OOP principles and design patterns
- The dark-themed UI uses ANSI escape codes for professional appearance