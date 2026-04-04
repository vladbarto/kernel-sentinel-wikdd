# kernel-sentinel-wikdd
"System Informer"-like console application implementation using C-style threads and kernel-level drivers 

## Useful commands (to be run in CMD with admin rights):
### For WPP Tracing session create + start:
```bash
logman create trace "kernel-sentinel-driver.sys" -ets -p {bb6301c6-4e62-4bbf-b869-33b533287481} 0xffffffff 0xff -o "Z:\\mylog.etl"
```
And check if the your driver is there with the list displayed by the command:
```bash
logman query -ets
```

### Deploy jobs for drivers
Create:
```bash
sc.exe create kernel-sentinel-driver binPath=c:\Users\vboxuser\Documents\kernel-sentinel-driver.sys type=kernel
```

Start:
```bash
sc.exe start kernel-sentinel-driver
```

Stop:
```bash
sc.exe stop kernel-sentinel-driver
```

Delete:
```bash
sc.exe delete kernel-sentinel-driver
```

### Driver verifier
Deactivate it:
```bash
verifier /reset
```
+ reboot the system

Activate it: 
```bash
verifier /standard /driver kernel-sentinel-driver.sys
```

Reboot system: (restart in t = 0 seconds)
```bash
shutdown /r /t 0
```

Shutdown system: (shutdown in t = 0 seconds)
```bash
shutdown /s /t 0
```