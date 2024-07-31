#include <UnitTest/UnitTest.hpp>

int main(int argc, char **argv)
{
    std::filesystem::path current_dir = argv[0];
    std::filesystem::current_path(current_dir.parent_path().generic_string());
    ::testing::InitGoogleTest(&argc, argv);
    std::string DatabasePath = current_dir.parent_path().generic_string() + "/test/DB/AppInstaller.db";
    std::string VersionsDBPAth = current_dir.parent_path().generic_string() + "/test/DB/Versions.db";
    if (std::filesystem::exists(DatabasePath))
    {
        std::filesystem::remove(DatabasePath);
    }
    if (std::filesystem::exists(VersionsDBPAth))
    {
        std::filesystem::remove(VersionsDBPAth);
    }
    return RUN_ALL_TESTS();
}