#include "../ft_ping.h"

double getMinTime(cmd *command)
{
    double  min = DBL_MAX;
    packet  *curr = command->packets;
    
    while (curr)
    {
        if (curr->time_to_tramsmit_packet < min)
            min = curr->time_to_tramsmit_packet;
        curr = curr->next;
    }
    return min;
}

double getMaxTime(cmd *command) 
{
    double  max = 0.0;
    packet  *curr = command->packets;
    
    while (curr)
    {
        if (curr->time_to_tramsmit_packet > max)
            max = curr->time_to_tramsmit_packet;
        curr = curr->next;
    }
    return max;
}

double calcAvgTime(cmd *command)
{
    double  sum = 0.0;
    int     count = 0;
    packet  *curr = command->packets;
    
    while (curr)
    {
        sum += curr->time_to_tramsmit_packet;
        count++;
        curr = curr->next;
    }
    if (count > 0)
        return (sum / count);
    else
        return 0;
}

double calcStddev(cmd *command) 
{
    double  mean = calcAvgTime(command);
    double  sum_sq_diff = 0.0;
    int     count = 0;
    packet  *curr = command->packets;
    
    while (curr)
    {
        double diff = curr->time_to_tramsmit_packet - mean;
        sum_sq_diff += diff * diff;
        count++;
        curr = curr->next;
    }
    if (count > 1)
        return (sqrt(sum_sq_diff / count));
    else
        return 0;
}

void printStatistics(cmd *command)
{
    int packet_loss = ((command->nb_of_transmitted_packets -command->nb_of_received_packets) / command->nb_of_transmitted_packets) * 100;
    
    printf("--- %s ft_ping statistics ---\n", command->raw_adress);
    printf("%d packets transmitted, %d packets received, %d%% packet loss\n", command->nb_of_transmitted_packets, command->nb_of_received_packets, packet_loss);
    if (command->packets != NULL)
    {
        double min = getMinTime(command);
        double avg = calcAvgTime(command);
        double max = getMaxTime(command);
        double dev = calcStddev(command);
        printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", min, avg, max, dev);
    }
}