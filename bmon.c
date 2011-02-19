/* 
 * Bandwidth monitor
 * Wed Jan 24 16:37:38 IST 2007
 */

/* XXX: REFERENCE
 * getifaddrs(3)
 * /usr/include/net/if.h
 * /usr/src/usr.sbin/ifstated
 * ~/code/c/freebsd.c 
 */

#include <sys/types.h>
#include <sys/socket.h>

#include <net/if.h>

#include <err.h>
#include <ifaddrs.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	struct ifaddrs *ifap, *ifa;
	struct if_data *ifd; 
	char *name = argv[1];
	unsigned long prv_rx = 0, prv_tx = 0, rx, tx;
	unsigned short int found = 0;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s interface\n", argv[0]);
		exit(1);
	}

/*	for(;;) { */
		/* get list of interfaces */
		if (getifaddrs(&ifap) != 0)
			err(1, "getifaddrs");

		/* traverse the list */
		for (ifa = ifap; ifa; ifa = ifa->ifa_next) { 
			if (ifa->ifa_addr->sa_family != AF_LINK)
				continue;

			/* match for argument */
			if (!strcmp(ifa->ifa_name, name)) {
				ifd = ifa->ifa_data;

				/* check for link status */
				/* XXX: doesn't work with lo0 */
				if (ifd->ifi_link_state <= 1) {
					printf("%s: Down\n", name);
					return 0;
				}

				if (ifd) {
					found = 1;
					tx = ifd->ifi_obytes;
					rx = ifd->ifi_ibytes;

					/* see if the counter was reset */
					if (rx < prv_rx) {
						rx = (4294967295U - prv_rx) + ifd->ifi_ibytes;
					}
					if (tx < prv_tx)
						tx = (4294967295U - prv_tx) + ifd->ifi_obytes;

					printf("%s: %lu / %lu\n", name, rx, tx);
					/* flush stdout - required for ion's statusbar */
					fflush(stdout);

					/* save the values */
					prv_tx = tx;
					prv_rx = rx;
				
					break;
				}
			}
		} 
		if (!found) {
			fprintf(stderr, "%s not found\n", name);
			exit(1);
		}

/*		sleep(6); */
		freeifaddrs(ifap);
/*	} */

	return 0;
}
