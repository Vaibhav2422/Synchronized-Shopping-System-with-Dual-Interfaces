#include "CustomerManager.h"
#include "Logger.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <cstdint>
#include <cstring>

// ─────────────────────────────────────────────────────────────────────────────
// Pure C++ SHA-256 implementation (no OpenSSL)
// Based on the FIPS 180-4 specification.
// ─────────────────────────────────────────────────────────────────────────────
namespace sha256_impl {

static const uint32_t K[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,
    0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
    0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
    0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,
    0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
    0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,
    0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,
    0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
    0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

static inline uint32_t rotr(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
}

static inline uint32_t ch(uint32_t x, uint32_t y, uint32_t z)  { return (x & y) ^ (~x & z); }
static inline uint32_t maj(uint32_t x, uint32_t y, uint32_t z) { return (x & y) ^ (x & z) ^ (y & z); }
static inline uint32_t sig0(uint32_t x) { return rotr(x,2)  ^ rotr(x,13) ^ rotr(x,22); }
static inline uint32_t sig1(uint32_t x) { return rotr(x,6)  ^ rotr(x,11) ^ rotr(x,25); }
static inline uint32_t gam0(uint32_t x) { return rotr(x,7)  ^ rotr(x,18) ^ (x >> 3);  }
static inline uint32_t gam1(uint32_t x) { return rotr(x,17) ^ rotr(x,19) ^ (x >> 10); }

std::string hash(const std::string& input) {
    // Initial hash values
    uint32_t h[8] = {
        0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
        0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19
    };

    // Pre-processing: build padded message
    std::vector<uint8_t> msg(input.begin(), input.end());
    uint64_t bitLen = static_cast<uint64_t>(input.size()) * 8;
    msg.push_back(0x80);
    while (msg.size() % 64 != 56) msg.push_back(0x00);
    for (int i = 7; i >= 0; --i)
        msg.push_back(static_cast<uint8_t>((bitLen >> (i * 8)) & 0xFF));

    // Process each 512-bit (64-byte) chunk
    for (size_t chunk = 0; chunk < msg.size(); chunk += 64) {
        uint32_t w[64];
        for (int i = 0; i < 16; ++i) {
            w[i] = (static_cast<uint32_t>(msg[chunk + i*4])     << 24) |
                   (static_cast<uint32_t>(msg[chunk + i*4 + 1]) << 16) |
                   (static_cast<uint32_t>(msg[chunk + i*4 + 2]) <<  8) |
                   (static_cast<uint32_t>(msg[chunk + i*4 + 3]));
        }
        for (int i = 16; i < 64; ++i)
            w[i] = gam1(w[i-2]) + w[i-7] + gam0(w[i-15]) + w[i-16];

        uint32_t a=h[0],b=h[1],c=h[2],d=h[3],e=h[4],f=h[5],g=h[6],hh=h[7];
        for (int i = 0; i < 64; ++i) {
            uint32_t t1 = hh + sig1(e) + ch(e,f,g) + K[i] + w[i];
            uint32_t t2 = sig0(a) + maj(a,b,c);
            hh=g; g=f; f=e; e=d+t1;
            d=c;  c=b; b=a; a=t1+t2;
        }
        h[0]+=a; h[1]+=b; h[2]+=c; h[3]+=d;
        h[4]+=e; h[5]+=f; h[6]+=g; h[7]+=hh;
    }

    // Produce hex digest
    char hex[65];
    for (int i = 0; i < 8; ++i)
        std::snprintf(hex + i*8, 9, "%08x", h[i]);
    hex[64] = '\0';
    return std::string(hex);
}

} // namespace sha256_impl

// ─────────────────────────────────────────────────────────────────────────────
// Minimal JSON helpers (no external library)
// ─────────────────────────────────────────────────────────────────────────────
static std::string jsonEscape(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"')  out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else out += c;
    }
    return out;
}

static std::string jsonUnescape(const std::string& s) {
    std::string out;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            ++i;
            if      (s[i] == '"')  out += '"';
            else if (s[i] == '\\') out += '\\';
            else if (s[i] == 'n')  out += '\n';
            else if (s[i] == 'r')  out += '\r';
            else if (s[i] == 't')  out += '\t';
            else { out += '\\'; out += s[i]; }
        } else {
            out += s[i];
        }
    }
    return out;
}

// Extract a string value for a given key from a flat JSON object string
static std::string extractStr(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t kp = json.find(searchKey);
    if (kp == std::string::npos) return "";
    size_t cp = json.find(':', kp + searchKey.size());
    if (cp == std::string::npos) return "";
    size_t vs = cp + 1;
    while (vs < json.size() && (json[vs]==' '||json[vs]=='\t'||json[vs]=='\n'||json[vs]=='\r')) ++vs;
    if (vs >= json.size() || json[vs] != '"') return "";
    size_t ve = vs + 1;
    while (ve < json.size()) {
        if (json[ve] == '\\') { ve += 2; continue; }
        if (json[ve] == '"') break;
        ++ve;
    }
    return jsonUnescape(json.substr(vs + 1, ve - vs - 1));
}

// Extract an integer value for a given key
static int extractInt(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t kp = json.find(searchKey);
    if (kp == std::string::npos) return 0;
    size_t cp = json.find(':', kp + searchKey.size());
    if (cp == std::string::npos) return 0;
    size_t vs = cp + 1;
    while (vs < json.size() && (json[vs]==' '||json[vs]=='\t'||json[vs]=='\n'||json[vs]=='\r')) ++vs;
    size_t ve = vs;
    while (ve < json.size() && json[ve] != ',' && json[ve] != '}' && json[ve] != '\n') ++ve;
    std::string val = json.substr(vs, ve - vs);
    try { return std::stoi(val); } catch (...) { return 0; }
}

// Extract an array of ints for a given key (e.g. "orderIds": [1, 4, 7])
static std::vector<int> extractIntArray(const std::string& json, const std::string& key) {
    std::vector<int> result;
    std::string searchKey = "\"" + key + "\"";
    size_t kp = json.find(searchKey);
    if (kp == std::string::npos) return result;
    size_t cp = json.find(':', kp + searchKey.size());
    if (cp == std::string::npos) return result;
    size_t ab = json.find('[', cp);
    if (ab == std::string::npos) return result;
    size_t ae = json.find(']', ab);
    if (ae == std::string::npos) return result;
    std::string arr = json.substr(ab + 1, ae - ab - 1);
    std::istringstream ss(arr);
    std::string token;
    while (std::getline(ss, token, ',')) {
        // trim
        size_t s = token.find_first_not_of(" \t\n\r");
        if (s == std::string::npos) continue;
        size_t e = token.find_last_not_of(" \t\n\r");
        token = token.substr(s, e - s + 1);
        try { result.push_back(std::stoi(token)); } catch (...) {}
    }
    return result;
}

// ─────────────────────────────────────────────────────────────────────────────
// CustomerManager implementation
// ─────────────────────────────────────────────────────────────────────────────

CustomerManager& CustomerManager::getInstance() {
    static CustomerManager instance;
    return instance;
}

CustomerManager::CustomerManager() : nextIndex(1) {
    loadFromFile();
}

std::string CustomerManager::toLower(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    return out;
}

std::string CustomerManager::generateId() const {
    char buf[16];
    std::snprintf(buf, sizeof(buf), "CUST%04d", nextIndex);
    return std::string(buf);
}

std::string CustomerManager::hashPassword(const std::string& password) {
    return sha256_impl::hash(password);
}

int CustomerManager::computePointsEarned(double amount) {
    return static_cast<int>(std::floor(amount / 100.0));
}

CustomerRecord CustomerManager::registerCustomer(const std::string& name,
                                                  const std::string& phone,
                                                  const std::string& password) {
    CustomerRecord rec;
    rec.customerId    = generateId();
    rec.name          = name;
    rec.phone         = phone;
    rec.passwordHash  = hashPassword(password);
    rec.loyaltyPoints = 0;
    rec.createdAt     = ""; // caller may set if needed

    customers.push_back(rec);
    const_cast<int&>(nextIndex)++;
    saveToFile();
    Logger::logInfo("CustomerManager", "Registered customer: " + rec.customerId);
    return rec;
}

std::vector<CustomerRecord> CustomerManager::search(const std::string& query) const {
    std::string q = toLower(query);
    std::vector<CustomerRecord> results;
    for (const auto& c : customers) {
        if (toLower(c.customerId) == q ||
            toLower(c.phone) == q ||
            toLower(c.name).find(q) != std::string::npos) {
            results.push_back(c);
        }
    }
    return results;
}

CustomerRecord* CustomerManager::findById(const std::string& id) {
    for (auto& c : customers)
        if (c.customerId == id) return &c;
    return nullptr;
}

const CustomerRecord* CustomerManager::findById(const std::string& id) const {
    for (const auto& c : customers)
        if (c.customerId == id) return &c;
    return nullptr;
}

CustomerRecord* CustomerManager::findByPhone(const std::string& phone) {
    for (auto& c : customers)
        if (c.phone == phone) return &c;
    return nullptr;
}

bool CustomerManager::authenticate(const std::string& id, const std::string& password) const {
    const CustomerRecord* rec = findById(id);
    if (!rec) return false;
    return rec->passwordHash == hashPassword(password);
}

void CustomerManager::addLoyaltyPoints(const std::string& customerId, int points) {
    CustomerRecord* rec = findById(customerId);
    if (!rec) throw std::runtime_error("Customer not found: " + customerId);
    rec->loyaltyPoints += points;
    saveToFile();
}

void CustomerManager::redeemLoyaltyPoints(const std::string& customerId, int points) {
    CustomerRecord* rec = findById(customerId);
    if (!rec) throw std::runtime_error("Customer not found: " + customerId);
    if (rec->loyaltyPoints < points)
        throw std::runtime_error("Insufficient loyalty points");
    rec->loyaltyPoints -= points;
    saveToFile();
}

void CustomerManager::linkOrder(const std::string& customerId, int orderId) {
    CustomerRecord* rec = findById(customerId);
    if (!rec) throw std::runtime_error("Customer not found: " + customerId);
    rec->orderIds.push_back(orderId);
    saveToFile();
}

const std::vector<CustomerRecord>& CustomerManager::getAllCustomers() const {
    return customers;
}

void CustomerManager::reset() {
    customers.clear();
    nextIndex = 1;
}

// ─── Persistence ─────────────────────────────────────────────────────────────

void CustomerManager::saveToFile() const {
    std::ofstream f(DATA_FILE);
    if (!f.is_open()) {
        Logger::logError("CustomerManager", "Cannot open " + std::string(DATA_FILE) + " for writing");
        return;
    }

    f << "{\n  \"customers\": [\n";
    for (size_t i = 0; i < customers.size(); ++i) {
        const auto& c = customers[i];
        f << "    {\n";
        f << "      \"customerId\": \""   << jsonEscape(c.customerId)   << "\",\n";
        f << "      \"name\": \""         << jsonEscape(c.name)         << "\",\n";
        f << "      \"phone\": \""        << jsonEscape(c.phone)        << "\",\n";
        f << "      \"passwordHash\": \"" << jsonEscape(c.passwordHash) << "\",\n";
        f << "      \"loyaltyPoints\": "  << c.loyaltyPoints            << ",\n";
        f << "      \"orderIds\": [";
        for (size_t j = 0; j < c.orderIds.size(); ++j) {
            if (j) f << ", ";
            f << c.orderIds[j];
        }
        f << "],\n";
        f << "      \"createdAt\": \""    << jsonEscape(c.createdAt)    << "\"\n";
        f << "    }";
        if (i + 1 < customers.size()) f << ",";
        f << "\n";
    }
    f << "  ],\n";
    f << "  \"nextIndex\": " << nextIndex << "\n";
    f << "}\n";
}

void CustomerManager::loadFromFile() {
    std::ifstream f(DATA_FILE);
    if (!f.is_open()) {
        Logger::logWarning("CustomerManager",
            std::string(DATA_FILE) + " not found. Starting with empty customer list.");
        return;
    }

    std::ostringstream ss;
    ss << f.rdbuf();
    std::string json = ss.str();
    if (json.empty()) {
        Logger::logWarning("CustomerManager", "customers.json is empty.");
        return;
    }

    customers.clear();

    // Find the customers array
    size_t arrStart = json.find("\"customers\"");
    if (arrStart == std::string::npos) return;
    size_t bracketOpen = json.find('[', arrStart);
    if (bracketOpen == std::string::npos) return;

    // Walk through each object in the array
    size_t pos = bracketOpen + 1;
    while (pos < json.size()) {
        size_t objStart = json.find('{', pos);
        if (objStart == std::string::npos) break;

        // Find matching closing brace
        int depth = 0;
        size_t objEnd = objStart;
        for (; objEnd < json.size(); ++objEnd) {
            if (json[objEnd] == '{') ++depth;
            else if (json[objEnd] == '}') { --depth; if (depth == 0) break; }
        }
        std::string obj = json.substr(objStart, objEnd - objStart + 1);

        CustomerRecord rec;
        rec.customerId    = extractStr(obj, "customerId");
        rec.name          = extractStr(obj, "name");
        rec.phone         = extractStr(obj, "phone");
        rec.passwordHash  = extractStr(obj, "passwordHash");
        rec.loyaltyPoints = extractInt(obj, "loyaltyPoints");
        rec.orderIds      = extractIntArray(obj, "orderIds");
        rec.createdAt     = extractStr(obj, "createdAt");

        if (!rec.customerId.empty())
            customers.push_back(rec);

        pos = objEnd + 1;
    }

    // Load nextIndex
    int ni = extractInt(json, "nextIndex");
    if (ni > 0) nextIndex = ni;
    else nextIndex = static_cast<int>(customers.size()) + 1;

    Logger::logInfo("CustomerManager",
        "Loaded " + std::to_string(customers.size()) + " customers from file.");
}
