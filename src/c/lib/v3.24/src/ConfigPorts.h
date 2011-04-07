#ifndef _CONFIG_PORTS
#define _CONFIG_PORTS

enum CONFIG_PORT_INDEXES {	I_PAT_CONFIG_PORT,
							I_PROGRAM_CONFIG_PORT,
							I_NETWORK_PORT,
							I_PID_PORT,
							I_RF_PORT,
							I_PRODUCTID_CONFIG_PORT,
							I_TRAP_CONFIG_PORT,
							I_SET_PASSWORD_PORT,
							I_STATUS_POLL_PORT,
							I_FK_CONFIG_PORT,
							I_IP_REMAP_CONFIG_PORT,
							I_DEFAULT_CONFIG_PORT,
							I_PSI_TABLE_PORT,
							I_CAM_WATCHDOG_PORT,
							I_RESET_PORT,
							I_PROGRAM_INFO_PORT,
							I_VIDEO_PROGRAM_PORT,
							I_CIPHER_SABER_PORT,
							I_NUM_CONFIG_PORTS		};


#define SETUP_PORT_TABLE \
	memset( config_ports, 0, I_NUM_CONFIG_PORTS * sizeof( unsigned short ) ); \
	config_ports[I_PAT_CONFIG_PORT		] = PAT_CONFIG_PORT; \
	config_ports[I_PROGRAM_CONFIG_PORT	] = PROGRAM_CONFIG_PORT; \
	config_ports[I_NETWORK_PORT			] = NETWORK_CONFIG_PORT; \
	config_ports[I_PID_PORT				] = PID_CONFIG_PORT; \
	config_ports[I_RF_PORT				] = RF_CONFIG_PORT; \
	config_ports[I_PRODUCTID_CONFIG_PORT] = PRODUCTID_CONFIG_PORT; \
	config_ports[I_TRAP_CONFIG_PORT		] = TRAP_CONFIG_PORT; \
	config_ports[I_SET_PASSWORD_PORT	] = SET_PASSWORD_PORT; \
	config_ports[I_STATUS_POLL_PORT		] = STATUS_POLL_PORT; \
	config_ports[I_FK_CONFIG_PORT		] = FK_CONFIG_PORT; \
	config_ports[I_IP_REMAP_CONFIG_PORT	] = IP_REMAP_PORT; \
	config_ports[I_DEFAULT_CONFIG_PORT	] = DEFAULT_CONFIG_PORT; \
	config_ports[I_PSI_TABLE_PORT		] = PSI_TABLE_PORT; \
	config_ports[I_CAM_WATCHDOG_PORT	] = CAM_WATCHDOG_PORT; \
	config_ports[I_RESET_PORT			] = RESET_PORT; \
	config_ports[I_PROGRAM_INFO_PORT	] = PROGRAM_INFO_PORT; \
	config_ports[I_VIDEO_PROGRAM_PORT	] = VIDEO_PROGRAM_PORT; \
	config_ports[I_CIPHER_SABER_PORT	] = CIPHER_SABER_PORT; 

#endif

