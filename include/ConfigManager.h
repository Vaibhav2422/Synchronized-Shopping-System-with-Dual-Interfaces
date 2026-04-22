#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <string>
#include <unordered_map>

/**
 * ConfigManager singleton class for external configuration management.
 * Loads configuration from data/config.json at startup.
 * Falls back to hardcoded defaults if config file is missing or invalid.
 * Requirements: 23.1, 23.2, 23.3, 23.4, 23.5
 */
class ConfigManager {
public:
    static ConfigManager& getInstance();

    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    ConfigManager(ConfigManager&&) = delete;
    ConfigManager& operator=(ConfigManager&&) = delete;

    double getGSTRate() const;
    double getCartDiscountThreshold() const;
    double getCartDiscountRate() const;
    std::string getAdminUsername() const;
    std::string getAdminPassword() const;
    const std::unordered_map<std::string, double>& getCoupons() const;

    bool isConfigLoaded() const;

private:
    ConfigManager();

    void setDefaults();
    void loadConfig(const std::string& filePath);

    // Parsed values
    double gstRate;
    double cartDiscountThreshold;
    double cartDiscountRate;
    std::string adminUsername;
    std::string adminPassword;
    std::unordered_map<std::string, double> coupons;
    bool configLoaded;

    static constexpr const char* CONFIG_FILE = "data/config.json";

    // Default values
    static constexpr double DEFAULT_GST_RATE = 0.18;
    static constexpr double DEFAULT_CART_DISCOUNT_THRESHOLD = 5000.0;
    static constexpr double DEFAULT_CART_DISCOUNT_RATE = 0.05;
    static constexpr const char* DEFAULT_ADMIN_USERNAME = "admin";
    static constexpr const char* DEFAULT_ADMIN_PASSWORD = "admin123";
};

#endif // CONFIGMANAGER_H
