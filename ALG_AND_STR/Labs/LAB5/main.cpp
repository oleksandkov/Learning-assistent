#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <random>
#include <cassert>
#include <cstdint>
#include <cstring>

inline uint32_t rotl32(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

class Shabal256 {
private:
    uint32_t a[12];
    uint32_t b[16];
    uint32_t c[16];
    uint32_t wlow;
    uint32_t whigh;
    uint8_t buffer[64];
    size_t buf_len;

    static constexpr uint32_t A_INIT[12] = {
        0x52F84552, 0xE54B7999, 0x2D8EE3EC, 0xB9645191,
        0xE0078B86, 0xBB7C44C9, 0xD2B5C1CA, 0xB0D2EB8C,
        0x14CE5A45, 0x22AF50DC, 0xEFFDBC6B, 0xEB21B74A
    };

    static constexpr uint32_t B_INIT[16] = {
        0xB555C6EE, 0x3E710596, 0xA72A652F, 0x9301515F,
        0xDA28C1FA, 0x696FD868, 0x9CB6BF72, 0x0AFE4002,
        0xA6E03615, 0x5138C1D4, 0xBE216306, 0xB38B8890,
        0x3EA8B96B, 0x3299ACE4, 0x30924DD4, 0x55CB34A5
    };

    static constexpr uint32_t C_INIT[16] = {
        0xB405F031, 0xC4233EBA, 0xB3733979, 0xC0DD9D55,
        0xC51C28AE, 0xA327B8E1, 0x56C56167, 0xED614433,
        0x88B59D60, 0x60E2CEBA, 0x758B4B8B, 0x83E82A7F,
        0xBC968828, 0xE6E00BF7, 0xBA839E55, 0x9B491C60
    };

    void perm(const uint32_t* m) {
        for (int i = 0; i < 16; ++i) {
            b[i] = rotl32(b[i], 17);
        }
        for (int i = 0; i < 48; ++i) {
            int xa0 = i % 12;
            int xa1 = (i + 11) % 12;
            int xb0 = i % 16;
            int xb1 = (i + 13) % 16;
            int xb2 = (i + 9) % 16;
            int xb3 = (i + 6) % 16;
            int xc0 = (24 - (i % 16)) % 16;
            uint32_t xm = m[i % 16];

            uint32_t t1 = rotl32(a[xa1], 15);
            uint32_t t2 = t1 * 5U;
            uint32_t t3 = (a[xa0] ^ t2 ^ c[xc0]) * 3U;
            a[xa0] = t3 ^ b[xb1] ^ (b[xb2] & ~b[xb3]) ^ xm;
            uint32_t t = rotl32(b[xb0], 1);
            b[xb0] = ~(t ^ a[xa0]);
        }
        for (int j = 0; j < 36; ++j) {
            int a_idx = (11 - (j % 12) + 12) % 12;
            int c_idx = (6 - (j % 16) + 16) % 16;
            a[a_idx] += c[c_idx];
        }
    }

    void xor_w() {
        a[0] ^= wlow;
        a[1] ^= whigh;
    }

    void incr_w() {
        if (++wlow == 0) {
            ++whigh;
        }
    }

    void process_block(const uint8_t* block) {
        uint32_t m[16];
        for (int i = 0; i < 16; ++i) {
            m[i] = (uint32_t)block[4 * i] |
                   ((uint32_t)block[4 * i + 1] << 8) |
                   ((uint32_t)block[4 * i + 2] << 16) |
                   ((uint32_t)block[4 * i + 3] << 24);
            b[i] += m[i];
        }
        xor_w();
        perm(m);
        for (int i = 0; i < 16; ++i) {
            c[i] -= m[i];
            std::swap(b[i], c[i]);
        }
        incr_w();
    }

public:
    Shabal256() {
        init();
    }

    void init() {
        std::memcpy(a, A_INIT, sizeof(a));
        std::memcpy(b, B_INIT, sizeof(b));
        std::memcpy(c, C_INIT, sizeof(c));
        wlow = 1;
        whigh = 0;
        buf_len = 0;
    }

    void update(const void* data, size_t len) {
        const uint8_t* ptr = static_cast<const uint8_t*>(data);
        while (len > 0) {
            size_t take = std::min(len, 64 - buf_len);
            std::memcpy(buffer + buf_len, ptr, take);
            buf_len += take;
            ptr += take;
            len -= take;
            if (buf_len == 64) {
                process_block(buffer);
                buf_len = 0;
            }
        }
    }

    std::array<uint8_t, 32> finalize() {
        uint8_t pad[64];
        std::memcpy(pad, buffer, buf_len);
        pad[buf_len] = 0x80;
        std::memset(pad + buf_len + 1, 0, 63 - buf_len);

        uint32_t m[16];
        for (int i = 0; i < 16; ++i) {
            m[i] = (uint32_t)pad[4 * i] |
                   ((uint32_t)pad[4 * i + 1] << 8) |
                   ((uint32_t)pad[4 * i + 2] << 16) |
                   ((uint32_t)pad[4 * i + 3] << 24);
            b[i] += m[i];
        }
        xor_w();
        perm(m);
        for (int round = 0; round < 3; ++round) {
            for (int i = 0; i < 16; ++i) {
                std::swap(b[i], c[i]);
            }
            xor_w();
            perm(m);
        }

        std::array<uint8_t, 32> out{};
        for (int i = 0; i < 8; ++i) {
            uint32_t word = b[8 + i];
            out[4 * i]     = static_cast<uint8_t>(word);
            out[4 * i + 1] = static_cast<uint8_t>(word >> 8);
            out[4 * i + 2] = static_cast<uint8_t>(word >> 16);
            out[4 * i + 3] = static_cast<uint8_t>(word >> 24);
        }
        return out;
    }

    static std::string to_hex(const std::array<uint8_t, 32>& digest) {
        std::ostringstream oss;
        for (uint8_t byte : digest) {
            oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        }
        return oss.str();
    }

    static std::string hash_string(const std::string& str) {
        Shabal256 ctx;
        ctx.update(str.data(), str.size());
        return to_hex(ctx.finalize());
    }

    static uint64_t hash64(const std::string& str) {
        Shabal256 ctx;
        ctx.update(str.data(), str.size());
        auto digest = ctx.finalize();
        uint64_t res = 0;
        for (int i = 0; i < 8; ++i) {
            res |= (static_cast<uint64_t>(digest[i]) << (8 * i));
        }
        return res;
    }
};

class MultiplicativeHash {
public:
    static uint64_t hash64(const std::string& str) {
        uint64_t key = 14695981039346656037ULL;
        for (char ch : str) {
            key = (key ^ static_cast<uint8_t>(ch)) * 1099511628211ULL;
        }
        return key * 11400714819323198485ULL;
    }

    static uint32_t hash32(const std::string& str) {
        uint32_t key = 2166136261U;
        for (char ch : str) {
            key = (key ^ static_cast<uint8_t>(ch)) * 16777619U;
        }
        return key * 2654435769U;
    }
};

struct HashTableEntry {
    std::string key;
    std::string value;
};

class HashTable {
private:
    std::vector<std::vector<HashTableEntry>> buckets;
    size_t count;
    bool use_shabal;

    size_t get_index(const std::string& key) const {
        if (buckets.empty()) return 0;
        uint64_t h = use_shabal ? Shabal256::hash64(key) : MultiplicativeHash::hash64(key);
        return static_cast<size_t>(h % buckets.size());
    }

public:
    HashTable(size_t size = 13, bool use_shabal_mode = true)
        : buckets(size), count(0), use_shabal(use_shabal_mode) {}

    bool insert(const std::string& key, const std::string& value) {
        size_t idx = get_index(key);
        for (auto& entry : buckets[idx]) {
            if (entry.key == key) {
                entry.value = value;
                return false;
            }
        }
        buckets[idx].push_back({key, value});
        ++count;
        return true;
    }

    bool find(const std::string& key, std::string& out_value) const {
        size_t idx = get_index(key);
        for (const auto& entry : buckets[idx]) {
            if (entry.key == key) {
                out_value = entry.value;
                return true;
            }
        }
        return false;
    }

    bool remove(const std::string& key) {
        size_t idx = get_index(key);
        for (auto it = buckets[idx].begin(); it != buckets[idx].end(); ++it) {
            if (it->key == key) {
                buckets[idx].erase(it);
                --count;
                return true;
            }
        }
        return false;
    }

    void clear() {
        for (auto& b : buckets) {
            b.clear();
        }
        count = 0;
    }

    size_t size() const {
        return count;
    }

    size_t bucket_count() const {
        return buckets.size();
    }

    double load_factor() const {
        return buckets.empty() ? 0.0 : static_cast<double>(count) / buckets.size();
    }

    size_t count_collisions() const {
        size_t collisions = 0;
        for (const auto& b : buckets) {
            if (b.size() > 1) {
                collisions += (b.size() - 1);
            }
        }
        return collisions;
    }

    size_t max_chain_length() const {
        size_t max_len = 0;
        for (const auto& b : buckets) {
            max_len = std::max(max_len, b.size());
        }
        return max_len;
    }

    size_t empty_buckets() const {
        size_t emp = 0;
        for (const auto& b : buckets) {
            if (b.empty()) ++emp;
        }
        return emp;
    }

    void print() const {
        std::cout << "\nHash Table (" << (use_shabal ? "SHABAL-256" : "Multiplication Method")
                  << ", capacity=" << buckets.size() << ", elements=" << count
                  << ", load_factor=" << std::fixed << std::setprecision(3) << load_factor() << "):\n";
        for (size_t i = 0; i < buckets.size(); ++i) {
            std::cout << "[" << std::setw(3) << i << "]: ";
            if (buckets[i].empty()) {
                std::cout << "EMPTY\n";
            } else {
                for (size_t j = 0; j < buckets[i].size(); ++j) {
                    std::cout << "(\"" << buckets[i][j].key << "\" : \"" << buckets[i][j].value << "\")";
                    if (j + 1 < buckets[i].size()) std::cout << " -> ";
                }
                std::cout << "\n";
            }
        }
    }
};

class FileChecksum {
public:
    static const std::string MARKER;

    static std::string calculate(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) return "";
        Shabal256 ctx;
        char buf[4096];
        while (file.read(buf, sizeof(buf))) {
            ctx.update(buf, file.gcount());
        }
        if (file.gcount() > 0) {
            ctx.update(buf, file.gcount());
        }
        return Shabal256::to_hex(ctx.finalize());
    }

    static bool append_checksum(const std::string& path) {
        std::string checksum = calculate(path);
        if (checksum.empty()) return false;
        std::ofstream file(path, std::ios::app | std::ios::binary);
        if (!file.is_open()) return false;
        file << "\n" << MARKER << checksum << "\n";
        return true;
    }

    static bool verify_checksum(const std::string& path, std::string& expected, std::string& actual) {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) return false;
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        size_t pos = content.rfind("\n" + MARKER);
        if (pos == std::string::npos) {
            pos = content.find(MARKER);
            if (pos == std::string::npos) return false;
        }
        std::string original_data = content.substr(0, pos);
        size_t hex_start = pos + 1 + MARKER.size();
        std::string rest = content.substr(hex_start);
        std::istringstream iss(rest);
        if (!(iss >> expected)) return false;
        std::string trailing;
        if (iss >> trailing) return false;
        actual = Shabal256::hash_string(original_data);
        return expected == actual;
    }
};

const std::string FileChecksum::MARKER = "--- SHABAL-256 CHECKSUM: ";

void run_benchmarks() {
    std::cout << "\n=========================================================================\n";
    std::cout << "                 COMPARISON OF HASH FUNCTIONS (VARIANT 1.12)\n";
    std::cout << "       SHABAL-256 (Algorithm 1.12) vs MULTIPLICATION METHOD (Variant 12)\n";
    std::cout << "=========================================================================\n";

    std::vector<size_t> capacities = {1000, 2000, 5000, 10000};
    std::vector<double> target_loads = {0.5, 0.7};

    std::mt19937_64 rng(123456789ULL);

    std::cout << std::left << std::setw(9)  << "Capacity"
              << std::setw(8)  << "Target"
              << std::setw(8)  << "Elements"
              << std::setw(15) << "Algorithm"
              << std::setw(11) << "Collisions"
              << std::setw(10) << "MaxChain"
              << std::setw(11) << "EmptySlots"
              << std::setw(12) << "Time (us)" << "\n";
    std::cout << std::string(84, '-') << "\n";

    for (size_t cap : capacities) {
        for (double load : target_loads) {
            size_t n = static_cast<size_t>(cap * load);
            std::vector<std::string> keys;
            keys.reserve(n);
            for (size_t i = 0; i < n; ++i) {
                uint64_t val = rng();
                keys.push_back("key_" + std::to_string(val) + "_" + std::to_string(i));
            }

            HashTable table_shabal(cap, true);
            auto t0 = std::chrono::steady_clock::now();
            for (size_t i = 0; i < n; ++i) {
                table_shabal.insert(keys[i], "val_" + std::to_string(i));
            }
            auto t1 = std::chrono::steady_clock::now();
            auto dur_shabal = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

            HashTable table_mult(cap, false);
            t0 = std::chrono::steady_clock::now();
            for (size_t i = 0; i < n; ++i) {
                table_mult.insert(keys[i], "val_" + std::to_string(i));
            }
            t1 = std::chrono::steady_clock::now();
            auto dur_mult = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

            std::cout << std::left << std::setw(9)  << cap
                      << std::setw(8)  << load
                      << std::setw(8)  << n
                      << std::setw(15) << "SHABAL-256"
                      << std::setw(11) << table_shabal.count_collisions()
                      << std::setw(10) << table_shabal.max_chain_length()
                      << std::setw(11) << table_shabal.empty_buckets()
                      << std::setw(12) << dur_shabal << "\n";

            std::cout << std::left << std::setw(9)  << cap
                      << std::setw(8)  << load
                      << std::setw(8)  << n
                      << std::setw(15) << "Multiplication"
                      << std::setw(11) << table_mult.count_collisions()
                      << std::setw(10) << table_mult.max_chain_length()
                      << std::setw(11) << table_mult.empty_buckets()
                      << std::setw(12) << dur_mult << "\n";
            std::cout << std::string(84, '-') << "\n";
        }
    }
}

void run_tests() {
    assert(Shabal256::hash_string("") == "aec750d11feee9f16271922fbaf5a9be142f62019ef8d720f858940070889014");
    assert(Shabal256::hash_string("abc") == "07225fab83ca48fb480d22219410d5ca008359efbfd315829029afe2cb3f0404");

    uint64_t m1 = MultiplicativeHash::hash64("test");
    uint64_t m2 = MultiplicativeHash::hash64("test");
    assert(m1 == m2);
    assert(MultiplicativeHash::hash64("test1") != m1);

    HashTable ht(7, true);
    assert(ht.size() == 0);
    assert(ht.insert("Koval", "Grade_95"));
    assert(ht.insert("Ivanov", "Grade_88"));
    assert(ht.insert("Shevchenko", "Grade_92"));
    assert(ht.size() == 3);

    std::string val;
    assert(ht.find("Koval", val) && val == "Grade_95");
    assert(ht.find("Ivanov", val) && val == "Grade_88");
    assert(!ht.find("Unknown", val));

    assert(!ht.insert("Koval", "Grade_100"));
    assert(ht.size() == 3);
    assert(ht.find("Koval", val) && val == "Grade_100");

    assert(ht.remove("Ivanov"));
    assert(ht.size() == 2);
    assert(!ht.find("Ivanov", val));
    assert(!ht.remove("Ivanov"));

    std::string test_filename = "test_checksum_tmp.txt";
    {
        std::ofstream f(test_filename);
        f << "Algorithm and Data Structures Lab 5 Test File Data";
    }
    assert(FileChecksum::append_checksum(test_filename));
    std::string expected, actual;
    assert(FileChecksum::verify_checksum(test_filename, expected, actual));
    assert(expected == actual);

    {
        std::ofstream f(test_filename, std::ios::app);
        f << "corrupted";
    }
    assert(!FileChecksum::verify_checksum(test_filename, expected, actual));
    std::remove(test_filename.c_str());

    std::cout << "[SELF-TEST] All assertions passed successfully.\n";
}

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--test") {
        run_tests();
        return 0;
    }

    HashTable student_table(13, true);
    student_table.insert("Koval Oleksandr", "122-Computer-Science");
    student_table.insert("Melnyk Dmytro", "122-Computer-Science");
    student_table.insert("Tkachenko Anna", "122-Computer-Science");
    student_table.insert("Shevchenko Taras", "122-Computer-Science");
    student_table.insert("Bondarenko Iryna", "122-Computer-Science");

    int choice = -1;
    while (choice != 0) {
        std::cout << "\n======================================================\n";
        std::cout << "    LABORATORY WORK 5: HASH FUNCTIONS AND TABLES     \n";
        std::cout << "  Variant 1.12: SHABAL-256 | Multiplication Method   \n";
        std::cout << "======================================================\n";
        std::cout << "1. Calculate SHABAL-256 & Multiplicative Hash for String\n";
        std::cout << "2. File Checksum: Compute, Append, and Verify (Task 3b)\n";
        std::cout << "3. Hash Table: Insert Student Record\n";
        std::cout << "4. Hash Table: Search Student Record\n";
        std::cout << "5. Hash Table: Delete Student Record\n";
        std::cout << "6. Hash Table: Display Table Graph\n";
        std::cout << "7. Run Collision & Performance Benchmark (Task 5)\n";
        std::cout << "8. Run Automated Self-Tests\n";
        std::cout << "0. Exit\n";
        std::cout << "Choose option: ";

        if (!(std::cin >> choice)) {
            break;
        }

        switch (choice) {
            case 1: {
                std::cout << "Enter input string: ";
                std::string input;
                std::cin.ignore();
                std::getline(std::cin, input);
                std::cout << "\nInput string: \"" << input << "\"\n";
                std::cout << "SHABAL-256 Hash:        " << Shabal256::hash_string(input) << "\n";
                std::cout << "Multiplicative Hash 64: " << std::hex << MultiplicativeHash::hash64(input) << std::dec << "\n";
                std::cout << "Multiplicative Hash 32: " << std::hex << MultiplicativeHash::hash32(input) << std::dec << "\n";
                break;
            }
            case 2: {
                std::cout << "\n--- File Checksum Operations (Task 3b) ---\n";
                std::cout << "1. Create sample file and append SHABAL-256 checksum\n";
                std::cout << "2. Verify checksum of existing file\n";
                std::cout << "Select action: ";
                int fchoice = 0;
                std::cin >> fchoice;
                if (fchoice == 1) {
                    std::string fname = "sample_data.txt";
                    std::ofstream f(fname);
                    f << "Student: Koval Oleksandr\nCourse: Algorithms and Data Structures\nLab: 5\n";
                    f.close();
                    if (FileChecksum::append_checksum(fname)) {
                        std::cout << "File \"" << fname << "\" created and checksum appended successfully.\n";
                    } else {
                        std::cout << "Failed to append checksum.\n";
                    }
                } else if (fchoice == 2) {
                    std::cout << "Enter filename to verify: ";
                    std::string fname;
                    std::cin >> fname;
                    std::string exp, act;
                    if (FileChecksum::verify_checksum(fname, exp, act)) {
                        std::cout << "INTEGRITY VERIFIED: OK\n";
                        std::cout << "Expected: " << exp << "\nActual:   " << act << "\n";
                    } else {
                        std::cout << "INTEGRITY FAILURE or missing checksum marker!\n";
                        std::cout << "Expected: " << exp << "\nActual:   " << act << "\n";
                    }
                }
                break;
            }
            case 3: {
                std::cout << "Enter Student Full Name (Key): ";
                std::string key, val;
                std::cin.ignore();
                std::getline(std::cin, key);
                std::cout << "Enter Speciality/Info (Value): ";
                std::getline(std::cin, val);
                if (student_table.insert(key, val)) {
                    std::cout << "Inserted successfully.\n";
                } else {
                    std::cout << "Key already existed. Value updated.\n";
                }
                break;
            }
            case 4: {
                std::cout << "Enter Student Full Name to search: ";
                std::string key, val;
                std::cin.ignore();
                std::getline(std::cin, key);
                if (student_table.find(key, val)) {
                    std::cout << "FOUND: " << key << " => " << val << "\n";
                } else {
                    std::cout << "NOT FOUND: \"" << key << "\"\n";
                }
                break;
            }
            case 5: {
                std::cout << "Enter Student Full Name to delete: ";
                std::string key;
                std::cin.ignore();
                std::getline(std::cin, key);
                if (student_table.remove(key)) {
                    std::cout << "Deleted \"" << key << "\" successfully.\n";
                } else {
                    std::cout << "Record not found.\n";
                }
                break;
            }
            case 6: {
                student_table.print();
                break;
            }
            case 7: {
                run_benchmarks();
                break;
            }
            case 8: {
                run_tests();
                break;
            }
            case 0: {
                std::cout << "Exiting program.\n";
                break;
            }
            default: {
                std::cout << "Invalid choice.\n";
                break;
            }
        }
    }

    return 0;
}
