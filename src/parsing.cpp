//
// Created by Maksym Protsyk on 1/4/22.
//
#include "parsing.h"


WorkingTimeData parsing_function(std::string row, const std::string& separators) {
    WorkingTimeData data;
    std::vector<std::string> columns;

    // splitting the row using the given separator
    boost::algorithm::split(
        columns,
        row,
        boost::is_any_of(separators),
        boost::token_compress_on
    );

    // if row is invalid
    if (columns.size() != COLUMNS_NUM) {
        // setting the error message
        data.error =
            "The row contains "
            + std::to_string(columns.size())
            + " columns instead of "
            + std::to_string(COLUMNS_NUM);
        return data;
    }

    data.name = columns[0];

    // creating "month and year" string and returning error messages in case of caught exceptions
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

    // getting hours number and returning error messages in case of caught exceptions
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
        // scheduling async parsing task
        tasks.push_back(std::async(
            std::launch::async,
            parsing_function,
            line,
            std::cref(separator)
        ));
    }

    working_hours_map_t global_map;
    // getting result of every task and adding it to map with working hours of every worker
    for (auto& task: tasks) {
        auto res = task.get();
        // exiting in case of error
        if (!res.error.empty()) {
            std::cout << "Error: " << res.error << std::endl;
            return -2;
        }
        // inserting the worker
        global_map.insert(std::make_pair(res.name, std::unordered_map<std::string, size_t>()));
        // inserting the month
        global_map[res.name].insert(std::make_pair(res.month, 0));
        // adding hours num
        global_map[res.name][res.month] += res.hours;

    }

    std::fstream outfile(output_name, std::ios::out);

    if (!outfile.is_open()) {
        std::cout << "Error: can't open the output file" << std::endl;
        return -3;
    }

    // new csv file
    outfile << "Name" << separator << "Month" << separator << "Total hours" << std::endl;

    for (const auto& worker_entry: global_map) {
        for (const auto& month_entry: worker_entry.second) {
            outfile << worker_entry.first << separator << month_entry.first << separator << month_entry.second << std::endl;
        }
    }


    return 0;
}
