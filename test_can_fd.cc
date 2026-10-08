// Tests for the CAN_FD_MESSAGE (100) and CAN_FD_MESSAGE_64 (101) readers.
// Builds synthetic lobj payloads in the layout of Vector's binlog_objects.h
// and checks that the fields land where expected. Needs no BLF file.

#include "blf_reader.hh"
#include "blf_structs.hh"

#include <cstdio>
#include <cstring>
#include <vector>

static_assert(sizeof(lblf::blf_struct::CanFdMessage_obh) == 100, "CAN_FD_MESSAGE payload is 100 bytes");
static_assert(sizeof(lblf::blf_struct::CanFdMessage64_obh) == 56, "CAN_FD_MESSAGE_64 fixed part is 56 bytes");

namespace
{

int failures = 0;

void check(bool condition, const char *what)
{
    if (!condition)
        {
            std::printf("FAIL: %s\n", what);
            failures++;
        }
}

template <typename value_type>
void put(std::vector<char> &buffer, size_t offset, value_type value)
{
    std::memcpy(buffer.data() + offset, &value, sizeof(value));
}

void test_can_fd_message()
{
    lblf::lobj obj;
    obj.base_header.objectType = lblf::ObjectType_e::CAN_FD_MESSAGE;
    obj.payload.assign(100, 0);
    put<uint64_t>(obj.payload, 8, 123456789);    // objectTimeStamp
    put<uint16_t>(obj.payload, 16, 2);           // channel
    put<uint8_t>(obj.payload, 18, 0x01);         // flags: Tx
    put<uint8_t>(obj.payload, 19, 13);           // dlc 13 = 32 bytes
    put<uint32_t>(obj.payload, 20, 0x80000123);  // extended id
    put<uint8_t>(obj.payload, 29, 0x03);         // EDL | BRS
    put<uint8_t>(obj.payload, 30, 32);           // validDataBytes
    for (size_t i = 0; i < 64; ++i)
        {
            put<uint8_t>(obj.payload, 36 + i, static_cast<uint8_t>(i));
        }

    lblf::blf_struct::CanFdMessage_obh msg;
    check(lblf::read_blf_struct(obj, msg) == 100, "fd100: reads 100 bytes");
    check(msg.obh.objectTimeStamp == 123456789, "fd100: timestamp");
    check(msg.channel == 2, "fd100: channel");
    check(msg.flags == 0x01, "fd100: flags");
    check(msg.dlc == 13, "fd100: dlc");
    check(msg.id == 0x80000123, "fd100: id");
    check(lblf::blf_struct::CAN_FD_MSG_EDL(msg.canFdFlags) == 1, "fd100: EDL");
    check(lblf::blf_struct::CAN_FD_MSG_BRS(msg.canFdFlags) == 1, "fd100: BRS");
    check(msg.validDataBytes == 32, "fd100: validDataBytes");
    check(msg.data[0] == 0 && msg.data[31] == 31 && msg.data[63] == 63, "fd100: data");

    obj.payload.resize(99);
    check(lblf::read_blf_struct(obj, msg) == 0, "fd100: short payload rejected");
}

void test_can_fd_message_64()
{
    lblf::lobj obj;
    obj.base_header.objectType = lblf::ObjectType_e::CAN_FD_MESSAGE_64;
    obj.payload.assign(56 + 12, 0);
    put<uint64_t>(obj.payload, 8, 987654321);    // objectTimeStamp
    put<uint8_t>(obj.payload, 16, 3);            // channel
    put<uint8_t>(obj.payload, 17, 9);            // dlc 9 = 12 bytes
    put<uint8_t>(obj.payload, 18, 12);           // validDataBytes
    put<uint32_t>(obj.payload, 20, 0x1A3);       // id
    put<uint32_t>(obj.payload, 28, lblf::blf_struct::CAN_FD_MSG64_EDL | lblf::blf_struct::CAN_FD_MSG64_BRS);
    put<uint8_t>(obj.payload, 50, 1);            // dir: Tx
    for (size_t i = 0; i < 12; ++i)
        {
            put<uint8_t>(obj.payload, 56 + i, static_cast<uint8_t>(0xA0 + i));
        }

    lblf::blf_struct::CanFdMessage64_obh header;
    std::array<uint8_t, 64> data {};
    check(lblf::read_can_fd_message_64(obj, header, data), "fd64: read succeeds");
    check(header.obh.objectTimeStamp == 987654321, "fd64: timestamp");
    check(header.channel == 3, "fd64: channel");
    check(header.dlc == 9, "fd64: dlc");
    check(header.validDataBytes == 12, "fd64: validDataBytes");
    check(header.id == 0x1A3, "fd64: id");
    check((header.flags & lblf::blf_struct::CAN_FD_MSG64_EDL) != 0, "fd64: EDL");
    check((header.flags & lblf::blf_struct::CAN_FD_MSG64_BRS) != 0, "fd64: BRS");
    check(header.dir == 1, "fd64: dir");
    check(data[0] == 0xA0 && data[11] == 0xAB && data[12] == 0, "fd64: data");

    obj.payload.resize(56 + 11);
    check(!lblf::read_can_fd_message_64(obj, header, data), "fd64: truncated data rejected");

    obj.payload.assign(56, 0);
    put<uint8_t>(obj.payload, 18, 65);
    check(!lblf::read_can_fd_message_64(obj, header, data), "fd64: validDataBytes > 64 rejected");
}

} // namespace

int main()
{
    test_can_fd_message();
    test_can_fd_message_64();
    if (failures == 0)
        {
            std::printf("All CAN FD tests passed\n");
        }
    return failures == 0 ? 0 : 1;
}
