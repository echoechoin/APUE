#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>	// inet_ntop()
 
static void printhost(struct hostent *host);
 
int
main(int argc, char *argv[])
{
	struct hostent *host = NULL;
	sethostent(1);
	while( (host = gethostent()) != 0 )
	{
		printhost(host);
		printf("\n");
	}
	endhostent();
	return EXIT_SUCCESS;
}
static void printhost(struct hostent *host)
{
	char **aliases = NULL;
	char **addr_list = NULL;
	char addr_p[INET_ADDRSTRLEN];	// IPv4
	
	// print host name and aliases
	printf("hostname: %s\n", host->h_name);
	for(aliases = host->h_aliases; *aliases; aliases++)
	{
		printf("alternate name: %s\n", *aliases);
	}
	
	// print address type and length
	if(host->h_addrtype == AF_INET)
	{
		printf("address type: AF_INET\n");
	}
	else
	{
		printf("Not an IPv4 address.\n");
	}
	printf("address length: %d\n", host->h_length);
	
	// print address list
	//printf("%d\n", sizeof(*(host->h_addr_list)));
	printf("%x\n", *(int *)(*(host->h_addr_list)));
	for(addr_list = host->h_addr_list; *addr_list; addr_list++)
	{
		printf("address: %s\n", inet_ntop(host->h_addrtype, *addr_list, addr_p, INET_ADDRSTRLEN));
	}
}
