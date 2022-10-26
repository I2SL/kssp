#include "network_info.h"

NetworkInfo::NetworkInfo(
        boost::uint8_t DHCP,
        unsigned char* mac_address,
        boost::uint32_t ip_address,
        boost::uint32_t subnet_mask,
        boost::uint32_t gateway,
        boost::uint8_t device_type,
        boost::uint8_t device_addr
        ):
    DHCP(DHCP),
    mac_address(mac_address),
    ip_address(ip_address),
    subnet_mask(subnet_mask),
    gateway(gateway),
    device_type(device_type),
    device_addr(device_addr)
{

};

std::string NetworkInfo::to_string() {
    memcpy(&mac_str, &mac_address, sizeof(mac_address));
    std::string result("NETWORK INFO:\n");
    result += std::string("-------------\n");
    result += fmt::format("DHCP: {}\n", DHCP);
    result += fmt::format("MAC Address: {}\n", mac_str);
    result += fmt::format("IP Address: {}\n", ip_address);
    result += fmt::format("Subnet Mask: {}\n", subnet_mask);
    result += fmt::format("Gateway: {}\n", gateway);
    result += fmt::format("Device Type: {}\n", device_type);
    result += fmt::format("Device Address: {}\n", device_addr);

    return result;
}