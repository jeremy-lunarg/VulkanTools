#! /bin/bash -x
objdump -T build/layersvt/libVkLayer_device_simulation.so  |grep vk
