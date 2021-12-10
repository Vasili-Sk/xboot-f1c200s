#ifndef __WIFI_H__
#define __WIFI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum wifi_status_t {
	WIFI_STATUS_DISCONNECTED	= 0,
	WIFI_STATUS_CONNECTING		= 1,
	WIFI_STATUS_CONNECTED		= 2,
	WIFI_STATUS_CLIENT_OPENING	= 3,
	WIFI_STATUS_CLIENT_OPENED	= 4,
};

struct wifi_t
{
	char * name;

	bool_t (*connect)(struct wifi_t * wifi, const char * ssid, const char * passwd);
	bool_t (*disconnect)(struct wifi_t * wifi);
	bool_t (*client_open)(struct wifi_t * wifi, const char * ip, int port);
	bool_t (*client_close)(struct wifi_t * wifi);
	enum wifi_status_t (*status)(struct wifi_t * wifi);
	int (*read)(struct wifi_t * wifi, void * buf, int count);
	int (*write)(struct wifi_t * wifi, void * buf, int count);
	int (*ioctl)(struct wifi_t * wifi, const char * cmd, void * arg);

	void * priv;
};

struct wifi_t * search_wifi(const char * name);
struct wifi_t * search_first_wifi(void);
struct device_t * register_wifi(struct wifi_t * wifi, struct driver_t * drv);
void unregister_wifi(struct wifi_t * wifi);

bool_t wifi_connect(struct wifi_t * wifi, const char * ssid, const char * passwd);
bool_t wifi_disconnect(struct wifi_t * wifi);
bool_t wifi_client_open(struct wifi_t * wifi, const char * ip, int port);
bool_t wifi_client_close(struct wifi_t * wifi);
enum wifi_status_t wifi_status(struct wifi_t * wifi);
int wifi_read(struct wifi_t * wifi, void * buf, int count);
int wifi_write(struct wifi_t * wifi, void * buf, int count);
int wifi_ioctl(struct wifi_t * wifi, const char * cmd, void * arg);

#ifdef __cplusplus
}
#endif

#endif /* __WIFI_H__ */
