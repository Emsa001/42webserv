#include <gtest/gtest.h>
#include <Webserv.hpp>

class LocationTest : public ::testing::Test {
protected:
    void SetUp() override {
        config.parse("tests/valid_configs/locations.yml");
        server = new Server(config.getServers()[0].getMap());
    }

    void TearDown() override {
        delete server;
    }

    Config& config = Config::instance();
    Server* server;
};

// Test exact path matching
TEST_F(LocationTest, ExactPathMatch) {
    const config_map* location = server->findLocation("/static");
    ASSERT_NE(location, nullptr);
    EXPECT_EQ(location->at("path").getString(), "/static");
    EXPECT_EQ(location->at("root").getString(), "/static/");
    EXPECT_TRUE(Config::getSafe(*location, "exact", false));
}

// Test exact path matching with trailing content (should not match exact location)
TEST_F(LocationTest, ExactPathNoMatchWithTrailing) {
    const config_map* location = server->findLocation("/static/file.html");
    ASSERT_NE(location, nullptr);
    // Should not match the exact "/static" location, should fall back to root "/"
    EXPECT_EQ(location->at("path").getString(), "/");
}

// Test extension-based matching - PHP files
TEST_F(LocationTest, ExtensionMatchPHP) {
    const config_map* location = server->findLocation("/index.php");
    ASSERT_NE(location, nullptr);
    EXPECT_EQ(Config::getSafe(*location, "extension", std::string("")).getString(), ".php");
    EXPECT_EQ(location->at("root").getString(), "/php/");
    EXPECT_TRUE(Config::getSafe(*location, "cgi", false));
}

// Test extension-based matching - PHP files in subdirectory
TEST_F(LocationTest, ExtensionMatchPHPSubdir) {
    const config_map* location = server->findLocation("/subdir/script.php");
    ASSERT_NE(location, nullptr);
    EXPECT_EQ(Config::getSafe(*location, "extension", std::string("")).getString(), ".php");
    EXPECT_EQ(location->at("root").getString(), "/php/");
}

// Test extension-based matching - Python files in cgi-bin
TEST_F(LocationTest, ExtensionMatchPythonCGI) {
    const config_map* location = server->findLocation("/cgi-bin/script.py");
    ASSERT_NE(location, nullptr);
    EXPECT_EQ(Config::getSafe(*location, "extension", std::string("")).getString(), ".py");
    EXPECT_EQ(location->at("root").getString(), "/cgi-bin/");
}

// Test extension-based matching - ZIP files in downloads
TEST_F(LocationTest, ExtensionMatchZIP) {
    const config_map* location = server->findLocation("/downloads/file.zip");
    ASSERT_NE(location, nullptr);
    EXPECT_EQ(Config::getSafe(*location, "extension", std::string("")).getString(), ".zip");
    EXPECT_EQ(location->at("root").getString(), "/downloads/");
}

// Test extension-based matching - wrong path for extension
TEST_F(LocationTest, ExtensionMatchWrongPath) {
    // .py extension but not in /cgi-bin path
    const config_map* location = server->findLocation("/other/script.py");
    ASSERT_NE(location, nullptr);
    // Should fall back to root location since extension location requires /cgi-bin path
    EXPECT_EQ(location->at("path").getString(), "/");
    EXPECT_EQ(location->at("root").getString(), "/cgi-bin/");
    EXPECT_EQ(location->at("cgi").getBool(), true);
}

// Test best path prefix matching
TEST_F(LocationTest, BestPathPrefixMatch) {
    const config_map* location = server->findLocation("/api/v1/users");
    ASSERT_NE(location, nullptr);
    // Should match the more specific "/api/v1" instead of "/api"
    EXPECT_EQ(location->at("path").getString(), "/api/v1");
    EXPECT_EQ(location->at("root").getString(), "/api/v1/");
}

// Test path prefix matching with shorter prefix
TEST_F(LocationTest, PathPrefixMatchShorter) {
    const config_map* location = server->findLocation("/api/users");
    ASSERT_NE(location, nullptr);
    // Should match "/api" since "/api/v1" doesn't match
    EXPECT_EQ(location->at("path").getString(), "/api");
    EXPECT_EQ(location->at("root").getString(), "/api/");
}

// Test root path matching
TEST_F(LocationTest, RootPathMatch) {
    const config_map* location = server->findLocation("/");
    ASSERT_NE(location, nullptr);
    EXPECT_EQ(location->at("path").getString(), "/");
    EXPECT_EQ(location->at("root").getString(), "/root/");
}

// Test fallback to root for unmatched paths
TEST_F(LocationTest, FallbackToRoot) {
    const config_map* location = server->findLocation("/unknown/path");
    ASSERT_NE(location, nullptr);
    // Should fall back to root location
    EXPECT_EQ(location->at("path").getString(), "/");
    EXPECT_EQ(location->at("root").getString(), "/root/");
}

// Test file without extension
TEST_F(LocationTest, FileWithoutExtension) {
    const config_map* location = server->findLocation("/uploads/document");
    ASSERT_NE(location, nullptr);
    // Should match "/uploads" path
    EXPECT_EQ(location->at("path").getString(), "/uploads");
    EXPECT_EQ(location->at("root").getString(), "/uploads/");
}

// Test path with multiple dots (only last one should be considered extension)
TEST_F(LocationTest, FileWithMultipleDots) {
    const config_map* location = server->findLocation("/file.backup.php");
    ASSERT_NE(location, nullptr);
    // Should match .php extension
    EXPECT_EQ(Config::getSafe(*location, "extension", std::string("")).getString(), ".php");
}

// Test extension priority over path matching
TEST_F(LocationTest, ExtensionPriorityOverPath) {
    const config_map* location = server->findLocation("/uploads/script.php");
    ASSERT_NE(location, nullptr);
    // Should match .php extension location instead of /uploads path location
    EXPECT_EQ(Config::getSafe(*location, "extension", std::string("")).getString(), ".php");
    EXPECT_EQ(location->at("root").getString(), "/php/");
}

// Test edge case: extension at the end of directory name
TEST_F(LocationTest, ExtensionInDirectoryName) {
    const config_map* location = server->findLocation("/folder.php/file.txt");
    ASSERT_NE(location, nullptr);
    // Should not match .php extension since .txt is the actual file extension
    EXPECT_EQ(location->at("path").getString(), "/");
}
