include the readVcc library: ```#include <readVcc.h>```


Add this to the setup:

```
// Define voltages

int MIN_V = 2400; // empty voltage (0%)
int MAX_V = 3200; // full voltage (100%)
int oldBatteryPcnt;
```

Add this to the loop:

```
// Measure battery
  float batteryV = readVcc();
  int batteryPcnt = (((batteryV - MIN_V) / (MAX_V - MIN_V)) * 100 );
  if (batteryPcnt > 100) {
    batteryPcnt = 100;
  }
```

If you want to send the percentage to the mysensors gateway you have to add:

```
 if (batteryPcnt != oldBatteryPcnt) {
    gw.sendBatteryLevel(batteryPcnt); // Send battery percentage
    oldBatteryPcnt = batteryPcnt;
  }

```
