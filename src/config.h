// WiFi settings
// You can add settings for multiple hotspots and the
// device will auto connect to any one available
#define SSID_1 "ssid"
#define PSK_1 "password"
#define SSID_2 "ssid"
#define PSK_2 "password"
#define SSID_3 "ssid"
#define PSK_3 "password"

// LND settings
// sploony connects to LND's REST gateway
// NOTE: LND only listens from localhost by default, you 
// have to edit your lnd.conf to allow external requests
#define LND_HOST "https://<your lnd's external ip>:8080"

// Invoice settings
// value to be paid in Satoshis
#define INV_VALUE 3
// expiry in seconds
#define INV_EXPIRY 86400
