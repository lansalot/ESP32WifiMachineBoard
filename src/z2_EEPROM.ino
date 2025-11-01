#ifndef _Z2_EEPROM_INO_
#define _Z2_EEPROM_INO_
#include <Arduino.h>
#include <EEPROM.h>

// EEPROM Configuration Class
// Stores sketch ID and up to 3 WiFi network credentials

#define EEPROM_SIZE 512
#define MAGIC_ID 0x1309
#define MAX_WIFI_NETWORKS 3
#define SSID_MAX_LENGTH 32
#define PASSWORD_MAX_LENGTH 64

class EEPROMConfig {
private:
  struct WifiNetwork {
    char ssid[SSID_MAX_LENGTH + 1];
    char password[PASSWORD_MAX_LENGTH + 1];
  };

  struct ConfigData {
    uint16_t magicID;
    WifiNetwork networks[MAX_WIFI_NETWORKS];
  };

  ConfigData config;

public:
  // Initialize EEPROM
  void begin() {
    EEPROM.begin(EEPROM_SIZE);
    Serial.println("EEPROM initialized");
  }

  // Check if EEPROM contains valid data from this sketch
  bool isValid() {
    uint16_t storedID;
    EEPROM.get(0, storedID);
    return (storedID == MAGIC_ID);
  }

  // Initialize EEPROM with default values
  void initializeDefaults() {
    Serial.println("Initializing EEPROM with default values...");
    config.magicID = MAGIC_ID;
    
    // Clear all network entries
    for (int i = 0; i < MAX_WIFI_NETWORKS; i++) {
      memset(config.networks[i].ssid, 0, SSID_MAX_LENGTH + 1);
      memset(config.networks[i].password, 0, PASSWORD_MAX_LENGTH + 1);
    }
    
    saveToEEPROM();
  }

  // Save configuration to EEPROM
  void saveToEEPROM() {
    EEPROM.put(0, config);
    EEPROM.commit();
    Serial.println("Configuration saved to EEPROM");
  }

  // Load configuration from EEPROM
  void loadFromEEPROM() {
    if (!isValid()) {
      Serial.println("EEPROM data invalid or empty. Initializing with defaults.");
      initializeDefaults();
    } else {
      EEPROM.get(0, config);
      Serial.println("Configuration loaded from EEPROM");
      Serial.print("Magic ID verified: 0x");
      Serial.println(config.magicID, HEX);
    }
  }

  // Set WiFi network credentials at specified index (0-2)
  bool setWifiNetwork(uint8_t index, const char* ssid, const char* password) {
    if (index >= MAX_WIFI_NETWORKS) {
      Serial.println("Error: Network index out of range (0-2)");
      return false;
    }

    if (strlen(ssid) > SSID_MAX_LENGTH) {
      Serial.println("Error: SSID too long");
      return false;
    }

    if (strlen(password) > PASSWORD_MAX_LENGTH) {
      Serial.println("Error: Password too long");
      return false;
    }

    strncpy(config.networks[index].ssid, ssid, SSID_MAX_LENGTH);
    config.networks[index].ssid[SSID_MAX_LENGTH] = '\0';
    
    strncpy(config.networks[index].password, password, PASSWORD_MAX_LENGTH);
    config.networks[index].password[PASSWORD_MAX_LENGTH] = '\0';

    Serial.print("WiFi network ");
    Serial.print(index);
    Serial.print(" set: ");
    Serial.println(ssid);
    
    return true;
  }

  // Get WiFi SSID at specified index
  const char* getSSID(uint8_t index) {
    if (index >= MAX_WIFI_NETWORKS) {
      return "";
    }
    return config.networks[index].ssid;
  }

  // Get WiFi password at specified index
  const char* getPassword(uint8_t index) {
    if (index >= MAX_WIFI_NETWORKS) {
      return "";
    }
    return config.networks[index].password;
  }

  // Check if network at index is configured (has non-empty SSID)
  bool isNetworkConfigured(uint8_t index) {
    if (index >= MAX_WIFI_NETWORKS) {
      return false;
    }
    return (strlen(config.networks[index].ssid) > 0);
  }

  // Clear a specific network entry
  void clearNetwork(uint8_t index) {
    if (index >= MAX_WIFI_NETWORKS) {
      Serial.println("Error: Network index out of range (0-2)");
      return;
    }
    
    memset(config.networks[index].ssid, 0, SSID_MAX_LENGTH + 1);
    memset(config.networks[index].password, 0, PASSWORD_MAX_LENGTH + 1);
    
    Serial.print("Network ");
    Serial.print(index);
    Serial.println(" cleared");
  }

  // Clear all networks
  void clearAllNetworks() {
    for (int i = 0; i < MAX_WIFI_NETWORKS; i++) {
      clearNetwork(i);
    }
  }

  // Print all stored networks (for debugging)
  void printNetworks() {
    Serial.println("\n===== Stored WiFi Networks =====");
    for (int i = 0; i < MAX_WIFI_NETWORKS; i++) {
      Serial.print("Network ");
      Serial.print(i);
      Serial.print(": ");
      if (isNetworkConfigured(i)) {
        Serial.print("SSID: ");
        Serial.print(config.networks[i].ssid);
        Serial.print(" | Password: ");
        // Print masked password for security
        for (size_t j = 0; j < strlen(config.networks[i].password); j++) {
          Serial.print("*");
        }
        Serial.println();
      } else {
        Serial.println("(not configured)");
      }
    }
    Serial.println("================================\n");
  }

  // Get the magic ID (for verification)
  uint16_t getMagicID() {
    return config.magicID;
  }
};

// Global instance
EEPROMConfig eepromConfig;
#endif