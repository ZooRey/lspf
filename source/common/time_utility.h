#ifndef COMMON_TIME_UTILITY_H_
#define COMMON_TIME_UTILITY_H_

#include <stdint.h>
#include <string>

class TimeUtility {
public:
    // 得到当前的毫秒
    static int64_t GetCurremtMs();

    // 得到字符串形式的时间格式：strSplit为空则是20150410101112 strSplit为 "-"则是2015-04-10 10:11:12
    static std::string GetStringTime(const std::string &strSplit = "");

    // 将字符串格式(2015-04-10 10:11:12)的时间，转为time_t
    static time_t ChangeTime(const std::string &time);

    // 取得两个时间戳字符串t1-t2的时间差，精确到秒,时间格式为2015-04-10 10:11:12
    static time_t GetTimeDiff(const std::string &t1, const std::string &t2);
};

class TimerClock
{
public:
    /**
     * @brief remembers start time during construction
     */
    TimerClock()
    {
        start_ = TimeUtility::GetCurremtMs();
    }

    /**
     * @brief resets start time to current time.
     */
    void Restart()
    {
        start_ = TimeUtility::GetCurremtMs();
    }

    /**
     * @brief returns elapsed milliseconds since remembered start time.
     *
     * @return elapsed time in milliseconds, some platform can returns fraction
     * representing more precise time.
     */
    int64_t Elapsed() const
    {
        int64_t now = TimeUtility::GetCurremtMs();

        return now - start_;
    }

private:
    int64_t start_; /**< remembers start time */
};

#endif  // COMMON_TIME_UTILITY_H_
