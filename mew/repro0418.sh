#! /bin/bash -x

cd dbuild/tests/

unset LD_LIBRARY_PATH
unset VK_LAYER_PATH
unset VK_INSTANCE_LAYERS
export VK_DEVSIM_130_LAYER_ENABLE=1
export VK_DEVSIM_FILENAME=devsim_test3_in1.json
export VK_DEVSIM_DEBUG_ENABLE="1"
#export VK_DEVSIM_EXIT_ON_ERROR="1"
#export VK_LOADER_DEBUG="all"

# set up implicit filesystem
rm -rf /home/mikew/.local/share/vulkan/implicit_layer.d
mkdir -p /home/mikew/.local/share/vulkan/implicit_layer.d
ln -s /home/mikew/gits/github.com/LunarG/VulkanTools/layersvt/linux/VkLayer_device_simulation_IMPLICIT.json /home/mikew/.local/share/vulkan/implicit_layer.d
ln -s /home/mikew/gits/github.com/LunarG/VulkanTools/dbuild/layersvt/libVkLayer_device_simulation.so /home/mikew/.local/share/vulkan/implicit_layer.d

# delete old temp files
rm -f devsim_test3_temp1.json devsim_test3_temp2.json

/home/mikew/gits/github.com/LunarG/VulkanTools/dbuild/submodules/Vulkan-LoaderAndValidationLayers/demos/vulkaninfo -j |less

# delete old implicit filesystem
rm -rf /home/mikew/.local/share/vulkan/implicit_layer.d
