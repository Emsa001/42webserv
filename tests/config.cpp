#include <gtest/gtest.h>
#include <Webserv.hpp>
#include <fstream>
#include <string>
#include <vector>

class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup before each test
    }

    void TearDown() override {
        // Cleanup after each test
    }

    // Helper function to test if a config file is valid
    bool testConfigFile(const std::string& filename) {
        try {
            ConfigParser parser(filename);
            return parser.parse();
        } catch (const std::exception& e) {
            return false;
        }
    }

    // Helper function to get servers from a valid config
    config_array getServersFromConfig(const std::string& filename) {
        ConfigParser parser(filename);
        if (parser.parse()) {
            return parser.getServers();
        }
        return config_array();
    }
};

// ============ VALID CONFIG TESTS ============

// Test minimal valid configuration
TEST_F(ConfigTest, ValidMinimalConfig) {
    EXPECT_TRUE(testConfigFile("tests/valid_configs/minimal.yml"));
    
    config_array servers = getServersFromConfig("tests/valid_configs/minimal.yml");
    EXPECT_GE(servers.size(), 1);
}

// Test full featured configuration
TEST_F(ConfigTest, ValidFullFeaturedConfig) {
    EXPECT_TRUE(testConfigFile("tests/valid_configs/full_featured.yml"));
    
    config_array servers = getServersFromConfig("tests/valid_configs/full_featured.yml");
    EXPECT_GE(servers.size(), 1);
}

// Test unicode and special characters
TEST_F(ConfigTest, ValidUnicodeConfig) {
    EXPECT_TRUE(testConfigFile("tests/valid_configs/unicode_special_chars.yml"));
    
    config_array servers = getServersFromConfig("tests/valid_configs/unicode_special_chars.yml");
    EXPECT_GE(servers.size(), 1);
}

// Test performance stress configuration
TEST_F(ConfigTest, ValidPerformanceStressConfig) {
    EXPECT_TRUE(testConfigFile("tests/valid_configs/performance_stress.yml"));
    
    config_array servers = getServersFromConfig("tests/valid_configs/performance_stress.yml");
    EXPECT_GE(servers.size(), 1);
}

// Test extreme values configuration
TEST_F(ConfigTest, ValidExtremeValuesConfig) {
    EXPECT_TRUE(testConfigFile("tests/valid_configs/extreme_values.yml"));
    
    config_array servers = getServersFromConfig("tests/valid_configs/extreme_values.yml");
    EXPECT_GE(servers.size(), 1);
}

// Test duplicate server names (should be valid as they're on different ports)
TEST_F(ConfigTest, ValidDuplicateServerNamesConfig) {
    EXPECT_TRUE(testConfigFile("tests/valid_configs/duplicate_server_names.yml"));
    
    config_array servers = getServersFromConfig("tests/valid_configs/duplicate_server_names.yml");
    EXPECT_GE(servers.size(), 2);
}

// Test duplicate listen ports (should be valid if on different hosts)
TEST_F(ConfigTest, ValidDuplicateListenPortsConfig) {
    EXPECT_TRUE(testConfigFile("tests/valid_configs/duplicate_listen_ports.yml"));
    
    config_array servers = getServersFromConfig("tests/valid_configs/duplicate_listen_ports.yml");
    EXPECT_GE(servers.size(), 2);
}

// Test circular redirects configuration
TEST_F(ConfigTest, ValidCircularRedirectsConfig) {
    EXPECT_TRUE(testConfigFile("tests/valid_configs/circular_redirects.yml"));
    
    config_array servers = getServersFromConfig("tests/valid_configs/circular_redirects.yml");
    EXPECT_GE(servers.size(), 1);
}

// ============ INVALID CONFIG TESTS ============

// Test missing required server_name
TEST_F(ConfigTest, InvalidMissingServerName) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/missing_required_server_name.yml"));
}

// Test missing required listen
TEST_F(ConfigTest, InvalidMissingListen) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/missing_required_listen.yml"));
}

// Test missing required max_client_body_size
TEST_F(ConfigTest, InvalidMissingMaxClientBodySize) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/missing_required_max_client_body_size.yml"));
}

// Test missing required max_client_header_size
TEST_F(ConfigTest, InvalidMissingMaxClientHeaderSize) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/missing_required_max_client_header_size.yml"));
}

// Test missing required location path
TEST_F(ConfigTest, InvalidMissingLocationPath) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/missing_required_location_path.yml"));
}

// Test wrong data type for listen (should be STRING not INT)
TEST_F(ConfigTest, InvalidListenTypeInt) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/wrong_type_listen_int.yml"));
}

// Test wrong data type for server_name (should be STRING not INT)
TEST_F(ConfigTest, InvalidServerNameTypeInt) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/wrong_type_server_name_int.yml"));
}

// Test wrong data type for max_client_body_size (should be INT not STRING)
TEST_F(ConfigTest, InvalidBodySizeTypeString) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/wrong_type_max_client_body_size_string.yml"));
}

// Test wrong data type for keep_alive (should be INT not STRING)
TEST_F(ConfigTest, InvalidKeepAliveTypeString) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/wrong_type_keep_alive_string.yml"));
}

// Test wrong data type for autoindex (should be BOOL not STRING)
TEST_F(ConfigTest, InvalidAutoindexTypeString) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/wrong_type_autoindex_string.yml"));
}

// Test wrong data type for exact (should be BOOL not INT)
TEST_F(ConfigTest, InvalidExactTypeInt) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/wrong_type_exact_int.yml"));
}

// Test wrong data type for cgi (should be BOOL not STRING)
TEST_F(ConfigTest, InvalidCgiTypeString) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/wrong_type_cgi_string.yml"));
}

// Test unknown field in server block
TEST_F(ConfigTest, InvalidUnknownServerField) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/unknown_field_server.yml"));
}

// Test unknown field in location block
TEST_F(ConfigTest, InvalidUnknownLocationField) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/unknown_field_location.yml"));
}

// Test unknown field in root block
TEST_F(ConfigTest, InvalidUnknownRootField) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/unknown_field_root.yml"));
}

// Test malformed YAML with inconsistent indentation
TEST_F(ConfigTest, InvalidMalformedIndentation) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/malformed_indentation.yml"));
}

// Test malformed YAML with invalid syntax
TEST_F(ConfigTest, InvalidMalformedSyntax) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/malformed_syntax.yml"));
}

// Test empty configuration file
TEST_F(ConfigTest, InvalidEmptyConfig) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/empty_config.yml"));
}

// Test empty strings in required fields
TEST_F(ConfigTest, InvalidEmptyStrings) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/empty_strings.yml"));
}

// Test negative values where they shouldn't be allowed
TEST_F(ConfigTest, InvalidNegativeValues) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/negative_values.yml"));
}

// Test zero values where they shouldn't be allowed
TEST_F(ConfigTest, InvalidZeroValues) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/zero_values.yml"));
}

// Test extremely long strings that might cause buffer overflows
TEST_F(ConfigTest, InvalidExtremeLongStrings) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/extreme_long_strings_wrong_type.yml"));
}

// Test massive configuration with many servers - one has missing required field
TEST_F(ConfigTest, InvalidMassiveServers) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/massive_servers_missing_listen.yml"));
}

// Test massive configuration with many locations - one has missing required field
TEST_F(ConfigTest, InvalidMassiveLocations) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/massive_locations_missing_path.yml"));
}

// Test file that doesn't exist
TEST_F(ConfigTest, InvalidNonexistentFile) {
    EXPECT_FALSE(testConfigFile("tests/invalid_configs/nonexistent_file.yml"));
}
