# Kernel design
When kernel loads and starts, firstly, _start in init/init.asm is called, it sets stack and start init_main(init/main.cpp)

init_main setups basic enviroment, and Kernel::Main(kernel/main.cpp) is called.

Kernel::Main loads drivers, creates idle task, and starts scheduler

# Take note that this is concept, and it can be changed.