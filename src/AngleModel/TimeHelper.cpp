#include <iostream>
#include <chrono>

namespace crn = std::chrono;
using tp = crn::time_point<std::chrono::system_clock>;

/**
 * @brief Class representing a point in time with useful helpers.
 * 
 */
class Time {
    // Thanks https://en.cppreference.com/w/cpp/chrono/year_month_day
    // Barely doable. C++ is kinda weird for this.
    public:
        Time(tp d) {
            const auto preDate = crn::floor<crn::days>(d);
            date = {preDate};

            const auto preTime = d - preDate;
            time = crn::hh_mm_ss {preTime};
        }

        static auto now() -> Time {
            const auto duration{crn::system_clock::now()};
            return Time(duration);
        };

        auto year() -> int {
            return static_cast<int>(date.year()); 
        };

        auto month() -> unsigned {
            return static_cast<unsigned>(date.month()); 
        };

        auto day() -> unsigned {
            return static_cast<unsigned>(date.day()); 
        };

        auto hour() -> unsigned {
            return time.hours().count();
        }

        auto minute() -> unsigned {
            return time.minutes().count();
        }

        auto second() -> unsigned {
            return time.seconds().count();
        }

    private:
        crn::year_month_day date;
        crn::hh_mm_ss<std::chrono::system_clock::duration> time;
};


int main() {
    std::cout << "Testing TimeHelper\n";
    Time test = Time::now();
    
    std::cout << test.year() << ":"
         << test.month() << ":"
         << test.day() << ":"
         << test.hour() << ":"
         << test.minute() << ":"
         << test.second() << "\n";
};