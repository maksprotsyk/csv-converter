//
// Created by Maksym Protsyk on 1/4/22.
//

#ifndef HOURSCOUNTER_PARSING_H
#define HOURSCOUNTER_PARSING_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <future>
#include <unordered_map>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace datetime = boost::gregorian;

constexpr size_t COLUMNS_NUM = 8;

typedef std::unordered_map<std::string, std::unordered_map<std::string, size_t>> working_hours_map_t;

// struct containing data we need about the row of csv file
struct WorkingTimeData {
    std::string error = "";
    std::string name = "";
    size_t hours = 0;
    std::string month = "";
};


WorkingTimeData parsing_function(std::string row, const std::string& separators);

int convert_csv(const std::string& input_name, const std::string& output_name, const std::string& separator);


#endif //HOURSCOUNTER_PARSING_H
