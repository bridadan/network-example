#include "mbed.h"
#include "TCPSocket.h"

#define ETHERNET        1
#define WIFI            2

#if MBED_CONF_APP_NETWORK_INTERFACE == ETHERNET
#include "EthernetInterface.h"
EthernetInterface eth;
#elif MBED_CONF_APP_NETWORK_INTERFACE == WIFI
#include "ESP8266Interface.h"
ESP8266Interface wifi(D1, D0);
#endif

TCPSocket socket;

int main()
{
    printf("Example network-socket HTTP client\r\n");
    NetworkInterface *network_interface = 0;
    
    #if MBED_CONF_APP_NETWORK_INTERFACE == ETHERNET
    printf("Using Ethernet\r\n");
    eth.connect();
    network_interface = &eth;
    #elif MBED_CONF_APP_NETWORK_INTERFACE == WIFI
    printf("Using WiFi\r\n");
    wifi.connect(MBED_CONF_APP_WIFI_SSID, NULL);
    network_interface = &wifi;
    #endif
    
    // Brings up the network interface
    const char *ip = network_interface->get_ip_address();
    printf("IP address is: %s\r\n", ip ? ip : "No IP");

    // Open a socket on the network interface, and create a TCP connection to mbed.org
    socket.open(network_interface);
    socket.connect("api.ipify.org", 80);

    // Send a simple http request
    char sbuffer[] = "GET / HTTP/1.1\r\nHost: api.ipify.org\r\n\r\n";
    int scount = socket.send(sbuffer, sizeof sbuffer);
    printf("sent %d [%.*s]\r\n", scount, strstr(sbuffer, "\r\n")-sbuffer, sbuffer);

    // Recieve a simple http response and print out the response line
    char rbuffer[256] = {0};
    int rcount = socket.recv(rbuffer, sizeof rbuffer);
    char *print_start = strstr(rbuffer, "\r\n\r\n") + 4;
    int print_count = rcount - ((int)(print_start) - (int)rbuffer);
    
    printf("Public IP address (from api.ipify.org) is: %.*s\r\n", print_count, print_start);

    // Close the socket to return its memory and bring down the network interface
    socket.close();
    
    #if MBED_CONF_APP_NETWORK_INTERFACE == ETHERNET
    eth.disconnect();
    #elif MBED_CONF_APP_NETWORK_INTERFACE == WIFI
    wifi.disconnect();
    #endif

    printf("Done\r\n");
}