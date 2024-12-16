# linux_kernel_memory_monitor
A linux kernel module that terminates user level processes once a specified memory threshold is crossed to protect the system from memory overload


## Copy the source code by running;
  git clone https://github.com/dinisusmc/linux_kernel_memory_monitor.git


## Navigate into the directory
  cd <path_to_directory>

## Compile the code by running;
  make


## Load the kernel module
  sudo insmod memory_manager.ko


## Launch 2 python scripts to test functionality
  python3 ./test/important.py
  Python3 ./test/enough.py
    #### Note - You will need to adjust the size of the dataframe in enough.py to exceed 80% ram


## Remove the module when finished
  sudo rmmod memory_manage.ko

