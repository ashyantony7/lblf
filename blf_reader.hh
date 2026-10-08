#ifndef BLF_READER_HH
#define BLF_READER_HH

#include "blf_structs.hh"
#include <cstring>
#include <deque>
#include <fstream>

namespace lblf
{


/**
 * @brief Static lobj reading function
 *
 * @tparam type_data typically a struct with that aligns with blf type
 * @param indata incoming lobj frame
 * @param blf_struct_data var parameter of type_data
 * @return size_t Number of read bytes
 */
template <typename type_data>
auto read_blf_struct(const struct lobj &indata, type_data &blf_struct_data) -> size_t
{
    if (indata.payload.size() >= sizeof(type_data))
        {
            std::memcpy(reinterpret_cast<char *>(&blf_struct_data),
                indata.payload.data(),
                sizeof(type_data));
            return sizeof(type_data);
        }
    return 0;
}


/**
 * @brief Reads a variable-length CAN_FD_MESSAGE_64 object
 *
 * @param indata incoming lobj frame of type CAN_FD_MESSAGE_64
 * @param header var parameter for the fixed part of the object
 * @param data var parameter for the payload; header.validDataBytes are valid
 * @return true if the fixed part and all valid data bytes were present
 */
inline auto read_can_fd_message_64(const struct lobj &indata,
    blf_struct::CanFdMessage64_obh &header,
    std::array<uint8_t, 64> &data) -> bool
{
    if (read_blf_struct(indata, header) == 0 || header.validDataBytes > data.size())
        {
            return false;
        }
    if (indata.payload.size() < sizeof(header) + header.validDataBytes)
        {
            return false;
        }
    std::memcpy(data.data(), indata.payload.data() + sizeof(header), header.validDataBytes);
    return true;
}


/**
 * @brief blf_reader class
 *
 */
class blf_reader
{
private:
    std::fstream fileStream;
    std::deque<char> logcontainer_que;
    blf_struct::fileStatistics fileStat;
    uint32_t fileLength {0};
    size_t BaseHeaderRead {0};
    auto fill_deque() -> bool;
    auto read_baseHeader(blf_struct::BaseHeader &ohb) -> bool;
    auto read_fileStatistics() -> bool;

public:
    blf_reader(const blf_reader &) = delete;
    blf_reader(blf_reader &&) = delete;
    auto operator=(const blf_reader &) -> blf_reader & = delete;
    auto operator=(blf_reader &&) -> blf_reader & = delete;
    explicit blf_reader(const std::string &filename);
    ~blf_reader();
    auto next() -> bool;
    auto data() -> lblf::lobj;
    auto getfileStatistics() const -> const struct blf_struct::fileStatistics &;
    auto getBaseHeadRead() const -> size_t;
};

} // namespace lblf

#endif
