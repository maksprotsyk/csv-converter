#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <future>
#include <unordered_map>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace datetime = boost::gregorian;

constexpr size_t COLUMNS_NUM = 8;

typedef std::unordered_map<std::string, std::unordered_map<std::string, size_t>> working_hours_map_t;


struct WorkingTimeData {
    std::string error = "";
    std::string name = "";
    size_t hours = 0;
    std::string month = "";
};


WorkingTimeData parsing_function(std::string row, const std::string& separators) {
    WorkingTimeData data;
    std::vector<std::string> columns;

    boost::algorithm::split(
        columns,
        row,
        boost::is_any_of(separators),
        boost::token_compress_on
    );

    if (columns.size() != COLUMNS_NUM) {
        data.error =
            "The row contains "
            + std::to_string(columns.size())
            + " columns instead of "
            + std::to_string(COLUMNS_NUM);
        return data;
    }

    data.name = columns[0];

    try {
        datetime::date date(datetime::from_simple_string(columns[6]));
        datetime::date::ymd_type ymd = date.year_month_day();
        std::string month = ymd.month.as_long_string();
        month += " ";
        month += std::to_string(ymd.year);
        data.month = month;
    } catch (boost::bad_lexical_cast&) {
        data.error = "Invalid argument while creating date boost object";
        return data;

    } catch (std::out_of_range&) {
        data.error = "Out of range while creating date boost object";
        return data;
    }


    try {
        size_t hours = std::stoul(columns[7]);
        data.hours = hours;
        if (hours > 24) {
            data.error = "The number of hours exceeds 24";
            return data;
        }
    } catch (std::invalid_argument&) {
        data.error = "Invalid argument while calling stoul function";
        return data;

    } catch (std::out_of_range&) {
        data.error = "Out of range while calling stoul function";
        return data;
    }
    return data;
}


int convert_csv(const std::string& input_name, const std::string& output_name, const std::string& separator) {
    std::fstream infile(input_name);
    if(!infile.is_open()) {
        std::cout << "Error: can't open the input file" << std::endl;
        return -1;
    }

    std::string line;
    std::getline(infile, line);
    std::vector<std::future<WorkingTimeData>> tasks;
    while (std::getline(infile, line)) {
        tasks.push_back(std::async(
            std::launch::async,
            parsing_function,
            line,
            std::cref(separator)
        ));
    }

    working_hours_map_t global_map;
    for (auto& task: tasks) {
        auto res = task.get();
        if (!res.error.empty()) {
            std::cout << "Error: " << res.error << std::endl;
            return -2;
        }
        global_map.insert(std::make_pair(res.name, std::unordered_map<std::string, size_t>()));
        global_map[res.name].insert(std::make_pair(res.month, 0));
        global_map[res.name][res.month] += res.hours;

    }

    std::fstream outfile(output_name, std::ios::out);

    if (!outfile.is_open()) {
        std::cout << "Error: can't open the output file" << std::endl;
        return -3;
    }

    outfile << "Name" << separator << "Month" << separator << "Total hours" << std::endl;

    for (const auto& worker_entry: global_map) {
        for (const auto& month_entry: worker_entry.second) {
            outfile << worker_entry.first << separator << month_entry.first << separator << month_entry.second << std::endl;
        }
    }


    return 0;
}


int main()
{
    std::string input_name;
    std::string output_name;
    std::string separator;

    std::cout << "Enter the input filename: ";
    std::cin >> input_name;
    std::cout << "Enter the output filename: ";
    std::cin >> output_name;
    while (true) {
        std::cout << "Enter the separator (for example: ';'): ";
        std::cin >> separator;
        if (separator.size() == 1) {
            break;
        }
        std::cout << "Error: separator size should be exactly 1" << std::endl;
    }


    return convert_csv(input_name, output_name, separator);



}
 