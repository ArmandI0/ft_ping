#include "../ft_ping.h"

void freePacketList(packet *head)
{
    packet *temp;
    while (head != NULL)
    {
        temp = head;
        head = head->next;
        free(temp);
    }
}

void appendPacket(packet **head, packet *new_packet)
{
    if (*head == NULL)
        *head = new_packet;
    else
    {
        packet *temp = *head;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = new_packet;
        new_packet->prev = temp;
    }
}

cmd* initCommandStruct(void)
{
	cmd *command;
	command = malloc(sizeof(cmd));
	if (!command)
		return NULL;
	command->verbose = false;
	command->help = false;
	command->addr = NULL;
	command->packet = NULL;
    command->packets = NULL;
	command->socket = -1;
    command->nb_of_transmitted_packets = 0;
    command->nb_of_received_packets = 0;
    command->nb_of_received_packets = 0;
    command->ttl = 64;
	return command;
}

void freeAndExit(cmd *command, int exit_code)
{
    if (command != NULL) {
        if (command->addr != NULL)
            freeaddrinfo(command->addr);
        if (command->packet != NULL)
            free(command->packet);
        if (command->packets != NULL)
            freePacketList(command->packets);
        if (command->socket != -1)
            close(command->socket);
        free(command);
    }
    exit(exit_code);
}

packet* createPacket(char *data, double time_to_transmit_packet)
{
    packet *new_packet = (packet *)malloc(sizeof(packet));
    if (new_packet != NULL)
    {
		new_packet->data = data;
		new_packet->time_to_tramsmit_packet = time_to_transmit_packet;
		new_packet->next = NULL;
		new_packet->prev = NULL;
    }
    return new_packet;
}

