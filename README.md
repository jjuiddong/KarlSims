
KarlSims
========

Evolved Virtual Creatures of Karl Sims Project 1994 with DX3D, PhysX, GA Algorithm, Neural Net Algorithm, Genotype Script Parser
- http://www.karlsims.com/evolved-virtual-creatures.html
- summary paper https://github.com/jjuiddong/KarlSims/wiki
- IDE - vs2010
- DX3D - DX 9.0
- PhysX - PhysX 3.3.0 Beta
- Genotype script https://github.com/jjuiddong/KarlSims/wiki/Genotype-Script


NVIDIA PhysX 3.3.0 Beta configure setting
---------
- Configure Variable
 - PHYSX_DIR - NVIDIA PhysX Library Directory
- No NVidia Graphic Card Computer
 - define RENDERER_PVD  in SampleBase, Sample Rederer project


BOOST LIBRARY 1_55_0
------------



wxMemMonitor (with wxWidgets 3.0.0)
---------

#### Usage:
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

* config file settging (json format)
	- filename: config_evc.json 
	- path : bin/win32 or win64/
	- contents: 
```
{
	"pdbpath" : "EvolvedVirtualCreaturesDEBUG.pdb" ,
	"sharedmemoryname" : "EVC"
}
```


* ambigious function 'floor' 
    - modify -> floor to ::floor (in wxWidgets/geometry.h)


License
----

MIT
