# Sound Optimation for Raspberry PI based audio players

Based on [Linux Audio Adjustments](https://github.com/brianlight/Linux-Audio-Adjustments)

- added in /boot/cmdline.txt: isolcpus=2,3
- changed options in /lib/systemd/system/mpd.service
  ```
  CPUSchedulingPolicy=fifo
  CPUSchedulingPriority=70
  Nice=-15
  ExecStart=/usr/bin/taskset -c 2,3 /usr/local/bin/mpd --no-daemon $MPDCONF
  ```
- /etc/sysctl.conf
  ```
  net.core.rmem\_max = 16777216
  net.core.wmem\_max = 16777216
  ```
- /etc/security/limits.conf
  ```
  audio - rtprio 99
  @audio - memlock 512000
  @audio - nice -20
  ```

- Run sound.sh at boot

  ```
  #!/bin/sh
  #Reduce Audio thread latency
  chrt -f -p 54 $(pgrep ksoftirqd/0)
  chrt -f -p 54 $(pgrep ksoftirqd/1)
  chrt -f -p 54 $(pgrep ksoftirqd/2)
  chrt -f -p 54 $(pgrep ksoftirqd/3)
  
  #Uncomment for MPD Affinity and Priority
  #chrt -f -p 81 $(pidof mpd)
  #taskset -c -p 2,3 $(pidof mpd)
  
  #USB Dacs Uncomment to reduce USB latency
  #modprobe snd-usb-audio nrpacks=1
  
  #SPDIF HAT and WiFi users Uncomment to turn off power to [Ethernet and USB] ports
  #echo 0x0 > /sys/devices/platform/soc/3f980000.usb/buspower
  
  #Reduce operating system latency
  echo noop > /sys/block/mmcblk0/queue/scheduler
  echo 1000000 > /proc/sys/kernel/sched_latency_ns
  echo 100000 > /proc/sys/kernel/sched_min_granularity_ns
  echo 25000 > /proc/sys/kernel/sched_wakeup_granularity_ns
  ```
