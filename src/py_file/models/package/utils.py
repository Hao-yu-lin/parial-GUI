import math
import torch
from torch import nn
from timm.models.layers import trunc_normal_
import cv2
import random
import numpy as np




def weights_init(m):
    classname = m.__class__.__name__
    if classname.find('Conv') != -1:
        m.weight.data.normal_(0.0, 0.02)
    elif classname.find('BatchNorm2d') != -1 or classname.find('InstanceNorm2d') != -1:
        m.weight.data.normal_(1.0, 0.02)
        m.bias.data.fill_(0)

@staticmethod
def print_network(net):
    num_params = 0
    for param in net.parameters():
        num_params += param.numel()
    print(net)
    print('Total number of parameters: %d' % num_params)
    

def init_weights(m):
    if isinstance(m, nn.Linear):
        trunc_normal_(m.weight, std=.02)
        if isinstance(m, nn.Linear) and m.bias is not None:
            nn.init.constant_(m.bias, 0)
    elif isinstance(m, nn.LayerNorm):
        nn.init.constant_(m.bias, 0)
        nn.init.constant_(m.weight, 1.0)
    elif isinstance(m, nn.Conv2d):
        fan_out = m.kernel_size[0] * m.kernel_size[1] * m.out_channels
        fan_out //= m.groups
        m.weight.data.normal_(0, math.sqrt(2.0 / fan_out))
        if m.bias is not None:
            m.bias.data.zero_()
            
@staticmethod
def denorm(x):
    return x * 0.5 + 0.5

@staticmethod
def tensor2numpy(x):
    return x.detach().cpu().numpy().transpose(1,2,0) # H X W X C

@staticmethod
def RGB2BGR(x):
    return cv2.cvtColor(x, cv2.COLOR_RGB2BGR)

@staticmethod
def combine_patches(patches, origin_size, img_size=128):
    patches = np.array(patches)
    # print(patches.shape)－
    origin_h, origin_w = origin_size
    n_h = (int(origin_h/img_size) if origin_h % img_size == 0 else int(origin_h/img_size) + 1)
    n_w = (int(origin_w/img_size) if origin_w % img_size == 0 else int(origin_w/img_size) + 1)
    new_concat = []
    for i in range(n_w):
        new = np.concatenate(patches[i*n_h:i*n_h+n_h], axis=1)
        new_concat.append(new)
    new_concat = np.array(new_concat)
    new_concat = np.concatenate(new_concat, axis=0)
    new_img = new_concat[:origin_w, :origin_h]
    # print(new_img.shape)
    return new_img

@staticmethod
def save_shadowimg(path, img):

    cv2.imwrite(path, img, [cv2.IMWRITE_PNG_COMPRESSION, 5])
    # cv2.imwrite(path, img, [cv2.IMWRITE_JPEG_QUALITY, 100])
    