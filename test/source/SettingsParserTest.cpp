#include <SettingsParserTest.hpp>

#include <gtest/gtest.h>


TEST_F(SettingsParserTest, ValidConfig) {
    ConfigMock::Config config;
    config.logging.console.use = false;
    auto result = parseConfig(config);
    EXPECT_TRUE(result);
}

TEST_F(SettingsParserTest, HelpOption) {
    ConfigMock::Config config;
    config.logging.console.use = false;
    auto result = parseConfig(config, {"--help"});
    EXPECT_FALSE(result);
}