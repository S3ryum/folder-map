#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <map>
#include <string>
#include <system_error>
#include <vector>

namespace fs = std::filesystem;
struct Summary { std::uintmax_t bytes = 0; std::size_t files = 0; };

int main(int argc, char* argv[]) {
    const fs::path root = argc > 1 ? fs::path(argv[1]) : fs::current_path();
    std::error_code error;
    if (!fs::is_directory(root, error)) {
        std::cerr << "Not a readable directory: " << root.string() << '\n';
        return 2;
    }
    std::map<std::string, Summary> by_extension;
    std::uintmax_t total_bytes = 0;
    std::size_t total_files = 0, skipped = 0;
    fs::recursive_directory_iterator it(root, fs::directory_options::skip_permission_denied, error);
    const fs::recursive_directory_iterator end;
    while (it != end) {
        if (error) { ++skipped; error.clear(); it.increment(error); continue; }
        std::error_code item_error;
        if (it->is_regular_file(item_error) && !item_error) {
            const auto bytes = it->file_size(item_error);
            if (!item_error) {
                std::string extension = it->path().extension().string();
                if (extension.empty()) extension = "[no extension]";
                auto& summary = by_extension[extension];
                summary.bytes += bytes;
                ++summary.files;
                total_bytes += bytes;
                ++total_files;
            } else { ++skipped; }
        } else if (item_error) { ++skipped; }
        it.increment(error);
    }
    std::vector<std::pair<std::string, Summary>> rows(by_extension.begin(), by_extension.end());
    std::sort(rows.begin(), rows.end(), [](const auto& left, const auto& right) {
        return left.second.bytes > right.second.bytes;
    });
    std::cout << "Folder Map: " << root.string() << '\n';
    std::cout << "Files: " << total_files << " | Total size: " << total_bytes << " bytes\n";
    std::cout << "By extension:\n";
    for (const auto& [extension, summary] : rows) {
        std::cout << "  " << extension << "  " << summary.files << " files  "
                  << summary.bytes << " bytes\n";
    }
    if (skipped > 0) std::cout << "Unreadable entries skipped: " << skipped << '\n';
    return 0;
}
