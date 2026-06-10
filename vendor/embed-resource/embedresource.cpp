// -*- mode: c++; c-basic-offset: 4; encoding: utf-8; -*-

#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << std::format("USAGE: {} sym rsrc [--bin]\n\n  Creates {{sym}}.cpp from the contents of {{rsrc}}", argv[0]) << std::endl;
        return 1;
    }

    bool bin_mode = false;
    if (argc > 3) {
        std::string bin_arg{argv[3]};
        if (bin_arg == "--bin") {
            bin_mode = true;
        }
    }

    std::filesystem::path dst{argv[1]};
    std::filesystem::path src{argv[2]};

    std::string sym = src.filename().string();
    std::replace(sym.begin(), sym.end(), '.', '_');
    std::replace(sym.begin(), sym.end(), '-', '_');

    std::filesystem::create_directories(dst.parent_path());

    std::ofstream ofs{dst};

    auto mode = std::ios::ate;
    if (bin_mode) { mode |= std::ios::binary; }
    std::ifstream ifs{src, mode};
    auto file_size = ifs.tellg();
    std::vector<char> data(file_size);
    ifs.seekg(0, std::ios::beg);
    ifs.read(data.data(), file_size);
    ifs.close();

    ofs << "#include <vector>\n";
    ofs << "extern const std::vector<char> _resource_" << sym << " = {\n";

    size_t lineCount = 0;
    for (auto c : data) {
        ofs << std::hex << std::showbase << (c & 0xff) << ", ";
        if (++lineCount == 10) {
            ofs << "\n";
            lineCount = 0;
        }
    }

    ofs << "};" << std::endl;

    return 0;
}
