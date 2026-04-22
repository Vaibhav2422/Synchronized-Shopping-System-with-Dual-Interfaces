/**
 * Shopping Management System - REST API Server
 * Raw Winsock2 HTTP/1.1 server — no external dependencies.
 * Compatible with MinGW GCC 6.3 on Windows.
 * Listens on localhost:8080.
 */

// Include all project headers FIRST (before windows.h to avoid macro conflicts)
#include "Inventory.h"
#include "Cart.h"
#include "Bill.h"
#include "Coupon.h"
#include "Order.h"
#include "OrderHistory.h"
#include "Admin.h"
#include "ReceiptWriter.h"
#include "Logger.h"
#include "ConfigManager.h"
#include "ProductFactory.h"
#include "Electronics.h"
#include "Clothing.h"
#include "Book.h"
#include "ShoppingException.h"
#include "CouponException.h"
#include "StockException.h"
#include "PaymentException.h"
#include "UPIPayment.h"
#include "CardPayment.h"
#include "CashPayment.h"
#include "CustomerManager.h"

// Windows networking — included AFTER project headers to avoid macro conflicts
// (windows.h defines ERROR as a macro which conflicts with Logger::LogLevel::ERROR)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
// Undefine the ERROR macro from windows.h if already pulled in transitively
#ifdef ERROR
#undef ERROR
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#ifdef ERROR
#undef ERROR
#endif

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cstdio>

// ─── JSON helpers ─────────────────────────────────────────────────────────────

static std::string esc(const std::string& s) {
    std::string o;
    for (char c : s) {
        if      (c == '"')  o += "\\\"";
        else if (c == '\\') o += "\\\\";
        else if (c == '\n') o += "\\n";
        else if (c == '\r') o += "\\r";
        else if (c == '\t') o += "\\t";
        else o += c;
    }
    return o;
}

static std::string customerJson(const CustomerRecord& c) {
    std::ostringstream o;
    o << "{\"customerId\":\"" << esc(c.customerId) << "\""
      << ",\"name\":\""       << esc(c.name)       << "\""
      << ",\"phone\":\""      << esc(c.phone)       << "\""
      << ",\"loyaltyPoints\":" << c.loyaltyPoints
      << ",\"createdAt\":\""  << esc(c.createdAt)  << "\""
      << ",\"orderIds\":[";
    for (size_t i = 0; i < c.orderIds.size(); i++) {
        if (i) o << ",";
        o << c.orderIds[i];
    }
    o << "]}";
    return o.str();
}

static std::string productJson(const Product* p) {
    if (!p) return "null";
    std::ostringstream o;
    o << std::fixed;
    o << "{\"id\":"       << p->getProductId()
      << ",\"name\":\""   << esc(p->getName())     << "\""
      << ",\"price\":"    << p->getPrice()
      << ",\"quantity\":" << p->getQuantityAvailable()
      << ",\"category\":\"" << esc(p->getCategory()) << "\""
      << ",\"discount\":" << p->calculateDiscount()
      << "}";
    return o.str();
}

static std::string jStr(const std::string& body, const std::string& key) {
    std::string s = "\"" + key + "\"";
    auto p = body.find(s); if (p == std::string::npos) return "";
    p = body.find(':', p); if (p == std::string::npos) return "";
    p = body.find('"', p); if (p == std::string::npos) return "";
    auto e = body.find('"', p+1); if (e == std::string::npos) return "";
    return body.substr(p+1, e-p-1);
}

static double jDbl(const std::string& body, const std::string& key) {
    std::string s = "\"" + key + "\"";
    auto p = body.find(s); if (p == std::string::npos) return 0.0;
    p = body.find(':', p); if (p == std::string::npos) return 0.0;
    p++;
    while (p < body.size() && (body[p]==' '||body[p]=='\t')) p++;
    try { return std::stod(body.substr(p)); } catch (...) { return 0.0; }
}

static int jInt(const std::string& body, const std::string& key) {
    return (int)jDbl(body, key);
}

static bool jBool(const std::string& body, const std::string& key) {
    std::string s = "\"" + key + "\":true";
    return body.find(s) != std::string::npos;
}

// ─── Global state ─────────────────────────────────────────────────────────────

static Inventory    g_inv;
static Cart         g_cart(&g_inv);
static OrderHistory g_hist("data/orders.txt");
static Admin        g_admin(&g_inv);

struct LoyaltyState { int points = 0; double redeemValue = 1.0; } g_loyalty;
static std::vector<std::string> g_coupons;
static int g_split = 1;

// ─── Bill computation ─────────────────────────────────────────────────────────

static std::string billJson(bool redeem = false) {
    double sub   = g_cart.calculateSubtotal();
    double pdisc = g_cart.calculateProductDiscounts();
    double cdisc = g_cart.calculateCartDiscount();
    double after = sub - pdisc - cdisc;

    const auto& valid = ConfigManager::getInstance().getCoupons();
    double stacked = 0.0;
    for (auto& c : g_coupons) {
        auto it = valid.find(c);
        if (it != valid.end()) stacked += (after - stacked) * it->second;
    }

    double loyDisc = 0.0;
    if (redeem && g_loyalty.points > 0) {
        loyDisc = g_loyalty.points * g_loyalty.redeemValue;
        double cap = after - stacked;
        if (loyDisc > cap) loyDisc = cap;
    }

    double taxable = after - stacked - loyDisc;
    double cgst = taxable * 0.09;
    double sgst = taxable * 0.09;
    double total = taxable + cgst + sgst;
    double per   = (g_split > 1) ? total / g_split : total;
    int earned   = (int)(total / 100.0);

    std::ostringstream o; o << std::fixed;
    o << "{\"subtotal\":"            << sub
      << ",\"productDiscounts\":"    << pdisc
      << ",\"cartDiscount\":"        << cdisc
      << ",\"stackedCouponDiscount\":" << stacked
      << ",\"appliedCoupons\":[";
    for (size_t i = 0; i < g_coupons.size(); i++) {
        if (i) o << ",";
        o << "\"" << esc(g_coupons[i]) << "\"";
    }
    o << "],\"loyaltyDiscount\":"    << loyDisc
      << ",\"loyaltyPoints\":"       << g_loyalty.points
      << ",\"taxableAmount\":"       << taxable
      << ",\"cgst\":"                << cgst
      << ",\"sgst\":"                << sgst
      << ",\"gst\":"                 << (cgst+sgst)
      << ",\"finalTotal\":"          << total
      << ",\"splitCount\":"          << g_split
      << ",\"perPerson\":"           << per
      << ",\"earnedPoints\":"        << earned
      << "}";
    return o.str();
}

// ─── HTTP helpers ─────────────────────────────────────────────────────────────

static std::string httpResp(int code, const std::string& body,
                            const std::string& ct = "application/json") {
    std::string status;
    if      (code == 200) status = "200 OK";
    else if (code == 201) status = "201 Created";
    else if (code == 204) status = "204 No Content";
    else if (code == 400) status = "400 Bad Request";
    else if (code == 401) status = "401 Unauthorized";
    else if (code == 403) status = "403 Forbidden";
    else if (code == 404) status = "404 Not Found";
    else                  status = "500 Internal Server Error";

    std::ostringstream r;
    r << "HTTP/1.1 " << status << "\r\n"
      << "Content-Type: " << ct << "\r\n"
      << "Content-Length: " << body.size() << "\r\n"
      << "Access-Control-Allow-Origin: *\r\n"
      << "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
      << "Access-Control-Allow-Headers: Content-Type, Authorization\r\n"
      << "Connection: close\r\n"
      << "\r\n"
      << body;
    return r.str();
}

static std::string ok(const std::string& body) { return httpResp(200, body); }
static std::string err(int c, const std::string& msg) {
    return httpResp(c, "{\"error\":\"" + esc(msg) + "\"}");
}

// ─── Request parsing ──────────────────────────────────────────────────────────

struct Req {
    std::string method, path, body;
    std::string param(const std::string& key) const {
        auto q = path.find('?');
        if (q == std::string::npos) return "";
        std::string qs = path.substr(q+1);
        std::string search = key + "=";
        auto p = qs.find(search);
        if (p == std::string::npos) return "";
        p += search.size();
        auto e = qs.find('&', p);
        std::string val = (e == std::string::npos) ? qs.substr(p) : qs.substr(p, e-p);
        // URL decode spaces
        std::string out;
        for (size_t i = 0; i < val.size(); i++) {
            if (val[i] == '+') out += ' ';
            else if (val[i] == '%' && i+2 < val.size()) {
                int c; sscanf(val.substr(i+1,2).c_str(), "%x", &c);
                out += (char)c; i += 2;
            } else out += val[i];
        }
        return out;
    }
    std::string pathBase() const {
        auto q = path.find('?');
        return (q == std::string::npos) ? path : path.substr(0, q);
    }
    // Extract :id from path like /api/admin/product/123
    std::string lastSegment() const {
        auto b = pathBase();
        auto p = b.rfind('/');
        return (p == std::string::npos) ? b : b.substr(p+1);
    }
};

static Req parseRequest(const std::string& raw) {
    Req r;
    std::istringstream ss(raw);
    std::string line;
    std::getline(ss, line);
    std::istringstream ls(line);
    ls >> r.method >> r.path;
    // skip headers, find body
    std::string prev;
    while (std::getline(ss, line)) {
        if (line == "\r" || line.empty()) break;
    }
    // rest is body
    std::string rest;
    while (std::getline(ss, line)) rest += line + "\n";
    r.body = rest;
    return r;
}

// ─── Route handler ────────────────────────────────────────────────────────────

static std::string handle(const Req& r) {
    const std::string& m = r.method;
    std::string p = r.pathBase();

    // CORS preflight
    if (m == "OPTIONS") return httpResp(204, "");

    // Health
    if (m == "GET" && p == "/api/health")
        return ok("{\"status\":\"ok\",\"version\":\"1.0.0\"}");

    // Products
    if (m == "GET" && p == "/api/products") {
        const auto& prods = g_inv.getProducts();
        std::string b = "[";
        for (size_t i = 0; i < prods.size(); i++) {
            if (i) b += ",";
            b += productJson(prods[i].get());
        }
        return ok(b + "]");
    }

    if (m == "GET" && p == "/api/products/search") {
        auto res = g_inv.searchByName(r.param("q"));
        std::string b = "[";
        for (size_t i = 0; i < res.size(); i++) { if (i) b += ","; b += productJson(res[i]); }
        return ok(b + "]");
    }

    if (m == "GET" && p == "/api/products/filter") {
        auto res = g_inv.filterByCategory(r.param("category"));
        std::string b = "[";
        for (size_t i = 0; i < res.size(); i++) { if (i) b += ","; b += productJson(res[i]); }
        return ok(b + "]");
    }

    // Cart
    if (m == "GET" && p == "/api/cart") {
        const auto& items = g_cart.getItems();
        std::string b = "{\"items\":[";
        for (size_t i = 0; i < items.size(); i++) {
            if (i) b += ",";
            std::ostringstream o; o << std::fixed;
            o << "{\"product\":" << productJson(items[i].product.get())
              << ",\"quantity\":" << items[i].quantity
              << ",\"lineTotal\":" << items[i].getLineTotal()
              << ",\"lineDiscount\":" << items[i].getLineDiscount() << "}";
            b += o.str();
        }
        std::ostringstream o; o << std::fixed;
        o << b << "],\"itemCount\":" << g_cart.getItemCount()
          << ",\"subtotal\":" << g_cart.calculateSubtotal() << "}";
        return ok(o.str());
    }

    if (m == "POST" && p == "/api/cart/add") {
        int id = jInt(r.body, "productId");
        int qty = jInt(r.body, "quantity"); if (qty <= 0) qty = 1;
        try { g_cart.addProduct(id, qty); return ok("{\"success\":true}"); }
        catch (const std::exception& e) { return err(400, e.what()); }
    }

    if (m == "POST" && p == "/api/cart/remove") {
        int id = jInt(r.body, "productId");
        try {
            bool ok2 = g_cart.removeProduct(id);
            return ok2 ? ok("{\"success\":true}") : err(404, "Product not in cart");
        } catch (const std::exception& e) { return err(400, e.what()); }
    }

    if (m == "POST" && p == "/api/cart/clear") {
        g_cart.clear(); g_coupons.clear(); g_split = 1;
        return ok("{\"success\":true}");
    }

    // Coupons
    if (m == "GET" && p == "/api/coupons") {
        const auto& cv = ConfigManager::getInstance().getCoupons();
        std::string b = "["; bool first = true;
        for (const auto& kv : cv) {
            if (!first) b += ","; first = false;
            std::ostringstream o; o << std::fixed;
            o << "{\"code\":\"" << esc(kv.first) << "\",\"discount\":" << kv.second << "}";
            b += o.str();
        }
        return ok(b + "]");
    }

    if (m == "POST" && p == "/api/coupon/apply") {
        std::string code = jStr(r.body, "code");
        const auto& valid = ConfigManager::getInstance().getCoupons();
        if (valid.find(code) == valid.end()) return err(400, "Invalid coupon: " + code);
        if (std::find(g_coupons.begin(), g_coupons.end(), code) != g_coupons.end())
            return err(400, "Coupon already applied");
        g_coupons.push_back(code);
        return ok("{\"success\":true,\"code\":\"" + esc(code) + "\"}");
    }

    if (m == "POST" && p == "/api/coupon/remove") {
        std::string code = jStr(r.body, "code");
        auto it = std::find(g_coupons.begin(), g_coupons.end(), code);
        if (it != g_coupons.end()) { g_coupons.erase(it); return ok("{\"success\":true}"); }
        return err(404, "Coupon not applied");
    }

    // Bill
    if (m == "GET" && p == "/api/bill") {
        bool redeem = (r.param("redeem") == "true");
        return ok(billJson(redeem));
    }

    if (m == "POST" && p == "/api/bill/split") {
        int count = jInt(r.body, "count");
        if (count < 1) count = 1;
        g_split = count;
        return ok(billJson());
    }

    // Loyalty
    if (m == "GET" && p == "/api/loyalty") {
        std::ostringstream o; o << std::fixed;
        o << "{\"points\":" << g_loyalty.points
          << ",\"redeemValue\":" << g_loyalty.redeemValue
          << ",\"rupeeValue\":" << (g_loyalty.points * g_loyalty.redeemValue) << "}";
        return ok(o.str());
    }

    // Checkout
    if (m == "POST" && p == "/api/checkout") {
        if (g_cart.isEmpty()) return err(400, "Cart is empty");
        std::string method = jStr(r.body, "method");
        bool redeemLoyalty = jBool(r.body, "redeemLoyalty");
        std::string customerId = jStr(r.body, "customerId");

        // Compute final total
        double sub   = g_cart.calculateSubtotal();
        double pdisc = g_cart.calculateProductDiscounts();
        double cdisc = g_cart.calculateCartDiscount();
        double after = sub - pdisc - cdisc;
        const auto& valid = ConfigManager::getInstance().getCoupons();
        double stacked = 0.0;
        for (auto& c : g_coupons) {
            auto it = valid.find(c);
            if (it != valid.end()) stacked += (after - stacked) * it->second;
        }
        double loyDisc = 0.0;
        if (redeemLoyalty && g_loyalty.points > 0) {
            loyDisc = g_loyalty.points * g_loyalty.redeemValue;
            double cap = after - stacked;
            if (loyDisc > cap) loyDisc = cap;
        }
        double taxable   = after - stacked - loyDisc;
        double finalTotal = taxable * 1.18;

        try {
            std::unique_ptr<Payment> payment;
            if (method == "UPI") {
                std::string uid = jStr(r.body, "upiId");
                if (uid.empty()) uid = "user@upi";
                payment = std::unique_ptr<Payment>(new UPIPayment(finalTotal, uid));
            } else if (method == "Card") {
                std::string cn = jStr(r.body, "cardNumber");
                std::string ch = jStr(r.body, "cardHolder");
                if (cn.empty()) cn = "0000000000000000";
                if (ch.empty()) ch = "Customer";
                payment = std::unique_ptr<Payment>(new CardPayment(finalTotal, cn, ch));
            } else if (method == "Cash") {
                double tendered = jDbl(r.body, "cashTendered");
                payment = std::unique_ptr<Payment>(new CashPayment(finalTotal, tendered));
            } else {
                return err(400, "Invalid payment method");
            }

            payment->pay();

            // Build bill for receipt
            Bill bill;
            Coupon fc;
            if (!g_coupons.empty()) { try { fc.apply(g_coupons[0]); } catch (...) {} }
            bill.calculate(g_cart, fc.isApplied() ? &fc : nullptr);

            Order order(g_cart, finalTotal, method);

            // Resolve customer name/ID for receipt header
            std::string custName, custId;
            if (!customerId.empty()) {
                auto* cust = CustomerManager::getInstance().findById(customerId);
                if (cust) {
                    custName = cust->name;
                    custId   = cust->customerId;
                }
            }

            std::string receipt = ReceiptWriter::generateReceipt(order, bill, custName, custId);
            ReceiptWriter::saveReceipt(receipt, order.getOrderId());
            g_hist.addOrder(order);
            g_hist.saveToFile();

            // Link order to customer and award loyalty points
            if (!customerId.empty()) {
                CustomerManager::getInstance().linkOrder(customerId, order.getOrderId());
                int pts = CustomerManager::computePointsEarned(finalTotal);
                CustomerManager::getInstance().addLoyaltyPoints(customerId, pts);
                CustomerManager::getInstance().saveToFile();
            }

            int earned = (int)(finalTotal / 100.0);
            if (redeemLoyalty) g_loyalty.points = 0;
            g_loyalty.points += earned;

            g_cart.clear(); g_coupons.clear(); g_split = 1;

            std::ostringstream o; o << std::fixed;
            o << "{\"success\":true"
              << ",\"orderId\":"     << order.getOrderId()
              << ",\"finalTotal\":"  << finalTotal
              << ",\"receipt\":\""   << esc(receipt) << "\""
              << ",\"earnedPoints\":" << earned
              << ",\"totalPoints\":" << g_loyalty.points;
            if (!custId.empty())
                o << ",\"customerId\":\"" << esc(custId) << "\"";
            o << "}";
            return ok(o.str());

        } catch (const std::exception& e) { return err(400, e.what()); }
    }

    // ── Customer endpoints ────────────────────────────────────────────────────

    // GET /api/customers/search?q=
    if (m == "GET" && p == "/api/customers/search") {
        std::string q = r.param("q");
        auto results = CustomerManager::getInstance().search(q);
        std::string b = "[";
        for (size_t i = 0; i < results.size(); i++) {
            if (i) b += ",";
            b += customerJson(results[i]);
        }
        return ok(b + "]");
    }

    // POST /api/customers/register
    if (m == "POST" && p == "/api/customers/register") {
        std::string name  = jStr(r.body, "name");
        std::string phone = jStr(r.body, "phone");
        std::string pass  = jStr(r.body, "password");
        if (name.empty() || phone.empty() || pass.empty())
            return err(400, "name, phone, and password are required");
        try {
            auto rec = CustomerManager::getInstance().registerCustomer(name, phone, pass);
            CustomerManager::getInstance().saveToFile();
            std::ostringstream o;
            o << "{\"success\":true,\"customerId\":\"" << esc(rec.customerId) << "\""
              << ",\"customer\":" << customerJson(rec) << "}";
            return httpResp(201, o.str());
        } catch (const std::exception& e) { return err(400, e.what()); }
    }

    // GET /api/customers/ranked
    if (m == "GET" && p == "/api/customers/ranked") {
        const auto& all = CustomerManager::getInstance().getAllCustomers();
        // Compute total spending per customer from order history
        struct RankedEntry { CustomerRecord rec; double totalSpent; };
        std::vector<RankedEntry> ranked;
        for (const auto& c : all) {
            double total = 0.0;
            for (int oid : c.orderIds) {
                for (const auto& o : g_hist.getOrders()) {
                    if (o.getOrderId() == oid) { total += o.getTotalAmount(); break; }
                }
            }
            ranked.push_back({c, total});
        }
        std::sort(ranked.begin(), ranked.end(),
            [](const RankedEntry& a, const RankedEntry& b){ return a.totalSpent > b.totalSpent; });

        std::string b = "[";
        for (size_t i = 0; i < ranked.size(); i++) {
            if (i) b += ",";
            std::ostringstream o; o << std::fixed;
            o << "{\"rank\":"        << (i+1)
              << ",\"customer\":"    << customerJson(ranked[i].rec)
              << ",\"totalSpent\":"  << ranked[i].totalSpent
              << ",\"visitCount\":"  << ranked[i].rec.orderIds.size()
              << "}";
            b += o.str();
        }
        return ok(b + "]");
    }

    // POST /api/customers/:id/link-order
    if (m == "POST" && p.find("/api/customers/") == 0 && p.find("/link-order") != std::string::npos) {
        // Extract customer ID from path: /api/customers/{id}/link-order
        std::string prefix = "/api/customers/";
        std::string suffix = "/link-order";
        std::string custId = p.substr(prefix.size(), p.size() - prefix.size() - suffix.size());
        int orderId = jInt(r.body, "orderId");
        auto* cust = CustomerManager::getInstance().findById(custId);
        if (!cust) return err(404, "Customer not found");
        CustomerManager::getInstance().linkOrder(custId, orderId);
        CustomerManager::getInstance().saveToFile();
        return ok("{\"success\":true}");
    }

    // ── Orders ────────────────────────────────────────────────────────────────

    // Orders
    if (m == "GET" && p == "/api/orders") {
        const auto& orders = g_hist.getOrders();
        std::string b = "[";
        for (size_t i = 0; i < orders.size(); i++) {
            if (i) b += ",";
            const auto& o = orders[i];
            std::ostringstream os; os << std::fixed;
            os << "{\"orderId\":"      << o.getOrderId()
               << ",\"totalAmount\":"  << o.getTotalAmount()
               << ",\"paymentMethod\":\"" << esc(o.getPaymentMethod()) << "\""
               << ",\"orderDate\":\""  << esc(o.getOrderDate()) << "\""
               << ",\"orderTime\":\""  << esc(o.getOrderTime()) << "\""
               << ",\"products\":[";
            const auto& prods = o.getProducts();
            for (size_t j = 0; j < prods.size(); j++) {
                if (j) os << ",";
                os << "{\"name\":\"" << esc(prods[j].first)
                   << "\",\"quantity\":" << prods[j].second << "}";
            }
            os << "]}";
            b += os.str();
        }
        return ok(b + "]");
    }

    // Analytics
    if (m == "GET" && p == "/api/analytics") {
        std::ostringstream o; o << std::fixed;
        o << "{\"mostPurchased\":\"" << esc(g_hist.getMostPurchasedProduct()) << "\""
          << ",\"totalRevenue\":"    << g_hist.getTotalRevenue()
          << ",\"totalGST\":"        << g_hist.getTotalGST()
          << ",\"orderCount\":"      << g_hist.getOrders().size() << "}";
        return ok(o.str());
    }

    // Admin
    if (m == "POST" && p == "/api/admin/login") {
        std::string u = jStr(r.body, "username");
        std::string pw = jStr(r.body, "password");
        if (g_admin.authenticate(u, pw)) return ok("{\"success\":true}");
        return err(401, "Invalid credentials");
    }

    if (m == "POST" && p == "/api/admin/product") {
        if (!g_admin.isAuthenticated()) return err(403, "Not authenticated");
        std::string type = jStr(r.body, "type");
        int    id    = jInt(r.body, "id");
        std::string name = jStr(r.body, "name");
        double price = jDbl(r.body, "price");
        int    qty   = jInt(r.body, "quantity");
        try {
            std::unique_ptr<Product> prod;
            if (type == "Electronics") {
                prod = std::unique_ptr<Product>(new Electronics(id, name, price, qty,
                    jStr(r.body,"brand"), jInt(r.body,"warranty")));
            } else if (type == "Clothing") {
                prod = std::unique_ptr<Product>(new Clothing(id, name, price, qty,
                    jStr(r.body,"size"), jStr(r.body,"fabric")));
            } else if (type == "Books") {
                prod = std::unique_ptr<Product>(new Book(id, name, price, qty,
                    jStr(r.body,"author"), jStr(r.body,"genre")));
            } else return err(400, "Unknown type");
            g_inv.addProduct(std::move(prod));
            return ok("{\"success\":true}");
        } catch (const std::exception& e) { return err(400, e.what()); }
    }

    // PUT /api/admin/product/123
    if (m == "PUT" && p.find("/api/admin/product/") == 0) {
        if (!g_admin.isAuthenticated()) return err(403, "Not authenticated");
        int id = std::stoi(r.lastSegment());
        double price = jDbl(r.body, "price");
        int    qty   = jInt(r.body, "quantity");
        bool updated = false;
        if (price > 0)  updated |= g_inv.updatePrice(id, price);
        if (qty >= 0)   updated |= g_inv.updateStock(id, qty);
        return updated ? ok("{\"success\":true}") : err(404, "Product not found");
    }

    // DELETE /api/admin/product/123
    if (m == "DELETE" && p.find("/api/admin/product/") == 0) {
        if (!g_admin.isAuthenticated()) return err(403, "Not authenticated");
        int id = std::stoi(r.lastSegment());
        return g_inv.removeProduct(id) ? ok("{\"success\":true}") : err(404, "Product not found");
    }

    return err(404, "Not found");
}

// ─── Main server loop ─────────────────────────────────────────────────────────

int main() {
    g_inv.initializeSampleData();
    g_hist.loadFromFile();
    CustomerManager::getInstance().loadFromFile();

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        std::cerr << "WSAStartup failed\n"; return 1;
    }

    SOCKET srv = socket(AF_INET, SOCK_STREAM, 0);
    if (srv == INVALID_SOCKET) { std::cerr << "socket() failed\n"; return 1; }

    int opt = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(8081);

    if (bind(srv, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "bind() failed — port 8080 in use?\n"; return 1;
    }
    listen(srv, SOMAXCONN);

    std::cout << "=== Shopping Management System API Server ===\n";
    std::cout << "Listening on http://localhost:8081\n";
    std::cout << "Press Ctrl+C to stop.\n";

    while (true) {
        SOCKET client = accept(srv, nullptr, nullptr);
        if (client == INVALID_SOCKET) continue;

        // Read request
        std::string raw;
        char buf[4096];
        int n;
        while ((n = recv(client, buf, sizeof(buf)-1, 0)) > 0) {
            buf[n] = '\0';
            raw += buf;
            // Stop reading when we have headers + body
            if (raw.find("\r\n\r\n") != std::string::npos) {
                // Check Content-Length to see if we have full body
                auto cl_pos = raw.find("Content-Length:");
                if (cl_pos == std::string::npos) break;
                int cl = std::stoi(raw.substr(cl_pos + 15));
                auto body_start = raw.find("\r\n\r\n") + 4;
                if ((int)(raw.size() - body_start) >= cl) break;
            }
        }

        Req req = parseRequest(raw);
        std::string resp = handle(req);

        send(client, resp.c_str(), (int)resp.size(), 0);
        closesocket(client);
    }

    closesocket(srv);
    WSACleanup();
    return 0;
}
