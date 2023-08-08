#include "./date_and_time.hpp"

string systemDateTime::GetTime()
{
  auto timepoint = system_clock::now();
  auto coarse = system_clock::to_time_t(timepoint);
  auto fine = time_point_cast<std::chrono::milliseconds>(timepoint);
  memset(this->buffer, 0, sizeof(this->buffer));

  std::snprintf
  (
    this->buffer + std::strftime
    (
      this->buffer, 
      sizeof(this->buffer) - 3,
      "%T.", 
      std::localtime(&coarse)
    ),
    4, 
    "%03lu", 
    fine.time_since_epoch().count() % 1000
  );

  return (string)this->buffer;
}

string systemDateTime::GetDate()
{
  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);

  std::stringstream ss;
  ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d");
  return ss.str();
}
