#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MSG_CONFIRM 0

#define PORT 8080 // Art-Net uses port 6454
#define MAXLINE 1024

// Art-Net packet constants
const char ART_NET_ID[] = "Art-Net";
const uint16_t ART_DMX_OPCODE = 0x5000;

// Driver code
int main()
{
    int sockfd;
    uint8_t buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr,
             sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    socklen_t len;
    int n;

    len = sizeof(cliaddr); // len is value/result

    while (true)
    {
        // Clear the buffer to ensure no stale data is processed
        memset(buffer, 0, MAXLINE);

        n = recvfrom(sockfd, buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&cliaddr,
                     &len);

        if (n > 0)
        {
            // Check for Art-Net ID
            if (memcmp(buffer, ART_NET_ID, sizeof(ART_NET_ID) - 1) == 0)
            {
                // Extract OpCode
                uint16_t opcode = buffer[8] | (buffer[9] << 8);

                if (opcode == ART_DMX_OPCODE)
                {
                    // Extract DMX data length
                    uint16_t dmx_length = buffer[17] | (buffer[16] << 8);

                    // Ensure DMX length is within bounds
                    if (dmx_length > MAXLINE - 18)
                        dmx_length = MAXLINE - 18;

                    // Print DMX data
                    std::cout << "Received Art-Net DMX data:" << std::endl;
                    for (int i = 0; i < dmx_length; ++i)
                    {
                        if (buffer[18 + i] == 0x00) // not displaying 0x00 values for better readability
                            continue;
                        printf("Channel %d: %02x\n", i + 1, buffer[18 + i]);
                    }
                }
                else
                {
                    std::cerr << "Unsupported OpCode: " << std::hex << opcode << std::endl;
                }
            }
            else
            {
                std::cerr << "Invalid Art-Net ID" << std::endl;
            }
        }
        else
        {
            std::cerr << "Failed to receive data or connection closed" << std::endl;
        }
    }

    return 0;
}