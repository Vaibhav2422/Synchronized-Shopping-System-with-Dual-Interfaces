/**
 * Customer Server — Standalone Winsock2 HTTP server on port 8082.
 * Handles customer authentication, personal dashboard, stats, orders, and loyalty.
 * Requirements: 3.1–3.5, 4.4, 5.1–5.7, 9.5
 */

#include "CustomerManager.h"
#include "OrderHistory.h"
#include "Logger.h"

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
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
#include <map>
#include <algorithm>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <cstdlib>

// ─── Session store ────────────────────────────────────────────────────────────
// Maps session token → customerId
static std::map<std::string, std::string> g_sessions;

// ─── Order history (shared file with shopkeeper server) ───────────────────────
static OrderHistory g_hist("data/orders.txt");

// ─── UUID-like token generation ───────────────────────────────────────────────
static std::string generateToken() {
    char buf[33];
    static const char hex[] = "0123456789abcdef";
    for (int i = 0; i < 32; ++i)
        buf[i] = hex[rand() % 16];
    buf[32] = '\0';
    return std::string(buf);
}

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

static std::string jStr(const std::string& body, const std::string& key) {
    std::string s = "\"" + key + "\"";
    auto p = body.find(s); if (p == std::string::npos) return "";
    p = body.find(':', p); if (p == std::string::npos) return "";
    p = body.find('"', p); if (p == std::string::npos) return "";
    auto e = body.find('"', p+1); if (e == std::string::npos) return "";
    return body.substr(p+1, e-p-1);
}

static int jInt(const std::string& body, const std::string& key) {
    std::string s = "\"" + key + "\"";
    auto p = body.find(s); if (p == std::string::npos) return 0;
    p = body.find(':', p); if (p == std::string::npos) return 0;
    p++;
    while (p < body.size() && (body[p]==' '||body[p]=='\t')) p++;
    try { return std::stoi(body.substr(p)); } catch (...) { return 0; }
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
      << "Access-Control-Allow-Headers: Content-Type, Authorization, X-Token\r\n"
      << "Connection: close\r\n"
      << "\r\n"
      << body;
    return r.str();
}

static std::string ok(const std::string& body)  { return httpResp(200, body); }
static std::string err(int c, const std::string& msg) {
    return httpResp(c, "{\"error\":\"" + esc(msg) + "\"}");
}

// ─── Request parsing ──────────────────────────────────────────────────────────
struct Req {
    std::string method, path, body, token;
    std::string param(const std::string& key) const {
        auto q = path.find('?');
        if (q == std::string::npos) return "";
        std::string qs = path.substr(q+1);
        std::string search = key + "=";
        auto p = qs.find(search);
        if (p == std::string::npos) return "";
        p += search.size();
        auto e = qs.find('&', p);
        return (e == std::string::npos) ? qs.substr(p) : qs.substr(p, e-p);
    }
    std::string pathBase() const {
        auto q = path.find('?');
        return (q == std::string::npos) ? path : path.substr(0, q);
    }
};

static Req parseRequest(const std::string& raw) {
    Req r;
    std::istringstream ss(raw);
    std::string line;
    std::getline(ss, line);
    std::istringstream ls(line);
    ls >> r.method >> r.path;

    // Parse headers for X-Token
    while (std::getline(ss, line)) {
        if (line == "\r" || line.empty()) break;
        // Check for X-Token header (case-insensitive prefix check)
        std::string lower = line;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (lower.find("x-token:") == 0) {
            r.token = line.substr(8);
            // trim whitespace
            size_t s = r.token.find_first_not_of(" \t\r\n");
            size_t e = r.token.find_last_not_of(" \t\r\n");
            if (s != std::string::npos) r.token = r.token.substr(s, e-s+1);
            else r.token = "";
        }
    }
    std::string rest;
    while (std::getline(ss, line)) rest += line + "\n";
    r.body = rest;
    return r;
}

// ─── Auth helper: resolve customerId from token ───────────────────────────────
static std::string resolveCustomer(const std::string& token) {
    if (token.empty()) return "";
    auto it = g_sessions.find(token);
    if (it == g_sessions.end()) return "";
    return it->second;
}

// ─── Stats computation ────────────────────────────────────────────────────────
struct CustomerStats {
    double totalSpent    = 0.0;
    double totalSaved    = 0.0;
    int    totalProducts = 0;
    std::string topCategory;
    int    loyaltyPoints = 0;
};

static CustomerStats computeStats(const CustomerRecord& cust) {
    CustomerStats stats;
    stats.loyaltyPoints = cust.loyaltyPoints;

    // Reload order history to get latest data
    g_hist.loadFromFile();
    const auto& orders = g_hist.getOrders();

    std::map<std::string, int> categoryCount;

    for (int oid : cust.orderIds) {
        for (const auto& o : orders) {
            if (o.getOrderId() == oid) {
                stats.totalSpent += o.getTotalAmount();
                for (const auto& prod : o.getProducts()) {
                    stats.totalProducts += prod.second;
                    // Heuristic: derive category from product name
                    // Electronics keywords
                    std::string nameLower = prod.first;
                    std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
                    std::string cat = "General";
                    if (nameLower.find("phone") != std::string::npos ||
                        nameLower.find("laptop") != std::string::npos ||
                        nameLower.find("tablet") != std::string::npos ||
                        nameLower.find("headphone") != std::string::npos ||
                        nameLower.find("camera") != std::string::npos ||
                        nameLower.find("tv") != std::string::npos ||
                        nameLower.find("speaker") != std::string::npos) {
                        cat = "Electronics";
                    } else if (nameLower.find("shirt") != std::string::npos ||
                               nameLower.find("jeans") != std::string::npos ||
                               nameLower.find("dress") != std::string::npos ||
                               nameLower.find("jacket") != std::string::npos ||
                               nameLower.find("saree") != std::string::npos ||
                               nameLower.find("kurta") != std::string::npos) {
                        cat = "Clothing";
                    } else if (nameLower.find("book") != std::string::npos ||
                               nameLower.find("novel") != std::string::npos ||
                               nameLower.find("guide") != std::string::npos) {
                        cat = "Books";
                    }
                    categoryCount[cat] += prod.second;
                }
                break;
            }
        }
    }

    // Approximate savings: GST portion (18%) of total
    // totalSpent already includes GST, so savings ≈ discounts applied
    // We use a simple heuristic: 0 (actual discount data not stored in orders.txt)
    stats.totalSaved = 0.0;

    // Top category
    if (!categoryCount.empty()) {
        auto best = std::max_element(categoryCount.begin(), categoryCount.end(),
            [](const std::pair<std::string,int>& a, const std::pair<std::string,int>& b){
                return a.second < b.second;
            });
        stats.topCategory = best->first;
    } else {
        stats.topCategory = "None";
    }

    return stats;
}

// ─── Route handler ────────────────────────────────────────────────────────────
static std::string handle(const Req& r) {
    const std::string& m = r.method;
    std::string p = r.pathBase();

    // CORS preflight
    if (m == "OPTIONS") return httpResp(204, "");

    // Health
    if (m == "GET" && p == "/api/health")
        return ok("{\"status\":\"ok\",\"server\":\"customer\",\"version\":\"1.0.0\"}");

    // ── POST /api/auth/login ──────────────────────────────────────────────────
    // Requirements: 3.1, 3.2, 3.3
    if (m == "POST" && p == "/api/auth/login") {
        std::string customerId = jStr(r.body, "customerId");
        std::string password   = jStr(r.body, "password");

        if (customerId.empty() || password.empty())
            return err(400, "customerId and password are required");

        // Reload from file to get latest data
        CustomerManager::getInstance().loadFromFile();

        if (!CustomerManager::getInstance().authenticate(customerId, password))
            return err(401, "Invalid credentials");

        std::string token = generateToken();
        g_sessions[token] = customerId;

        std::ostringstream o;
        o << "{\"success\":true,\"token\":\"" << esc(token) << "\""
          << ",\"customerId\":\"" << esc(customerId) << "\"}";
        return ok(o.str());
    }

    // ── GET /api/me ───────────────────────────────────────────────────────────
    // Requirements: 3.4, 3.5, 5.1
    if (m == "GET" && p == "/api/me") {
        std::string custId = resolveCustomer(r.token);
        if (custId.empty()) return err(401, "Unauthorized");

        CustomerManager::getInstance().loadFromFile();
        const CustomerRecord* cust = CustomerManager::getInstance().findById(custId);
        if (!cust) return err(404, "Customer not found");

        std::ostringstream o;
        o << "{\"customerId\":\"" << esc(cust->customerId) << "\""
          << ",\"name\":\""       << esc(cust->name)       << "\""
          << ",\"phone\":\""      << esc(cust->phone)       << "\""
          << ",\"loyaltyPoints\":" << cust->loyaltyPoints
          << ",\"createdAt\":\""  << esc(cust->createdAt)  << "\""
          << ",\"orderCount\":"   << cust->orderIds.size()
          << "}";
        return ok(o.str());
    }

    // ── GET /api/me/stats ─────────────────────────────────────────────────────
    // Requirements: 5.2, 5.3, 5.5, 5.6, 5.7
    if (m == "GET" && p == "/api/me/stats") {
        std::string custId = resolveCustomer(r.token);
        if (custId.empty()) return err(401, "Unauthorized");

        CustomerManager::getInstance().loadFromFile();
        const CustomerRecord* cust = CustomerManager::getInstance().findById(custId);
        if (!cust) return err(404, "Customer not found");

        CustomerStats stats = computeStats(*cust);

        std::ostringstream o; o << std::fixed;
        o << "{\"totalSpent\":"    << stats.totalSpent
          << ",\"totalSaved\":"    << stats.totalSaved
          << ",\"totalProducts\":" << stats.totalProducts
          << ",\"topCategory\":\"" << esc(stats.topCategory) << "\""
          << ",\"loyaltyPoints\":" << stats.loyaltyPoints
          << "}";
        return ok(o.str());
    }

    // ── GET /api/me/orders ────────────────────────────────────────────────────
    // Requirements: 5.4
    if (m == "GET" && p == "/api/me/orders") {
        std::string custId = resolveCustomer(r.token);
        if (custId.empty()) return err(401, "Unauthorized");

        CustomerManager::getInstance().loadFromFile();
        const CustomerRecord* cust = CustomerManager::getInstance().findById(custId);
        if (!cust) return err(404, "Customer not found");

        g_hist.loadFromFile();
        const auto& allOrders = g_hist.getOrders();

        std::string b = "[";
        bool first = true;
        for (int oid : cust->orderIds) {
            for (const auto& o : allOrders) {
                if (o.getOrderId() == oid) {
                    if (!first) b += ",";
                    first = false;
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
                    break;
                }
            }
        }
        return ok(b + "]");
    }

    // ── GET /api/me/loyalty ───────────────────────────────────────────────────
    // Requirements: 5.6, 9.5
    if (m == "GET" && p == "/api/me/loyalty") {
        std::string custId = resolveCustomer(r.token);
        if (custId.empty()) return err(401, "Unauthorized");

        CustomerManager::getInstance().loadFromFile();
        const CustomerRecord* cust = CustomerManager::getInstance().findById(custId);
        if (!cust) return err(404, "Customer not found");

        std::ostringstream o;
        o << "{\"loyaltyPoints\":" << cust->loyaltyPoints
          << ",\"redeemValue\":1.0"
          << ",\"rupeeValue\":"    << cust->loyaltyPoints
          << "}";
        return ok(o.str());
    }

    // ── POST /api/me/redeem ───────────────────────────────────────────────────
    // Requirements: 9.5
    if (m == "POST" && p == "/api/me/redeem") {
        std::string custId = resolveCustomer(r.token);
        if (custId.empty()) return err(401, "Unauthorized");

        CustomerManager::getInstance().loadFromFile();
        const CustomerRecord* cust = CustomerManager::getInstance().findById(custId);
        if (!cust) return err(404, "Customer not found");

        int points = jInt(r.body, "points");
        if (points <= 0) return err(400, "points must be a positive integer");

        try {
            CustomerManager::getInstance().redeemLoyaltyPoints(custId, points);
            CustomerManager::getInstance().saveToFile();

            const CustomerRecord* updated = CustomerManager::getInstance().findById(custId);
            std::ostringstream o;
            o << "{\"success\":true"
              << ",\"remainingPoints\":" << (updated ? updated->loyaltyPoints : 0)
              << "}";
            return ok(o.str());
        } catch (const std::exception& e) {
            return err(400, e.what());
        }
    }

    return err(404, "Not found");
}

// ─── Main server loop ─────────────────────────────────────────────────────────
int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    CustomerManager::getInstance().loadFromFile();
    g_hist.loadFromFile();

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
    addr.sin_port        = htons(8082);

    if (bind(srv, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "bind() failed — port 8082 in use?\n"; return 1;
    }
    listen(srv, SOMAXCONN);

    std::cout << "=== Customer Server ===\n";
    std::cout << "Listening on http://localhost:8082\n";
    std::cout << "Press Ctrl+C to stop.\n";

    while (true) {
        SOCKET client = accept(srv, nullptr, nullptr);
        if (client == INVALID_SOCKET) continue;

        std::string raw;
        char buf[4096];
        int n;
        while ((n = recv(client, buf, sizeof(buf)-1, 0)) > 0) {
            buf[n] = '\0';
            raw += buf;
            if (raw.find("\r\n\r\n") != std::string::npos) {
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
