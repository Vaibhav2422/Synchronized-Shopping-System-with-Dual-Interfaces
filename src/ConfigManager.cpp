#include "ConfigManager.h"
#include "Logger.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

ConfigManager::ConfigManager() : configLoaded(false) {
    setDefaults();
    loadConfig(CONFIG_FILE);
}

void ConfigManager::setDefaults() {
    gstRate = DEFAULT_GST_RATE;
    cartDiscountThreshold = DEFAULT_CART_DISCOUNT_THRESHOLD;
    cartDiscountRate = DEFAULT_CART_DISCOUNT_RATE;
    adminUsername = DEFAULT_ADMIN_USERNAME;
    adminPassword = DEFAULT_ADMIN_PASSWORD;
    coupons = {
        {"SAVE10", 0.10},
        {"FESTIVE20", 0.20}
    };
}

// Extract the raw value string after a JSON key (handles strings and numbers)
static std::string extractValue(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t keyPos = json.find(searchKey);
    if (keyPos == std::string::npos) return "";

    size_t colonPos = json.find(':', keyPos + searchKey.size());
    if (colonPos == std::string::npos) return "";

    size_t valueStart = colonPos + 1;
    while (valueStart < json.size() &&
           (json[valueStart] == ' ' || json[valueStart] == '\t' ||
            json[valueStart] == '\n' || json[valueStart] == '\r'))
        ++valueStart;

    if (valueStart >= json.size()) return "";

    if (json[valueStart] == '"') {
        size_t strEnd = json.find('"', valueStart + 1);
        if (strEnd == std::string::npos) return "";
        return json.substr(valueStart + 1, strEnd - valueStart - 1);
    }

    size_t valueEnd = valueStart;
    while (valueEnd < json.size() &&
           json[valueEnd] != ',' && json[valueEnd] != '}' &&
           json[valueEnd] != '\n' && json[valueEnd] != '\r')
        ++valueEnd;

    std::string val = json.substr(valueStart, valueEnd - valueStart);
    while (!val.empty() && (val.back() == ' ' || val.back() == '\t'))
        val.pop_back();
    return val;
}

// Extract the content of a JSON object block for a given key
static std::string extractBlock(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t keyPos = json.find(searchKey);
    if (keyPos == std::string::npos) return "";

    size_t bracePos = json.find('{', keyPos + searchKey.size());
    if (bracePos == std::string::npos) return "";

    int depth = 0;
    size_t end = bracePos;
    for (; end < json.size(); ++end) {
        if (json[end] == '{') ++depth;
        else if (json[end] == '}') {
            --depth;
            if (depth == 0) break;
        }
    }
    return json.substr(bracePos, end - bracePos + 1);
}

// Parse key-value pairs from a flat JSON object block
static std::unordered_map<std::string, std::string> parseObjectPairs(const std::string& block) {
    std::unordered_map<std::string, std::string> result;
    size_t pos = 0;
    while (pos < block.size()) {
        size_t keyStart = block.find('"', pos);
        if (keyStart == std::string::npos) break;
        size_t keyEnd = block.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        std::string key = block.substr(keyStart + 1, keyEnd - keyStart - 1);

        size_t colonPos = block.find(':', keyEnd + 1);
        if (colonPos == std::string::npos) break;

        size_t valStart = colonPos + 1;
        while (valStart < block.size() &&
               (block[valStart] == ' ' || block[valStart] == '\t' || block[valStart] == '\n'))
            ++valStart;

        std::string value;
        if (valStart < block.size() && block[valStart] == '"') {
            size_t valEnd = block.find('"', valStart + 1);
            if (valEnd == std::string::npos) break;
            value = block.substr(valStart + 1, valEnd - valStart - 1);
            pos = valEnd + 1;
        } else {
            size_t valEnd = valStart;
            while (valEnd < block.size() &&
                   block[valEnd] != ',' && block[valEnd] != '}' && block[valEnd] != '\n')
                ++valEnd;
            value = block.substr(valStart, valEnd - valStart);
            while (!value.empty() && (value.back() == ' ' || value.back() == '\t'))
                value.pop_back();
            pos = valEnd + 1;
        }
        if (!key.empty()) result[key] = value;
    }
    return result;
}

void ConfigManager::loadConfig(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        Logger::logWarning("ConfigManager", "Config file not found: " + filePath + ". Using defaults.");
        std::cout << "[ConfigManager] Config file not found. Using default configuration.\n";
        return;
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    std::string json = ss.str();

    if (json.empty()) {
        Logger::logWarning("ConfigManager", "Config file is empty: " + filePath + ". Using defaults.");
        std::cout << "[ConfigManager] Config file is empty. Using default configuration.\n";
        return;
    }

    bool valid = true;

    try {
        // Parse gst_rate
        std::string gstStr = extractValue(json, "gst_rate");
        if (!gstStr.empty()) {
            double val = std::stod(gstStr);
            if (val >= 0.0 && val <= 1.0) {
                gstRate = val;
            } else {
                Logger::logWarning("ConfigManager", "gst_rate out of range [0,1]: " + gstStr + ". Using default.");
                valid = false;
            }
        }

        // Parse cart_discount_threshold
        std::string threshStr = extractValue(json, "cart_discount_threshold");
        if (!threshStr.empty()) {
            double val = std::stod(threshStr);
            if (val > 0.0) {
                cartDiscountThreshold = val;
            } else {
                Logger::logWarning("ConfigManager", "cart_discount_threshold must be positive. Using default.");
                valid = false;
            }
        }

        // Parse cart_discount_rate
        std::string rateStr = extractValue(json, "cart_discount_rate");
        if (!rateStr.empty()) {
            double val = std::stod(rateStr);
            if (val > 0.0) {
                cartDiscountRate = val;
            } else {
                Logger::logWarning("ConfigManager", "cart_discount_rate must be positive. Using default.");
                valid = false;
            }
        }

        // Parse admin block
        std::string adminBlock = extractBlock(json, "admin");
        if (!adminBlock.empty()) {
            auto pairs = parseObjectPairs(adminBlock);
            if (pairs.count("username") && !pairs["username"].empty()) {
                adminUsername = pairs["username"];
            } else {
                Logger::logWarning("ConfigManager", "admin.username missing or empty. Using default.");
                valid = false;
            }
            if (pairs.count("password") && !pairs["password"].empty()) {
                adminPassword = pairs["password"];
            } else {
                Logger::logWarning("ConfigManager", "admin.password missing or empty. Using default.");
                valid = false;
            }
        }

        // Parse coupons block
        std::string couponsBlock = extractBlock(json, "coupons");
        if (!couponsBlock.empty()) {
            auto pairs = parseObjectPairs(couponsBlock);
            if (!pairs.empty()) {
                coupons.clear();
                for (auto it2 = pairs.begin(); it2 != pairs.end(); ++it2) {
                    const std::string& code = it2->first;
                    const std::string& pctStr = it2->second;
                    try {
                        double pct = std::stod(pctStr);
                        if (pct >= 0.0 && pct <= 100.0) {
                            coupons[code] = pct / 100.0;  // store as fraction
                        } else {
                            Logger::logWarning("ConfigManager", "Coupon " + code + " discount out of range [0,100]. Skipping.");
                            valid = false;
                        }
                    } catch (...) {
                        Logger::logWarning("ConfigManager", "Invalid discount for coupon: " + code + ". Skipping.");
                        valid = false;
                    }
                }
            }
        }

        configLoaded = true;
        if (valid) {
            Logger::logInfo("ConfigManager", "Configuration loaded successfully from " + filePath);
            std::cout << "[ConfigManager] Configuration loaded from " << filePath << "\n";
        } else {
            Logger::logWarning("ConfigManager", "Configuration loaded with some invalid values; defaults used for those fields.");
            std::cout << "[ConfigManager] Configuration loaded with warnings. Some defaults applied.\n";
        }

    } catch (const std::exception& e) {
        Logger::logError("ConfigManager", std::string("Failed to parse config: ") + e.what() + ". Using defaults.");
        std::cout << "[ConfigManager] Config parse error: " << e.what() << ". Using default configuration.\n";
        setDefaults();
        configLoaded = false;
    }
}

double ConfigManager::getGSTRate() const { return gstRate; }
double ConfigManager::getCartDiscountThreshold() const { return cartDiscountThreshold; }
double ConfigManager::getCartDiscountRate() const { return cartDiscountRate; }
std::string ConfigManager::getAdminUsername() const { return adminUsername; }
std::string ConfigManager::getAdminPassword() const { return adminPassword; }
const std::unordered_map<std::string, double>& ConfigManager::getCoupons() const { return coupons; }
bool ConfigManager::isConfigLoaded() const { return configLoaded; }
