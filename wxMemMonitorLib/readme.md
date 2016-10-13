
wxMemMonitorLib
----------------


wxWidgets Setting
---------


Usage:
---------

```sh
#include "../wxMemMonitorLib/wxMemMonitor.h"

MEMORYMONITOR_INNER_PROCESS();

if (!memmonitor::Init(memmonitor::INNER_PROCESS,hInstance,"config_target.json" ))
{
    MessageBoxA(NULL, memmonitor::GetLastError().c_str(), "ERROR", MB_OK);
}
~~
~~
memmonitor::Cleanup();
```

* ambigious funciton 'floor' 
	- modify -> floor to ::floor (in wxWidgets/geometry.h)



