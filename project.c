/**
 * 
 * @file   project.c
 * @author Anastasiia Berezovska (xberez04)
 * @brief  HTTP server implemented in c.
 * 
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>     
#include <sys/socket.h>
#include <arpa/inet.h>      
#include <netinet/in.h>

#define LINE_SIZE 100


void get_cpu_load(int new_socket){
	unsigned long long int user, nice, system, idle, iowait, irq, softirq, steal, non_idle, total;
	unsigned long long int prev_idle, prev_non_idle, prev_total;
	unsigned long long int total_d, idle_d;
	char load_str[10];
	char cpu[4];		//to extract word "cpu" from line.

    FILE *file = popen("cat /proc/stat | grep 'cpu' ", "r");
	fscanf(file, "%s %llu %llu %llu %llu %llu %llu %llu %llu", cpu, &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);

	//Count previous values (before sleeping).
	prev_idle = idle + iowait;
	prev_non_idle = user + nice + system + irq + softirq + steal;
	prev_total = prev_idle + prev_non_idle;
	fclose(file);

	sleep(1);

	FILE *file2 = popen("cat /proc/stat | grep 'cpu' ", "r");
	fscanf(file2, "%s %llu %llu %llu %llu %llu %llu %llu %llu", cpu ,&user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);

	//Count "actual" values (after sleeping).
	idle = idle + iowait;
	non_idle = user + nice + system + irq + softirq + steal;
	total = idle + non_idle;

	//Differentiate: actual value minus the previous one.
	total_d = total - prev_total;
	idle_d = idle - prev_idle;

	double cpu_load = (double)(total_d - idle_d)/total_d;
	cpu_load *= 100;
	fclose(file2);

	//Convert double to string.
	sprintf(load_str, "%.2lf%% \n", cpu_load);

	char message[255];
	strcpy(message,"HTTP/1.1 200 OK\r\nContent-Type:text/plain;\r\n\r\n");
	strcat(message, load_str);
	send(new_socket, message, strlen(message), 0);

    return;
}

void get_hostname(int new_socket){
	char hostname[LINE_SIZE];
    gethostname(hostname, LINE_SIZE);
	char message[255];
	strcpy(message,"HTTP/1.1 200 OK\r\nContent-Type:text/plain;\r\n\r\n");
	strcat(message, hostname);
	strcat(message, "\n");
	send(new_socket, message, strlen(message), 0);

	return;
}

void get_cpu_name(int new_socket){
    char info[LINE_SIZE];
    FILE *cpu_i = popen("cat /proc/cpuinfo | grep 'model name' | head -n 1 | awk -F ': ' '{print $2}'", "r");
    fgets(info, LINE_SIZE, cpu_i);
	char message[255];
	strcpy(message,"HTTP/1.1 200 OK\r\nContent-Type:text/plain;\r\n\r\n");
	strcat(message, info);
	send(new_socket, message, strlen(message), 0);

    return;
}

int main(int argc , char *argv[]){

	int sock, new_socket, c, port;
	struct sockaddr_in server , client;
	int opt = 1;
	
	//Create socket.
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1){
		perror("ERROR: socket()");
		exit(EXIT_FAILURE);
	}

	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
		perror("ERROR: setsockopt()");
		exit(EXIT_FAILURE);
	}

	port = atoi(argv[1]);
	
	//Prepare the sockaddr_in structure.
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
	
	//Bind.
	if(bind(sock,(struct sockaddr *)&server , sizeof(server)) < 0){
		perror("ERROR: bind()");
		exit(EXIT_FAILURE);
	}

	//Listen.
	if(listen(sock, 3) < 0){
		perror("ERROR: listen()");
		exit(EXIT_FAILURE);
	}
	
	//Accepting incoming connection.
	while(1){
		c = sizeof(struct sockaddr_in);
		char request[1500];
		char* bad_request = "HTTP/1.1 400 Bad request\r\n\r\n";
		
		new_socket = accept(sock, (struct sockaddr *)&client, (socklen_t*)&c);
		if (new_socket<0){
		perror("ERROR: accept()");
		exit(EXIT_FAILURE);
		}

		read(new_socket, request, 1500);

		if(strstr(request, "GET /cpu-name") != 0){
			get_cpu_name(new_socket);
		
		}
		else if(strstr(request, "GET /load") != 0){
			get_cpu_load(new_socket);

		}
		else if(strstr(request, "GET /hostname") != 0){
			get_hostname(new_socket);
		}
		else{
			send(new_socket, bad_request, strlen(bad_request), 0);
		}
		
		close(new_socket);
	}
	
	return 0;
}
