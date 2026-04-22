requirements-
# Requirements Document

## Introduction

The Shopping Management System is a console-based application built in C++ that enables users to browse products, manage a shopping cart, apply discounts and coupons, and complete purchases through multiple payment methods. The system demonstrates Object-Oriented Programming principles including encapsulation, inheritance, polymorphism, and abstraction while providing a professional dark-themed user interface.

## Glossary

- **System**: The Shopping Management System application
- **Product**: An item available for purchase with attributes like ID, name, price, and quantity
- **Cart**: A collection of products selected by the user for purchase
- **Inventory**: The stock management system that tracks product availability
- **Coupon**: A promotional code that provides additional discounts
- **Payment_Method**: A mechanism for completing a transaction (UPI, Card, or Cash)
- **Bill**: A detailed breakdown of charges including subtotal, discounts, GST, and final total
- **User**: The person interacting with the system
- **GST**: Goods and Services Tax, a fixed 18% tax applied to purchases
- **Product_Discount**: Category-specific discount (Electronics: 10%, Clothing: 20%, Books: 5%)
- **Cart_Discount**: Additional 5% discount applied when cart total exceeds 5000
- **Receipt**: A text file containing the transaction details

## Requirements

### Requirement 1: Product Management

**User Story:** As a user, I want to view available products with their details, so that I can make informed purchasing decisions.

#### Acceptance Criteria

1. THE System SHALL maintain a product catalog with Electronics, Clothing, and Book categories
2. THE System SHALL display product ID, name, price, and available quantity for each product
3. WHEN a user requests to view products, THE System SHALL display all products organized by category
4. THE System SHALL use ANSI escape codes to display product information with cyan text on black background
5. FOR ALL products, THE System SHALL inherit from a base Product class with virtual display() method

### Requirement 2: Product Categorization and Discounts

**User Story:** As a user, I want different product categories to have specific attributes and discounts, so that I receive category-appropriate pricing.

#### Acceptance Criteria

1. THE Electronics class SHALL extend Product and include brand and warrantyYears attributes
2. THE Clothing class SHALL extend Product and include size and fabric attributes
3. THE Book class SHALL extend Product and include author and genre attributes
4. WHEN calculating discount for Electronics, THE System SHALL apply 10% discount
5. WHEN calculating discount for Clothing, THE System SHALL apply 20% discount
6. WHEN calculating discount for Books, THE System SHALL apply 5% discount
7. THE System SHALL implement calculateDiscount() as a virtual function in the Product base class

### Requirement 3: Shopping Cart Management

**User Story:** As a user, I want to add and remove products from my cart, so that I can manage my purchase selections.

#### Acceptance Criteria

1. THE Cart SHALL store products using a vector of Product pointers
2. WHEN a user adds a product to cart, THE System SHALL verify product availability in inventory
3. WHEN a user adds a product to cart, THE System SHALL reduce the inventory quantity by one
4. WHEN a user removes a product from cart, THE System SHALL restore the inventory quantity by one
5. THE System SHALL validate product ID before adding to cart
6. IF a product is out of stock, THEN THE System SHALL display an error message in red text
7. THE System SHALL display cart contents with product details and quantities

### Requirement 4: Inventory Management

**User Story:** As a system administrator, I want inventory to be automatically updated, so that stock levels remain accurate.

#### Acceptance Criteria

1. WHEN a product is added to cart, THE System SHALL decrement the quantityAvailable by one
2. WHEN a product is removed from cart, THE System SHALL increment the quantityAvailable by one
3. IF quantityAvailable reaches zero, THEN THE System SHALL prevent further additions to cart
4. THE System SHALL display current stock levels when showing products
5. FOR ALL inventory operations, THE System SHALL maintain data consistency

### Requirement 5: Coupon System

**User Story:** As a user, I want to apply coupon codes, so that I can receive additional discounts on my purchase.

#### Acceptance Criteria

1. THE System SHALL support coupon codes SAVE10 and FESTIVE20
2. WHEN a user applies SAVE10, THE System SHALL provide 10% additional discount
3. WHEN a user applies FESTIVE20, THE System SHALL provide 20% additional discount
4. IF an invalid coupon is entered, THEN THE System SHALL display an error message
5. THE System SHALL allow only one coupon per transaction
6. WHEN a valid coupon is applied, THE System SHALL display confirmation in green text

### Requirement 6: Cart Discount Calculation

**User Story:** As a user, I want to receive additional discounts on large purchases, so that I am incentivized to buy more.

#### Acceptance Criteria

1. WHEN cart subtotal exceeds 5000, THE System SHALL apply an additional 5% cart discount
2. THE System SHALL calculate cart discount after product discounts but before coupon discounts
3. THE System SHALL display cart discount separately in the bill breakdown
4. THE System SHALL apply cart discount automatically without user action

### Requirement 7: Bill Generation and GST Calculation

**User Story:** As a user, I want to see a detailed bill breakdown, so that I understand all charges and discounts applied.

#### Acceptance Criteria

1. THE System SHALL calculate subtotal as sum of all product prices before discounts
2. THE System SHALL calculate and display total product discounts
3. THE System SHALL calculate and display cart discount if applicable
4. THE System SHALL calculate and display coupon discount if applicable
5. THE System SHALL apply 18% GST to the amount after all discounts
6. THE System SHALL display final total including GST
7. THE System SHALL format bill with clear sections for each component
8. THE System SHALL use yellow text for monetary values in the bill

### Requirement 8: Payment Processing with Polymorphism

**User Story:** As a user, I want to choose my preferred payment method, so that I can complete my purchase conveniently.

#### Acceptance Criteria

1. THE System SHALL provide three payment options: UPI, Card, and Cash
2. THE Payment base class SHALL define a virtual pay() method
3. THE UPIPayment class SHALL extend Payment and implement UPI-specific payment processing
4. THE CardPayment class SHALL extend Payment and implement card-specific payment processing
5. THE CashPayment class SHALL extend Payment and implement cash-specific payment processing
6. WHEN a user selects a payment method, THE System SHALL invoke the appropriate pay() method through runtime polymorphism
7. WHEN payment is successful, THE System SHALL display confirmation in green text

### Requirement 9: Receipt Generation

**User Story:** As a user, I want to save my receipt to a file, so that I have a record of my transaction.

#### Acceptance Criteria

1. WHEN checkout is completed, THE System SHALL generate a receipt text file
2. THE Receipt SHALL include transaction date and time
3. THE Receipt SHALL include all purchased items with prices
4. THE Receipt SHALL include complete bill breakdown with all discounts and GST
5. THE Receipt SHALL include payment method used
6. THE System SHALL save the receipt with a unique filename
7. WHEN receipt is saved, THE System SHALL display the filename to the user

### Requirement 10: User Interface and Menu System

**User Story:** As a user, I want a clean and intuitive menu interface, so that I can easily navigate the system.

#### Acceptance Criteria

1. THE System SHALL display a dark-themed interface using ANSI escape codes with black background
2. THE System SHALL use cyan text for menu options
3. THE System SHALL use yellow text for headings and important information
4. THE System SHALL use green text for success messages
5. THE System SHALL use red text for error messages
6. THE System SHALL provide menu options: View Products, Add to Cart, Remove from Cart, View Cart, Apply Coupon, Checkout, Exit
7. WHEN a user selects an option, THE System SHALL execute the corresponding functionality
8. THE System SHALL validate menu input and handle invalid selections

### Requirement 11: Input Validation

**User Story:** As a user, I want the system to validate my inputs, so that I receive clear feedback on errors.

#### Acceptance Criteria

1. WHEN a user enters a product ID, THE System SHALL verify it exists in the catalog
2. WHEN a user enters a menu choice, THE System SHALL verify it is within valid range
3. IF invalid input is provided, THEN THE System SHALL display an error message in red
4. IF invalid input is provided, THEN THE System SHALL prompt the user to try again
5. THE System SHALL handle non-numeric input gracefully without crashing

### Requirement 12: Memory Management

**User Story:** As a developer, I want proper memory management, so that the application does not leak memory.

#### Acceptance Criteria

1. THE Product base class SHALL define a virtual destructor
2. THE Cart class SHALL properly deallocate Product pointers when destroyed
3. THE System SHALL use RAII principles for resource management
4. THE System SHALL compile without memory leaks when tested with valgrind or similar tools
5. WHERE smart pointers are used, THE System SHALL prefer them over raw pointers

### Requirement 13: Code Organization

**User Story:** As a developer, I want well-organized code, so that the system is maintainable and follows best practices.

#### Acceptance Criteria

1. THE System SHALL separate class declarations into header files (.h)
2. THE System SHALL separate class implementations into source files (.cpp)
3. THE System SHALL avoid global variables
4. THE System SHALL use meaningful variable and function names
5. THE System SHALL include comments explaining complex logic
6. THE System SHALL compile with C++17 standard
7. THE System SHALL use STL vector for dynamic collections

### Requirement 14: Sample Data Initialization

**User Story:** As a user, I want the system to have sample products available, so that I can immediately test the functionality.

#### Acceptance Criteria

1. WHEN the System starts, THE System SHALL initialize at least 3 Electronics products
2. WHEN the System starts, THE System SHALL initialize at least 3 Clothing products
3. WHEN the System starts, THE System SHALL initialize at least 3 Book products
4. THE System SHALL assign unique product IDs to all sample products
5. THE System SHALL set realistic prices and quantities for sample products

### Requirement 15: Order History Tracking

**User Story:** As a user, I want to view my previous orders, so that I can track my purchase history.

#### Acceptance Criteria

1. THE System SHALL define an Order class with attributes for purchased products, total amount, date, and payment method
2. WHEN checkout is successful, THE System SHALL create an Order instance with transaction details
3. WHEN checkout is successful, THE System SHALL store the Order instance in order history
4. WHEN a user requests order history, THE System SHALL display all past orders with dates and totals
5. THE System SHALL persist order data to a file for retrieval across sessions
6. THE System SHALL load order history from file when the application starts

### Requirement 16: Product Search and Filtering

**User Story:** As a user, I want to search products by name or category, so that I can quickly find what I need.

#### Acceptance Criteria

1. THE System SHALL provide a search function that accepts product name as input
2. WHEN a user searches by name, THE System SHALL display all products with partial name matches
3. THE System SHALL provide a filter function that accepts category as input
4. WHEN a user filters by category, THE System SHALL display only products in that category
5. IF no products match the search criteria, THEN THE System SHALL display a message indicating no results found
6. THE System SHALL perform case-insensitive matching for product name searches

### Requirement 17: Quantity Support in Cart

**User Story:** As a user, I want to add multiple quantities of the same product, so that I don't have to add items repeatedly.

#### Acceptance Criteria

1. THE Cart SHALL store quantity information for each unique product
2. WHEN a user adds a product already in cart, THE System SHALL increment the quantity instead of creating a duplicate entry
3. WHEN displaying cart contents, THE System SHALL show quantity and line total for each product
4. WHEN a user adds multiple quantities, THE System SHALL verify sufficient inventory is available
5. WHEN a user adds multiple quantities, THE System SHALL decrement inventory by the requested quantity
6. WHEN a user removes a product from cart, THE System SHALL restore inventory by the product's quantity

### Requirement 18: Admin Panel

**User Story:** As an admin, I want to manage products, so that I can maintain the inventory system.

#### Acceptance Criteria

1. THE System SHALL provide an Admin login option with credential verification
2. WHEN admin credentials are valid, THE System SHALL display admin menu options
3. THE Admin SHALL be able to add new products with all required attributes
4. THE Admin SHALL be able to update product prices by product ID
5. THE Admin SHALL be able to update stock quantities by product ID
6. THE Admin SHALL be able to remove products from the catalog by product ID
7. THE System SHALL restrict admin menu access to authenticated admin users only
8. WHEN admin modifies inventory, THE System SHALL persist changes to storage

### Requirement 19: Sales Analytics

**User Story:** As a system administrator, I want to view sales summary, so that I can analyze business performance.

#### Acceptance Criteria

1. THE System SHALL track all completed transactions for analytics calculation
2. THE System SHALL calculate the most purchased product based on order history
3. THE System SHALL calculate total revenue from all completed orders
4. THE System SHALL calculate total GST collected from all completed orders
5. WHEN a user requests sales summary, THE System SHALL display most purchased product, total revenue, and total GST
6. THE System SHALL provide sales summary as a menu option or at program exit

### Requirement 20: Modern C++ Compliance

**User Story:** As a developer, I want to use modern C++ features, so that the code follows current best practices.

#### Acceptance Criteria

1. WHERE dynamic memory allocation is required, THE System SHALL use unique_ptr instead of raw pointers
2. THE System SHALL follow RAII principles for all resource management
3. THE System SHALL avoid manual memory deletion using delete keyword
4. THE System SHALL compile successfully with -std=c++17 compiler flag
5. THE System SHALL use smart pointers for managing Product instances in Cart
6. THE System SHALL use move semantics where appropriate to optimize performance

### Requirement 21: Exception Safety

**User Story:** As a user, I want the system to handle errors gracefully, so that it doesn't crash unexpectedly.

#### Acceptance Criteria

1. WHEN invalid input is provided, THE System SHALL throw appropriate exceptions
2. THE System SHALL catch and handle exceptions at appropriate levels
3. WHEN an exception occurs, THE System SHALL display a meaningful error message to the user
4. WHEN an exception is handled, THE System SHALL continue operation without terminating
5. THE System SHALL not crash when encountering runtime errors
6. THE System SHALL use try-catch blocks around operations that may fail

### Requirement 22: UI Layout Standardization

**User Story:** As a user, I want a polished and professional interface, so that the system feels premium.

#### Acceptance Criteria

1. WHEN displaying any menu, THE System SHALL clear the screen before rendering
2. THE System SHALL display ASCII borders around menu sections for visual separation
3. THE System SHALL center headings within their display sections
4. WHEN processing checkout, THE System SHALL display a loading animation to indicate progress
5. THE System SHALL maintain consistent spacing and alignment throughout all interface screens
6. THE System SHALL use consistent color schemes across all menus and displays

### Requirement 23: Configuration Management

**User Story:** As a system administrator, I want to configure system parameters externally, so that I can adjust settings without recompiling the application.

#### Acceptance Criteria

1. THE System SHALL load configuration from a config.json file at startup
2. THE Configuration SHALL include GST rate, discount thresholds, admin credentials, and coupon codes
3. IF config.json is missing, THEN THE System SHALL use default configuration values
4. IF config.json is malformed, THEN THE System SHALL display an error message and use default values
5. THE System SHALL validate all configuration values are within acceptable ranges
6. WHEN configuration is loaded, THE System SHALL display confirmation message with loaded settings
7. THE System SHALL define a Configuration class to encapsulate all configurable parameters

### Requirement 24: Transaction Atomicity and Rollback

**User Story:** As a user, I want my cart and inventory restored if checkout fails, so that I don't lose my selections or corrupt inventory data.

#### Acceptance Criteria

1. WHEN checkout begins, THE System SHALL create a transaction checkpoint with current cart and inventory state
2. IF payment processing fails, THEN THE System SHALL restore cart contents from checkpoint
3. IF receipt generation fails, THEN THE System SHALL restore cart contents and inventory from checkpoint
4. IF order save fails, THEN THE System SHALL restore cart contents and inventory from checkpoint
5. WHEN rollback occurs, THE System SHALL display an error message explaining the failure
6. THE System SHALL implement a Transaction class with commit() and rollback() methods
7. WHEN checkout completes successfully, THE System SHALL discard the checkpoint

### Requirement 25: System Metadata and Versioning

**User Story:** As a user, I want to see system version information, so that I know which version I am using.

#### Acceptance Criteria

1. WHEN the System starts, THE System SHALL display version number in semantic versioning format
2. WHEN the System starts, THE System SHALL display build timestamp
3. WHEN the System starts, THE System SHALL display author name
4. THE System SHALL include license headers in all source files
5. THE System SHALL maintain a VERSION file with current semantic version
6. THE System SHALL define version metadata as compile-time constants
7. THE System SHALL provide a menu option to display version information

### Requirement 26: Performance and Complexity Documentation

**User Story:** As a developer, I want to understand performance characteristics, so that I can optimize and scale the system appropriately.

#### Acceptance Criteria

1. THE System SHALL document time complexity for inventory search operations as O(n)
2. THE System SHALL document time complexity for coupon lookup operations as O(1)
3. THE System SHALL document space complexity for cart storage as O(n)
4. THE System SHALL include performance benchmarks in documentation for key operations
5. THE System SHALL document scalability limits for product catalog size
6. THE System SHALL document scalability limits for order history size
7. THE System SHALL include complexity analysis comments in code for non-trivial algorithms

### Requirement 27: Security Awareness

**User Story:** As a security-conscious user, I want the system to follow security best practices, so that my data is protected.

#### Acceptance Criteria

1. THE System SHALL document that hardcoded passwords must not be used in production
2. WHEN displaying card numbers, THE System SHALL mask all but the last 4 digits
3. THE System SHALL not store sensitive payment information to disk
4. THE System SHALL sanitize all user inputs before processing
5. THE System SHALL protect log files from unauthorized access
6. THE System SHALL document security considerations in a SECURITY.md file
7. THE System SHALL validate all file paths to prevent directory traversal attacks

### Requirement 28: Deployment and Environment Support

**User Story:** As a developer, I want clear deployment instructions, so that I can build and run the system on different platforms.

#### Acceptance Criteria

1. THE System SHALL provide compilation instructions for Linux using g++
2. THE System SHALL provide compilation instructions for Windows using MinGW
3. THE System SHALL provide compilation instructions for WSL environments
4. THE System SHALL document ANSI terminal requirements for proper display
5. THE System SHALL include troubleshooting guide for common compilation errors
6. THE System SHALL specify minimum compiler version requirements
7. THE System SHALL document required C++ standard library dependencies

### Requirement 29: Continuous Integration Setup

**User Story:** As a developer, I want automated testing and quality checks, so that code quality is maintained consistently.

#### Acceptance Criteria

1. THE System SHALL include a GitHub Actions workflow configuration file
2. THE CI workflow SHALL compile the code with all warnings enabled
3. THE CI workflow SHALL execute all unit tests and report results
4. THE CI workflow SHALL run AddressSanitizer to detect memory issues
5. THE CI workflow SHALL fail if any compiler warnings are present
6. THE CI workflow SHALL generate code coverage reports
7. THE System SHALL maintain CI configuration in .github/workflows directory

### Requirement 30: Test Coverage and Quality Metrics

**User Story:** As a developer, I want comprehensive test coverage, so that I can ensure code quality and catch regressions.

#### Acceptance Criteria

1. THE System SHALL achieve minimum 90% branch coverage across all modules
2. THE System SHALL include an edge case test matrix covering boundary conditions
3. THE System SHALL include negative tests for all error handling paths
4. THE System SHALL include boundary tests for all numeric inputs
5. THE System SHALL generate HTML coverage reports using gcov or similar tools
6. THE System SHALL document test coverage metrics in test documentation
7. THE System SHALL fail CI builds if coverage drops below 90% threshold