import time
import os

pid = os.getpid()

## Protected PID's that will not be killed
os.system(f'echo "{pid}" > /proc/ram_monitor_pids')

## Process names that are okay to kill
os.system('echo "python3" > /proc/ram_monitor_config')


while True:
    print("Very important process...")
    time.sleep(5)
