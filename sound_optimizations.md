# Sound Optimation for Raspberry PI based audio players

Based on [Linux Audio Adjustments](https://github.com/brianlight/Linux-Audio-Adjustments)

- added in /boot/cmdline.txt: isolcpus=2,3
  ```
  console=tty1 root=PARTUUID=4dce75b8-02 rootfstype=ext4 elevator=deadline fsck.repair=yes rootwait quiet splash plymouth.ignore-serial-consoles isolcpus=2,3
  ```
- changed options in /lib/systemd/system/mpd.service
  ```
  CPUSchedulingPolicy=fifo
  CPUSchedulingPriority=70
  Nice=-15
  ExecStart=/usr/bin/taskset -c 2,3 /usr/local/bin/mpd --no-daemon $MPDCONF
  ```
- /etc/sysctl.conf
  ```
  net.core.rmem_max = 16777216
  net.core.wmem_max = 16777216
  ```
- /etc/security/limits.conf
  ```
  audio - rtprio 99
  @audio - memlock 512000
  @audio - nice -20
  ```

- Run /home/pi/bin/optimize\_sound at boot

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
- Create /usr/lib/systemd/system/sound.service

  ```
  [Unit]
  Description=Sound

  [Service]
  ExecStart=/home/pi/bin/optimize_sound

  [Install]
  WantedBy=multi-user.target
  ```

- Reording ALSA devices by creating /etc/modprobe.d/alsa-base.conf

  Refer to this [Document](https://gist.github.com/rnagarajanmca/63badce0fe0e2ad126041c7c139970ea)

  Let us say you have two cards (snd\_soc\_allo\_piano\_dac\_plus on I2C an another DAC on usb) and we want the I2C audio card to appear first followed by the USB DAC.

  ```
  cat /proc/asound/modules
  0 vc4
  1 vc4
  2 snd_usb_audio
  3 snd_soc_allo_piano_dac_plus
  ```

  Now you need to create /etc/modprobe.d/alsa-base.conf like this

  ```
  options snd_soc_allo_piano_dac_plus index=0
  options snd_usb_audio index=1
  options vc4 index=2
  options snd slots=snd_soc_allo_piano_dac_plus,snd_usb_audio,vc4
  ```
