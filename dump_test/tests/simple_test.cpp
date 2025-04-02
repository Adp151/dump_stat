#include <cstdio>
#include <cstdlib>

#include <fstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

static const std::string l_filePath = "./stat_data_left";
static const std::string r_filePath = "./stat_data_right";
static const std::string out_filePath = "./stat_data_out";

static const float FLOAT_PRECISION = 0.001;

class StatDataDumpTest : public ::testing::Test {
public:
    void SetUp() override;
    void TearDown() override;

    struct StatData {
        long id;
        int count;
        float cost;
        unsigned int primary;
        unsigned int mode;

        bool operator==(const StatData & other) const {
        return id == other.id &&
               count == other.count &&
               abs(cost - other.cost) < FLOAT_PRECISION &&
               primary == other.primary &&
               mode == other.mode;
        }
    };
protected:
    void RunTest(
        const std::vector<StatData> & l_data,
        const std::vector<StatData> & r_data,
        const std::vector<StatData> & expected_data
    ) const;
private:
    void StoreStatData(std::ofstream & stream, const std::vector<StatData> & data) const;
    void LoadStatData(std::ifstream & stream, std::vector<StatData> & out_data) const;

    void RunStatDump() const;
    void CheckResult(const std::vector<StatData> & expected_data) const;
protected:
    mutable std::ofstream l_statDataStream;
    mutable std::ofstream r_statDataStream;
    mutable std::ifstream out_statDataStream;
};

void StatDataDumpTest::SetUp() {
    l_statDataStream.open(l_filePath, std::ios::binary);
    r_statDataStream.open(r_filePath, std::ios::binary);

    ASSERT_TRUE(l_statDataStream && r_statDataStream);
}

void StatDataDumpTest::TearDown() {
    if(out_statDataStream.is_open()) {
        out_statDataStream.close();
        ASSERT_EQ(0, remove(out_filePath.c_str()));
    }

    ASSERT_EQ(0, remove(l_filePath.c_str()));
    ASSERT_EQ(0, remove(r_filePath.c_str()));
}

void StatDataDumpTest::StoreStatData(std::ofstream & stream, const std::vector<StatData> & data) const {
    ASSERT_TRUE(stream.is_open());

    stream.write(reinterpret_cast<const char*>(&data.front()), sizeof(StatData) * data.size());
    stream.close();
}

void StatDataDumpTest::LoadStatData(std::ifstream & stream, std::vector<StatData> & out_data) const {
    ASSERT_TRUE(stream.is_open());

    stream.seekg(0, std::ios::end);
    std::streamsize size = stream.tellg();
    stream.seekg(0, std::ios::beg);

    size_t count = size / sizeof(StatData);
    out_data.resize(count);

    stream.read(reinterpret_cast<char*>(out_data.data()), size);
}

void StatDataDumpTest::RunStatDump() const {
    const std::string cmd = "./dump_stat -l " + l_filePath + " -r " + r_filePath + " -o " + out_filePath;
    int result = system(cmd.c_str());
    ASSERT_TRUE(result == 0) << "Result code " << result;
}

void StatDataDumpTest::CheckResult(const std::vector<StatData> & expected_data) const {
    std::vector<StatData> result_data;
    out_statDataStream.open(out_filePath);
    ASSERT_NO_FATAL_FAILURE(LoadStatData(out_statDataStream, result_data));
    ASSERT_EQ(expected_data, result_data);
}

void StatDataDumpTest::RunTest(
    const std::vector<StatData> & l_data,
    const std::vector<StatData> & r_data,
    const std::vector<StatData> & expected_data
) const {
    ASSERT_NO_FATAL_FAILURE(StoreStatData(l_statDataStream, l_data));
    ASSERT_NO_FATAL_FAILURE(StoreStatData(r_statDataStream, r_data));
    ASSERT_NO_FATAL_FAILURE(RunStatDump());
    ASSERT_NO_FATAL_FAILURE(CheckResult(expected_data));
}

/*
Пример теста из ТЗ. Покрывает следующие случаи: строки с одинаковыми id объединяются по правилам из ТЗ
Сортировка по cost
*/
TEST_F(StatDataDumpTest, TestExample) {
    const std::vector<StatData> l_data = 
        {
            {.id = 90889, .count = 13, .cost = 3.567, .primary = 0, .mode=3 },
            {.id = 90089, .count = 1, .cost = 88.90, .primary = 1, .mode=0 }
        };

     const std::vector<StatData> r_data = 
        {
            {.id = 90089, .count = 13, .cost = 0.011, .primary = 0, .mode=2 },
            {.id = 90189, .count = 1000, .cost = 1.00003, .primary = 1, .mode=2}
        };

    const std::vector<StatData> expected_data =
    {
        {.id = 90189, .count = 1000, .cost = 1.00003, .primary = 1, .mode = 2 },
        {.id = 90889, .count = 13, .cost = 3.567, .primary = 0, .mode = 3 },
        {.id = 90089, .count = 14, .cost = 88.911, .primary = 0, .mode = 2 }
    };
    
    ASSERT_NO_FATAL_FAILURE(RunTest(l_data, r_data, expected_data));
}

TEST_F(StatDataDumpTest, TwoEmptyArray){
    const std::vector<StatData> l_data, r_data, expected_data;
    ASSERT_NO_FATAL_FAILURE(RunTest(l_data, r_data, expected_data));
}

TEST_F(StatDataDumpTest, OneEmptyArray) {
    const std::vector<StatData> l_data;
    const std::vector<StatData> r_data =
    {
        { .id = 1, .count = 1, .cost = 1.11, .primary = 1, .mode = 5 }
    };
    const auto expected_data = r_data;

    ASSERT_NO_FATAL_FAILURE(RunTest(l_data, r_data, expected_data));
}

TEST_F(StatDataDumpTest, AllUniqueIdsUniqueCost) {
    const std::vector<StatData> l_data = 
    {
        { .id = 1, .count = 1, .cost = 1.11, .primary = 1, .mode = 5 },
        { .id = 2, .count = 1, .cost = 1.12, .primary = 1, .mode = 5 }
    };
    const std::vector<StatData> r_data =
    {
        { .id = 3, .count = 1, .cost = 1.13, .primary = 1, .mode = 5 },
        { .id = 4, .count = 1, .cost = 1.14, .primary = 1, .mode = 5 }
    };
    const std::vector<StatData> expected_data =
    {
        { .id = 1, .count = 1, .cost = 1.11, .primary = 1, .mode = 5 },
        { .id = 2, .count = 1, .cost = 1.12, .primary = 1, .mode = 5 },
        { .id = 3, .count = 1, .cost = 1.13, .primary = 1, .mode = 5 },
        { .id = 4, .count = 1, .cost = 1.14, .primary = 1, .mode = 5 }
    };

    ASSERT_NO_FATAL_FAILURE(RunTest(l_data, r_data, expected_data));
}

// Проверка, что при одинаковой cost сортировка будет по id
TEST_F(StatDataDumpTest, OneCostForAll) {
    const std::vector<StatData> l_data = 
    {
        { .id = 4, .count = 1, .cost = 1.11, .primary = 1, .mode = 5 }
    };
    const std::vector<StatData> r_data =
    {
        { .id = 1, .count = 1, .cost = 1.11, .primary = 1, .mode = 5 }
    };
    const std::vector<StatData> expected_data = 
    {
        { .id = 1, .count = 1, .cost = 1.11, .primary = 1, .mode = 5 },
        { .id = 4, .count = 1, .cost = 1.11, .primary = 1, .mode = 5 }
    };

    ASSERT_NO_FATAL_FAILURE(RunTest(l_data, r_data, expected_data));
}

// Проверка, что при одинаковой cost сортировка будет по id
TEST_F(StatDataDumpTest, OneCostCheckSortById) {
    const std::vector<StatData> l_data = 
    {
        { .id = 4, .count = 1, .cost = 1.11, .primary = 1, .mode = 5 }
    };
    const std::vector<StatData> r_data =
    {
        { .id = 1, .count = 1, .cost = 1.11, .primary = 1, .mode = 5 }
    };
    const std::vector<StatData> expected_data = 
    {
        { .id = 1, .count = 1, .cost = 1.11, .primary = 1, .mode = 5 },
        { .id = 4, .count = 1, .cost = 1.11, .primary = 1, .mode = 5 }
    };

    ASSERT_NO_FATAL_FAILURE(RunTest(l_data, r_data, expected_data));
}
