#include <cstdlib>
#include <string>

u_int32_t find(const u_int8_t* data, const std::size_t length, const u_int32_t interval,
               const u_int32_t target);
int load_gear_items(GearItem** gear_items, const u_int32_t address, const u_int8_t* data,
                    const std::size_t length);
void scan_data(const u_int8_t* data, const std::size_t length, std::string& json_text);
